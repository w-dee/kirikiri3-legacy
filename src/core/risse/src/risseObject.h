//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オブジェクトの C++ インターフェースの定義と実装
//---------------------------------------------------------------------------
#ifndef risseObjectH
#define risseObjectH

#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAssert.h"
#include "risseGC.h"
#include "risseString.h"
#include "risseOpCodes.h"
#include "risseMethod.h"
#include "risseOperateRetValue.h"
#include "risseObjectInterfaceArg.h"
#include "risseVariant.h"
#include "risseThread.h"

namespace Risse
{
class tVariant;
class tRTTI;
//---------------------------------------------------------------------------
/**
 * Risseオブジェクトインターフェース
 */
class tObjectInterface : public tCollectee, public tOperateRetValue
{
	const tRTTI * RTTI; //!< このオブジェクトインターフェースの「型」をC++レベルで
					//!< 識別するためのメンバ。簡易RTTI。とくに識別しない場合は
					//!< NULLを入れておく。
	tCriticalSection * CS; //!< このオブジェクトを保護するためのクリティカルセクション

public:
	/**
	 * コンストラクタ
	 */
	tObjectInterface() { RTTI = NULL; CS = new tCriticalSection();/*TODO: CSのキャッシュ*/}

	/**
	 * コンストラクタ(RTTIを指定)
	 * @param rtti	RTTI
	 */
	tObjectInterface(const tRTTI * rtti) { RTTI = rtti; CS = new tCriticalSection();/*TODO: CSのキャッシュ*/}

	/**
	 * コンストラクタ(RTTIとCSを指定)
	 * @param rtti	RTTI
	 */
	tObjectInterface(const tRTTI * rtti, tCriticalSection * cs) { RTTI = rtti; CS = cs; }

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tObjectInterface() {}

	/**
	 * オブジェクトに対して操作を行う
	 * @param code		オペレーションコード
	 * @param result	結果の格納先 (NULLの場合は結果が要らない場合)
	 * @param name		操作を行うメンバ名
	 *					(空文字列の場合はこのオブジェクトそのものに対しての操作)
	 * @param flags		オペレーションフラグ
	 * @param args		引数
	 * @param This		メソッドが実行されるべき"Thisオブジェクト"
	 *					(NULL="Thisオブジェクト"を指定しない場合)
	 * @return	エラーコード
	 * @note	何か操作に失敗した場合は例外が発生する。ただし、tRetValueにあるような
	 *			エラーの場合は例外ではなくてエラーコードを返さなければならない。
	 */
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG) = 0;

	/**
	 * オブジェクトに対して操作を行う(失敗した場合は例外を発生させる)
	 * @param code		オペレーションコード
	 * @param result	結果の格納先 (NULLの場合は結果が要らない場合)
	 * @param name		操作を行うメンバ名
	 *					(空文字列の場合はこのオブジェクトそのものに対しての操作)
	 * @param flags		オペレーションフラグ
	 * @param args		引数
	 * @param This		メソッドが実行されるべき"Thisオブジェクト"
	 *					(NULL="Thisオブジェクト"を指定しない場合)
	 * @note	何か操作に失敗した場合は例外が発生する。このため、このメソッドに
	 *			エラーコードなどの戻り値はない
	 */
	void Do(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG)
	{
		tRetValue ret = Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
		if(ret != rvNoError) RaiseError(ret, name);
	}

	/**
	 * RTTI情報を得る
	 */
	const tRTTI * GetRTTI() const { return RTTI; }

	/**
	 * RTTI情報を設定する
	 * @param rtti	RTTI情報
	 */
	void SetRTTI(const tRTTI * rtti) { RTTI = rtti; }

	/**
	 * CS を持っているかどうかを返す
	 */
	bool HasCS() const { return CS != NULL; }

	/**
	 * CS を返す
	 * @note	CS のロックは tSynchronizer の方を使うこと。
	 *			これを使うのは RISSE_ASSERT_CS_LOCKED ぐらいにしておくこと。
	 */
	tCriticalSection * GetCS() const { return CS; }

	/**
	 * synchronize を行うクラス
	 */
	class tSynchronizer
	{
	private:
		char Locker[sizeof(tCriticalSection::tLocker)]; //!< ロックオブジェクトを配置する先
		tCriticalSection * CS; //!< ロックが行うCS
			//!< (たんにロックが行われたかどうかを表すbool値でもよいのだが
			//!<  この構造体のサイズを推測しなければならない理由が risseVariant.h にあり
			//!< 推測しやすいポインタサイズとした (bool はパディングがどうなるかが分かりづらい) )
		// void * operator new(size_t); //!< heap 上に作成できません
		// void * operator new[](size_t); //!< heap 上に作成できません
		tSynchronizer(const tSynchronizer &); //!< copy 出来ません
		void operator =(const tSynchronizer &); //!< copy 出来ません
	public:
		/**
		 * コンストラクタ
		 * @param intf	オブジェクトインターフェース
		 */
		tSynchronizer(const tObjectInterface * intf)
		{
			// intf が非 null かつ intf が CS を持っている場合のみに
			// ロックを行う。
			// tCriticalSection::tLocker はコンストラクタでロックを
			// 行い、デストラクタでロックの解除をするため、ロックを行う
			// 必要がある場合はプレースメント new でロックを行う。解除は
			// 同様にデストラクタを個別に呼び出すことで実現する。
			if(intf && intf->CS)
			{
				// ロックを行う
				CS = intf->CS;
				new (reinterpret_cast<tCriticalSection::tLocker*>(Locker))
					tCriticalSection::tLocker(*(intf->CS));
			}
			else
			{
				CS = NULL;
			}
		}

		/**
		 * デストラクタ
		 */
		~tSynchronizer()
		{
			if(CS)
			{
				(reinterpret_cast<tCriticalSection::tLocker*>(Locker))->
						~tLocker();
			}
		}
	};

};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * 識別用クラス
 * @note	このクラスは tObjectInterface を継承するが、なにも
 *			実装をおこなわない。このインスタンスを作り、tVariantに
 *			のせることができるが、インスタンスによる区別を行いたい場合のみに作る
 */
class tIdentifyObject : public tObjectInterface
{
public:
	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tIdentifyObject() {}

	tIdentifyObject() : tObjectInterface((const tRTTI *)NULL, (tCriticalSection *)NULL) {;}

	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------


}
#endif


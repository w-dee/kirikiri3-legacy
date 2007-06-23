//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
class tRisseVariantBlock;
typedef tRisseVariantBlock tRisseVariant;
class tRisseRTTI;
//---------------------------------------------------------------------------
//! @brief		Risseオブジェクトインターフェース
//---------------------------------------------------------------------------
class tRisseObjectInterface : public tRisseCollectee, public tRisseOperateRetValue
{
	const tRisseRTTI * RTTI; //!< このオブジェクトインターフェースの「型」をC++レベルで
					//!< 識別するためのメンバ。簡易RTTI。とくに識別しない場合は
					//!< NULLを入れておく。
	tRisseCriticalSection * CS; //!< このオブジェクトを保護するためのクリティカルセクション

public:
	//! @brief		コンストラクタ
	tRisseObjectInterface() { RTTI = NULL; CS = new tRisseCriticalSection();/*TODO: CSのキャッシュ*/}

	//! @brief		コンストラクタ(RTTIを指定)
	//! @param		rtti		RTTI
	tRisseObjectInterface(const tRisseRTTI * rtti) { RTTI = rtti; CS = new tRisseCriticalSection();/*TODO: CSのキャッシュ*/}

	//! @brief		コンストラクタ(RTTIとCSを指定)
	//! @param		rtti		RTTI
	tRisseObjectInterface(const tRisseRTTI * rtti, tRisseCriticalSection * cs) { RTTI = rtti; CS = cs; }

	//! @brief		オブジェクトに対して操作を行う
	//! @param		code	オペレーションコード
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		name	操作を行うメンバ名
	//!						(空文字列の場合はこのオブジェクトそのものに対しての操作)
	//! @param		flags	オペレーションフラグ
	//! @param		args	引数
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @return		エラーコード
	//! @note		何か操作に失敗した場合は例外が発生する。ただし、tRetValueにあるような
	//!				エラーの場合は例外ではなくてエラーコードを返さなければならない。
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG) = 0;

	//! @brief		オブジェクトに対して操作を行う(失敗した場合は例外を発生させる)
	//! @param		code	オペレーションコード
	//! @param		result	結果の格納先 (NULLの場合は結果が要らない場合)
	//! @param		name	操作を行うメンバ名
	//!						(空文字列の場合はこのオブジェクトそのものに対しての操作)
	//! @param		flags	オペレーションフラグ
	//! @param		args	引数
	//! @param		This	メソッドが実行されるべき"Thisオブジェクト"
	//!						(NULL="Thisオブジェクト"を指定しない場合)
	//! @note		何か操作に失敗した場合は例外が発生する。このため、このメソッドに
	//!				エラーコードなどの戻り値はない
	void Do(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG)
	{
		tRetValue ret = Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
		if(ret != rvNoError) RaiseError(ret, name);
	}

	//! @brief		RTTI情報を得る
	//! @param		RTTI情報
	const tRisseRTTI * GetRTTI() const { return RTTI; }

	//! @brief		RTTI情報を設定する
	//! @param		rtti		RTTI情報
	void SetRTTI(const tRisseRTTI * rtti) { RTTI = rtti; }

	//! @brief		CS を持っているかどうかを返す
	bool HasCS() const { return CS != NULL; }

	//! @brief		synchronize を行うクラス
	class tSynchronizer
	{
	private:
		char Locker[sizeof(tRisseCriticalSection::tLocker)]; //!< ロックオブジェクトを配置する先
		tRisseCriticalSection * CS; //!< ロックが行うCS
			//!< (たんにロックが行われたかどうかを表すbool値でもよいのだが
			//!<  この構造体のサイズを推測しなければならない理由が risseVariant.h にあり
			//!< 推測しやすいポインタサイズとした (bool はパディングがどうなるかが分かりづらい) )
		// void * operator new(size_t); //!< heap 上に作成できません
		// void * operator new[](size_t); //!< heap 上に作成できません
		tSynchronizer(const tSynchronizer &); //!< copy 出来ません
		void operator =(const tSynchronizer &); //!< copy 出来ません
	public:
		//! @brief	コンストラクタ
		//! @param	intf	オブジェクトインターフェース
		tSynchronizer(const tRisseObjectInterface * intf)
		{
			// intf が非 null かつ intf が CS を持っている場合のみに
			// ロックを行う。
			// tRisseCriticalSection::tLocker はコンストラクタでロックを
			// 行い、デストラクタでロックの解除をするため、ロックを行う
			// 必要がある場合はプレースメント new でロックを行う。解除は
			// 同様にデストラクタを個別に呼び出すことで実現する。
			if(intf && intf->CS)
			{
				// ロックを行う
				CS = intf->CS;
				new (reinterpret_cast<tRisseCriticalSection::tLocker*>(Locker))
					tRisseCriticalSection::tLocker(*(intf->CS));
			}
			else
			{
				CS = NULL;
			}
		}

		//! @brief	デストラクタ
		~tSynchronizer()
		{
			if(CS)
			{
				(reinterpret_cast<tRisseCriticalSection::tLocker*>(Locker))->
						~tLocker();
			}
		}
	};

};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		識別用クラス
//! @note		このクラスは tRisseObjectInterface を継承するが、なにも
//!				実装をおこなわない。このインスタンスを作り、tRisseVariantに
//!				のせることができるが、インスタンスによる区別を行いたい場合のみに作る
//---------------------------------------------------------------------------
class tRisseIdentifyObject : public tRisseObjectInterface
{
public:
	tRisseIdentifyObject() : tRisseObjectInterface((const tRisseRTTI *)NULL, (tRisseCriticalSection *)NULL) {;}

	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------


}
#endif


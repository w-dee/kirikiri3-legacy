//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Thread" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseThreadClassH
#define risseThreadClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"

namespace Risse
{
class tRisseScriptThread;
//---------------------------------------------------------------------------
//! @brief		"Thread" クラスのインスタンス用 C++クラス
//---------------------------------------------------------------------------
class tRisseThreadInstance : public tRisseObjectBase
{
private:
	tRisseScriptThread * Thread; //!< スレッドの実装

	tRisseVariant Method; //!< 実行するメソッド
	tRisseVariant Context; //!< 実行するメソッドのコンテキスト
	tRisseVariant Ret; //!< Execute メソッドの戻り値
	const tRisseVariant *Exception; //!< 例外が発生した場合の例外オブジェクト

public:
	//! @brief		コンストラクタ
	tRisseThreadInstance();

	//! @brief		ダミーのデストラクタ(おそらく呼ばれない)
	virtual ~tRisseThreadInstance() {;}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tRisseNativeCallInfo & info);
	void run() const;
	void start();
	tRisseVariant join() const;

	// friend 指定
	friend class tRisseScriptThread;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"Thread" クラス
//---------------------------------------------------------------------------
class tRisseThreadClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseThreadClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tRisseVariant ovulate();

public:
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif

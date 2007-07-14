//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse スクリプトブロックの実装
//---------------------------------------------------------------------------
#ifndef ScriptBlockH
#define ScriptBlockH

#include "../risseScriptBlockClass.h"

namespace Risse
{
//---------------------------------------------------------------------------
class tRisseScriptBlockInstance : public tScriptBlockInstance
{
public:
	//! @brief		コンストラクタ
	tRisseScriptBlockInstance();

	//! @brief		AST のルートノードを取得する(下位クラスで実装すること)
	//! @param		need_result		結果が必要かどうか
	//! @param		is_expression	式モードかどうか
	//! @return		AST ルートノード
	tASTNode * GetASTRootNode(bool need_result = false, bool is_expression = false);


public: // Risse用メソッドなど
	void construct();
	void initialize(
		const tString &script, const tString & name, risse_size lineofs,
		const tNativeCallInfo &info);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"ScriptBlock" クラス
//---------------------------------------------------------------------------
class tRisseScriptBlockClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseScriptBlockClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public:
};
//---------------------------------------------------------------------------


} // namespace Risse

#endif


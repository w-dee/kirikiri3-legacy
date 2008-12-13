//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
	/**
	 * コンストラクタ
	 */
	tRisseScriptBlockInstance();

	/**
	 * AST のルートノードを取得する(下位クラスで実装すること)
	 * @param need_result	結果が必要かどうか
	 * @param is_expression	式モードかどうか
	 * @return	AST ルートノード
	 */
	tASTNode * GetASTRootNode(bool need_result = false, bool is_expression = false);


public: // Risse用メソッドなど
	void construct();
	void initialize(
		const tString &script, const tString & name, risse_size lineofs,
		const tNativeCallInfo &info);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
/**
 * "ScriptBlock" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tRisseScriptBlockClass, tClassBase, tRisseScriptBlockInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------


} // namespace Risse

#endif


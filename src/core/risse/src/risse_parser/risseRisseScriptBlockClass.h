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
#ifndef RisseScriptBlockH
#define RisseScriptBlockH

#include "../risseScriptBlockClass.h"

namespace Risse
{
//---------------------------------------------------------------------------
class tRisseRisseScriptBlockInstance : public tRisseScriptBlockInstance
{
public:
	//! @brief		コンストラクタ
	tRisseRisseScriptBlockInstance();

	//! @brief		AST のルートノードを取得する(下位クラスで実装すること)
	//! @param		need_result		結果が必要かどうか
	//! @param		is_expression	式モードかどうか
	//! @return		AST ルートノード
	tRisseASTNode * GetASTRootNode(bool need_result = false, bool is_expression = false);


public: // Risse用メソッドなど
	void construct();
	void initialize(
		const tRisseString &script, const tRisseString & name, risse_size lineofs,
		const tRisseNativeCallInfo &info);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"RisseScriptBlock" クラス
//---------------------------------------------------------------------------
class tRisseRisseScriptBlockClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseRisseScriptBlockClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	tRisseVariant CreateNewObjectBase();

public:
};
//---------------------------------------------------------------------------


} // namespace Risse

#endif


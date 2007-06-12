//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse パーサ
//---------------------------------------------------------------------------
#ifndef risseParserH
#define risseParserH

#include "risseLexer.h"
#include "../risseTypes.h"
#include "../risseGC.h"
#include "../compiler/risseAST.h"
#include "../compiler/risseDeclAttribute.h"


namespace Risse {
// このファイルは Risse の名前空間内で include しないとだめ
#include "risseParser.inc"


class tRisseScriptBlockInstance;
//---------------------------------------------------------------------------
//! @brief		Risse パーサクラス
//---------------------------------------------------------------------------
class tRisseParser : public tRisseCollectee
{
	tRisseScriptBlockInstance * ScriptBlockInstance; //!< スクリプトブロック
	tRisseASTNode * Root; //!< ルートノード
	tRisseLexer * Lexer; //!< 字句解析器

public:
	//! @brief		コンストラクタ
	//! @param		sb			スクリプトブロック
	//! @param		lexer		字句解析器へのポインタ
	tRisseParser(tRisseScriptBlockInstance * sb, tRisseLexer * lexer);

public:
	//! @brief		スクリプトブロックを得る
	//! @return		スクリプトブロック
	tRisseScriptBlockInstance * GetScriptBlockInstance() const { return ScriptBlockInstance; }

	//! @brief		字句解析を一つ進める
	//! @param		トークンの値を格納する先
	//! @return		トークンID
	int GetToken(tRisseVariant & value);

	//! @brief		Lexerを得る
	//! @return		Lexer
	tRisseLexer * GetLexer() const { return Lexer; }

	//! @brief		ルートノードを設定する
	//! @param		root		ルートノード
	void SetRootNode(tRisseASTNode * root);

	//! @brief		ルートノードを得る
	//! @return		ルートノード
	tRisseASTNode * GetRoot() const { return Root; }
};
//---------------------------------------------------------------------------
} // namespace Risse

#endif


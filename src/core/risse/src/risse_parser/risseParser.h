//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

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


class tScriptBlockInstance;
//---------------------------------------------------------------------------
/**
 * Risse パーサクラス
 */
class tParser : public tCollectee
{
	tScriptBlockInstance * ScriptBlockInstance; //!< スクリプトブロック
	tASTNode * Root; //!< ルートノード
	tLexer * Lexer; //!< 字句解析器

public:
	/**
	 * コンストラクタ
	 * @param sb	スクリプトブロック
	 * @param lexer	字句解析器へのポインタ
	 */
	tParser(tScriptBlockInstance * sb, tLexer * lexer);

public:
	/**
	 * スクリプトブロックを得る
	 * @return	スクリプトブロック
	 */
	tScriptBlockInstance * GetScriptBlockInstance() const { return ScriptBlockInstance; }

	/**
	 * 字句解析を一つ進める
	 * @return	トークンID
	 */
	int GetToken(tVariant & value);

	/**
	 * Lexerを得る
	 * @return	Lexer
	 */
	tLexer * GetLexer() const { return Lexer; }

	/**
	 * ルートノードを設定する
	 * @param root	ルートノード
	 */
	void SetRootNode(tASTNode * root);

	/**
	 * ルートノードを得る
	 * @return	ルートノード
	 */
	tASTNode * GetRoot() const { return Root; }
};
//---------------------------------------------------------------------------
} // namespace Risse

#endif


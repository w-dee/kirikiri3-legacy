//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief スクリプトブロック管理
//---------------------------------------------------------------------------
#ifndef risseScriptBlockClassH
#define risseScriptBlockClassH

#include "risseTypes.h"
#include "risseString.h"
#include "risseVariant.h"
#include "risseAssert.h"
#include "risseGC.h"
#include "risseObjectBase.h"
#include "risseNativeBinder.h"
#include "risseClass.h"

namespace Risse
{
class tASTNode;
class tCodeBlock;
class tScriptEngine;
class tBindingInfo;
//---------------------------------------------------------------------------
//! @brief		スクリプトブロックの基底クラス
//---------------------------------------------------------------------------
class tScriptBlockInstance : public tObjectBase
{
private:
	tString Script; //!< スクリプトの内容
	tString Name; //!< スクリプトブロックの名称(たいていはファイル名)
	risse_size LineOffset; //!< スクリプトの行オフセット (ドキュメント埋め込みスクリプト用)
	mutable risse_size * LinesToPosition; //!< 各行の先頭に対応するコードポイント位置の配列
	mutable risse_size LineCount; //!< スクリプトの行数

	tCodeBlock * RootCodeBlock; //!< ルート位置(主にグローバル位置) にあるコードブロック(一度実行されてしまえば要らないはず)
	gc_vector<tCodeBlock *> *CodeBlocks; //!< コードブロック配列(一度コンパイルが済めばクリアされる)
	gc_vector<void *> *TryIdentifiers; //!< Tryを識別するための識別子(一度コンパイルが済めばクリアされる)

protected:
	//! @brief		コンストラクタ
	tScriptBlockInstance();

	//! @brief		スクリプトと名前の設定
	//! @param		script		スクリプトの内容
	//! @param		name		スクリプトブロックの名称
	//! @param		lineofs		行オフセット(ドキュメント埋め込みスクリプト用に、
	//!							スクリプトのオフセットを記録できる)
	//! @note		構築直後に１回だけ呼ぶこと。
	void SetScriptAndName(const tString & script, const tString & name, int lineofs);

	//! @brief		デストラクタ(呼ばれない)
	virtual ~tScriptBlockInstance() {;}

	//! @brief		LinesToPosition の内容を作成する
	void CreateLinesToPositionArary() const;

	//! @brief		LinesToPosition の内容が作成されていなければ作成する
	void EnsureLinesToPositionArary() const
	{ if(!LinesToPosition) CreateLinesToPositionArary(); }

public:
	//! @brief		スクリプトエンジンを返す
	//! @return		スクリプトエンジン
	tScriptEngine * GetScriptEngine() const { return GetRTTI()->GetScriptEngine(); }

	//! @brief		スクリプトの内容を得る	@return スクリプトの内容
	const tString & GetScript() const { return Script; }

	//! @brief		スクリプトブロックの名称を得る	@return スクリプトブロックの名称
	const tString & GetName() const { return Name; }

	//! @brief		スクリプトのコードポイント位置から行/桁位置への変換を行う
	//! @param		pos			コードポイント位置
	//! @param		line		行位置(0～; 興味がない場合はnull可)
	//! @param		col			桁位置(0～; 興味がない場合はnull可)
	void PositionToLineAndColumn(risse_size pos, risse_size *line, risse_size *col) const;

	//! @brief		スクリプトのコードポイント位置から行への変換を行う
	//! @param		pos			コードポイント位置
	//! @return		行位置(0～)
	risse_size PositionToLine(risse_size pos) const;

	//! @brief		指定行のスクリプトの内容を返す
	//! @param		line		行位置(0～)
	//! @return		その行にあるスクリプト(最後の改行記号は取り除かれる)
	//! @note		line が範囲外だった場合は空文字列が帰る
	tString GetLineAt(risse_size line);

	//! @brief		スクリプト上の位置から "%1 at %2" 形式のメッセージを組み立てる
	//! @param		pos		スクリプト上の位置
	//! @param		message	メッセージ
	//! @return		組み立てられたメッセージ
	tString BuildMessageAt(risse_size pos, const tString & message);

	//! @brief		警告を表示する
	//! @param		pos		スクリプト上の位置
	//! @param		message	メッセージ
	void OutputWarning(risse_size pos, const tString & message);

protected:
	//! @brief		ASTを元にコンパイルを行う
	//! @param		root		ルートASTノード
	//! @param		binding		バインディング情報
	//! @param		need_result		評価時に結果が必要かどうか
	//! @param		is_expression	式評価モードかどうか
	void Compile(tASTNode * root, const tBindingInfo & binding, bool need_result, bool is_expression);


public:
	//! @brief		ルート位置にあるコードブロックを設定する
	//! @param		codeblock		コードブロック
	void SetRootCodeBlock(tCodeBlock * codeblock) { RootCodeBlock = codeblock; }

	//! @brief		コードブロックを追加する
	//! @param		codeblock		コードブロック
	//! @return		そのコードブロックのインデックス
	risse_size AddCodeBlock(tCodeBlock * codeblock);

	//! @brief		try識別子を追加する
	//! @return		その識別子へのインデックス
	risse_size AddTryIdentifier();

	//! @brief		スクリプトブロックのfixupを行う
	//! @note		コンパイル直後のスクリプトブロックやロード
	//!				直後のスクリプトブロックは、
	//!				スクリプトブロック中のコードブロック同士の
	//!				依存関係等を解決するためにfixupを行う必要がある。
	//!				fixupが行われるとCodeBlocksはクリアされる。
	void Fixup();

	//! @brief		指定インデックスのコードブロックを得る
	//! @param		index		インデックス
	//! @return		そのインデックスにあるコードブロック
	tCodeBlock * GetCodeBlockAt(risse_size index) const;

	//! @brief		指定インデックスのtry識別子を得る
	//! @param		index		インデックス
	//! @return		そのインデックスにあるtry識別子
	void * GetTryIdentifierAt(risse_size index) const;

public:
	//! @brief		スクリプトを評価する
	//! @param		binding			バインディング情報
	//! @param		result			実行の結果(NULL可)
	//! @param		is_expression	式モードかどうか
	//! @note		もしスクリプトがコンパイルが必要な場合、
	//!				Evaluate は評価に先立って Compile() を呼び、コンパイルを行う。
	//!				その後、Fixup() を呼んでから RootCodeBlock を実行する。
	//!				is_expression	は Risse のように文と式を区別しない言語では常にfalseでよい。
	void Evaluate(const tBindingInfo & binding, tVariant * result = NULL, bool is_expression = false);

protected:
	//! @brief		AST のルートノードを取得する(下位クラスで実装すること)
	//! @param		need_result		結果が必要かどうか
	//! @param		is_expression	式モードかどうか
	//! @return		AST ルートノード
	virtual tASTNode * GetASTRootNode(bool need_result = false, bool is_expression = false) = 0;


public: // Risse用メソッドなど
	void construct();
	void initialize(
		const tString &script, const tString & name, risse_size lineofs,
		const tNativeCallInfo &info);
	tString mnString() { return GetScript(); }
	tString get_script() { return GetScript(); }
	tString get_name() { return GetName(); }
	tString getLineAt(risse_size line) { return GetLineAt(line); }
	risse_size positionToLine(risse_size pos) { return PositionToLine(pos); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"ScriptBlock" クラス
//---------------------------------------------------------------------------
class tScriptBlockClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tScriptBlockClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public:
};
//---------------------------------------------------------------------------

} // namespace Risse

#endif


//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コードブロック
//---------------------------------------------------------------------------
#ifndef risseCodeBlockH
#define risseCodeBlockH

#include "risseGC.h"
#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseVariant.h"
#include "risseObject.h"

//---------------------------------------------------------------------------
namespace Risse
{
class tRisseCodeGenerator;
class tRisseCodeExecutor;
class tRisseScriptBlockBase;
//---------------------------------------------------------------------------
//! @brief		コードブロッククラス
//! @note		コードブロックは Risse の実行単位である「関数」などごとに、
//!				VMが実行すべき環境についての情報を保持するクラスである
//---------------------------------------------------------------------------
class tRisseCodeBlock : public tRisseObjectInterface
{
	tRisseScriptBlockBase * ScriptBlock; //!< スクリプトブロックインスタンス
	risse_uint32 *Code; //!< コード
	risse_size CodeSize; //!< コードのサイズ(ワード単位)
	tRisseVariant *Consts; //!< 定数領域
	risse_size ConstsSize; //!< 定数領域のサイズ(個)
	risse_size NumRegs; //!< 必要なレジスタ数
	risse_size NestLevel; //!< 関数のネストレベル
	risse_size MaxNestLevel; //!< 関数の最大のネストレベル (NestLevel==0のコードブロックでのみ有効)
	risse_size NumSharedVars; //!< 必要な共有変数の数
	std::pair<risse_size, risse_size> * CodeToSourcePosition; //!< コード上の位置からソースコード上の位置へのマッピングの配列
	risse_size CodeToSourcePositionSize; //!< コード上の位置からソースコード上の位置へのマッピングの配列のサイズ

	typedef std::pair<risse_size, risse_size> tRelocation; //!<  コードブロック再配置情報のtypedef
	tRelocation * CodeBlockRelocations; //!< コードブロック再配置情報
	risse_size CodeBlockRelocationSize;

	tRelocation * TryIdentifierRelocations; //!< try識別子再配置情報
	risse_size TryIdentifierRelocationSize;

	tRisseCodeExecutor * Executor; //!< コード実行クラスのインスタンス

public:
	//! @brief		コンストラクタ
	//! @param		sb		スクリプトブロックインスタンス
	tRisseCodeBlock(tRisseScriptBlockBase * sb);

	//! @brief		スクリプトブロックインスタンスを得る
	//! @return		スクリプトブロックインスタンス
	tRisseScriptBlockBase * GetScriptBlock() const { return ScriptBlock; }

	//! @brief		コードを設定する(コードジェネレータから)
	//! @param		gen		コードジェネレータ
	void Assign(const tRisseCodeGenerator *gen);

	//! @brief		最大のネストレベルを設定する
	//! @param		level		最大のネストレベル
	void SetMaxNestLevel(risse_size level);

	//! @brief		再配置情報の fixup を行う
	//! @param		sb		スクリプトブロック
	void Fixup();

	//! @brief		このコードブロックのオブジェクトを得る
	//! @return		このコードブロックを表すオブジェクト
	tRisseVariant GetObject();

	//! @brief		内容をダンプ(逆アセンブル)する
	//! @return		ダンプした結果
	tRisseString Dump() const;

 	//! @brief		コードを得る
 	//! @return		コード
 	const risse_uint32 * GetCode() const { return Code; }

 	//! @brief		コードのサイズ(ワード単位)を得る
	//! @return		コードのサイズ
 	risse_size GetCodeSize() const { return CodeSize; }

	//! @brief		定数領域を得る
	//! @return		定数領域
	const tRisseVariant * GetConsts() const { return Consts; }

	//! @brief		定数領域のサイズ(個)を得る
	//! @return		定数領域のサイズ(個)
	risse_size GetConstsSize() const { return ConstsSize; }

	//! @brief		必要なレジスタ数を得る
	//! @return		必要なレジスタ数
	risse_size GetNumRegs() const { return NumRegs; }

	//! @brief		関数のネストレベルを得る
	//! @return		関数のネストレベル
	risse_size GetNestLevel() const { return NestLevel; }

	//! @brief		関数の最大のネストレベルを得る
	//! @return		関数の最大のネストレベル
	risse_size GetMaxNestLevel() const { return MaxNestLevel; }

	//! @brief		必要な共有変数の数を得る
	//! @return		必要な共有変数の数
	risse_size GetNumSharedVars() const { return NumSharedVars; }

	//! @brief		コード実行クラスのインスタンスを得る
	tRisseCodeExecutor * GetExecutor() const { return Executor; }

	//! @brief		VM コード位置からソースコード上の位置へ変換する
	//! @param		pos			VMコード位置(ワード単位)
	//! @return		ソースコード上の位置
	risse_size CodePositionToSourcePosition(risse_size pos) const;

public: // tRisseObjectInterface メンバ

	tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief 共有変数フレーム
//---------------------------------------------------------------------------
class tRisseSharedVariableFrames : public tRisseCollectee
{
	gc_vector<tRisseVariant *> Frames; //!< 共有変数フレームの配列 (関数のネストレベルによりフレームが異なる)

public:
	//! @brief		コンストラクタ
	//! @param		max_nest_level		最大の関数のネストレベル(このサイズにて Frames が確保される)
	tRisseSharedVariableFrames(risse_size max_nest_level)
	{
		Frames.resize(max_nest_level);
	}

	//! @brief		指定のネストレベルのフレームを確保する
	//! @param		level		ネストレベル
	//! @param		size		フレームのサイズ
	//! @return		そのネストレベルの位置に以前にあったフレーム
	tRisseVariant * Set(risse_size level, risse_size size)
	{
		RISSE_ASSERT(level < Frames.size());
		tRisseVariant * prev = Frames[level];
		Frames[level] = size ? new tRisseVariant[size] : NULL;
		return prev;
	}

	//! @brief		指定のネストレベルのフレームを設定する
	//! @param		level		ネストレベル
	//! @param		frame		フレーム
	//! @return		そのネストレベルの位置に以前にあったフレーム
	tRisseVariant * Set(risse_size level, tRisseVariant * frame)
	{
		RISSE_ASSERT(level < Frames.size());
		tRisseVariant * prev = Frames[level];
		Frames[level] = frame;
		return prev;
	}

	//! @brief		指定位置の共有変数への参照を返す
	//! @param		level		ネストレベル
	//! @param		num			位置
	//! @return		その位置にある共有変数への参照
	tRisseVariant & At(risse_size level, risse_size num)
	{
		RISSE_ASSERT(level < Frames.size());
		RISSE_ASSERT(Frames[level] != NULL);
		return Frames[level][num];
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief コードブロックスタックアダプタ
//! @note	このクラスは tRisseCodeBlock を内部に参照として持つが、
//!			それが実行されるべきスタックフレームや共有フレーム情報をも持つ。
//!			指定のスタックフレームや共有フレームでコードブロックを実行したいときに
//!			このアダプタを利用する。
//!			このインスタンスはコード実行時に動的に作成される。
//---------------------------------------------------------------------------
class tRisseCodeBlockStackAdapter : public tRisseObjectInterface
{
	const tRisseCodeBlock * CodeBlock; //!< コードブロック
	tRisseVariant * Frame; //!< スタックフレーム
	tRisseSharedVariableFrames Shared; //!< 共有フレーム

public:
	//! @brief		コンストラクタ
	//! @param		codeblock		コードブロック
	//! @param		frame			スタックフレーム
	//! @param		shared			共有フレーム
	tRisseCodeBlockStackAdapter(const tRisseCodeBlock * codeblock,
		tRisseVariant * frame , const tRisseSharedVariableFrames & shared):
		 CodeBlock(codeblock), Frame(frame), Shared(shared) {;}

public: // tRisseObjectInterface メンバ

	tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);

	//! @brief	デストラクタ (コンパイラの警告潰し用;実際には呼ばれることはない)
	virtual ~tRisseCodeBlockStackAdapter() {;}
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risse
#endif


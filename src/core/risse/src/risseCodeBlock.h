//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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
class tRisseCodeBlock : public tRisseObjectInterface, public tRisseCollectee
{
	risse_uint32 *Code; //!< コード
	risse_size CodeSize; //!< コードのサイズ(ワード単位)
	tRisseVariant *Consts; //!< 定数領域
	risse_size ConstsSize; //!< 定数領域のサイズ(個)
	risse_size NumRegs; //!< 必要なレジスタ数
	risse_size NumSharedVars; //!< 必要な共有変数の数

	typedef std::pair<risse_size, risse_size> tRelocation; //!<  コードブロック再配置情報のtypedef
	tRelocation * CodeBlockRelocations; //!< コードブロック再配置情報
	risse_size CodeBlockRelocationSize;

	tRelocation * TryIdentifierRelocations; //!< try識別子再配置情報
	risse_size TryIdentifierRelocationSize;

	tRisseCodeExecutor * Executor; //!< コード実行クラスのインスタンス

public:
	//! @brief		コンストラクタ
	tRisseCodeBlock();

	//! @brief		コードを設定する(コードジェネレータから)
	//! @param		gen		コードジェネレータ
	void Assign(const tRisseCodeGenerator *gen);

	//! @brief		再配置情報の fixup を行う
	//! @param		sb		スクリプトブロック
	void Fixup(tRisseScriptBlockBase * sb);

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

	//! @brief		必要な共有変数の数を得る
	//! @return		必要な共有変数の数
	risse_size GetNumSharedVars() const { return NumSharedVars; }

	//! @brief		コード実行クラスのインスタンスを得る
	tRisseCodeExecutor * GetExecutor() const { return Executor; }

public: // tRisseObjectInterface メンバ

	tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------
} // namespace Risse
#endif

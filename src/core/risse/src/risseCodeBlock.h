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

//---------------------------------------------------------------------------
namespace Risse
{
class tRisseCodeGenerator;
//---------------------------------------------------------------------------
//! @brief		コードブロッククラス
//! @note		コードブロックは Risse の実行単位である「関数」などごとに、
//!				VMが実行すべき環境についての情報を保持するクラスである
//---------------------------------------------------------------------------
class tRisseCodeBlock : public tRisseCollectee
{
	risse_uint32 *Code; //!< コード
	risse_size CodeSize; //!< コードのサイズ(ワード単位)
	tRisseVariant *Consts; //!< 定数領域
	risse_size ConstsSize; //!< 定数領域のサイズ(個)
	risse_size NumRegs; //!< 必要なレジスタ数

public:
	//! @brief		コンストラクタ(コードジェネレータから)
	//! @param		gen		コードジェネレータ
	tRisseCodeBlock(const tRisseCodeGenerator *gen);

	//! @brief		内容をダンプ(逆アセンブル)する
	//! @return		ダンプした結果
	tRisseString Dump() const;
};
//---------------------------------------------------------------------------
} // namespace Risse
#endif


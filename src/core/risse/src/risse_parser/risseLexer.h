//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse レキシカル・アナライザ(字句解析器)の実装
//---------------------------------------------------------------------------
#ifndef RisseLexerH
#define RisseLexerH

#include "../risseLexerUtils.h"
#include "../risseCxxString.h"

namespace Risse
{

//---------------------------------------------------------------------------
class tRisseLexer : public tRisseLexerUtility, public gc
{
	tRisseString Script; //!< スクリプト
	const risse_char * Ptr; //!< 解析ポインタ
public:
	//! @brief		コンストラクタ
	//! @param		script		入力スクリプト
	tRisseLexer(const tRisseString & script);

	//! @brief		トークンを得る
	//! @param		val		トークンの値の格納先
	//! @return		トークンID
	int GetToken(tRisseVariant & val);
};
//---------------------------------------------------------------------------

} // namespace Risse

#endif


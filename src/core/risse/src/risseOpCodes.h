//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オペレーションコード定義
//---------------------------------------------------------------------------
#ifndef risseOpeCodesH
#define risseOpeCodesH

#include "risseGC.h"
#include "risseCharUtils.h"
#include "risseTypes.h"
//---------------------------------------------------------------------------

/*
	#undef risseOpeCodesH
	してから #define RISSE_OC_DEFINE_NAMES
	してこのファイルをもう一度 include すると、各 enum に
	対応した文字列の表を得ることができる。
*/

#ifdef RISSE_OC_ENUM_DEF
	#undef RISSE_OC_ENUM_DEF
#endif

#ifdef RISSE_OC_ENUM_ITEM
	#undef RISSE_OC_ENUM_ITEM
#endif

#ifdef RISSE_OC_ENUM_END
	#undef RISSE_OC_ENUM_END
#endif


#ifndef RISSE_OC_DEFINE_NAMES
	#define RISSE_OC_ENUM_DEF() enum tRisseOpCodeType {
	#define RISSE_OC_ENUM_ITEM(X) oc##X,
	#define RISSE_OC_ENUM_END };
#else
	#define RISSE_OC_ENUM_DEF() static const char * RisseOpCodesNames[] = {
	#define RISSE_OC_ENUM_ITEM(X) #X,
	#define RISSE_OC_ENUM_END };
#endif


//---------------------------------------------------------------------------
RISSE_OC_ENUM_DEF()
// 代入
	RISSE_OC_ENUM_ITEM(Assign			)		//!< = (ローカル変数の代入)

// 可変引数
	RISSE_OC_ENUM_ITEM(FuncCall			)		//!< function call
	RISSE_OC_ENUM_ITEM(New				)		//!< "new"

// 引数なし
	RISSE_OC_ENUM_ITEM(LogNot			)		//!< "!" logical not
	RISSE_OC_ENUM_ITEM(BitNot			)		//!< "~" bit not
	RISSE_OC_ENUM_ITEM(Dec				)		//!< "--" decrement
	RISSE_OC_ENUM_ITEM(Inc				)		//!< "++" increment
	RISSE_OC_ENUM_ITEM(Plus				)		//!< "+"
	RISSE_OC_ENUM_ITEM(Minus			)		//!< "-"

// 引数一つ
	RISSE_OC_ENUM_ITEM(LogOr			)		//!< ||
	RISSE_OC_ENUM_ITEM(LogAnd			)		//!< &&
	RISSE_OC_ENUM_ITEM(BitOr			)		//!< |
	RISSE_OC_ENUM_ITEM(BitXor			)		//!< ^
	RISSE_OC_ENUM_ITEM(BitAnd			)		//!< &
	RISSE_OC_ENUM_ITEM(NotEqual			)		//!< !=
	RISSE_OC_ENUM_ITEM(Equal			)		//!< ==
	RISSE_OC_ENUM_ITEM(DiscNotEqual		)		//!< !==
	RISSE_OC_ENUM_ITEM(DiscEqual		)		//!< ===
	RISSE_OC_ENUM_ITEM(Lesser			)		//!< <
	RISSE_OC_ENUM_ITEM(Greater			)		//!< >
	RISSE_OC_ENUM_ITEM(LesserOrEqual	)		//!< <=
	RISSE_OC_ENUM_ITEM(GreaterOrEqual	)		//!< >=
	RISSE_OC_ENUM_ITEM(RBitShift		)		//!< >>>
	RISSE_OC_ENUM_ITEM(LShift			)		//!< <<
	RISSE_OC_ENUM_ITEM(RShift			)		//!< >>
	RISSE_OC_ENUM_ITEM(Mod				)		//!< %
	RISSE_OC_ENUM_ITEM(Div				)		//!< /
	RISSE_OC_ENUM_ITEM(Idiv				)		//!< \ (integer div)
	RISSE_OC_ENUM_ITEM(Mul				)		//!< *
	RISSE_OC_ENUM_ITEM(Add				)		//!< +
	RISSE_OC_ENUM_ITEM(Sub				)		//!< -
	RISSE_OC_ENUM_ITEM(IncontextOf		)		//!< incontextof

	RISSE_OC_ENUM_ITEM(DirectGet		)		//!< get .
	RISSE_OC_ENUM_ITEM(IndirectGet		)		//!< get [ ]

	RISSE_OC_ENUM_ITEM(DirectSet		)		//!< set .
	RISSE_OC_ENUM_ITEM(IndirectSet		)		//!< set [ ]

	RISSE_OC_ENUM_ITEM(DirectDelete		)		//!< delete .
	RISSE_OC_ENUM_ITEM(IndirectDelete	)		//!< delete [ ]

RISSE_OC_ENUM_END
//---------------------------------------------------------------------------
} // namespace Risse
#endif


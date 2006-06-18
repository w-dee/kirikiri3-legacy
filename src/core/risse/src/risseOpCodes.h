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
#ifndef risseOpCodesH
#define risseOpCodesH

#include "risseTypes.h"
#include "risseCharUtils.h"
//---------------------------------------------------------------------------

namespace Risse
{
//! @brief オペーレーションコードの名前の配列
extern const char * RisseOpCodeNames[];


/*
	#undef risseOpCodesH
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


#ifdef RISSE_OC_DEFINE_NAMES
	#define RISSE_OC_ENUM_DEF() const char * RisseOpCodeNames[] = {
	#define RISSE_OC_ENUM_ITEM(X) #X,
	#define RISSE_OC_ENUM_END };
#else
	#define RISSE_OC_ENUM_DEF() enum tRisseOpCode {
	#define RISSE_OC_ENUM_ITEM(X) oc##X,
	#define RISSE_OC_ENUM_END };
#endif


//---------------------------------------------------------------------------
RISSE_OC_ENUM_DEF()
// なにもしない
	RISSE_OC_ENUM_ITEM(NoOperation		)		//!< なにもしない

// 代入
	RISSE_OC_ENUM_ITEM(Assign			)		//!< = (ローカル変数の代入)
	RISSE_OC_ENUM_ITEM(AssignConstant	)		//!< = 定数の代入
	RISSE_OC_ENUM_ITEM(AssignThis		)		//!< = thisの代入
	RISSE_OC_ENUM_ITEM(AssignSuper		)		//!< = superの代入
	RISSE_OC_ENUM_ITEM(AssignGlobal		)		//!< = globalの代入
	RISSE_OC_ENUM_ITEM(AssignNewArray	)		//!< = 新しい配列オブジェクトの代入
	RISSE_OC_ENUM_ITEM(AssignNewDict	)		//!< = 新しい辞書配列オブジェクトの代入
	RISSE_OC_ENUM_ITEM(AssignNewRegExp	)		//!< = 新しい正規表現オブジェクトの代入 (引数2つ)

// 可変引数
	RISSE_OC_ENUM_ITEM(FuncCall			)		//!< function call
	RISSE_OC_ENUM_ITEM(New				)		//!< "new"
	RISSE_OC_ENUM_ITEM(Phi				)		//!< φ関数

// ジャンプ/分岐/制御/補助
	RISSE_OC_ENUM_ITEM(Jump				)		//!< 単純なジャンプ
	RISSE_OC_ENUM_ITEM(Branch			)		//!< 分岐
	RISSE_OC_ENUM_ITEM(Debugger			)		//!< debugger ステートメント
	RISSE_OC_ENUM_ITEM(Throw			)		//!< throw ステートメント
	RISSE_OC_ENUM_ITEM(Return			)		//!< return ステートメント
	RISSE_OC_ENUM_ITEM(DefineLazyBlock	)		//!< 遅延評価ブロックの定義

// 名前空間
	RISSE_OC_ENUM_ITEM(ParentWrite		)		//!< 親名前空間への書き込み
	RISSE_OC_ENUM_ITEM(ParentRead		)		//!< 親名前空間からの読み込み

// 引数1+なし
	RISSE_OC_ENUM_ITEM(LogNot			)		//!< "!" logical not
	RISSE_OC_ENUM_ITEM(BitNot			)		//!< "~" bit not
	RISSE_OC_ENUM_ITEM(DecAssign		)		//!< "--" decrement
	RISSE_OC_ENUM_ITEM(IncAssign		)		//!< "++" increment
	RISSE_OC_ENUM_ITEM(Plus				)		//!< "+"
	RISSE_OC_ENUM_ITEM(Minus			)		//!< "-"

// 引数1+1つ
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

	RISSE_OC_ENUM_ITEM(DGet				)		//!< get .  
	RISSE_OC_ENUM_ITEM(IGet				)		//!< get [ ]

	RISSE_OC_ENUM_ITEM(BitAndAssign		)		//!< &=
	RISSE_OC_ENUM_ITEM(BitOrAssign		)		//!< |=
	RISSE_OC_ENUM_ITEM(BitXorAssign		)		//!< ^=
	RISSE_OC_ENUM_ITEM(SubAssign		)		//!< -=
	RISSE_OC_ENUM_ITEM(AddAssign		)		//!< +=
	RISSE_OC_ENUM_ITEM(ModAssign		)		//!< %=
	RISSE_OC_ENUM_ITEM(DivAssign		)		//!< /=
	RISSE_OC_ENUM_ITEM(IdivAssign		)		//!< \=
	RISSE_OC_ENUM_ITEM(MulAssign		)		//!< *=
	RISSE_OC_ENUM_ITEM(LogOrAssign		)		//!< ||=
	RISSE_OC_ENUM_ITEM(LogAndAssign		)		//!< &&=
	RISSE_OC_ENUM_ITEM(RBitShiftAssign	)		//!< >>>=
	RISSE_OC_ENUM_ITEM(LShiftAssign		)		//!< <<=
	RISSE_OC_ENUM_ITEM(RShiftAssign		)		//!< >>=


	RISSE_OC_ENUM_ITEM(DDelete			)		//!< delete .
	RISSE_OC_ENUM_ITEM(IDelete			)		//!< delete [ ]

// 引数1+2つ
	RISSE_OC_ENUM_ITEM(DSet				)		//!< set .
	RISSE_OC_ENUM_ITEM(ISet				)		//!< set [ ]

RISSE_OC_ENUM_END
//---------------------------------------------------------------------------
} // namespace Risse
#endif


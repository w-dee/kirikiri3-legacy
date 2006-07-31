//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バイトコード実行クラスのインターフェースとインタプリタの実装
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseCodeExecutor.h"
#include "risseOpCodes.h"
/*
	このソースは、実行スピード重視の、いわばダーティーな実装を行う。
	ダーティーな実装は極力コメントを残し、わかりやすくしておくこと。
*/


namespace Risse
{
/*
スタックフレームと定数領域へのアクセスなど以下のマクロを使うこと。
frame[num] のように書くと num に毎回 sizeof(frame[0]) の乗算が発生するため、
将来的に、あらかじめ num は乗算を済ましておき、
*(tRisseVariant*)((risse_uint8*)frame + (num)) のようなマクロに置き換える
可能性がある。
*/
//! @brief		スタックフレームにアクセス
#define AR(num) (frame[(num)])
//! @brief		定数領域にアクセス
#define AC(num) (consts[(num)])
//! @brief		レジスタのオペランド -> レジスタ/定数インデックスへの変換
#define CI(num) (num)

//---------------------------------------------------------------------------
RISSE_DEFINE_SOURCE_ID(38733,31388,53292,19613,29887,64791,9160,61431);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseCodeInterpreter::tRisseCodeInterpreter(tRisseCodeBlock *cb) :
	tRisseCodeExecutor(cb)
{
	
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeInterpreter::Execute(const tRisseVariant & this_obj,
	tRisseVariant * result)
{
	// スタックを割り当てる
	// TODO: スタックフレームの再利用など
	// 毎回スタックを new で割り当てるのは効率が悪い？
	tRisseVariant * frame = new tRisseVariant[CodeBlock->GetNumRegs()];


	// ローカル変数に値を持ってくる
	// いくつかのローカル変数は ASSERT が有効になっていなければ
	// 必要ないので、#ifdef ～ #endif で場合分けをする。
#ifdef RISSE_ASSERT_ENABLED
	risse_size framesize = CodeBlock->GetNumRegs();
#endif
	const risse_uint32 * code = CodeBlock->GetCode();
#ifdef RISSE_ASSERT_ENABLED
	const risse_uint32 * code_origin = CodeBlock->GetCode();
	risse_size codesize = CodeBlock->GetCodeSize();
#endif
	const tRisseVariant * consts = CodeBlock->GetConsts();
#ifdef RISSE_ASSERT_ENABLED
	risse_size constssize = CodeBlock->GetConstsSize();
#endif

	// ループ
	while(true)
	{
		switch(*code)
		{
		case ocNoOperation	: // nop	 なにもしない
			code += 1;
			break;

		case ocAssign			: // cp		 = (ローカル変数の代入
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			AR(code[1]) = AR(code[2]);
			code += 3;
			break;

		case ocAssignConstant	: // const	 = 定数の代入
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < constssize);
			AR(code[1]) = AC(code[2]);
			code += 3;
			break;

		case ocAssignThis		: // this	 = thisの代入
			/* incomplete */
			RISSE_ASSERT(CI(code[1]) < framesize);
			code += 2;
			break;

		case ocAssignSuper	: // super	 = superの代入
			/* incomplete */
			RISSE_ASSERT(CI(code[1]) < framesize);
			code += 2;
			break;

		case ocAssignGlobal	: // global	 = globalの代入
			/* incomplete */
			RISSE_ASSERT(CI(code[1]) < framesize);
			code += 2;
			break;

		case ocAssignNewArray	: // array	 = 新しい配列オブジェクトの代入
			/* incomplete */
			RISSE_ASSERT(CI(code[1]) < framesize);
			code += 2;
			break;

		case ocAssignNewDict	: // dict	 = 新しい辞書配列オブジェクトの代入
			/* incomplete */
			RISSE_ASSERT(CI(code[1]) < framesize);
			code += 2;
			break;

		case ocAssignNewRegExp: // regexp	 = 新しい正規表現オブジェクトの代入 (引数2つ
			/* incomplete */
			RISSE_ASSERT(CI(code[1]) < framesize);
			code += 2;
			break;

		case ocFuncCall		: // call	 function call
			/* incomplete */
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			code += code[4] + 5;
			break;

		case ocNew			: // new	 "new"
			/* incomplete */
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			code += code[4] + 5;
			break;

		case ocFuncCallBlock	: // callb	 function call with lazyblock
			/* incomplete */
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			code += code[4] + code[5] + 6;
			break;

		case ocJump			: // jump	 単純なジャンプ
			// アドレスはジャンプコードの開始番地に対する相対指定
			code += static_cast<risse_int32>(code[1]);
			break;

		case ocBranch			: // branch	 分岐
			RISSE_ASSERT(CI(code[1]) < framesize);
			if((bool)AC(code[1]))
				code += static_cast<risse_int32>(code[2]);
			else
				code += static_cast<risse_int32>(code[3]);
			break;

		case ocDebugger		: // dbg	 debugger ステートメント
			/* incomplete */
			code += 1;
			break;

		case ocThrow			: // throw	 throw ステートメント
			/* incomplete */
			RISSE_ASSERT(CI(code[1]) < framesize);
			code += 2;
			break;

		case ocReturn			: // ret	 return ステートメント
			/* incomplete */
			RISSE_ASSERT(CI(code[1]) < framesize);
			code += 2;
			return;

		case ocLogNot			: // lnot	 "!" logical not
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			AR(code[1]) = !AR(code[2]);
			code += 3;
			break;

		case ocBitNot			: // bnot	 "~" bit not
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			AR(code[1]) = ~AR(code[2]);
			code += 3;
			break;

		case ocDecAssign		: // ERR	 "--" decrement
			/* incomplete */
			code += 3;
			break;

		case ocIncAssign		: // ERR	 "++" increment
			/* incomplete */
			code += 3;
			break;

		case ocPlus			: // plus	 "+"
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			AR(code[1]) = +AR(code[2]);
			code += 3;
			break;

		case ocMinus			: // minus	 "-"
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			AR(code[1]) = -AR(code[2]);
			code += 3;
			break;

		case ocLogOr			: // lor	 ||
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) || AR(code[3]);
			code += 4;
			break;

		case ocLogAnd			: // land	 &&
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) && AR(code[3]);
			code += 4;
			break;

		case ocBitOr			: // bor	 |
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) | AR(code[3]);
			code += 4;
			break;

		case ocBitXor			: // bxor	 ^
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) ^ AR(code[3]);
			code += 4;
			break;

		case ocBitAnd			: // band	 &
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) & AR(code[3]);
			code += 4;
			break;

		case ocNotEqual		: // ne		 !=
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) != AR(code[3]);
			code += 4;
			break;

		case ocEqual			: // eq		 ==
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) == AR(code[3]);
			code += 4;
			break;

		case ocDiscNotEqual	: // dne	 !==
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]).DiscNotEqual(AR(code[3]));
			code += 4;
			break;

		case ocDiscEqual		: // deq	 ===
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]).DiscEqual(AR(code[3]));
			code += 4;
			break;

		case ocLesser			: // lt		 <
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) < AR(code[3]);
			code += 4;
			break;

		case ocGreater		: // gt		 >
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) > AR(code[3]);
			code += 4;
			break;

		case ocLesserOrEqual	: // lte	 <=
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) <= AR(code[3]);
			code += 4;
			break;

		case ocGreaterOrEqual	: // gte	 >=
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) >= AR(code[3]);
			code += 4;
			break;

		case ocRBitShift		: // rbs	 >>>
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]).RBitShift(AR(code[3]));
			code += 4;
			break;

		case ocLShift			: // ls		 <<
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) << AR(code[3]);
			code += 4;
			break;

		case ocRShift			: // rs		 >>
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) >> AR(code[3]);
			code += 4;
			break;

		case ocMod			: // mod	 %
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) % AR(code[3]);
			code += 4;
			break;

		case ocDiv			: // div	 /
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) / AR(code[3]);
			code += 4;
			break;

		case ocIdiv			: // idiv	 \ (integer div)
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]).Idiv(AR(code[3]));
			code += 4;
			break;

		case ocMul			: // mul	 *
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) * AR(code[3]);
			code += 4;
			break;

		case ocAdd			: // add	 +
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) + AR(code[3]);
			code += 4;
			break;

		case ocSub			: // sub	 -
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			AR(code[1]) = AR(code[2]) - AR(code[3]);
			code += 4;
			break;

		case ocIncontextOf	: // chgc	 incontextof
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			/* incomplete */
			code += 4;
			break;

		case ocDGet			: // dget	 get .  
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			/* incomplete */
			code += 4;
			break;

		case ocIGet			: // iget	 get [ ]
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			/* incomplete */
			code += 4;
			break;

		case ocDDelete		: // ddel	 delete .
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			/* incomplete */
			code += 4;
			break;

		case ocIDelete		: // idel	 delete [ ]
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			/* incomplete */
			code += 4;
			break;

		case ocDSet			: // dset	 set .
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			/* incomplete */
			code += 4;
			break;

		case ocISet			: // iset	 set [ ]
			RISSE_ASSERT(CI(code[1]) < framesize);
			RISSE_ASSERT(CI(code[2]) < framesize);
			RISSE_ASSERT(CI(code[3]) < framesize);
			/* incomplete */
			code += 4;
			break;

		default:
			// TODO: 本当はASSERTではなくて例外を発生した方がいい
			RISSE_ASSERT(!"unknown instruction code");
		}
		RISSE_ASSERT((risse_size)(code - code_origin) < codesize);
	}

}
//---------------------------------------------------------------------------


} // namespace Risse

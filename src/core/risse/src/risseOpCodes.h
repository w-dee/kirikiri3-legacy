//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オペレーションコード/VM命令定義
//---------------------------------------------------------------------------
#ifndef risseOpCodesH
#define risseOpCodesH

#include "risseTypes.h"
#include "risseCharUtils.h"
#include "risseGC.h"
#include "risseCxxString.h"
//---------------------------------------------------------------------------

namespace Risse
{
#ifndef RISSE_OC_DEFINE_INFO

//---------------------------------------------------------------------------
// 各種定数
//---------------------------------------------------------------------------
static const int RisseMaxVMInsnOperand = 5;
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		VM命令の情報を表す構造体
//---------------------------------------------------------------------------
struct tRisseVMInsnInfo
{
	//! @brief		tRisseVMInsnInfoで使用されるフラグ
	enum tInsnFlag
	{
		vifVoid =0, 	//!< V このオペランドは存在しない
		vifRegister,	//!< R このオペランドはレジスタを表している
		vifConstant,	//!< C このオペランドは定数を表している
		vifNumber,		//!< N このオペランドは(すべての必須オペランドが終了し
						//!<   た後に続く)追加オペランドの個数を表している
		vifAddress,		//!< A このオペランドはアドレスを表している
		vifSomething,	//!< S このオペランドはその他の何かを表している
	};

	const char * Name;								//!< 命令名
	const char * Mnemonic;							//!< ニーモニック
	tInsnFlag Flags[RisseMaxVMInsnOperand];	//!< オペランドnに対するフラグ
};
//---------------------------------------------------------------------------
#endif //#ifndef RISSE_OC_DEFINE_INFO
#ifdef RISSE_OC_DEFINE_INFO
	#ifdef R
		#undef R
	#endif
	#define		R		tRisseVMInsnInfo::vifRegister
	#ifdef C
		#undef C
	#endif
	#define		C		tRisseVMInsnInfo::vifConstant
	#ifdef N
		#undef N
	#endif
	#define		N		tRisseVMInsnInfo::vifNumber
	#ifdef A
		#undef A
	#endif
	#define		A		tRisseVMInsnInfo::vifAddress
	#ifdef S
		#undef S
	#endif
	#define		S		tRisseVMInsnInfo::vifSomething
	#ifdef P
		#undef P
	#endif
	#define P(a,b,c,d,e) {\
		(tRisseVMInsnInfo::tInsnFlag)(a),\
		(tRisseVMInsnInfo::tInsnFlag)(b),\
		(tRisseVMInsnInfo::tInsnFlag)(c),\
		(tRisseVMInsnInfo::tInsnFlag)(d),\
		(tRisseVMInsnInfo::tInsnFlag)(e) }
#endif
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief オペーレーションコードの名前の配列
//---------------------------------------------------------------------------
extern const tRisseVMInsnInfo RisseVMInsnInfo[];
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
/*
	#undef risseOpCodesH
	してから #define RISSE_OC_DEFINE_INFO
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


#ifdef RISSE_OC_DEFINE_INFO
	#define RISSE_OC_ENUM_DEF() const tRisseVMInsnInfo RisseVMInsnInfo[] = {
	#define RISSE_OC_ENUM_ITEM(X,Y,Z) {#X,#Y,Z},
	#define RISSE_OC_ENUM_END };

#else
	#define RISSE_OC_ENUM_DEF() enum tRisseOpCode {
	#define RISSE_OC_ENUM_ITEM(X,Y,Z) oc##X,
	#define RISSE_OC_ENUM_END };
#endif


//---------------------------------------------------------------------------
RISSE_OC_ENUM_DEF()
//						LongId			Mnemonic	 O1O2O3O4O5 operands

// なにもしない
	RISSE_OC_ENUM_ITEM(NoOperation		, nop		,P(0,0,0,0,0))//!< なにもしない

// 代入
	RISSE_OC_ENUM_ITEM(Assign			, cp		,P(R,R,0,0,0))//!< = (ローカル変数の代入)
	RISSE_OC_ENUM_ITEM(AssignConstant	, const		,P(R,C,0,0,0))//!< = 定数の代入
	RISSE_OC_ENUM_ITEM(AssignThis		, this		,P(R,0,0,0,0))//!< = thisの代入
	RISSE_OC_ENUM_ITEM(AssignSuper		, super		,P(R,0,0,0,0))//!< = superの代入
	RISSE_OC_ENUM_ITEM(AssignGlobal		, global	,P(R,0,0,0,0))//!< = globalの代入
	RISSE_OC_ENUM_ITEM(AssignNewArray	, array		,P(R,0,0,0,0))//!< = 新しい配列オブジェクトの代入
	RISSE_OC_ENUM_ITEM(AssignNewDict	, dict		,P(R,0,0,0,0))//!< = 新しい辞書配列オブジェクトの代入
	RISSE_OC_ENUM_ITEM(AssignNewRegExp	, regexp	,P(R,0,0,0,0))//!< = 新しい正規表現オブジェクトの代入 (引数2つ)

	RISSE_OC_ENUM_ITEM(FuncCall			, call		,P(R,S,N,0,0))//!< function call
	RISSE_OC_ENUM_ITEM(New				, new		,P(R,S,N,0,0))//!< "new"
	RISSE_OC_ENUM_ITEM(FuncCallBlock	, callb		,P(R,S,N,N,0))//!< function call with lazyblock

// ジャンプ/分岐/制御/補助
	RISSE_OC_ENUM_ITEM(Jump				, jump		,P(A,0,0,0,0))//!< 単純なジャンプ
	RISSE_OC_ENUM_ITEM(Branch			, branch	,P(R,A,A,0,0))//!< 分岐
	RISSE_OC_ENUM_ITEM(Debugger			, dbg		,P(0,0,0,0,0))//!< debugger ステートメント
	RISSE_OC_ENUM_ITEM(Throw			, throw		,P(R,0,0,0,0))//!< throw ステートメント
	RISSE_OC_ENUM_ITEM(Return			, ret		,P(R,0,0,0,0))//!< return ステートメント

// 引数1+なし
	RISSE_OC_ENUM_ITEM(LogNot			, lnot		,P(R,R,0,0,0))//!< "!" logical not
	RISSE_OC_ENUM_ITEM(BitNot			, bnot		,P(R,R,0,0,0))//!< "~" bit not
	RISSE_OC_ENUM_ITEM(DecAssign		, ERR		,P(0,0,0,0,0))//!< "--" decrement
	RISSE_OC_ENUM_ITEM(IncAssign		, ERR		,P(0,0,0,0,0))//!< "++" increment
	RISSE_OC_ENUM_ITEM(Plus				, plus		,P(R,R,0,0,0))//!< "+"
	RISSE_OC_ENUM_ITEM(Minus			, minus		,P(R,R,0,0,0))//!< "-"

// 引数1+1つ
	RISSE_OC_ENUM_ITEM(LogOr			, lor		,P(R,R,R,0,0))//!< ||
	RISSE_OC_ENUM_ITEM(LogAnd			, land		,P(R,R,R,0,0))//!< &&
	RISSE_OC_ENUM_ITEM(BitOr			, bor		,P(R,R,R,0,0))//!< |
	RISSE_OC_ENUM_ITEM(BitXor			, bxor		,P(R,R,R,0,0))//!< ^
	RISSE_OC_ENUM_ITEM(BitAnd			, band		,P(R,R,R,0,0))//!< &
	RISSE_OC_ENUM_ITEM(NotEqual			, ne		,P(R,R,R,0,0))//!< !=
	RISSE_OC_ENUM_ITEM(Equal			, eq		,P(R,R,R,0,0))//!< ==
	RISSE_OC_ENUM_ITEM(DiscNotEqual		, dne		,P(R,R,R,0,0))//!< !==
	RISSE_OC_ENUM_ITEM(DiscEqual		, deq		,P(R,R,R,0,0))//!< ===
	RISSE_OC_ENUM_ITEM(Lesser			, lt		,P(R,R,R,0,0))//!< <
	RISSE_OC_ENUM_ITEM(Greater			, gt		,P(R,R,R,0,0))//!< >
	RISSE_OC_ENUM_ITEM(LesserOrEqual	, lte		,P(R,R,R,0,0))//!< <=
	RISSE_OC_ENUM_ITEM(GreaterOrEqual	, gte		,P(R,R,R,0,0))//!< >=
	RISSE_OC_ENUM_ITEM(RBitShift		, rbs		,P(R,R,R,0,0))//!< >>>
	RISSE_OC_ENUM_ITEM(LShift			, ls		,P(R,R,R,0,0))//!< <<
	RISSE_OC_ENUM_ITEM(RShift			, rs		,P(R,R,R,0,0))//!< >>
	RISSE_OC_ENUM_ITEM(Mod				, mod		,P(R,R,R,0,0))//!< %
	RISSE_OC_ENUM_ITEM(Div				, div		,P(R,R,R,0,0))//!< /
	RISSE_OC_ENUM_ITEM(Idiv				, idiv		,P(R,R,R,0,0))//!< \ (integer div)
	RISSE_OC_ENUM_ITEM(Mul				, mul		,P(R,R,R,0,0))//!< *
	RISSE_OC_ENUM_ITEM(Add				, add		,P(R,R,R,0,0))//!< +
	RISSE_OC_ENUM_ITEM(Sub				, sub		,P(R,R,R,0,0))//!< -
	RISSE_OC_ENUM_ITEM(IncontextOf		, chgc		,P(R,R,R,0,0))//!< incontextof

	RISSE_OC_ENUM_ITEM(DGet				, dget		,P(R,R,R,0,0))//!< get .  
	RISSE_OC_ENUM_ITEM(IGet				, iget		,P(R,R,R,0,0))//!< get [ ]

	RISSE_OC_ENUM_ITEM(BitAndAssign		, ERR		,P(0,0,0,0,0))//!< &=
	RISSE_OC_ENUM_ITEM(BitOrAssign		, ERR		,P(0,0,0,0,0))//!< |=
	RISSE_OC_ENUM_ITEM(BitXorAssign		, ERR		,P(0,0,0,0,0))//!< ^=
	RISSE_OC_ENUM_ITEM(SubAssign		, ERR		,P(0,0,0,0,0))//!< -=
	RISSE_OC_ENUM_ITEM(AddAssign		, ERR		,P(0,0,0,0,0))//!< +=
	RISSE_OC_ENUM_ITEM(ModAssign		, ERR		,P(0,0,0,0,0))//!< %=
	RISSE_OC_ENUM_ITEM(DivAssign		, ERR		,P(0,0,0,0,0))//!< /=
	RISSE_OC_ENUM_ITEM(IdivAssign		, ERR		,P(0,0,0,0,0))//!< \=
	RISSE_OC_ENUM_ITEM(MulAssign		, ERR		,P(0,0,0,0,0))//!< *=
	RISSE_OC_ENUM_ITEM(LogOrAssign		, ERR		,P(0,0,0,0,0))//!< ||=
	RISSE_OC_ENUM_ITEM(LogAndAssign		, ERR		,P(0,0,0,0,0))//!< &&=
	RISSE_OC_ENUM_ITEM(RBitShiftAssign	, ERR		,P(0,0,0,0,0))//!< >>>=
	RISSE_OC_ENUM_ITEM(LShiftAssign		, ERR		,P(0,0,0,0,0))//!< <<=
	RISSE_OC_ENUM_ITEM(RShiftAssign		, ERR		,P(0,0,0,0,0))//!< >>=


	RISSE_OC_ENUM_ITEM(DDelete			, ddel		,P(R,R,R,0,0))//!< delete .
	RISSE_OC_ENUM_ITEM(IDelete			, idel		,P(R,R,R,0,0))//!< delete [ ]

// 引数1+2つ
	RISSE_OC_ENUM_ITEM(DSet				, dset		,P(R,R,R,0,0))//!< set .
	RISSE_OC_ENUM_ITEM(ISet				, iset		,P(R,R,R,0,0))//!< set [ ]


// SSA形式特有
	RISSE_OC_ENUM_ITEM(Phi				, ERR		,P(0,0,0,0,0))//!< φ関数
	RISSE_OC_ENUM_ITEM(DefineLazyBlock	, ERR		,P(0,0,0,0,0))//!< 遅延評価ブロックの定義
	RISSE_OC_ENUM_ITEM(ParentWrite		, ERR		,P(0,0,0,0,0))//!< 親名前空間への書き込み
	RISSE_OC_ENUM_ITEM(ParentRead		, ERR		,P(0,0,0,0,0))//!< 親名前空間からの読み込み
	RISSE_OC_ENUM_ITEM(ChildWrite		, ERR		,P(0,0,0,0,0))//!< 子名前空間への書き込み
	RISSE_OC_ENUM_ITEM(ChildRead		, ERR		,P(0,0,0,0,0))//!< 子名前空間からの読み込み

RISSE_OC_ENUM_END
//---------------------------------------------------------------------------




#ifndef RISSE_OC_DEFINE_INFO
//---------------------------------------------------------------------------
//! @brief		VMコード用イテレータ
//---------------------------------------------------------------------------
class tRisseVMCodeIterator : public tRisseCollectee
{
	const risse_uint32 *CodePointer; //!< コードへのポインタ
	risse_size Address; //!< 論理アドレス(risse_size_maxの場合は論理アドレス指定無し)

public:
	//! @brief		コンストラクタ
	//! @param		codepointer	コードへのポインタ
	//! @param		address		論理アドレス
	tRisseVMCodeIterator(const risse_uint32 *codepointer, risse_size address = risse_size_max)
	{
		CodePointer = codepointer;
		Address = address;
	}

	//! @brief		コピーコンストラクタ
	//! @param		ref			コピーもとオブジェクト
	tRisseVMCodeIterator(const tRisseVMCodeIterator & ref)
	{
		CodePointer = ref.CodePointer;
		Address = ref.Address;
	}

	//! @brief		コードポインタの代入
	//! @param		codepointer	コードポインタ
	//! @return		このオブジェクトへの参照
	//! @note		論理アドレスは「指定無し」にリセットされる。
	//!				論理アドレスもともに指定したい場合は SetCodePointer() を使うこと
	tRisseVMCodeIterator & operator = (const risse_uint32 *codepointer)
	{
		CodePointer = codepointer;
		Address = risse_size_max;
		return *this;
	}

	//! @brief		コードポインタへの変換
	operator const risse_uint32 *() const { return CodePointer; }

	//! @brief		コードポインタを設定する
	//! @param		codepointer	コードへのポインタ
	//! @param		address		論理アドレス
	void SetCodePointer(const risse_uint32 *codepointer, risse_size address = risse_size_max)
	{
		CodePointer = codepointer;
		Address = address;
	}

	//! @brief		コードポインタを取得する
	//! @return		コードポインタ
	const risse_uint32 * GetCodePointer() const { return CodePointer; }

	//! @brief		論理アドレスを設定する
	//! @param		address		論理アドレス
	void SetAddress(risse_size address) { Address = address; }

	//! @brief		論理アドレスを取得する
	risse_size GetAddress() const { return Address; }

	//! @brief		前置インクリメント演算子
	void operator ++()
	{
		risse_size size = GetInsnSize();
		CodePointer += size;
		if(Address != risse_size_max) Address += size;
	}

	//! @brief		このイテレータの示す命令のサイズをVMワード単位で得る
	//! @return		命令のサイズ
	risse_size GetInsnSize() const;

	//! @brief		このイテレータの示す命令をダンプ(逆アセンブル)する
	//! @return		ダンプ結果
	tRisseString Dump() const;
};
//---------------------------------------------------------------------------
#endif //#ifndef RISSE_OC_DEFINE_INFO

} // namespace Risse
#endif


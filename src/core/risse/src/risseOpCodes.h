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
#include "risseString.h"
//---------------------------------------------------------------------------

namespace Risse
{
#ifndef RISSE_OC_DEFINE_INFO

//---------------------------------------------------------------------------
// 各種定数
//---------------------------------------------------------------------------
static const int RisseMaxVMInsnOperand = 6;
	//!< 命令のオペランドの最大数(ただし可変オペランド部分をのぞく)
static const risse_uint32 RisseFuncCallFlag_Omitted = 0x80000000L;
	//!< call などのフラグの定数 - 関数呼び出しは ... を伴っているか (引数省略されているか)
static const risse_uint32 RisseInvalidRegNum = (risse_uint32)0xffffffff;
	//!< 無効なレジスタを表す値
static const risse_size RisseMaxArgCount = 30;
	//!< 引数の最大個数
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
		vifParameter,	//!< A このオペランドは関数へのパラメータを表している
		vifShared,		//!< S このオペランドは共有空間の変数を表している
		vifOthers,		//!< O このオペランドはその他の何かを表している
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
	#ifdef P
		#undef P
	#endif
	#define		P		tRisseVMInsnInfo::vifParameter
	#ifdef S
		#undef S
	#endif
	#define		S		tRisseVMInsnInfo::vifShared
	#ifdef O
		#undef O
	#endif
	#define		O		tRisseVMInsnInfo::vifOthers
	#ifdef Z
		#undef Z
	#endif
	#define Z(a,b,c,d,e,f) {\
		(tRisseVMInsnInfo::tInsnFlag)(a),\
		(tRisseVMInsnInfo::tInsnFlag)(b),\
		(tRisseVMInsnInfo::tInsnFlag)(c),\
		(tRisseVMInsnInfo::tInsnFlag)(d),\
		(tRisseVMInsnInfo::tInsnFlag)(e),\
		(tRisseVMInsnInfo::tInsnFlag)(f) }
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
/*
	ニーモニックに関するメモ:
	8 文字に納める
	人間がタイプするものではないので無理な省略はしない
*/
//---------------------------------------------------------------------------
RISSE_OC_ENUM_DEF()
//						LongId			Mnemonic  O1O2O3O4O5O6 operands

// なにもしない
	RISSE_OC_ENUM_ITEM(NoOperation		,nop	,Z(0,0,0,0,0,0))//!< なにもしない

// 代入
	RISSE_OC_ENUM_ITEM(Assign			,copy	,Z(R,R,0,0,0,0))//!< = (ローカル変数の代入)
	RISSE_OC_ENUM_ITEM(AssignConstant	,const	,Z(R,C,0,0,0,0))//!< = 定数の代入
	RISSE_OC_ENUM_ITEM(AssignThis		,this	,Z(R,0,0,0,0,0))//!< = thisの代入
	RISSE_OC_ENUM_ITEM(AssignSuper		,super	,Z(R,0,0,0,0,0))//!< = superの代入
	RISSE_OC_ENUM_ITEM(AssignGlobal		,global	,Z(R,0,0,0,0,0))//!< = globalの代入
	RISSE_OC_ENUM_ITEM(AssignNewArray	,array	,Z(R,0,0,0,0,0))//!< = 新しい配列オブジェクトの代入
	RISSE_OC_ENUM_ITEM(AssignNewDict	,dict	,Z(R,0,0,0,0,0))//!< = 新しい辞書配列オブジェクトの代入
	RISSE_OC_ENUM_ITEM(AssignNewRegExp	,regexp	,Z(R,0,0,0,0,0))//!< = 新しい正規表現オブジェクトの代入 (引数2つ)
	RISSE_OC_ENUM_ITEM(AssignParam		,param	,Z(R,P,0,0,0,0))//!< = (O番目の関数引数を代入)
	RISSE_OC_ENUM_ITEM(AssignBlockParam	,bparam	,Z(R,P,0,0,0,0))//!< = (O番目の関数ブロック引数を代入)

// 共有空間アクセス
	RISSE_OC_ENUM_ITEM(Write			,swrite	,Z(S,R,0,0,0,0))//!< 共有空間への書き込み
	RISSE_OC_ENUM_ITEM(Read				,sread	,Z(R,S,0,0,0,0))//!< 共有空間からの読み込み

// 関数定義/引数処理/関数呼び出し
	RISSE_OC_ENUM_ITEM(New				,new	,Z(R,R,O,N,0,0))//!< "new"
	RISSE_OC_ENUM_ITEM(TryFuncCall		,trycall,Z(R,R,O,N,0,0))//!< try function call
	RISSE_OC_ENUM_ITEM(FuncCall			,call	,Z(R,R,O,N,0,0))//!< function call
	RISSE_OC_ENUM_ITEM(FuncCallBlock	,callb	,Z(R,R,O,N,N,0))//!< function call with lazyblock(VMのみで使用)
	RISSE_OC_ENUM_ITEM(SetFrame			,sframe	,Z(R,0,0,0,0,0))//!< スタックフレームと共有空間を設定する
	RISSE_OC_ENUM_ITEM(SetShare			,sshare	,Z(R,0,0,0,0,0))//!< 共有空間のみ設定する

// ジャンプ/分岐/制御/補助
	RISSE_OC_ENUM_ITEM(Jump				,jump	,Z(A,0,0,0,0,0))//!< 単純なジャンプ
	RISSE_OC_ENUM_ITEM(Branch			,branch	,Z(R,A,A,0,0,0))//!< 分岐
	RISSE_OC_ENUM_ITEM(CatchBranch		,cbranch,Z(R,C,N,0,0,0))//!< 例外catch用の分岐
//	RISSE_OC_ENUM_ITEM(EnterTryBlock	,enttry	,Z(A,A,A,0,0,0))//!< 例外保護ブロックに入る(VMのみで使用)
//	RISSE_OC_ENUM_ITEM(ExitTryBlock		,exittry,Z(0,0,0,0,0,0))//!< 例外保護ブロックから抜ける(VMのみで使用)
	RISSE_OC_ENUM_ITEM(Return			,return	,Z(R,0,0,0,0,0))//!< return ステートメント
	RISSE_OC_ENUM_ITEM(Debugger			,debug	,Z(0,0,0,0,0,0))//!< debugger ステートメント
	RISSE_OC_ENUM_ITEM(Throw			,throw	,Z(R,0,0,0,0,0))//!< throw ステートメント
	RISSE_OC_ENUM_ITEM(ReturnException	,returne,Z(R,C,O,0,0,0))//!< return 例外を発生させる
	RISSE_OC_ENUM_ITEM(GetExitTryValue	,exitval,Z(R,R,0,0,0,0))//!< Try脱出用例外オブジェクトから値を得る

// 引数1+なし
	RISSE_OC_ENUM_ITEM(LogNot			,lnot	,Z(R,R,0,0,0,0))//!< "!" logical not
	RISSE_OC_ENUM_ITEM(BitNot			,bnot	,Z(R,R,0,0,0,0))//!< "~" bit not
	RISSE_OC_ENUM_ITEM(DecAssign		,ERR	,Z(0,0,0,0,0,0))//!< "--" decrement
	RISSE_OC_ENUM_ITEM(IncAssign		,ERR	,Z(0,0,0,0,0,0))//!< "++" increment
	RISSE_OC_ENUM_ITEM(Plus				,plus	,Z(R,R,0,0,0,0))//!< "+"
	RISSE_OC_ENUM_ITEM(Minus			,minus	,Z(R,R,0,0,0,0))//!< "-"

// 引数1+1つ
	RISSE_OC_ENUM_ITEM(LogOr			,lor	,Z(R,R,R,0,0,0))//!< ||
	RISSE_OC_ENUM_ITEM(LogAnd			,land	,Z(R,R,R,0,0,0))//!< &&
	RISSE_OC_ENUM_ITEM(BitOr			,bor	,Z(R,R,R,0,0,0))//!< |
	RISSE_OC_ENUM_ITEM(BitXor			,bxor	,Z(R,R,R,0,0,0))//!< ^
	RISSE_OC_ENUM_ITEM(BitAnd			,band	,Z(R,R,R,0,0,0))//!< &
	RISSE_OC_ENUM_ITEM(NotEqual			,ne		,Z(R,R,R,0,0,0))//!< !=
	RISSE_OC_ENUM_ITEM(Equal			,eq		,Z(R,R,R,0,0,0))//!< ==
	RISSE_OC_ENUM_ITEM(DiscNotEqual		,dne	,Z(R,R,R,0,0,0))//!< !==
	RISSE_OC_ENUM_ITEM(DiscEqual		,deq	,Z(R,R,R,0,0,0))//!< ===
	RISSE_OC_ENUM_ITEM(Lesser			,lt		,Z(R,R,R,0,0,0))//!< <
	RISSE_OC_ENUM_ITEM(Greater			,gt		,Z(R,R,R,0,0,0))//!< >
	RISSE_OC_ENUM_ITEM(LesserOrEqual	,lte	,Z(R,R,R,0,0,0))//!< <=
	RISSE_OC_ENUM_ITEM(GreaterOrEqual	,gte	,Z(R,R,R,0,0,0))//!< >=
	RISSE_OC_ENUM_ITEM(RBitShift		,rbs	,Z(R,R,R,0,0,0))//!< >>>
	RISSE_OC_ENUM_ITEM(LShift			,ls		,Z(R,R,R,0,0,0))//!< <<
	RISSE_OC_ENUM_ITEM(RShift			,rs		,Z(R,R,R,0,0,0))//!< >>
	RISSE_OC_ENUM_ITEM(Mod				,mod	,Z(R,R,R,0,0,0))//!< %
	RISSE_OC_ENUM_ITEM(Div				,div	,Z(R,R,R,0,0,0))//!< /
	RISSE_OC_ENUM_ITEM(Idiv				,idiv	,Z(R,R,R,0,0,0))//!< \ (integer div)
	RISSE_OC_ENUM_ITEM(Mul				,mul	,Z(R,R,R,0,0,0))//!< *
	RISSE_OC_ENUM_ITEM(Add				,add	,Z(R,R,R,0,0,0))//!< +
	RISSE_OC_ENUM_ITEM(Sub				,sub	,Z(R,R,R,0,0,0))//!< -
	RISSE_OC_ENUM_ITEM(IncontextOf		,chgc	,Z(R,R,R,0,0,0))//!< incontextof

	RISSE_OC_ENUM_ITEM(DGet				,dget	,Z(R,R,R,0,0,0))//!< get .  
	RISSE_OC_ENUM_ITEM(IGet				,iget	,Z(R,R,R,0,0,0))//!< get [ ]
	RISSE_OC_ENUM_ITEM(DDelete			,ddel	,Z(R,R,R,0,0,0))//!< delete .
	RISSE_OC_ENUM_ITEM(IDelete			,idel	,Z(R,R,R,0,0,0))//!< delete [ ]

// 引数1+2つ
	RISSE_OC_ENUM_ITEM(DSet				,dset	,Z(R,R,R,0,0,0))//!< set .
	RISSE_OC_ENUM_ITEM(ISet				,iset	,Z(R,R,R,0,0,0))//!< set [ ]

// 代入演算子の類
	RISSE_OC_ENUM_ITEM(BitAndAssign		,ERR	,Z(0,0,0,0,0,0))//!< &=
	RISSE_OC_ENUM_ITEM(BitOrAssign		,ERR	,Z(0,0,0,0,0,0))//!< |=
	RISSE_OC_ENUM_ITEM(BitXorAssign		,ERR	,Z(0,0,0,0,0,0))//!< ^=
	RISSE_OC_ENUM_ITEM(SubAssign		,ERR	,Z(0,0,0,0,0,0))//!< -=
	RISSE_OC_ENUM_ITEM(AddAssign		,ERR	,Z(0,0,0,0,0,0))//!< +=
	RISSE_OC_ENUM_ITEM(ModAssign		,ERR	,Z(0,0,0,0,0,0))//!< %=
	RISSE_OC_ENUM_ITEM(DivAssign		,ERR	,Z(0,0,0,0,0,0))//!< /=
	RISSE_OC_ENUM_ITEM(IdivAssign		,ERR	,Z(0,0,0,0,0,0))//!< \=
	RISSE_OC_ENUM_ITEM(MulAssign		,ERR	,Z(0,0,0,0,0,0))//!< *=
	RISSE_OC_ENUM_ITEM(LogOrAssign		,ERR	,Z(0,0,0,0,0,0))//!< ||=
	RISSE_OC_ENUM_ITEM(LogAndAssign		,ERR	,Z(0,0,0,0,0,0))//!< &&=
	RISSE_OC_ENUM_ITEM(RBitShiftAssign	,ERR	,Z(0,0,0,0,0,0))//!< >>>=
	RISSE_OC_ENUM_ITEM(LShiftAssign		,ERR	,Z(0,0,0,0,0,0))//!< <<=
	RISSE_OC_ENUM_ITEM(RShiftAssign		,ERR	,Z(0,0,0,0,0,0))//!< >>=

// VM コードの最後
	RISSE_OC_ENUM_ITEM(VMCodeLast		,ERR	,Z(0,0,0,0,0,0))//!< VMオペコードの最後


// SSA形式特有
	RISSE_OC_ENUM_ITEM(Phi				,ERR	,Z(0,0,0,0,0,0))//!< φ関数
	RISSE_OC_ENUM_ITEM(DefineAccessMap	,ERR	,Z(0,0,0,0,0,0))//!< アクセスマップ定義
	RISSE_OC_ENUM_ITEM(DefineLazyBlock	,ERR	,Z(0,0,0,0,0,0))//!< 遅延評価ブロックの定義
	RISSE_OC_ENUM_ITEM(EndAccessMap		,ERR	,Z(0,0,0,0,0,0))//!< アクセスマップの使用終了
	RISSE_OC_ENUM_ITEM(ParentWrite		,ERR	,Z(0,0,0,0,0,0))//!< 親名前空間への書き込み
	RISSE_OC_ENUM_ITEM(ParentRead		,ERR	,Z(0,0,0,0,0,0))//!< 親名前空間からの読み込み
	RISSE_OC_ENUM_ITEM(ChildWrite		,ERR	,Z(0,0,0,0,0,0))//!< 子名前空間への書き込み
	RISSE_OC_ENUM_ITEM(ChildRead		,ERR	,Z(0,0,0,0,0,0))//!< 子名前空間からの読み込み
	RISSE_OC_ENUM_ITEM(WriteVar			,ERR	,Z(0,0,0,0,0,0))//!< 変数への書き込み(変換の過程でocAssignかocWriteに変わる)
	RISSE_OC_ENUM_ITEM(ReadVar			,ERR	,Z(0,0,0,0,0,0))//!< 変数からの読み込み(変換の過程でocAssignかocReadに変わる)

	RISSE_OC_ENUM_ITEM(OpCodeLast		,ERR	,Z(0,0,0,0,0,0))//!< オペコードの最後
RISSE_OC_ENUM_END
//---------------------------------------------------------------------------




#ifndef RISSE_OC_DEFINE_INFO
class tRisseVariantBlock;
typedef tRisseVariantBlock tRisseVariant;
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

	//! @brief		このイテレータの示す命令をコメント付きでダンプ(逆アセンブル)する
	//! @param		consts		定数領域(コメントを表示するために必要)
	//! @return		ダンプ結果
	tRisseString Dump(const tRisseVariant * consts) const;
};
//---------------------------------------------------------------------------
#endif //#ifndef RISSE_OC_DEFINE_INFO

} // namespace Risse
#endif


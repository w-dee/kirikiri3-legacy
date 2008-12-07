//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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

//---------------------------------------------------------------------------
// 各種定数
//---------------------------------------------------------------------------
static const int MaxVMInsnOperand = 6;
	//!< 命令のオペランドの最大数(ただし可変オペランド部分をのぞく)
static const risse_uint32 FuncCallFlag_Omitted = 0x80000000L;
	//!< call などのフラグの定数 - 関数呼び出しは ... を伴っているか (引数省略されているか)
static const risse_uint32 InvalidRegNum = (risse_uint32)0xffffffff;
	//!< 無効なレジスタを表す値
static const risse_size MaxArgCount = 30;
	//!< 引数の最大個数
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/**
 * VM命令の情報を表す構造体
 */
struct tVMInsnInfo
{
	/**
	 * tVMInsnInfoで使用されるフラグ
	 */
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

	/**
	 * オペコードが副作用を持つかどうかを表す情報
	 */
	enum tInsnEffect
	{
		vieNonEffective, 	//!< N このオペコードは副作用を持たない
		vieEffective,		//!< E このオペコードは副作用を持つ
		vieVarying,			//!< V このオペコードはオペランドによっては副作用を持つ
		vieAssertion		//!< A このオペコードはアサーションが有効な時のみに副作用を持つ
	};

	const char * Name;							//!< 命令名
	const char * Mnemonic;						//!< ニーモニック
	tInsnFlag Flags[MaxVMInsnOperand];		//!< オペランドnに対するフラグ

	tInsnEffect Effect; //!< オペランドの副作用

	const tStringData RawMemberName;		//!< オブジェクトの演算子メンバ名

	/**
	 * 演算子メンバ名に対応するtStringオブジェクトを返す
	 */
	const tString & GetMemberName() const
	{
		return *reinterpret_cast<const tString *>(&RawMemberName);
	}
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * オペーレーションコードの名前の配列
 */
extern const tVMInsnInfo VMInsnInfo[];
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 列挙型定義のinclude
//---------------------------------------------------------------------------
#include "risseOpCodesEnum.inc"
//---------------------------------------------------------------------------



class tVariantBlock;
typedef tVariantBlock tVariant;
//---------------------------------------------------------------------------
/**
 * VMコード用イテレータ
 */
class tVMCodeIterator : public tCollectee
{
	const risse_uint32 *CodePointer; //!< コードへのポインタ
	risse_size Address; //!< 論理アドレス(risse_size_maxの場合は論理アドレス指定無し)

public:
	/**
	 * コンストラクタ
	 * @param codepointer	コードへのポインタ
	 * @param address		論理アドレス
	 */
	tVMCodeIterator(const risse_uint32 *codepointer, risse_size address = risse_size_max)
	{
		CodePointer = codepointer;
		Address = address;
	}

	/**
	 * コピーコンストラクタ
	 * @param ref	コピーもとオブジェクト
	 */
	tVMCodeIterator(const tVMCodeIterator & ref)
	{
		CodePointer = ref.CodePointer;
		Address = ref.Address;
	}

	/**
	 * コードポインタの代入
	 * @param codepointer	コードポインタ
	 * @return	このオブジェクトへの参照
	 * @note	論理アドレスは「指定無し」にリセットされる。
	 *			論理アドレスもともに指定したい場合は SetCodePointer() を使うこと
	 */
	tVMCodeIterator & operator = (const risse_uint32 *codepointer)
	{
		CodePointer = codepointer;
		Address = risse_size_max;
		return *this;
	}

	/**
	 * コードポインタへの変換
	 */
	operator const risse_uint32 *() const { return CodePointer; }

	/**
	 * コードポインタを設定する
	 * @param codepointer	コードへのポインタ
	 * @param address		論理アドレス
	 */
	void SetCodePointer(const risse_uint32 *codepointer, risse_size address = risse_size_max)
	{
		CodePointer = codepointer;
		Address = address;
	}

	/**
	 * コードポインタを取得する
	 * @return	コードポインタ
	 */
	const risse_uint32 * GetCodePointer() const { return CodePointer; }

	/**
	 * 論理アドレスを設定する
	 * @param address	論理アドレス
	 */
	void SetAddress(risse_size address) { Address = address; }

	/**
	 * 論理アドレスを取得する
	 */
	risse_size GetAddress() const { return Address; }

	/**
	 * 前置インクリメント演算子
	 */
	void operator ++()
	{
		risse_size size = GetInsnSize();
		CodePointer += size;
		if(Address != risse_size_max) Address += size;
	}

	/**
	 * このイテレータの示す命令のサイズをVMワード単位で得る
	 * @return	命令のサイズ
	 */
	risse_size GetInsnSize() const;

	/**
	 * このイテレータの示す命令をダンプ(逆アセンブル)する
	 * @return	ダンプ結果
	 */
	tString Dump() const;

	/**
	 * このイテレータの示す命令をコメント付きでダンプ(逆アセンブル)する
	 * @param consts	定数領域(コメントを表示するために必要)
	 * @return	ダンプ結果
	 */
	tString Dump(const tVariant * consts) const;
};
//---------------------------------------------------------------------------

} // namespace Risse
#endif


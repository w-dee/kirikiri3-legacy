//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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

	const char * Name;							//!< 命令名
	const char * Mnemonic;						//!< ニーモニック
	tInsnFlag Flags[RisseMaxVMInsnOperand];		//!< オペランドnに対するフラグ

	const tRisseStringData RawMemberName;		//!< オブジェクトの演算子メンバ名

	//! @brief 演算子メンバ名に対応するtRisseStringオブジェクトを返す
	const tRisseString & GetMemberName() const
	{
		return *reinterpret_cast<const tRisseString *>(&RawMemberName);
	}
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief オペーレーションコードの名前の配列
//---------------------------------------------------------------------------
extern const tRisseVMInsnInfo RisseVMInsnInfo[];
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 列挙型定義のinclude
//---------------------------------------------------------------------------
#include "risseOpCodesEnum.inc"
//---------------------------------------------------------------------------



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

} // namespace Risse
#endif


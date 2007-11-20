//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RINA プロパティ管理
//---------------------------------------------------------------------------
#include "visual/rina/rinaNode.h"

namespace Rina {
//---------------------------------------------------------------------------


/*!
	@note
	プロパティはなるべく参照時の負荷が少なくなるように序数でアクセスされる。
	どの序数を利用するべきかどうかはIDレジストリ機構にIDを登録することで行う。
*/

//---------------------------------------------------------------------------
//! @brief		プロパティ情報
//---------------------------------------------------------------------------
class tPropertyInfo : public tCollectee
{
	typedef tCollectee inherited;

public:
	//! @brief		プロパティ情報構造体
	struct tItemInfo : public tCollectee
	{
		risse_size Index; //!< インデックス
		tString Name; //!< プロパティ名(i18nしてはいけない)
		tString ShortDesc; //!< 短い説明(i18nしていい)
	};

private:
	bool Freezed; //!< 内容をフリーズしたかどうか
	typedef gc_map<tString, tItemInfo *> tMap; //!< Map の typedef
	tMap Map; //!< プロパティ名とその情報のマップ
	typedef gc_vector<tItemInfo *> tVector; //!< Vector の typedef
	tVector Vector; //!< 情報のマップ
	risse_size BaseIndex; //!< プロパティの序数の基数

public:
	//! @brief		コンストラクタ
	tPropertyInfo();

	//! @brief		内容をフリーズする
	//! @note		以降、内容を変更しようとする操作はASSERTION例外を吐いて失敗するようになる
	void Freeze() { Freezed = true; }

	//! @brief		利用可能なプロパティの序数の基数を得る
	//! @return		利用可能なプロパティの序数の基数
	risse_size GetBaseIndex() const { return IndexBase; }

	//! @brief		プロパティを一つ追加する
	//! @param		name		プロパティ名
	//! @param		short_desc	短い説明
	//! @return		プロパティ情報構造体へのポインタ
	const tItemInfo * Add(const tString & name, const tString & short_desc);

	//! @brief		プロパティ数を得る
	//! @return		プロパティ数
	risse_size GetCount() const { RISSE_ASSERT(Vector.size() == Map.size()); return Vector.size(); }

	//! @brief		指定インデックスのプロパティを得る
	//! @param		index		インデックス
	//! @return		プロパティ情報構造体へのポインタ(NULL=該当プロパティなし;範囲外)
	const tItemInfo * At(risse_size index) const;

	//! @brief		プロパティを名前から検索する
	//! @param		name		名前
	//! @return		プロパティ情報構造体へのポインタ(NULL=該当プロパティなし)
	const tItemInfo * Find(const tString & name) const;

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		プロパティセット
//---------------------------------------------------------------------------
class tPropertySet : public tCollectee
{
	typedef tCollectee inherited;

public:
	//! @brief プロパティ情報とこのセット内におけるプロパティ序数の開始値の組
	struct tInfoAndStartIndex
	{
		tPropertyInfo * Info; //!< プロパティ情報
		risse_size StartIndex; //!< このセット内におけるプロパティ序数の開始値
	};

private:
	typedef gc_vector<tInfoAndStartIndex> tInfos; //!< tInfoAndStartIndex の配列の typedef
	tInfos Infos; //!< tInfoAndStartIndex の配列

	typedef gc_map<risse_size, risse_size> tInfoMap; //!< プロパティ序数とInfos内のインデックスの typedef
	tInfoMap InfoMap; //!< プロパティ序数とInfos内のインデックス

	risse_size TotalCount; //!< プロパティの総数

public:
	//! @brief		コンストラクタ
	tPropertySet();

	//! @brief		プロパティの総数を得る
	//! @return		プロパティの総数
	void GetTotalCount() const { return TotalCount; }

	//! @brief		セットにプロパティ情報を追加する
	//! @param		info		プロパティ情報
	//! @note		info に渡したプロパティ情報の内容は、
	//!				セットに追加した以降は変更してはならない。
	void Add(tPropertyInfo * info);

	//! @brief		プロパティ序数に対応するプロパティ情報を返す
	//! @param		index		序数
	//! @return		tInfoAndStartIndex 構造体へのポインタ (NULL=該当無し)
	const tInfoAndStartIndex * GetInfoAndStartIndex(risse_size index) const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		プロパティ
//---------------------------------------------------------------------------
class tProperty : public tCollectee
{
	typedef tCollectee inherited;

	tPropertySet * PropertySet; //!< プロパティ情報
	gc_vector<tVariant> Properties; //!< プロパティ配列

public:
	//! @brief		コンストラクタ
	//! @param		prop_set		プロパティセット
	tProperty(PropertySet * prop_set);

	//! @brief		プロパティ情報を得る
	//! @param		index		序数(基数が加算されていると見なす)
	//! @return		そこの序数に対応する位置にあるプロパティ情報
	//! @return		プロパティ情報
	const tPropertyInfo * GetInfo() const { return Info; }

	//! @brief		指定位置の序数のプロパティを得る
	//! @param		index		序数(基数が加算されていると見なす)
	//! @return		その位置にあるプロパティ
	virtual tVariant GetValueAt(risse_size index) = 0;

	//! @brief		指定位置の序数のプロパティを設定する
	//! @param		index		序数(基数が加算されていると見なす)
	//! @param		value		その位置にあるプロパティ
	//! @note		サブクラスでオーバーライドした
	//!				場合は完全に処理を乗っ取ってもよい
	//!				(親クラスのこれを改めて呼ぶ必要は特にない)
	//!				ただしその場合は GetValueAt もオーバーライド
	//!				しないとたぶん変なことになる
	virtual void SetValueAt(risse_size index, tVariant & value) = 0;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}


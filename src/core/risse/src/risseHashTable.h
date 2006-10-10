//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ハッシュ表の実装
//---------------------------------------------------------------------------
#ifndef RisseHashTableH
#define RisseHashTableH

#include "risseTypes.h"
#include "risseGC.h"

// #define RISSE_HS_DEBUG_CHAIN  // to debug chain algorithm

namespace Risse
{




//---------------------------------------------------------------------------
//! @brief		ハッシュ関数の特性クラス(汎用)
//! @note		このクラスのMakeはTのビットレイアウトに対して
//!				ハッシュを作成するものであり、通常は使うべきではない。@r
//!				たとえ単純な構造体であっても、この関数オブジェクトはパディングと
//!				して使われている構造体のなかのゴミビットを拾ってしまう可能性がある。@r
//!				通常は、tRisseHashTraits のテンプレートを特化させるか、あるいは
//!				他のハッシュ関数オブジェクトを使うこと。@r
//!				このクラスは Make() と GetHint() と SetHint() と HasHint を実装
//!				する必要がある。
//---------------------------------------------------------------------------
template <typename T>
class tRisseHashTraits
{
public:

	// the hash function used here is similar to one which used in perl 5.8,
	// see also http://burtleburtle.net/bob/hash/doobs.html (One-at-a-Time Hash)

	//! @brief		ハッシュを作成する
	//! @param		val		値
	//! @return		ハッシュ値
	static risse_uint32 Make(const T &val)
	{
		const char *p = (const char*)&val;
		const char *plim = (const char*)&val + sizeof(T);
		register risse_uint32 ret = 0;
		while(p<plim)
		{
			ret += *p;
			ret += (ret << 10);
			ret ^= (ret >> 6);
			p++;
		}
		ret += (ret << 3);
		ret ^= (ret >> 11);
		ret += (ret << 15);
		if(!ret) ret = (risse_uint32)-1;
		return ret;
	}

	//! @brief		ハッシュのヒントを返す
	//! @param		val		値
	//! @return		ハッシュ値(0=ハッシュが無効)
	static risse_uint32 GetHint(const T &val)
	{
		return 0;
	}

	//! @brief		ハッシュのヒントを設定する
	//! @param		val		値
	//! @param		hash	ハッシュ値
	static void SetHint(T & val, risse_uint32 hash)
	{
	}

	//! @brief		ハッシュのヒントを設定する(const版)
	//! @param		val		値
	//! @param		hash	ハッシュ値
	//! @note		valのSetHintがmutableとして宣言されている向き
	static void SetHint(const T & val, risse_uint32 hash)
	{
	}


	static const bool HasHint = false; //!< ハッシュのヒントを得る事ができるかどうか
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ハッシュ関数の特性クラス(tRisseString用)
//---------------------------------------------------------------------------
template <>
class tRisseHashTraits<tRisseString>
{
	typedef tRisseString T;
public:
	//! @brief		ハッシュを作成する
	//! @param		val		値
	//! @return		ハッシュ値
	static risse_uint32 Make(const T &val)
	{
		return val.GetHash();
	}

	//! @brief		ハッシュのヒントを返す
	//! @param		val		値
	//! @return		ハッシュ値(0=ハッシュが無効)
	static risse_uint32 GetHint(const T &val)
	{
		return val.GetHint();
	}

	//! @brief		ハッシュのヒントを設定する
	//! @param		val		値
	//! @param		hash	ハッシュ値
	static void SetHint(T & val, risse_uint32 hash)
	{
		val.SetHint(hash);
	}

	//! @brief		ハッシュのヒントを設定する(const版)
	//! @param		val		値
	//! @param		hash	ハッシュ値
	//! @note		valのSetHintがmutableとして宣言されている向き
	static void SetHint(const T & val, risse_uint32 hash)
	{
		val.SetHint(hash);
	}

	static const bool HasHint = true; //!< ハッシュのヒントを得る事ができるかどうか
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ハッシュ表の要素を表す構造体
//---------------------------------------------------------------------------
template <
	typename KeyT,
	typename ValueT
		>
struct tRisseHashTableElement : public tRisseCollectee
{
	risse_uint32 Hash;
	risse_uint32 Flags; // management flag
	char Key[sizeof(KeyT)];
	char Value[sizeof(ValueT)];
	tRisseHashTableElement *Prev; // previous chain item
	tRisseHashTableElement *Next; // next chain item
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ハッシュ表の基本機能の実装
//---------------------------------------------------------------------------
template <
	typename KeyT,
	typename ValueT,
	typename HashTraitsT = tRisseHashTraits<KeyT>,
	typename ElementT = tRisseHashTableElement<KeyT, ValueT>
		>
class tRisseHashTable : public tRisseCollectee
{
	ElementT * Elms; //!< 要素の配列
	risse_size HashMask; //!< ハッシュマスク(要素のlevel1スロットの個数-1)
	risse_size Count; //!< 要素の個数

	static const risse_size InitialHashMask = (16-1); //!< 初期のHashMask

	static const risse_uint32 UsingFlag = 0x1; //!< その要素は使用中である
	static const risse_uint32 Lv1Flag	 = 0x2; //!< その要素はlevel1スロットにある


public:
	//! @brief	イテレータクラス
	//! @note	非常に限定的かつ非効率的。一方向へのイテレーションしかサポートしない。
	class tIterator : public tRisseCollectee
	{
		const tRisseHashTable * Table; //!< ハッシュ表
		const ElementT * Element; //!<現在操作中の要素
		risse_size Slot; //!< 現在操作中のlv1スロットのインデックス
	public:
		tIterator() { Table = NULL; Element = NULL; Slot = 0; }
		tIterator(const tRisseHashTable & table)
		{
			// 最初の要素を探す
			Table = &table;
			Element = NULL; Slot = 0;
			if(Table->Count > 0)
			{
				Slot = 0;
				FindNext(); // 最初の要素を探す
			}
			// 最初の要素は存在しない
		}

		void operator ++()
		{
			Element = Element->Next;
			if(!Element)
			{
				Slot++;
				FindNext();
			}
		}

		void operator ++(int dummy)
		{
			operator ++();
		}

	private:
		void FindNext()
		{
			while(true)
			{
				if(Slot <= Table->HashMask)
				{
					if(Table->Elms[Slot].Flags & UsingFlag)
					{
						// lv1 が使用中
						Element = Table->Elms + Slot;
						break;
					}
					else if(Table->Elms[Slot].Next != NULL)
					{
						// lv2 が存在する
						Element = Table->Elms[Slot].Next;
						break;
					}
				}
				else
				{
					// 見つからなかった
					break;
				}
				Slot ++;
			}
		}


	public:
		KeyT & GetKey() const
		{ return *(KeyT*)Element->Key; }

		ValueT & GetValue() const
		{ return *(ValueT*)Element->Value; }

		bool End() const { return Element == NULL; }
	};

public:
	//! @brief		コンストラクタ
	tRisseHashTable()
	{
		HashMask = InitialHashMask;
		InternalInit();
	}

	//! @brief		要素をすべてクリアする
	void Clear()
	{
		InternalClear();
	}

	//! @brief		要素を追加する
	//! @param		key		キー
	//! @param		value	値
	//! @note		すでにキーが存在していた場合は値が上書きされる
	void Add(const KeyT &key, const ValueT &value)
	{
		// add Key and Value
		AddWithHash(key, HashTraitsT::Make(key), value);
	}

	//! @brief		要素を追加する(キーのハッシュ値がすでに分かっている場合)
	//! @param		key		キー
	//! @param		hash	ハッシュ
	//! @param		value	値
	//! @note		すでにキーが存在していた場合は値が上書きされる
	void AddWithHash(const KeyT &key, risse_uint32 hash, const ValueT &value)
	{
		// add Key ( hash ) and Value
#ifdef RISSE_HS_DEBUG_CHAIN
		hash = 0;
#endif
		if(HashTraitsT::HasHint)
			HashTraitsT::SetHint(key, hash); // 計算したハッシュはキーに格納しておく

		ElementT *lv1 = Elms + (hash & HashMask);
		ElementT *elm = lv1->Next;
		while(elm)
		{
			if(hash == elm->Hash)
			{
				// same ?
				if(key == *(KeyT*)elm->Key)
				{
					// do copying instead of inserting if these are same
					*(ValueT*)elm->Value = value;
					return;
				}
			}
			elm = elm->Next;
		}

		// lv1 used ?
		if(!(lv1->Flags & UsingFlag))
		{
			// lv1 is unused
			Construct(*lv1, key, value);
			Count ++;
			lv1->Hash = hash;
			lv1->Prev = NULL;
			// not initialize lv1->Next here
			return;
		}

		// lv1 is used
		if(hash == lv1->Hash)
		{
			// same?
			if(key == *(KeyT*)lv1->Key)
			{
				// do copying instead of inserting if these are same
				*(ValueT*)lv1->Value = value;
				return;
			}
		}

		// insert after lv1
		ElementT *newelm = new ElementT;
		newelm->Flags = 0;
		Construct(*newelm, key, value);
		Count ++;
		newelm->Hash = hash;
		if(lv1->Next) lv1->Next->Prev = newelm;
		newelm->Next = lv1->Next;
		newelm->Prev = lv1;
		lv1->Next = newelm;
	}

	//! @brief		キーを検索する
	//! @param		key		キー
	//! @return		見つかった要素へのポインタ。null=見つからなかった
	ValueT * Find(const KeyT &key) const
	{
		// find key
		// return   NULL  if not found
		risse_uint32 hash;
		if(HashTraitsT::HasHint)
		{
			// ヒントを持っている場合
			// 一度ヒントで検索を試みる
			risse_uint32 hint = HashTraitsT::GetHint(key);
			if(hint != 0)
			{
				const ElementT * elm = InternalFindWithHash(key, hint);
				if(elm) return (ValueT*)elm->Value; // 見つかった
			}
			// ヒントを用いて検索をしたら見つからなかった
			// あるいはヒントが見つからなかった
			// 本当にメンバが存在しないか、あるいはヒントが間違ってるのかの
			// どちらかの可能性がある。
			// ハッシュを再計算する。
			hash = HashTraitsT::Make(key);
			if(hint == hash)
				return NULL; // ヒントとハッシュが同じだった場合はすでに試した
		}
		else
		{
			// ヒントを持っていない場合
			hash = HashTraitsT::Make(key);
		}

		const ElementT * elm = InternalFindWithHash(key, hash);
		if(!elm) return NULL;
		return (ValueT*)elm->Value;
	}

	//! @brief		キーを検索する(見つかったキーと値のペアを返す)
	//! @param		key		キー
	//! @param		keyout	見つかったキー
	//! @param		value	見つかった値
	//! @return		キーが見つかれば真、見つからなければ偽
	bool Find(const KeyT &key, const KeyT *& keyout, ValueT *& value) const
	{
		// find key
		// return   false  if not found
		return FindWithHash(key, HashTraitsT::Make(key), keyout, value);
	}

	//! @brief		キーを検索する(あらかじめハッシュが分かっている場合)
	//! @param		key		キー
	//! @param		hash	ハッシュ
	//! @return		見つかった要素へのポインタ。null=見つからなかった
	ValueT * FindWithHash(const KeyT &key, risse_uint32 hash) const
	{
		// find key ( hash )
		// return   NULL  if not found
#ifdef RISSE_HS_DEBUG_CHAIN
		hash = 0;
#endif
		const ElementT * elm = InternalFindWithHash(key, hash);
		if(!elm) return NULL;
		return (ValueT*)elm->Value;
	}

	//! @brief		キーを検索する(あらかじめハッシュが分かっている場合)(見つかったキーと値のペアを返す)
	//! @param		key		キー
	//! @param		hash	ハッシュ
	//! @param		keyout	見つかったキー
	//! @param		value	見つかった値
	//! @return		キーが見つかれば真、見つからなければ偽
	bool FindWithHash(const KeyT &key, risse_uint32 hash, const KeyT *& keyout, ValueT *& value) const
	{
		// find key
		// return   false  if not found
#ifdef RISSE_HS_DEBUG_CHAIN
		hash = 0;
#endif
		const ElementT * elm = InternalFindWithHash(key, hash);
		if(elm)
		{
			value = (ValueT*)elm->Value;
			keyout = (const KeyT*)elm->Key;
			return true;
		}
		return false;
	}

	//! @brief		キーを削除する
	//! @param		key		キー
	//! @return		キーが見つかり、削除されれば真、削除されなければ偽
	bool Delete(const KeyT &key)
	{
		// delete key and return true if successed
		return DeleteWithHash(key, HashTraitsT::Make(key));
	}

	//! @brief		キーを削除する(あらかじめハッシュが分かっている場合)
	//! @param		key		キー
	//! @param		hash	ハッシュ
	//! @return		キーが見つかり、削除されれば真、削除されなければ偽
	bool DeleteWithHash(const KeyT &key, risse_uint32 hash)
	{
		// delete key ( hash ) and return true if succeeded
#ifdef RISSE_HS_DEBUG_CHAIN
		hash = 0;
#endif
		if(HashTraitsT::HasHint)
			HashTraitsT::SetHint(key, hash); // 計算したハッシュはキーに格納しておく

		ElementT *lv1 = Elms + (hash & HashMask);
		if(lv1->Flags & UsingFlag && hash == lv1->Hash)
		{
			if(key == *(KeyT*)lv1->Key)
			{
				// delete lv1
				Destruct(*lv1);
				Count --;
				return true;
			}
		}

		ElementT *prev = lv1;
		ElementT *elm = lv1->Next;
		while(elm)
		{
			if(hash == elm->Hash)
			{
				if(key == *(KeyT*)elm->Key)
				{
					Destruct(*elm);
					Count --;
					prev->Next = elm->Next; // sever from the chain
					if(elm->Next) elm->Next->Prev = prev;
					delete elm;
					return true;
				}
			}
			prev = elm;
			elm = elm->Next;
		}
		return false; // not found
	}


	//! @brief		要素数を得る
	//! @return		要素数
	risse_size GetCount() const { return Count; }

private:
	//! @brief		内部の要素をすべてクリアする
	void InternalClear()
	{
		Count = 0;
		for(risse_size i = 0; i <= HashMask; i++)
		{
			// level1の要素は参照を断ち切るために破壊する
			if(Elms[i].Flags & UsingFlag)
				Destruct(Elms[i]);

			Elms[i].Flags = Lv1Flag;
			Elms[i].Prev = NULL; // level2はこれにより切り離される
			Elms[i].Next = NULL; // level2はこれにより切り離される
		}
	}

	//! @brief		内部を初期化する
	void InternalInit()
	{
//		Elms = reinterpret_cast<ElementT*>(RisseMallocCollectee(sizeof(ElementT)*(HashMask + 1)));
		Elms = new ElementT[HashMask + 1];
		InternalClear();
	}


	//! @brief		(内部関数)検索を行う
	//! @param		key		キー
	//! @param		hash	キーのハッシュ
	//! @return		見つかった要素へのポインタ (NULL=見つからなかった)
	const ElementT * InternalFindWithHash(const KeyT &key, risse_uint32 hash) const
	{
		// find key ( hash )
#ifdef RISSE_HS_DEBUG_CHAIN
		hash = 0;
#endif

		if(HashTraitsT::HasHint)
			HashTraitsT::SetHint(key, hash); // 計算したハッシュはキーに格納しておく

		// lv1を検索
		const ElementT *lv1 = Elms + (hash & HashMask);
		if(hash == lv1->Hash && lv1->Flags & UsingFlag)
		{
			if(key == *(KeyT*)lv1->Key) return lv1;
		}

		// lv2を検索
		ElementT *elm = lv1->Next;
		while(elm)
		{
			if(hash == elm->Hash)
			{
				if(key == *(KeyT*)elm->Key) return elm;
			}
			elm = elm->Next;
		}
		return NULL; // not found
	}

	//! @brief		要素をコピーコンストラクタで構築する
	//! @param		elm			要素
	//! @param		key			キーの値
	//! @param		value		値
	static void Construct(ElementT &elm, const KeyT &key, const ValueT &value)
	{
		::new (&elm.Key) KeyT(key);
		::new (&elm.Value) ValueT(value);
		elm.Flags |= UsingFlag;
	}

	//! @brief		要素を破壊する
	//! @param		elm		要素
	//!	@note		このメソッドは強制的に KeyT と ValueT の in-place
	//!				デストラクタを呼ぶ。@r
	//!				これにより、KeyT および ValueT の内部がクリアされる
	//!				ことを期待する物である。@r
	//!				GC対応のコードではデストラクタは通常は書かないが、
	//!				このクラスの KeyT や ValueT として使う場合は
	//!				フィールドが保持しているポインタを破壊するコードを
	//!				デストラクタに書くのが好ましい。@r
	//!				また、このクラスはすべての KeyT および ValueT のインスタンス
	//!				に対してデストラクタを呼び出す保証はないので、デストラクタが
	//!				呼ばれることに依存したコードを書かないこと。
	static void Destruct(ElementT &elm)
	{
		((KeyT*)(&elm.Key)) -> ~KeyT();
		((ValueT*)(&elm.Value)) -> ~ValueT();
		elm.Flags &= ~UsingFlag;
	}

};
//---------------------------------------------------------------------------


} // namespace Risse




#endif



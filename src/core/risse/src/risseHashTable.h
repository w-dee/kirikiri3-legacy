//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ハッシュ表の実装
//---------------------------------------------------------------------------
#ifndef HashTableH
#define HashTableH

#include "risseTypes.h"
#include "risseGC.h"

namespace Risse
{

/*
	ハッシュ表の実装。頑張ってテンプレートにしたけどグチャグチャであんまりよくない。
*/


//---------------------------------------------------------------------------
//! @brief		キーおよび値の特性クラス
//! @note		今のところ、キーおよび値が、メソッド 「Destruct」を持っているかどうかを
//!				あらわす HasDestruct が定義されているのみ。
//!				通常、GC 対応クラスではデストラクタを書かないのだが、オブジェクトを破棄
//!				したいとき、GC によって mark されないようにポインタを破壊したくなる。ただ、
//!				デストラクタは書きたくないときがある (たとえば、そのような
//!				オブジェクトをスタック上に置くと、コンパイラが
//!				スタックの巻き戻しのために例外処理コードを置いてしまうときなど)。
//!				このような場合、Destruct メソッドを定義し、その中でポインタなどを
//!				クリアするようなコードを書き、さらに tKeyAndValueTraits::HasDesturct
//!				が真になるようにこのクラスのテンプレート特化を行えば、デストラクタ
//!				ではなくて Destruct メソッドを呼んでくれるようになる (デストラクタは
//!				呼ばれない)。なお、Desturct メソッドが呼ばれるのは、チェーンハッシュのなかでも
//!				lv1 と呼ばれている領域に対してのみで、すべてのインスタンスの Destruct
//!				メソッドが呼ばれる保証はない。
//---------------------------------------------------------------------------
template <typename T>
struct tKeyAndValueTraits
{
	enum { HasDestruct = 0 }; //!< Destuct メソッドを持っているかどうか
};
//---------------------------------------------------------------------------
//! @brief		tKeyAndValueTraits の tString 特化
//---------------------------------------------------------------------------
template <>
struct tKeyAndValueTraits<tString>
{
	enum { HasDestruct = 1 }; //!< Destuct メソッドを持っているかどうか
};
//---------------------------------------------------------------------------
//! @brief		tKeyAndValueTraits の tVariant 特化
//---------------------------------------------------------------------------
template <>
struct tKeyAndValueTraits<tVariant>
{
	enum { HasDestruct = 1 }; //!< Destuct メソッドを持っているかどうか
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		ハッシュ関数の特性クラス(汎用)
//! @note		このクラスのMakeはTのビットレイアウトに対して
//!				ハッシュを作成するものであり、通常は使うべきではない。@r
//!				たとえ単純な構造体であっても、この関数オブジェクトはパディングと
//!				して使われている構造体のなかのゴミビットを拾ってしまう可能性がある。@r
//!				通常は、tHashTraits のテンプレートを特化させるか、あるいは
//!				他のハッシュ関数オブジェクトを使うこと。@r
//!				このクラスは Make() と GetHint() と SetHint() と HasHint を実装
//!				する必要がある。
//!				スレッド保護はない
//---------------------------------------------------------------------------
template <typename T>
class tHashTraits
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


	enum { HasHint = 0 }; //!< ハッシュのヒントを得る事ができるかどうか
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ハッシュ関数の特性クラス(tString用)
//---------------------------------------------------------------------------
template <>
class tHashTraits<tString>
{
	typedef tString T;
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

	enum { HasHint = 1 }; //!< ハッシュのヒントを得る事ができるかどうか
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		キーあるいは値を破壊するためのクラス(Destuctメソッドを持ってない場合)
//---------------------------------------------------------------------------
template <typename T, bool n>
struct tHashTableKeyAndValueDestructor
{
	void operator () (void * ptr)
	{
		((T*)(ptr)) -> ~T(); // デストラクタを呼ぶ
	}
};
//---------------------------------------------------------------------------
//! @brief		キーあるいは値を破壊するためのクラス(Destuctメソッドを持っている場合)
//---------------------------------------------------------------------------
template <typename T>
struct tHashTableKeyAndValueDestructor<T, true>
{
	void operator () (void * ptr)
	{
		((T*)(ptr)) -> Destruct(); // Destruct メソッドを呼ぶ
	}
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		ハッシュ表の要素を表す構造体
//---------------------------------------------------------------------------
template <
	typename KeyT,
	typename ValueT
		>
struct tHashTableElement : public tCollectee
{
	risse_uint32 Hash; //!< ハッシュの値
	risse_uint32 Flags; //!< management flag
	char Key[sizeof(KeyT)]; // !< キーを保存するためのストレージ
	char Value[sizeof(ValueT)]; //!< 値を保存するためのストレージ
	tHashTableElement *Prev; //!< previous chain item
	tHashTableElement *Next; //!< next chain item
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ハッシュ表の基本機能の実装
//---------------------------------------------------------------------------
template <
	typename KeyT,
	typename ValueT,
	typename HashTraitsT = tHashTraits<KeyT>,
	typename ElementT = tHashTableElement<KeyT, ValueT>
		>
class tHashTableBase : public tCollectee
{
public:
	typedef ElementT tElement; //!< 要素型のtypedef

protected:
	tElement * Elms; //!< 要素の配列
	risse_size HashMask; //!< ハッシュマスク(要素のlevel1スロットの個数-1)
	risse_size Count; //!< 要素の個数

	static const risse_size InitialHashMask = (16-1); //!< 初期のHashMask

	static const risse_uint32 UsingFlag = 0x1; //!< その要素は使用中である
	static const risse_uint32 Lv1Flag   = 0x2; //!< その要素はlevel1スロットにある


public:
	//! @brief	イテレータクラス
	//! @note	非常に限定的かつ非効率的。一方向へのイテレーションしかサポートしない。
	//! 		高速なイテレーションが必要ならば順序付きハッシュ表の仕様を検討すること
	class tDefaultIterator : public tCollectee
	{
		const tHashTableBase * Table; //!< ハッシュ表
		const tElement * Element; //!<現在操作中の要素
		risse_size Slot; //!< 現在操作中のlv1スロットのインデックス
	public:
		tDefaultIterator() { Table = NULL; Element = NULL; Slot = 0; }
		tDefaultIterator(const tHashTableBase & table)
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
			Element = static_cast<const tElement*>(Element->Next);
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

	protected:
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
						Element = static_cast<const tElement*>(Table->Elms[Slot].Next);
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

	//! @brief		イテレータのtypedef
	typedef tDefaultIterator tIterator;

public:
	//! @brief		コンストラクタ
	tHashTableBase()
	{
		HashMask = InitialHashMask;
		Elms = new tElement[HashMask + 1];
		InternalClear();
	}

protected:
	//! @brief		(内部関数)内部の要素をすべてクリアする
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


	//! @brief		(内部関数)要素を追加する(キーのハッシュ値がすでに分かっている場合)
	//! @param		key		キー
	//! @param		hash	ハッシュ
	//! @param		value	値
	//! @return		内部の tElement 型へのポインタ
	//! @note		すでにキーが存在していた場合は値が上書きされる
	tElement * InternalAddWithHash(const KeyT &key, risse_uint32 hash, const ValueT &value)
	{
		// add Key ( hash ) and Value
		if(HashTraitsT::HasHint)
			HashTraitsT::SetHint(key, hash); // 計算したハッシュはキーに格納しておく

		tElement *lv1 = Elms + (hash & HashMask);
		tElement *elm = static_cast<tElement * > (lv1->Next);
		while(elm)
		{
			if(hash == elm->Hash)
			{
				// same ?
				if(key == *(KeyT*)elm->Key)
				{
					// do copying instead of inserting if these are same
					*(ValueT*)elm->Value = value;
					return elm;
				}
			}
			elm = static_cast<tElement * > (elm->Next);
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
			return lv1;
		}

		// lv1 is used
		if(hash == lv1->Hash)
		{
			// same?
			if(key == *(KeyT*)lv1->Key)
			{
				// do copying instead of inserting if these are same
				*(ValueT*)lv1->Value = value;
				return lv1;
			}
		}

		// insert after lv1
		tElement *newelm = new tElement;
		newelm->Flags = 0;
		Construct(*newelm, key, value);
		Count ++;
		newelm->Hash = hash;
		if(lv1->Next) lv1->Next->Prev = newelm;
		newelm->Next = lv1->Next;
		newelm->Prev = lv1;
		lv1->Next = newelm;
		return newelm;
	}

	//! @brief		(内部関数)検索を行う
	//! @param		key		キー
	//! @param		hash	キーのハッシュ
	//! @return		見つかった要素へのポインタ (NULL=見つからなかった)
	const tElement * InternalFindWithHash(const KeyT &key, risse_uint32 hash) const
	{
		// find key ( hash )
		if(HashTraitsT::HasHint)
			HashTraitsT::SetHint(key, hash); // 計算したハッシュはキーに格納しておく

		// lv1を検索
		const tElement *lv1 = Elms + (hash & HashMask);
		if(hash == lv1->Hash && lv1->Flags & UsingFlag)
		{
			if(key == *(KeyT*)lv1->Key) return lv1;
		}

		// lv2を検索
		const tElement *elm = static_cast<const tElement * > (lv1->Next);
		while(elm)
		{
			if(hash == elm->Hash)
			{
				if(key == *(KeyT*)elm->Key) return elm;
			}
			elm = static_cast<const tElement * > (elm->Next);
		}
		return NULL; // not found
	}

	//! @brief		(内部関数)キーを削除する
	//! @param		key		キー
	//! @param		hash	ハッシュ
	//! @return		キーが見つかり、削除されれば非NULL、削除されなければNULL
	tElement * InternalDeleteWithHash(const KeyT &key, risse_uint32 hash)
	{
		// delete key ( hash ) and return true if succeeded
		if(HashTraitsT::HasHint)
			HashTraitsT::SetHint(key, hash); // 計算したハッシュはキーに格納しておく

		tElement *lv1 = Elms + (hash & HashMask);
		if(lv1->Flags & UsingFlag && hash == lv1->Hash)
		{
			if(key == *(KeyT*)lv1->Key)
			{
				// delete lv1
				Destruct(*lv1);
				Count --;
				return lv1;
			}
		}

		tElement *prev = lv1;
		tElement *elm = static_cast<tElement*>(lv1->Next);
		while(elm)
		{
			if(hash == elm->Hash)
			{
				if(key == *(KeyT*)elm->Key)
				{
					Count --;
					prev->Next = elm->Next; // sever from the chain
					if(elm->Next) elm->Next->Prev = prev;
					// ここでは *elm を Destruct したり elm を delete
					// したりしない(チェーンから切り離され、あとはGCが
					// 残飯処理をするので必要なし)
					return elm;
				}
			}
			prev = elm;
			elm = static_cast<tElement*>(elm->Next);
		}
		return NULL; // not found
	}

	//! @brief		(内部関数)キーを削除する(要素による指定)
	//! @param		elm		削除したい要素
	void InternalDeleteByElement(tElement * elm)
	{
		if(elm->Flags & Lv1Flag)
		{
			// 消したい要素はlv1にある
			Destruct(*elm); // elm を破棄
		}
		else
		{
			// チェーンの中にあるのでチェーンからはずす
			if(elm->Prev) static_cast<tElement*>(elm->Prev)->Next = static_cast<tElement*>(elm->Next);
			if(elm->Next) static_cast<tElement*>(elm->Next)->Prev = static_cast<tElement*>(elm->Prev);
		}
		Count --;
	}


	//! @brief		要素数を得る
	//! @return		要素数
	risse_size InternalGetCount() const { return Count; }


private:
	//! @brief		要素をコピーコンストラクタで構築する
	//! @param		elm			要素
	//! @param		key			キーの値
	//! @param		value		値
	static void Construct(tElement &elm, const KeyT &key, const ValueT &value)
	{
		::new (&elm.Key) KeyT(key);
		::new (&elm.Value) ValueT(value);
		elm.Flags |= UsingFlag;
	}

	//! @brief		要素を破壊する
	//! @param		elm		要素
	//!	@note		このメソッドは強制的に KeyT と ValueT の in-place
	//!				デストラクタあるいはDestructメソッドを呼ぶ。
	//!				(どちらが呼ばれるかはtKeyAndValueTraits<T>による)
	static void Destruct(tElement &elm)
	{
		tHashTableKeyAndValueDestructor<KeyT,
					tKeyAndValueTraits<KeyT>::HasDestruct>()(&elm.Key);
		tHashTableKeyAndValueDestructor<ValueT,
					tKeyAndValueTraits<ValueT>::HasDestruct>()(&elm.Value);

		if(tKeyAndValueTraits<KeyT>::HasDestruct == 0 &&
			sizeof(KeyT) == sizeof(void *) )
		{
			// これはただのヒープへのポインタの可能性が高いのでクリアする
			// そうじゃない場合で間違ってクリアしたとしてもどうってことはない
			*(void **)(&elm.Key) = NULL;
		}

		if(tKeyAndValueTraits<ValueT>::HasDestruct == 0 &&
			sizeof(ValueT) == sizeof(void *) )
		{
			// これはただのヒープへのポインタの可能性が高いのでクリアする
			// そうじゃない場合で間違ってクリアしたとしてもどうってことはない
			*(void **)(&elm.Value) = NULL;
		}

		elm.Flags &= ~UsingFlag;
	}

};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		ハッシュ表の要素を表す構造体(順序付き)
//---------------------------------------------------------------------------
template <
	typename KeyT,
	typename ValueT
		>
struct tOrderedHashTableElement : public tHashTableElement<KeyT, ValueT>
{
	typedef tHashTableElement<KeyT, ValueT> inherited;
	inherited *NPrev; //!< 順序付き要素チェーンにおける前の要素
	inherited *NNext; //!< 順序付き要素チェーンにおける後ろの要素
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		順序付きハッシュ表
//---------------------------------------------------------------------------
template <
	typename KeyT,
	typename ValueT,
	typename HashTraitsT = tHashTraits<KeyT>,
	typename ElementT = tOrderedHashTableElement<KeyT, ValueT>
		>
class tOrderedHashTableBase :
	public tHashTableBase<KeyT, ValueT, HashTraitsT, ElementT >
{
	typedef
		tHashTableBase<KeyT, ValueT, HashTraitsT, ElementT >
			inherited;
public:
	typedef ElementT tElement; //!< 要素型のtypedef

protected:
	tElement *NFirst; //!< 順序付き要素チェーンにおける最初の要素
	tElement *NLast;  //!< 順序付き要素チェーンにおける最後の要素

public:
	//! @brief	イテレータクラス
	class tOrderedIterator : public tCollectee // this differs a bit from STL's iterator
	{
		tElement * elm;
	public:
		tOrderedIterator() { elm = NULL; }

		tOrderedIterator(const tOrderedHashTableBase & table)
		{ elm = table.NFirst; }

		tOrderedIterator(tElement * r_elm)
		{ elm = r_elm; }

		tOrderedIterator operator ++()
		{ elm = elm->NNext; return elm;}

		tOrderedIterator operator --()
		{ elm = elm->NPrev; return elm;}

		tOrderedIterator operator ++(int dummy)
		{ tElement *b_elm = elm; elm = static_cast<tElement*>(elm->NNext); return b_elm; }

		tOrderedIterator operator --(int dummy)
		{ tElement *b_elm = elm; elm = static_cast<tElement*>(elm->NPrev); return b_elm; }

		void operator +(risse_int n)
		{ while(n--) elm = elm->NNext; }

		void operator -(risse_int n)
		{ while(n--) elm = elm->NPrev; }

		bool operator ==(const tOrderedIterator & ref) const
		{ return elm == ref.elm; }

		bool operator !=(const tOrderedIterator & ref) const
		{ return elm != ref.elm; }

		KeyT & GetKey()
		{ return *(KeyT*)elm->Key; }

		ValueT & GetValue()
		{ return *(ValueT*)elm->Value; }

		bool End() const { return elm == NULL; }
	};


	//! @brief		イテレータのtypedef
	typedef tOrderedIterator tIterator;

public:
	//! @brief		コンストラクタ
	tOrderedHashTableBase()
	{
		NFirst = NLast = NULL;
	}


protected:
	//! @brief		(内部関数)内部の要素をすべてクリアする
	void InternalClear()
	{
		inherited::InternalClear();
		NFirst = NLast = NULL;
	}

	//! @brief		(内部関数)要素を追加する(キーのハッシュ値がすでに分かっている場合)
	//! @param		key		キー
	//! @param		hash	ハッシュ
	//! @param		value	値
	//! @return		内部の tElement 型へのポインタ
	//! @note		すでにキーが存在していた場合は値が上書きされる
	tElement * InternalAddWithHash(const KeyT &key, risse_uint32 hash, const ValueT &value)
	{
		risse_size org_count = inherited::InternalGetCount();

		tElement * added = inherited::InternalAddWithHash(key, hash, value);

		if(org_count != inherited::InternalGetCount())
			CheckAddingElementOrder(added); // 値が増えた
		else
			CheckUpdateElementOrder(added); // 値は増えてないが更新が行われた

		return added;
	}

	//! @brief		(内部関数)検索を行い、見つかった要素を順序の先頭に持ってくる
	//! @param		key		キー
	//! @param		hash	キーのハッシュ
	//! @return		見つかった要素へのポインタ (NULL=見つからなかった)
	const tElement * InternalFindAndTouchWithHash(const KeyT &key, risse_uint32 hash)
	{
		const tElement * found = static_cast<const tElement*>(inherited::InternalFindWithHash(key, hash));
		if(found) CheckUpdateElementOrder(const_cast<tElement*>(found)); // 先頭に持ってくる

		return found;
	}

	//! @brief		(内部関数)キーを削除する(あらかじめハッシュが分かっている場合)
	//! @param		key		キー
	//! @param		hash	ハッシュ
	//! @return		キーが見つかり、削除されれば非NULL、削除されなければNULL
	tElement * InternalDeleteWithHash(const KeyT &key, risse_uint32 hash)
	{
		tElement * deleted = inherited::InternalDeleteWithHash(key, hash);

		if(deleted)
		{
			// 削除に成功した
			CheckDeletingElementOrder(deleted);
		}

		return deleted;
	}

	//! @brief		(内部関数)キーを削除する(要素による指定)
	//! @param		elm		削除したい要素
	void InternalDeleteByElement(tElement * elm)
	{
		inherited::InternalDeleteByElement(elm);
		CheckDeletingElementOrder(elm);
	}

protected:

	//! @brief	順序付き要素チェーンに要素を追加する
	//! @param	elm		追加する要素
	//! @note	count をインクリメントしたあとに呼ぶこと
	void CheckAddingElementOrder(tElement *elm)
	{
		if(inherited::InternalGetCount() == 1)
		{
			// 最初の追加だった
			NLast = elm; // first addition
			elm->NNext = NULL;
		}
		else
		{
			NFirst->NPrev = elm;
			elm->NNext = NFirst;
		}
		NFirst = elm;
		elm->NPrev = NULL;
	}

	//! @brief	順序付き要素チェーンから要素を削除する
	//! @param	elm		削除する要素
	//! @note	count をデクリメントしたあとに呼ぶこと
	void CheckDeletingElementOrder(tElement *elm)
	{
		if(inherited::InternalGetCount() > 0)
		{
			// まだ要素がある
			if(elm == NFirst)
			{
				// deletion of first item
				NFirst = static_cast<tElement*>(elm->NNext);
				NFirst->NPrev = NULL;
			}
			else if(elm == NLast)
			{
				// deletion of last item
				NLast = static_cast<tElement*>(elm->NPrev);
				NLast->NNext = NULL;
			}
			else
			{
				// deletion of intermediate item
				static_cast<tElement*>(elm->NPrev)->NNext = elm->NNext;
				static_cast<tElement*>(elm->NNext)->NPrev = elm->NPrev;
			}
		}
		else
		{
			// 要素がすべて0になったとき
			NFirst = NLast = NULL;
		}
	}

	//! @brief	順序付き要素を要素チェーンの先頭に持ってくる
	//! @param	elm		要素
	void CheckUpdateElementOrder(tElement *elm)
	{
		// move elm to the front of addtional order
		if(elm != NFirst)
		{
			if(NLast == elm) NLast = static_cast<tElement*>(elm->NPrev);
			static_cast<tElement*>(elm->NPrev)->NNext = elm->NNext;
			if(elm->NNext) static_cast<tElement*>(elm->NNext)->NPrev = elm->NPrev;
			elm->NNext = NFirst;
			elm->NPrev = NULL;
			NFirst->NPrev = elm;
			NFirst = elm;
		}
	}

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
template <
	typename KeyT,
	typename ValueT,
	typename HashTraitsT = tHashTraits<KeyT>,
	typename ElementT = tHashTableElement<KeyT, ValueT>,
	typename BaseClassT = tHashTableBase<KeyT, ValueT, HashTraitsT, ElementT>
		>
class tHashTable : public BaseClassT
{
	typedef BaseClassT inherited;
	typedef ElementT tElement;  //!< 要素型のtypedef

public:
	//! @brief		コンストラクタ
	tHashTable()
	{
	}

	//! @brief		要素をすべてクリアする
	void Clear()
	{
		inherited::InternalClear();
	}

	//! @brief		要素を追加する
	//! @param		key		キー
	//! @param		value	値
	//! @note		すでにキーが存在していた場合は値が上書きされる
	void Add(const KeyT &key, const ValueT &value)
	{
		// add Key and Value
		inherited::InternalAddWithHash(key, HashTraitsT::Make(key), value);
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
				const tElement * elm = inherited::InternalFindWithHash(key, hint);
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

		const tElement * elm = InternalFindWithHash(key, hash);
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
		return Find(key, HashTraitsT::Make(key), keyout, value);
	}

	//! @brief		キーを検索する(あらかじめハッシュが分かっている場合)
	//! @param		key		キー
	//! @param		hash	ハッシュ
	//! @return		見つかった要素へのポインタ。null=見つからなかった
	ValueT * FindWithHash(const KeyT &key, risse_uint32 hash) const
	{
		// find key ( hash )
		// return   NULL  if not found
		const tElement * elm = InternalFindWithHash(key, hash);
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
		const tElement * elm = InternalFindWithHash(key, hash);
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
		return InternalDeleteWithHash(key, HashTraitsT::Make(key));
	}


	//! @brief		要素数を得る
	//! @return		要素数
	risse_size GetCount() const
	{
		return inherited::InternalGetCount();
	}

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
template <
	typename KeyT,
	typename ValueT,
	typename HashTraitsT = tHashTraits<KeyT>,
	typename ElementT = tOrderedHashTableElement<KeyT, ValueT>,
	typename BaseClassT = tHashTable<KeyT, ValueT, HashTraitsT, ElementT, tOrderedHashTableBase<KeyT, ValueT, HashTraitsT, ElementT> >
		>
class tOrderedHashTable : public BaseClassT
{
	typedef BaseClassT inherited;
	typedef ElementT tElement;  //!< 要素型のtypedef

public:
	//! @brief		コンストラクタ
	tOrderedHashTable()
	{
	}

	//! @brief		キーを検索し、要素を先頭に持ってくる
	//! @param		key		キー
	//! @return		見つかった要素へのポインタ。null=見つからなかった
	ValueT * FindAndTouch(const KeyT &key)
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
				const tElement * elm = InternalFindAndTouchWithHash(key, hint);
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

		const tElement * elm = InternalFindAndTouchWithHash(key, hash);
		if(!elm) return NULL;
		return (ValueT*)elm->Value;
	}

	//! @brief		キーを検索し、要素を先頭に持ってくる(見つかったキーと値のペアを返す)
	//! @param		key		キー
	//! @param		keyout	見つかったキー
	//! @param		value	見つかった値
	//! @return		キーが見つかれば真、見つからなければ偽
	bool FindAndTouch(const KeyT &key, const KeyT *& keyout, ValueT *& value)
	{
		// find key
		// return   false  if not found
		return FindAndTouch(key, HashTraitsT::Make(key), keyout, value);
	}

	//! @brief		キーを検索し、要素を先頭に持ってくる(あらかじめハッシュが分かっている場合)
	//! @param		key		キー
	//! @param		hash	ハッシュ
	//! @return		見つかった要素へのポインタ。null=見つからなかった
	ValueT * FindAndTouchWithHash(const KeyT &key, risse_uint32 hash)
	{
		// find key ( hash )
		// return   NULL  if not found
		const tElement * elm = InternalFindWithHash(key, hash);
		if(!elm) return NULL;
		return (ValueT*)elm->Value;
	}

	//! @brief		キーを検索し、要素を先頭に持ってくる(あらかじめハッシュが分かっている場合)(見つかったキーと値のペアを返す)
	//! @param		key		キー
	//! @param		hash	ハッシュ
	//! @param		keyout	見つかったキー
	//! @param		value	見つかった値
	//! @return		キーが見つかれば真、見つからなければ偽
	bool FindAndTouchWithHash(const KeyT &key, risse_uint32 hash, const KeyT *& keyout, ValueT *& value)
	{
		// find key
		// return   false  if not found
		const tElement * elm = InternalFindWithHash(key, hash);
		if(elm)
		{
			value = (ValueT*)elm->Value;
			keyout = (const KeyT*)elm->Key;
			return true;
		}
		return false;
	}

	//! @brief		要素数を最大でn個に制限する
	//! @param		n		制限する数(これより元々要素数が少ない場合は何もしない)
	//! @note		余分な要素は追加/Touch順の最後の方から切り落とされる。
	//!				ハッシュ表を使ったキャッシュの実装に用いる
	void Crop(risse_size n)
	{
		if(n == 0)  { inherited::Clear(); return; }
		while(inherited::GetCount() > n)
			inherited::InternalDeleteByElement(inherited::NLast);
	}

};
//---------------------------------------------------------------------------
}
#endif

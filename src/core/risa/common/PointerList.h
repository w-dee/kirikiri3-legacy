//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オブジェクトポインタリスト
//---------------------------------------------------------------------------
#ifndef PointerListH
#define PointerListH

#include <vector>
#include <assert.h>
/*
	ここでは object_list という、「オブジェクトへのポインタ」を管理するため
	のリストを実装する。NULL は格納できない。ポインタの順序は保たれる。
	オブジェクトの生成や消滅の面倒は見ない。

	たとえばcallbackというメソッドを持つインターフェースの配列 vector に対して、
	それぞれについて
	for(item in vector) item->callback();
	のようにイテレーションを行った場合、callback 内で vector に対して削除や
	追加などを行った場合、たとえば STL の gc_vector を使用すると、
	vector に操作を行った時点でイテレータが無効になり、安全にイテレーションを
	することができない。
	このクラスは、イテレーション中に、イテレーション用配列を保護することにより、
	配列の要素の削除や追加に対して常に安全なイテレーションの機構を提供する。
	特定アイテムの削除にはいまのところ線形検索を用いているので大量の削除を行い
	たい場合は注意が必要。

	これ自身を保護するためのクリティカルセクションはこれ自身がもっている。
	しかし、

	volatile pointer_list<tA>::scoped_lock lock(A);
	size_t count = A.get_locked_count();
	for(size_t i = 0; i < count; i++)
	{
		tA * th = A.get_locked(i);
		(* ... *)
	}

	のようなコードとremove メソッドが同時に呼ばれたとき、th を取得した直後に
	他のスレッドが A からそれをremoveしてしまうと、A のリストには既に存在しない
	th にアクセスすることになる。

	このような状態を避けるためには、イテレーションは常に一つのスレッドが行う
	ようにすること。
*/
namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * ダミーのクリティカルセクションの実装
 */
class tDummyCriticalSection : public tCollectee
{
public:
	tDummyCriticalSection() { ; } //!< コンストラクタ
	~tDummyCriticalSection() { ; } //!< デストラクタ

private:
	tDummyCriticalSection(const tDummyCriticalSection &); // non-copyable

private:
	void Enter() { ; } //!< クリティカルセクションに入る
	void Leave() { ; } //!< クリティカルセクションから出る

public:
	class tLocker : public tCollectee
	{
	public:
		tLocker(tDummyCriticalSection & cs) { }
		~tLocker() { }
	private:
		tLocker(const tLocker &); // non-copyable
	};
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * void * オブジェクトポインタリスト
 */
template <typename CST>
class void_pointer_list : public tDestructee
{
	mutable CST CS; //!< クリティカルセクション
	gc_vector<void *> m_list; //!< ポインタリスト
	gc_vector<void *> *m_shadow_list; //!< シャドーリスト
	size_t m_lock_count; //!< ロックカウント
	size_t m_locked_item_count; //!< ロック中の要素数
	bool m_has_null; //!< NULL を含んだポインタが存在するかどうか

public:
	/**
	 * 配列のロックを行うための scoped_lock
	 */
	class scoped_lock
	{
		void_pointer_list<CST> & m_list;
	public:
		scoped_lock(void_pointer_list<CST> & list) : m_list(list)
			{	m_list.lock(); }
		~scoped_lock()
			{	m_list.unlock(); }
	};

public:
	/**
	 * デフォルトコンストラクタ
	 * @note	このデストラクタはメインスレッド以外から非同期に呼ばれる
	 */
	void_pointer_list()
	{
		volatile typename CST::tLocker lock(CST);
		m_shadow_list = NULL;
		m_lock_count = 0;
		m_locked_item_count = 0;
		m_has_null = false;
	}

	/**
	 * デストラクタ
	 */
	virtual ~void_pointer_list()
	{
		volatile typename CST::tLocker lock(CST);
		if(m_shadow_list) delete m_shadow_list;
	}

private:
	// 今のところこれらはコピー不可
	void_pointer_list(const void_pointer_list<CST> & ref);
	void operator = (const void_pointer_list<CST> & ref);

public:
	/**
	 * クリティカルセクションオブジェクトを得る
	 */
	CST & GetCS() const { return CS; }

public:
	/**
	 * 配列のサイズを得る
	 * @note	配列は NULL を含む場合があるが、ここではそれは考慮せず
	 *			NULL を含んだサイズを返す
	 */
	size_t get_count() const
	{
		volatile typename CST::tLocker lock(CST);
		return m_list.size();
	}

	/**
	 * 配列のロックされたサイズを得る
	 * @note	ロックされていない状態で呼び出さないこと
	 * @note	配列は NULL を含む場合があるが、ここではそれは考慮せず
	 *			NULL を含んだサイズを返す
	 */
	size_t get_locked_count() const
	{
		assert(m_lock_count > 0);
		volatile typename CST::tLocker lock(CST);
		return m_locked_item_count;
	}

	/**
	 * 指定位置の要素を得る
	 * @param index	インデックス
	 */
	void * get(size_t index) const
	{
		volatile typename CST::tLocker lock(CST);
		return m_list[index];
	}

	/**
	 * 指定位置のロックされた要素を得る
	 * @param index	インデックス
	 * @note	ロックされていない状態で呼び出さないこと
	 */
	void * get_locked(size_t index) const
	{
		assert(m_lock_count > 0);
		volatile typename CST::tLocker lock(CST);
		if(m_shadow_list)
			return (*m_shadow_list)[index];
		return m_list[index];
	}

	/**
	 * 指定位置の要素をセットする
	 * @param index	インデックス
	 * @param item	要素
	 */
	void set(size_t index, void *item)
	{
		if(item == NULL) m_has_null = true;
		volatile typename CST::tLocker lock(CST);
		m_list[index] = item;
	}

	/**
	 * 配列に要素を追加する
	 * @param item	要素
	 */
	void add(void * item)
	{
		if(item == NULL) return; // NULL は add できない
		volatile typename CST::tLocker lock(CST);
		m_list.push_back(item);
	}

	/**
	 * 配列から要素を削除する
	 * @param item	要素
	 * @note	実際には要素の削除はその要素にNULLを代入すること
	 *			になる。NULL を含んだ状態の配列の NULL を削除するには
	 *			compact() を呼ぶこと
	 */
	void remove(void * item)
	{
		if(item == NULL) return;
		volatile typename CST::tLocker lock(CST);
		gc_vector<void *>::iterator it = 
			std::find(m_list.begin(), m_list.end(), item);
		if(it != m_list.end()) { m_has_null = true; *it = NULL; }
		if(m_shadow_list)
		{
			gc_vector<void *>::iterator it = 
				std::find(m_shadow_list->begin(), m_shadow_list->end(), item);
			if(it != m_shadow_list->end()) *it = NULL;
		}
	}

	/**
	 * 配列に要素を挿入する
	 * @param index	挿入位置を表すインデックス
	 * @param item	要素
	 * @note	挿入は一般的に高価な操作なので使用には気をつけること
	 */
	void insert(size_t index, void * item)
	{
		if(item == NULL) return; // NULL は insert できない
		volatile typename CST::tLocker lock(CST);
		make_shadow();
		m_list.insert(m_list.begin() + index, item);
	}

	/**
	 * 配列中の NULL 要素を削除する
	 */
	void compact()
	{
		volatile typename CST::tLocker lock(CST);
		if(!m_has_null) return;
		make_shadow();
		gc_vector<void *>::iterator d_it = m_list.begin();
		for(gc_vector<void *>::iterator it = d_it;
			it != m_list.end(); it++)
		{
			// NULL をスキップしながら d_it にコピー
			if(*it != NULL)
			{
				if(d_it != it) *d_it = *it;
				d_it ++;
			}
		}
		m_list.resize(d_it - m_list.begin());
		m_has_null = false;
	}

	/**
	 * 要素を検索する
	 * @param item	検索したい要素
	 * @return	要素のインデックス (static_cast<size_t>(-1L) の場合は要素が見つからなかった)
	 */
	size_t find(void * item) const
	{
		if(item == NULL) return static_cast<size_t>(-1L); // null は探せない
		volatile typename CST::tLocker lock(CST);
		gc_vector<void *>::const_iterator it = 
			std::find(m_list.begin(), m_list.end(), item);
		if(it == m_list.end()) return static_cast<size_t>(-1L); // 見つからない
		return it - m_list.begin();
	}

private:
	/**
	 * 配列のロックを行う
	 */
	void lock()
	{
		volatile typename CST::tLocker lock(CST);
		if(m_lock_count++ == 0)
		{
			// ロックをする
			m_locked_item_count = m_list.size();
		}
	}

	/**
	 * 配列のロックの解除をする
	 */
	void unlock()
	{
		volatile typename CST::tLocker lock(CST);
		if(--m_lock_count == 0)
		{
			// ロックを解除する
			delete m_shadow_list;
			m_shadow_list = NULL;
			compact();
		}
	}

	/**
	 * 配列のシャドー化
	 */
	void make_shadow()
	{
		if(m_lock_count == 0) return; // ロック中でない場合はなにもしない
		if(m_shadow_list) return;
		m_shadow_list = new gc_vector<void*>();
		m_shadow_list->insert(m_shadow_list->begin(),
			m_list.begin(), m_list.begin() + m_locked_item_count); // コピーを作成
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 任意型のオブジェクトポインタリスト
 */
template <typename T>
class pointer_list : public tCollectee
{
	void_pointer_list<tCriticalSection> m_list;

public:
	/**
	 * 配列のロックを行うための scoped_lock
	 */
	class scoped_lock
	{
		void_pointer_list<tCriticalSection>::scoped_lock m_lock;
	public:
		scoped_lock(pointer_list<T> & list) : m_lock(list.m_list) {;}
	};

public:
	/**
	 * コンストラクタ
	 */
	pointer_list() {;}

public:
	/**
	 * クリティカルセクションオブジェクトを得る
	 */
	tCriticalSection & GetCS() const { return m_list.GetCS(); }

private:
	// 今のところこれらはコピー不可
	pointer_list(const pointer_list<T> & ref);
	void operator = (const pointer_list<T> & ref);

public:
	/**
	 * 配列のサイズを得る
	 * @note	配列は NULL を含む場合があるが、ここではそれは考慮せず
	 *			NULL を含んだサイズを返す
	 */
	size_t get_count() const
	{
		return m_list.get_count();
	}

	/**
	 * 配列のロックされたサイズを得る
	 * @note	ロックされていない状態で呼び出さないこと
	 */
	size_t get_locked_count() const
	{
		return m_list.get_locked_count();
	}

	/**
	 * 指定位置の要素を得る
	 * @param index	インデックス
	 */
	T * get(size_t index) const
	{
		return static_cast<T*>(m_list.get(index));
	}

	/**
	 * 指定位置のロックされた要素を得る
	 * @param index	インデックス
	 * @note	ロックされていない状態で呼び出さないこと
	 */
	T * get_locked(size_t index) const
	{
		return static_cast<T*>(m_list.get_locked(index));
	}

	/**
	 * 指定位置の要素をセットする
	 * @param index	インデックス
	 * @param item	要素
	 */
	void set(size_t index, T * item)
	{
		m_list.set(index, static_cast<void*>(item));
	}

	/**
	 * 配列に要素を追加する
	 * @param item	要素
	 */
	void add(T * item)
	{
		m_list.add(static_cast<void*>(item));
	}

	/**
	 * 配列から要素を削除する
	 * @param item	要素
	 * @note	実際には要素の削除はその要素にNULLを代入すること
	 *			になる。NULL を含んだ状態の配列の NULL を削除するには
	 *			compact() を呼ぶこと(compact() は scoped_lock を抜ける
	 *			際には暗黙的に呼び出される)。
	 */
	void remove(T * item)
	{
		m_list.remove(static_cast<void*>(item));
	}

	/**
	 * 配列に要素を挿入する
	 * @param index	挿入位置を表すインデックス
	 * @param item	要素
	 * @note	挿入は一般的に高価な操作なので使用には気をつけること
	 */
	void insert(size_t index, T * item)
	{
		m_list.insert(index, static_cast<void*>(item));
	}

	/**
	 * 配列中の NULL 要素を削除する
	 */
	void compact()
	{
		m_list.compact();
	}

	/**
	 * 要素を検索する
	 * @param item	検索したい要素
	 * @return	要素のインデックス (static_cast<size_t>(-1L) の場合は要素が見つからなかった)
	 */
	size_t find(T * item) const
	{
		return m_list.find(static_cast<void*>(item));
	}

};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif

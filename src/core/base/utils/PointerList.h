//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
	追加などを行った場合、たとえば STL の std::vector を使用すると、
	vector に操作を行った時点でイテレータが無効になり、安全にイテレーションを
	することができない。
	このクラスは、イテレーション中に、イテレーション用配列を保護することにより、
	配列の要素の削除や追加に対して常に安全なイテレーションの機構を提供する。
	特定アイテムの削除にはいまのところ線形検索を用いているので大量の削除を行い
	たい場合は注意が必要。
*/


//---------------------------------------------------------------------------
//! @brief  void * オブジェクトポインタリスト
//---------------------------------------------------------------------------
class void_pointer_list
{
	std::vector<void *> m_list; //!< ポインタリスト
	std::vector<void *> *m_shadow_list; //!< シャドーリスト
	size_t m_lock_count; //!< ロックカウント
	size_t m_locked_item_count; //!< ロック中の要素数
	bool m_has_null; //!< NULL を含んだポインタが存在するかどうか

public:
	//! @brief 配列のロックを行うための scoped_lock
	class scoped_lock
	{
		void_pointer_list & m_list;
	public:
		scoped_lock(void_pointer_list & list) : m_list(list)
			{	m_list.lock(); }
		~scoped_lock()
			{	m_list.unlock(); }
	};

public:
	//! @brief		デフォルトコンストラクタ
	void_pointer_list()
	{
		m_shadow_list = NULL;
		m_lock_count = 0;
		m_locked_item_count = 0;
		m_has_null = false;
	}

	//! @brief		デストラクタ
	~void_pointer_list()
	{
		delete m_shadow_list;
	}

private:
	// 今のところこれらはコピー不可
	void_pointer_list(const void_pointer_list & ref);
	void operator = (const void_pointer_list & ref);

public:
	//! @brief		配列のサイズを得る
	//! @note		配列は NULL を含む場合があるが、ここではそれは考慮せず
	//!				NULL を含んだサイズを返す
	size_t get_count() const
	{
		return m_list.size();
	}

	//! @brief		配列のロックされたサイズを得る
	//! @note		ロックされていない状態で呼び出さないこと
	size_t get_locked_count() const
	{
		assert(m_lock_count > 0);
		if(m_shadow_list)
			return m_shadow_list->size();
		else
			return m_locked_item_count;
	}

	//! @brief		指定位置の要素を得る
	//! @param		index インデックス
	void * get(size_t index) const
	{
		return m_list[index];
	}

	//! @brief		指定位置のロックされた要素を得る
	//! @param		index インデックス
	//! @note		ロックされていない状態で呼び出さないこと
	void * get_locked(size_t index) const
	{
		assert(m_lock_count > 0);
		if(m_shadow_list)
			return (*m_shadow_list)[index];
		return m_list[index];
	}

	//! @brief		指定位置の要素をセットする
	//! @param		index インデックス
	//! @param		item 要素
	void set(size_t index, void *item)
	{
		if(item == NULL) m_has_null = true;
		m_list[index] = item;
	}

	//! @brief		配列に要素を追加する
	//! @param		item 要素
	void add(void * item)
	{
		if(item == NULL) return; // NULL は add できない
		m_list.push_back(item);
	}

	//! @brief		配列から要素を削除する
	//! @param		item 要素
	//! @note		実際には要素の削除はその要素にNULLを代入すること
	//!				になる。NULL を含んだ状態の配列の NULL を削除するには
	//!				compact() を呼ぶこと
	void remove(void * item)
	{
		if(item == NULL) return;
		std::vector<void *>::iterator it = 
			std::find(m_list.begin(), m_list.end(), item);
		if(it != m_list.end()) { m_has_null = true; *it = NULL; }
		if(m_shadow_list)
		{
			std::vector<void *>::iterator it = 
				std::find(m_shadow_list->begin(), m_shadow_list->end(), item);
			if(it != m_shadow_list->end()) *it = NULL;
		}
	}

	//! @brief		配列に要素を挿入する
	//! @param		index 挿入位置を表すインデックス
	//! @param		item 要素
	//! @note		挿入は一般的に高価な操作なので使用には気をつけること
	void insert(size_t index, void * item)
	{
		if(item == NULL) return; // NULL は insert できない
		make_shadow();
		m_list.insert(m_list.begin() + index, item);
	}

	//! @brief		配列中の NULL 要素を削除する
	void compact()
	{
		if(!m_has_null) return;
		make_shadow();
		std::vector<void *>::iterator d_it = m_list.begin();
		for(std::vector<void *>::iterator it = d_it;
			it != m_list.end(); /**/)
		{
			// NULL をスキップしながら d_it にコピー
			if(*it != NULL)
			{
				if(d_it != it) *d_it = *it;
				d_it ++;
				it ++;
			}
			else
			{
				it ++;
			}
		}
		m_list.resize(d_it - m_list.begin());
		m_has_null = false;
	}

	//! @brief		要素を検索する
	//! @param		item 検索したい要素
	//! @return		要素のインデックス (static_cast<size_t>(-1L) の場合は要素が見つからなかった)
	size_t find(void * item) const
	{
		if(item == NULL) return static_cast<size_t>(-1L); // null は探せない
		std::vector<void *>::const_iterator it = 
			std::find(m_list.begin(), m_list.end(), item);
		if(it == m_list.end()) return static_cast<size_t>(-1L); // 見つからない
		return it - m_list.begin();
	}

private:
	//! @brief		配列のロックを行う
	void lock()
	{
		if(m_lock_count++ == 0)
		{
			// ロックをする
			m_locked_item_count = m_list.size();
		}
	}

	//! @brief		配列のロックの解除をする
	void unlock()
	{
		if(--m_lock_count == 0)
		{
			// ロックを解除する
			delete m_shadow_list;
			m_shadow_list = NULL;
			compact();
		}
	}

	//! @brief		配列のシャドー化
	void make_shadow()
	{
		if(m_lock_count == 0) return; // ロック中でない場合はなにもしない
		if(m_shadow_list) return;
		m_shadow_list = new std::vector<void*>(
				m_list.begin(), m_list.begin() + m_locked_item_count); // コピーを作成
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief  任意型のオブジェクトポインタリスト
//---------------------------------------------------------------------------
template <typename T>
class pointer_list
{
	void_pointer_list m_list;

public:
	//! @brief 配列のロックを行うための scoped_lock
	class scoped_lock
	{
		void_pointer_list::scoped_lock m_lock;
	public:
		scoped_lock(pointer_list<T> & list) : m_lock(list.m_list) {;}
	};

public:
	//! @brief コンストラクタ
	pointer_list() {;}

private:
	// 今のところこれらはコピー不可
	pointer_list(const pointer_list<T> & ref);
	void operator = (const pointer_list<T> & ref);

public:
	//! @brief		配列のサイズを得る
	//! @note		配列は NULL を含む場合があるが、ここではそれは考慮せず
	//!				NULL を含んだサイズを返す
	size_t get_count() const
	{
		return m_list.get_count();
	}

	//! @brief		配列のロックされたサイズを得る
	//! @note		ロックされていない状態で呼び出さないこと
	size_t get_locked_count() const
	{
		return m_list.get_locked_count();
	}

	//! @brief		指定位置の要素を得る
	//! @param		index インデックス
	T * get(size_t index) const
	{
		return reinterpret_cast<T*>(m_list.get(index));
	}

	//! @brief		指定位置のロックされた要素を得る
	//! @param		index インデックス
	//! @note		ロックされていない状態で呼び出さないこと
	T * get_locked(size_t index) const
	{
		return reinterpret_cast<T*>(m_list.get_locked(index));
	}

	//! @brief		指定位置の要素をセットする
	//! @param		index インデックス
	//! @param		item 要素
	void set(size_t index, T * item)
	{
		m_list.set(index, reinterpret_cast<void*>(item));
	}

	//! @brief		配列に要素を追加する
	//! @param		item 要素
	void add(T * item)
	{
		m_list.add(reinterpret_cast<void*>(item));
	}

	//! @brief		配列から要素を削除する
	//! @param		item 要素
	//! @note		実際には要素の削除はその要素にNULLを代入すること
	//!				になる。NULL を含んだ状態の配列の NULL を削除するには
	//!				compact() を呼ぶこと
	void remove(T * item)
	{
		m_list.remove(reinterpret_cast<void*>(item));
	}

	//! @brief		配列に要素を挿入する
	//! @param		index 挿入位置を表すインデックス
	//! @param		item 要素
	//! @note		挿入は一般的に高価な操作なので使用には気をつけること
	void insert(size_t index, T * item)
	{
		m_list.insert(index, reinterpret_cast<void*>(item));
	}

	//! @brief		配列中の NULL 要素を削除する
	void compact()
	{
		m_list.compact();
	}

	//! @brief		要素を検索する
	//! @param		item 検索したい要素
	//! @return		要素のインデックス (static_cast<size_t>(-1L) の場合は要素が見つからなかった)
	size_t find(T * item) const
	{
		return m_list.find(reinterpret_cast<void*>(item));
	}

};
//---------------------------------------------------------------------------

#endif

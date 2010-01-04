//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ガーベジ・コレクション
//---------------------------------------------------------------------------
#ifndef _GCH_
#define _GCH_

#include "risa/common/Singleton.h"
#include "risa/common/RisaThread.h"
#include <wx/fileconf.h>


namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * ガーベジコレクタ用スレッド
 */
class tCollectorThread : public singleton_base<tCollectorThread>
{
	tCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	/**
	 * コレクタスレッドのクラス
	 */
	class tThreadImpl : public tThread
	{
		tThreadEvent Event; //!< イベントオブジェクト
		tCollectorThread & Owner;
	public:

		/**
		 * コンストラクタ
		 * @param owner	このオブジェクトを所有する tCollectorThread オブジェクト
		 */
		tThreadImpl(tCollectorThread & owner);

		/**
		 * デストラクタ
		 */
		~tThreadImpl();

		/**
		 * スレッドのエントリーポイント
		 */
		void Execute();

		/**
		 * スレッドをたたき起こす
		 */
		void Wakeup() { Event.Signal(); }
	};

	tThreadImpl *Thread; //!< コレクタスレッド

public:
	/**
	 * コンストラクタ
	 */
	tCollectorThread();

	/**
	 * デストラクタ
	 */
	~tCollectorThread();

private:
	/**
	 * ファイナライズすべきオブジェクトがあった場合に
	 * GC から呼ばれるコールバック
	 */
	static void FinalizerNotifier();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * メインスレッドでデストラクタが走ってほしい非GC対応オブジェクトを保持するキュー
 */
class tMainThreadDestructorQueue : public singleton_base<tMainThreadDestructorQueue>
{
public:
	class tDestructorCaller
	{
	public:
		virtual ~tDestructorCaller() {;}
	};

private:
	std::deque<tDestructorCaller*> Pointers; // 削除まちオブジェクトへのポインタの配列
	tCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション
public:

	/**
	 * コンストラクタ
	 */
	tMainThreadDestructorQueue() {;}

	/**
	 * デストラクタ
	 */
	~tMainThreadDestructorQueue();

	/**
	 * キューにデストラクタを登録
	 * @param dtor	デストラクタ
	 */
	void Enqueue(tDestructorCaller * dtor);

	/**
	 * デストラクタを呼ぶ
	 */
	void CallDestructors();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * 自分の消滅時に指定クラスのデストラクタを呼ぶだけのテンプレートクラス
 */
/*
	デストラクタの呼び出し規約を想定できないため、いったんこのクラスを挟む。
*/
template <typename T>
class tDestructorCaller_Impl : public tMainThreadDestructorQueue::tDestructorCaller
{
	T Ptr;
public:
	tDestructorCaller_Impl(T ptr) { Ptr = ptr; }
	virtual ~tDestructorCaller_Impl() { delete Ptr; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * メインスレッドでデストラクタが走ってほしい非GC対応オブジェクトを保持する自動ポインタ。
 * @note	tDestructee 派生クラスなので注意(つまりデストラクタによる後処理が必要)。
 *			あとあんまり効率のよい実装ではない。
 */
template <typename T>
class tMainThreadAutoPtr : public tDestructee, depends_on<tMainThreadDestructorQueue>
{
	T* Pointer; //!< ポインタ



public:
	/**
	 * コンストラクタ
	 */
	tMainThreadAutoPtr()
	{
		Pointer = NULL;
	}

	/**
	 * コンストラクタ
	 * @param ptr	ポインタ
	 */
	tMainThreadAutoPtr(T * ptr)
	{
		Pointer = ptr;
	}

	/**
	 * デストラクタ
	 */
	~tMainThreadAutoPtr()
	{
		dispose();
	}

	/**
	 * ポインタを設定する
	 * @param ptr	ポインタ
	 */
	void set(T * ptr) { Pointer = ptr; }

	/**
	 * ポインタを設定する
	 */
	void operator = (T * ptr) { Pointer = ptr; }

	/**
	 * ポインタで表されている内容を破棄し、ポインタをクリアする
	 */
	void dispose() { 
		// キューに Pointer のデストラクタを登録
		if(Pointer)
			tMainThreadDestructorQueue::instance()->Enqueue(new tDestructorCaller_Impl<T*>(Pointer));
		Pointer = NULL;
		// ポインタをクリア
	}

	/**
	 * ポインタへの変換
	 */
	T* get() const { return Pointer; }

	/**
	 * ポインタへの変換
	 */
	operator T* () const { return Pointer; }

	/**
	 * ポインタへの変換
	 */
	T* operator -> () const { return Pointer; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 参照カウンタ付きオブジェクトを対象にしたポインタ
 * @note	AddRef() および Release() をメソッドに持つクラスが対象。
 */
template <typename T>
class tGCReferencePtr : public tDestructee
{
	T* Pointer; //!< ポインタ



public:
	/**
	 * コンストラクタ
	 */
	tGCReferencePtr()
	{
		Pointer = NULL;
	}

	/**
	 * コンストラクタ
	 * @param ptr	ポインタ
	 */
	tGCReferencePtr(T * ptr)
	{
		Pointer = ptr;
		if(Pointer) Pointer->AddRef();
	}

	/**
	 * コピーコンストラクタ
	 * @param ref	ポインタ
	 */
	tGCReferencePtr(const tGCReferencePtr & ref)
	{
		Pointer = ref.Pointer;
		if(Pointer) Pointer->AddRef();
	}

	/**
	 * デストラクタ
	 */
	~tGCReferencePtr()
	{
		if(Pointer) Pointer->Release();
	}

	/**
	 * ポインタを破棄する(NULLに設定する)
	 */
	void dispose() { set(NULL); }

	/**
	 * ポインタを設定する
	 * @param ptr	ポインタ
	 */
	void set(T * ptr) {
		if(Pointer != ptr) {
			if(Pointer) Pointer->Release();
			Pointer = ptr;
			if(Pointer) Pointer->AddRef();
		}
	}

	/**
	 * ポインタを設定する
	 */
	void operator = (T * ptr) { set(ptr); }

	/**
	 * ポインタへの変換
	 * @note	注意! AddRef() されます
	 */
	T* get() const { if(Pointer) Pointer->AddRef(); return Pointer; }

	/**
	 * ポインタへの変換
	 * @note	注意! AddRef() されません
	 */
	T* get_noaddref() const { return Pointer; }

	/**
	 * ポインタへの変換
	 */
	operator T* () const { return Pointer; }

	/**
	 * ポインタへの変換
	 */
	T* operator -> () const { return Pointer; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


#endif

//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ガーベジ・コレクション
//---------------------------------------------------------------------------
#ifndef _GCH_
#define _GCH_

#include "base/utils/Singleton.h"
#include "base/utils/RisaThread.h"
#include <wx/fileconf.h>


namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		ガーベジコレクタ用スレッド
//---------------------------------------------------------------------------
class tCollectorThread : public singleton_base<tCollectorThread>
{
	tCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	//! @brief	コレクタスレッドのクラス
	class tThreadImpl : public tThread
	{
		tThreadEvent Event; //!< イベントオブジェクト
		tCollectorThread & Owner;
	public:

		//! @brief		コンストラクタ
		//! @param		owner		このオブジェクトを所有する tCollectorThread オブジェクト
		tThreadImpl(tCollectorThread & owner);

		//! @brief		デストラクタ
		~tThreadImpl();

		//! @brief		スレッドのエントリーポイント
		void Execute();

		//! @brief		スレッドをたたき起こす
		void Wakeup() { Event.Signal(); }
	};

	tThreadImpl *Thread; //!< コレクタスレッド

public:
	//! @brief		コンストラクタ
	tCollectorThread();

	//! @brief		デストラクタ
	~tCollectorThread();

private:
	//! @brief		ファイナライズすべきオブジェクトがあった場合に
	//!				GC から呼ばれるコールバック
	static void FinalizerNotifier();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		自分の消滅時に指定クラスのデストラクタを呼ぶだけのテンプレートクラス
//---------------------------------------------------------------------------
/*
	デストラクタの呼び出し規約を想定できないため、いったんこのクラスを挟む。
*/
class tDestructorCaller
{
public:
	virtual ~tDestructorCaller() {;}
};

template <typename T>
class tDestructorCaller_Impl : public tDestructorCaller
{
	T* Ptr;
public:
	tDestructorCaller_Impl(T * ptr) { Ptr = ptr; }
	virtual ~tDestructorCaller_Impl() { delete Ptr; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		メインスレッドでデストラクタが走ってほしい非GC対応オブジェクトを保持するキュー
//---------------------------------------------------------------------------
class tMainThreadDestructorQueue : public singleton_base<tMainThreadDestructorQueue>
{
	std::deque<tDestructorCaller*> Pointers; // 削除まちオブジェクトへのポインタの配列
	tCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション
public:

	//! @brief		コンストラクタ
	tMainThreadDestructorQueue() {;}

	//! @brief		デストラクタ
	~tMainThreadDestructorQueue();

	//! @brief		キューにデストラクタを登録
	//! @param		dtor		デストラクタ
	void Enqueue(tDestructorCaller * dtor);

	//! @brief		デストラクタを呼ぶ
	void CallDestructors();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		メインスレッドでデストラクタが走ってほしい非GC対応オブジェクトを保持する自動ポインタ。
//! @note		tDestructee 派生クラスなので注意。あとあんまり効率のよい実装ではない。
//---------------------------------------------------------------------------
template <typename T>
class tMainThreadAutoPtr : public tDestructee, depends_on<tMainThreadDestructorQueue>
{
	T* Pointer; //!< ポインタ
public:
	//! @brief コンストラクタ
	tMainThreadAutoPtr()
	{
		Pointer = NULL;
	}

	//! @brief コンストラクタ
	//! @param	ptr	ポインタ
	tMainThreadAutoPtr(T * ptr)
	{
		Pointer = ptr;
	}

	//! @brief		デストラクタ
	~tMainThreadAutoPtr()
	{
		// キューに Pointer のデストラクタを登録
		if(Pointer)
			tMainThreadDestructorQueue::instance()->Enqueue(new tDestructorCaller_Impl<T>(Pointer));
	}

	//! @brief		ポインタを設定する
	//! @param		ptr		ポインタ
	void set(T * ptr) { Pointer = ptr; }

	// ポインタへの変換
	T* get() const { return Pointer; }
	operator T* () const { return Pointer; }
	T* operator -> () const { return Pointer; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


#endif

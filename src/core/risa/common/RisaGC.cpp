//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ガーベジ・コレクション
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/common/RisaGC.h"

#include "risseGC.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(13921,35495,35132,18542,18346,21061,47449,62904);
//---------------------------------------------------------------------------
/*
	既存クラスの GC との相性について

	まず間違いなく、wxWidgets 由来のクラスは GC との相性が悪い。

	とくに、後述の、デストラクタが非同期に他のスレッドから呼ばれることがある
	(デストラクタをスレッド保護しないとならない) というのはたいていの場合問題
	になる。

	wxString は、内部の wxStringData に対する操作がスレッドセーフでないため、
	GC 管理下のヒープにおいてはならない。デストラクタを実行する時点で、その
	文字列あるいはその文字列と wxStringData インスタンスを共有している他の
	wxString インスタンスが同時にアクセスされないことを保証出来るのであれば
	問題ないが、どこで文字列を共有してしまったか、を追跡するのはおそらく
	困難なため、使用しないこと。

	同じ理由で、std::string も使用を控えるべきである。

	wxWidgets の wxString と相性がやや良い (といってもwxStringとのバッファ
	の共有はできないが)、wchar_t ベースの非常にシンプルな文字列クラスは
	risseWCString に定義されている。

	以下のクラスは特に意味のあるデストラクタを含んでいないため、
	このクラスのインスタンスを GC 管理下のヒープにおいても問題を起こさない。

	wxLongLong
	wxDateTime
*/


/*
	Risse の GC 、あるいはそこで使用されている Boehm GC とのインター
	フェースを提供する


	tCollectorThread は現在の所、ファイナライザをサブスレッドで実行する
	ためのものである。

	GC は GC_finalize_on_demand を 非0 にしない限り、コレクタ内から
	ファイナライザを呼ぶことになる。コレクタが実行されるのは GC_malloc (または
	その仲間) 内か、あるいは明示的に GC_gcollect() を呼んだときだから、
	メモリ確保が必要な任意のタイミングで発生する可能性がある。また、その
	メモリ確保を行おうとしたスレッドから呼ばれることになる。

	これはたとえば

	lock(A)
	  malloc()
	unlock(A)

	というメモリ確保を伴うルーチンに対し、同じロック A を使用するファイナライザ

	lock(A)
	  finalize()
	unlock(A)

	があった場合、上記 malloc 内でこのファイナライザが呼ばれるとデッドロック
	を起こすことになる。

	GC は GC_finalize_on_demand を 非0 にすると、ファイナライザが必要なオブジェ
	クトを見つけると GC_finalizer_notifier で指定された関数を呼び出し、その場
	ではファイナライザを呼ばない。

	この場合、実際に GC がファイナライザを呼ぶのは、明示的に GC_invoke_finalizers
	を呼んだ場合のみとなる。

	tCollectorThread は、GC_finalizer_notifier をフックし、これが呼ばれると
	専用のスレッドで GC_invoke_finalizers() を呼ぶようになる。これならば上記のよう
	なデッドロックの心配はない。

	結局、ファイナライザがメインスレッド以外から呼ばれることになるが、これは
	別に tCollectorThread を使わなくても同じ事である。ファイナライザの
	呼び出しを無期限に延期するわけにもいかないし、仕方がない。

	ファイナライザが呼ばれるスレッドはもっぱらこのスレッドになるが、このスレッド
	であると仮定してはならない (他のスレッドが資源の強制的な開放などの目的で
	GC_invoke_finalizers() を呼んだときはそのスレッドからの呼び出しとなる )
*/


//---------------------------------------------------------------------------
static volatile bool ThreadExpired = false; // コレクタスレッドがもう動作停止状態かどうか
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tCollectorThread::tThreadImpl::tThreadImpl(tCollectorThread & owner) : Owner(owner)
{
	Run(); // スレッドの実行を開始
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tCollectorThread::tThreadImpl::~tThreadImpl()
{
	Terminate(); // スレッドの終了を伝える
	Event.Signal(); // スレッドをたたき起こす
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCollectorThread::tThreadImpl::Execute()
{
	while(!ShouldTerminate())
	{
		// しばらくの間まつ。べつに何が起こるわけでもなく
		// これがタイムアウトしても問題ない。
		Event.Wait(10*1000); // 10秒待つ

		// コレクションを行うかも。

		// 実行すべきファイナライザがあればそれを実行する
		if(GC_should_invoke_finalizers())
		{
			fprintf(stderr, "finalizing...\n");
			fflush(stderr);
			GC_invoke_finalizers();
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tCollectorThread::tCollectorThread()
{
	// GC の初期化
	GC_init();

	// ファイナライザが勝手に走らないように
	GC_finalize_on_demand = 1;

	// ファイナライズすべきオブジェクトがあった場合に通知する関数を設定
	GC_finalizer_notifier = &FinalizerNotifier;

	// コレクタスレッドを起動
	Thread = new tThreadImpl(*this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tCollectorThread::~tCollectorThread()
{
	// スレッドはもう動作停止
	ThreadExpired = true;

	GC_finalizer_notifier = NULL;
	delete Thread, Thread = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCollectorThread::FinalizerNotifier()
{
	// スレッドはもう動作停止しているか？
	if(ThreadExpired) return; // 動作を停止しているので何もしない

	// コレクタスレッドをたたき起こす
	tCollectorThread * instance = tCollectorThread::instance();
	if(instance) tCollectorThread::instance()->Thread->Wakeup();
}
//---------------------------------------------------------------------------















//---------------------------------------------------------------------------
tMainThreadDestructorQueue::~tMainThreadDestructorQueue()
{
	// とりあえずデストラクタを呼んでみる
	CallDestructors();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tMainThreadDestructorQueue::Enqueue(tDestructorCaller * dtor)
{
	volatile tCriticalSection::tLocker cs_holder(CS);

fprintf(stderr, "enqueuing destructor %p ...\n", dtor);
fflush(stderr);

	if(Pointers.size() == 0) ::wxWakeUpIdle(); // アイドルイベントを起動する
	Pointers.push_back(dtor);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void tMainThreadDestructorQueue::CallDestructors()
{
	// このメソッドは、主にアプリケーションのidleループ中に呼ばれる
	while(true)
	{
		// キューから一個とってくる
		tDestructorCaller * caller;
		{
			volatile tCriticalSection::tLocker cs_holder(CS);
			if(Pointers.size() == 0) break;
			caller = Pointers.front();
			Pointers.pop_front();
		}

		// デストラクタを呼ぶために caller を delete する
fprintf(stderr, "calling destructor %p ...\n", caller);
fflush(stderr);

		delete caller;
	}
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risa


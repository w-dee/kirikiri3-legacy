//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コルーチンの実装
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseCharUtils.h"
#include "risseAssert.h"
#include "risseCoroutine.h"


namespace Risse
{
//---------------------------------------------------------------------------
RISSE_DEFINE_SOURCE_ID(26814,21547,55164,18773,58509,59432,16039,26450);
//---------------------------------------------------------------------------
}
/*
	コルーチンとBoehm GCの相性の改善

	各プラットフォーム用に、コルーチンと The Boehm-Demers-Weiser
	conservative garbage collector (Boehm GC あるいは GC) の相性を改善する
	ためのコードがここにある。

	W.Deeの日記より(一部改変)

		Boehm GCはマシンスタック上に置かれたポインタをスキャンします。実際
		は現在実行中のスレッドのマシンスタック上だけではなくて他のスレッド
		のマシンスタック、あとは保存されたCPUのレジスタも調べます。CPUのレ
		ジスタにポインタが入っている場合があるのでこれを見逃すわけにはいき
		ません。

		Boehm GCはどこにマシンスタックが配置されているかはBoehm GC自身で探
		すことができまし、マルチスレッドの場合は他のスレッドのマシンスタッ
		クも探し出すことができます(中にはスレッドライブラリにフックが必要な
		プラットフォームもあるようですが)。

		しかしFiberやmakecontext系、あるいはmallocなどで自前でマシンスタッ
		クを確保して、それらを切り替える系はさすがに考慮外といった感じです。

	したがって、Boehm GC にそのコンテキストと保存されているレジスタを教える
	方法を考える必要がある。

	今のところ boost.coroutineを使う方向でコーディングを行っている。
	boost.coroutine側には手を入れずにBoehm GCとの相性を改善する方向で実装を
	する。


	現状、このコードはGCより一部のコードを流用している。

	Copyright (c) 1988, 1989 Hans-J. Boehm, Alan J. Demers
	Copyright (c) 1991-1996 by Xerox Corporation.  All rights reserved.
	Copyright (c) 1996-1999 by Silicon Graphics.  All rights reserved.
	Copyright (c) 1999-2004 Hewlett-Packard Development Company, L.P.

	The file linux_threads.c is also
	Copyright (c) 1998 by Fergus Henderson.  All rights reserved.

	The files Makefile.am, and configure.in are
	Copyright (c) 2001 by Red Hat Inc. All rights reserved.

	Several files supporting GNU-style builds are copyrighted by the Free
	Software Foundation, and carry a different license from that given
	below.

	THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED
	OR IMPLIED.  ANY USE IS AT YOUR OWN RISK.

	Permission is hereby granted to use or copy this program
	for any purpose,  provided the above notices are retained on all copies.
	Permission to modify the code and to distribute modified code is granted,
	provided the above notices are retained, and a notice that the code was
	modified is included with the above copyright notice.

	A few of the files needed to use the GNU-style build procedure come with
	slightly different licenses, though they are all similar in spirit.  A few
	are GPL'ed, but with an exception that should cover all uses in the
	collector.  (If you are concerned about such things, I recommend you look
	at the notice in config.guess or ltmain.sh.)

*/

// ここのプリプロセッサ分岐については
// coroutines/detail/default_context_impl.hpp を参照のこと
#include "boost/config.hpp"

#if defined(__linux) || defined(linux) || defined(__linux__)

/*
	Linux/i386の場合:
		すべての保存すべきレジスタはスタック上に置かれるようである。
		このためスタックをBoehm GCに教えてあげればよい。スタックは
		posix::alloc_stack と posix::free_stack で確保されているので
		これを置き換えて管理すれば何とかなると思う。
*/

#elif  defined(_POSIX_VERSION)

/*
	未調査
*/

#elif defined(BOOST_WINDOWS)
//========================================================================
//                             Windowsの場合
//========================================================================


/*
	Windowsの場合:
		boost.coroutineはWindowsの場合、Fiberを用いている。Fiberからスタッ
		クの場所を得るためには Boehm GC のようにVirtualQueryを使って探す方
		法もあるが、ここではGetCurrentFiberの戻り値の構造体(内容の詳細は
		MSからは公式にはドキュメント化されていない)を見ることにする。
	TODO: Win9x でのチェック
*/

extern "C" {
#include "private/gc_priv.h"
}

namespace Risse {

struct fiber_data
{
	void * param; // fiber parameter
	void * except; // SEH exception list
	void * stack_base; // stack base address
	void * stack_limit; // stack limit address
	void * stack_allocation; // stack allocation address
	CONTEXT context;
};

typedef std::vector<fiber_data *> fibers_t;
static fibers_t fibers;
static tRisseCriticalSection *fibers_critical_section = NULL;

static void register_fiber(fiber_data * data)
{
	tRisseCriticalSection::tLocker lock(*fibers_critical_section);
	fibers.push_back(data);
}

static void unregister_fiber(fiber_data * data)
{
	tRisseCriticalSection::tLocker lock(*fibers_critical_section);
	fibers_t::iterator i = std::find(fibers.begin(), fibers.end(), data);
	RISSE_ASSERT(i != fibers.end());
	fibers.erase(i);
}


static void (*old_GC_push_other_roots)() = 0;

static void new_GC_push_other_roots()
{
	tRisseCriticalSection::tLocker lock(*fibers_critical_section);
	bool found_me = false;
	fiber_data * current_fiber = (fiber_data*)GetCurrentFiber();
	int dummy;
	for(fibers_t::iterator i = fibers.begin(); i != fibers.end(); i++)
	{
		fiber_data * data = *i;
		ptr_t sp, stack_min;

		if (data == current_fiber) {
			// これは現在のファイバである
			// スタックの先頭を得るためにローカル変数のアドレスを
			// 得る (ローカル変数はスタック上に配置されるため)
			sp = (ptr_t) &dummy;
			found_me = true;
		} else {
			CONTEXT &context = data->context;

//---------------------------------------------------------------------
// ここの内容は GC の win32_threads.c の GC_push_all_stacks()
// と同じである。

        /* Push all registers that might point into the heap.  Frame	*/
        /* pointer registers are included in case client code was	*/
        /* compiled with the 'omit frame pointer' optimisation.		*/
#       define PUSH1(reg) GC_push_one((word)context.reg)
#       define PUSH2(r1,r2) PUSH1(r1), PUSH1(r2)
#       define PUSH4(r1,r2,r3,r4) PUSH2(r1,r2), PUSH2(r3,r4)
#       if defined(I386)
          PUSH4(Edi,Esi,Ebx,Edx), PUSH2(Ecx,Eax), PUSH1(Ebp);
	  sp = (ptr_t)context.Esp;
#       elif defined(ARM32)
	  PUSH4(R0,R1,R2,R3),PUSH4(R4,R5,R6,R7),PUSH4(R8,R9,R10,R11),PUSH1(R12);
	  sp = (ptr_t)context.Sp;
#       elif defined(SHx)
	  PUSH4(R0,R1,R2,R3), PUSH4(R4,R5,R6,R7), PUSH4(R8,R9,R10,R11);
	  PUSH2(R12,R13), PUSH1(R14);
	  sp = (ptr_t)context.R15;
#       elif defined(MIPS)
	  PUSH4(IntAt,IntV0,IntV1,IntA0), PUSH4(IntA1,IntA2,IntA3,IntT0);
	  PUSH4(IntT1,IntT2,IntT3,IntT4), PUSH4(IntT5,IntT6,IntT7,IntS0);
	  PUSH4(IntS1,IntS2,IntS3,IntS4), PUSH4(IntS5,IntS6,IntS7,IntT8);
	  PUSH4(IntT9,IntK0,IntK1,IntS8);
	  sp = (ptr_t)context.IntSp;
#       elif defined(PPC)
	  PUSH4(Gpr0, Gpr3, Gpr4, Gpr5),  PUSH4(Gpr6, Gpr7, Gpr8, Gpr9);
	  PUSH4(Gpr10,Gpr11,Gpr12,Gpr14), PUSH4(Gpr15,Gpr16,Gpr17,Gpr18);
	  PUSH4(Gpr19,Gpr20,Gpr21,Gpr22), PUSH4(Gpr23,Gpr24,Gpr25,Gpr26);
	  PUSH4(Gpr27,Gpr28,Gpr29,Gpr30), PUSH1(Gpr31);
	  sp = (ptr_t)context.Gpr1;
#       elif defined(ALPHA)
	  PUSH4(IntV0,IntT0,IntT1,IntT2), PUSH4(IntT3,IntT4,IntT5,IntT6);
	  PUSH4(IntT7,IntS0,IntS1,IntS2), PUSH4(IntS3,IntS4,IntS5,IntFp);
	  PUSH4(IntA0,IntA1,IntA2,IntA3), PUSH4(IntA4,IntA5,IntT8,IntT9);
	  PUSH4(IntT10,IntT11,IntT12,IntAt);
	  sp = (ptr_t)context.IntSp;
#       else
#         error "architecture is not supported"
#       endif
//---------------------------------------------------------------------

		}

		stack_min = (ptr_t)(data->stack_limit);

		if (sp >= stack_min && sp < (ptr_t)data->stack_base)
			GC_push_all_stack(sp, (ptr_t)data->stack_base);
		else {
			WARN("Fiber stack pointer 0x%lx out of range, pushing everything\n",
				 (unsigned long)sp);
			GC_push_all_stack(stack_min, (ptr_t)data->stack_base);
		}
/*
	void * high = data->stack_base;
	void * low = data->stack_limit; // stack_allocation ではなくて？
	if(high < low) std::swap(high, low);

	GC_push_all_stack(reinterpret_cast<char*>(low), reinterpret_cast<char*>(high));

	GC_push_all(reinterpret_cast<char*>(&data->context),
		(char*)(&data->context) + sizeof(CONTEXT)); // コンテキスト
*/
	} // for

	if (!found_me)
	{
		// 現在これを実行しているのがファイバの場合は自分のファイバ
		// が見つかっていなければおかしい
		void * current = GetCurrentFiber();
		// この 0x1e00 の意味については
		// boost-coroutine/boost/coroutine/detail/context_windows.hpp
		// を参照のこと
		if(current != 0 && current != reinterpret_cast<void *>(0x1E00))
			ABORT("Collecting from unknown fiber.");
	}

	old_GC_push_other_roots();
}

static void init_marker()
{
	// GC のマーキングのプロセスで上記の関数が実行されるように
	// フックを行う
	// このメソッドは GC_init の直後に呼ばれなければならない
	old_GC_push_other_roots = GC_push_other_roots;
	GC_push_other_roots = new_GC_push_other_roots;

	// ファイバのリストを管理するクリティカルセクションを作成する
	// このオブジェクトはプログラム内では開放されない
	fibers_critical_section = new tRisseCriticalSection();
}


WINBASEAPI BOOL WINAPI RISSE_NEW_ConvertFiberToThread(void)
{
	// スレッドであれはGCが探すことができるハズなので
	// スレッドに変換する前に登録を解除する
	unregister_fiber(reinterpret_cast<fiber_data*>(GetCurrentFiber()));
	return ConvertFiberToThread();
}

WINBASEAPI PVOID WINAPI RISSE_NEW_ConvertThreadToFiber(PVOID arg1)
{
	// ファイバに変換される場合は登録する
	PVOID ret = ConvertThreadToFiber(arg1);
	if(ret) register_fiber(reinterpret_cast<fiber_data*>(ret));
	return ret;
}

WINBASEAPI LPVOID WINAPI RISSE_NEW_CreateFiber(
	SIZE_T arg1, LPFIBER_START_ROUTINE arg2, LPVOID arg3)
{
	// ファイバを登録する
	PVOID ret = CreateFiber(arg1, arg2, arg3);
	if(ret) register_fiber(reinterpret_cast<fiber_data*>(ret));
	return ret;
}

WINBASEAPI void WINAPI RISSE_NEW_DeleteFiber(PVOID arg1)
{
	// ファイバの登録を解除する
	unregister_fiber(reinterpret_cast<fiber_data*>(arg1));
	DeleteFiber(arg1);
}

// 関数の置き換えを宣言する
#define ConvertFiberToThread Risse::RISSE_NEW_ConvertFiberToThread
#define ConvertThreadToFiber Risse::RISSE_NEW_ConvertThreadToFiber
#define CreateFiber Risse::RISSE_NEW_CreateFiber
#define DeleteFiber Risse::RISSE_NEW_DeleteFiber


} // namespace Risse

//========================================================================
//                        Windowsの場合 - 終わり
//========================================================================



#else

#error No Boehm GC+boost.coroutine support available for this system

#endif




// boost.coroutine の include
// これは、関数の置き換えを #define で行っているため、この位置で
// include しなければならない
#include "boost/coroutine/coroutine.hpp"
namespace coro = boost::coroutines;








namespace Risse
{

//---------------------------------------------------------------------------
void RisseInitCoroutine()
{
	init_marker();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @param		コルーチンの本当の実装クラス
//---------------------------------------------------------------------------
class tRisseCoroutineImpl : public tRisseDestructee /* デストラクタが呼ばれなければならない */
{
public:
	typedef coro::coroutine<tRisseVariant (tRisseCoroutineImpl * coro, const tRisseVariant &)> coroutine_type;
	coroutine_type Coroutine;

	tRisseCoroutine * RisseCoroutine;
	coroutine_type::self * CoroutineSelf;

	tRisseCoroutineImpl(tRisseCoroutine * risse_coroutine) : Coroutine(Body)
	{
		RisseCoroutine = risse_coroutine;
		CoroutineSelf = NULL;
	}

private:
	static tRisseVariant Body(coroutine_type::self &self, tRisseCoroutineImpl * coro, const tRisseVariant & arg)
	{
		coro->CoroutineSelf = &self;
		tRisseVariant ret;
		coro->RisseCoroutine->Function.FuncCall(
			coro->RisseCoroutine->Engine, &ret, tRisseString::GetEmptyString(), 0,
			tRisseMethodArgument::New(coro->RisseCoroutine->FunctionArg, arg));
		coro->CoroutineSelf = NULL;
		return ret;
	}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRisseCoroutine::tRisseCoroutine(tRisseScriptEngine * engine,
	const tRisseVariant & function, const tRisseVariant arg)
{
	Engine = engine;
	Function = function;
	FunctionArg = arg;
	Impl = new tRisseCoroutineImpl(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseCoroutine::Run(const tRisseVariant &arg)
{
	return Impl->Coroutine(Impl, arg);
}
//---------------------------------------------------------------------------

	//! @brief		コルーチンからyieldする
	//! @param		arg		Run() メソッドの戻り値となる値
	//! @return		Run() メソッドの引数
//---------------------------------------------------------------------------
tRisseVariant tRisseCoroutine::DoYield(const tRisseVariant &arg)
{
	return Impl->CoroutineSelf->yield(arg).get<1>();
		// yield の戻りはこの場合tupleになるので、2番目の値を取り出す
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseCoroutine::GetAlive() const
{
	return Impl->CoroutineSelf != NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCoroutine::Exit()
{
	Impl->Coroutine.exit();
	Impl->CoroutineSelf = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse


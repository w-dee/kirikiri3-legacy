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
#include "risseExceptionClass.h"


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
#include "gc_backptr.h"
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


#ifdef RISSE_TRACK_FIBERS
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
#endif

WINBASEAPI LPVOID WINAPI RISSE_NEW_CreateFiber(
	SIZE_T arg1, LPFIBER_START_ROUTINE arg2, LPVOID arg3)
{
	// ファイバを作成する。
	// 実はどうも Fiber は結構限りのある資源のようで、XP SP2では
	// 簡単な実験では500個弱しか確保できないことがわかった。
	// ここでは作成に失敗した場合にコレクションを行うことでなんとかしてみる。
	for(int i = 0; i < 3; i++) // 3回ほどリトライ
	{
		PVOID ret = CreateFiber(arg1, arg2, arg3);
		if(ret)
		{
			// 成功
#ifdef RISSE_TRACK_FIBERS
			register_fiber(reinterpret_cast<fiber_data*>(ret));
#endif
			return ret;
		}
		GC_gcollect(); // コレクトしてみる
		GC_invoke_finalizers(); // ファイナライザを呼び出してみる
#ifdef GC_DEBUG
		GC_dump();
		GC_generate_random_backtrace();
#endif
#ifdef RISSE_COROUTINE_DEBUG
		fflush(stdout); fflush(stderr);
		fprintf(stderr, "CreateFiber failed. fibers=%d, retried (%d)\n", (int)fibers.size(), (int)(i+1));
		fflush(stdout); fflush(stderr);
#endif
	}
	tRisseInsufficientResourceExceptionClass::ThrowCouldNotCreateCoroutine();
	return NULL; // 失敗...
}

#ifdef RISSE_TRACK_FIBERS
WINBASEAPI void WINAPI RISSE_NEW_DeleteFiber(PVOID arg1)
{
	// ファイバの登録を解除する
	unregister_fiber(reinterpret_cast<fiber_data*>(arg1));
	DeleteFiber(arg1);
}
#endif

// 関数の置き換えを宣言する
#ifdef RISSE_TRACK_FIBERS
	#define ConvertFiberToThread Risse::RISSE_NEW_ConvertFiberToThread
	#define ConvertThreadToFiber Risse::RISSE_NEW_ConvertThreadToFiber
#endif
#define CreateFiber Risse::RISSE_NEW_CreateFiber
#ifdef RISSE_TRACK_FIBERS
	#define DeleteFiber Risse::RISSE_NEW_DeleteFiber
#endif

// typedef など
typedef fiber_data tRisseCoroutineContext;

//! @brief 現在実行中のコルーチンコンテキストを得る
tRisseCoroutineContext * RisseGetCurrentCoroutineContext()
{
	tRisseCoroutineContext * p = (tRisseCoroutineContext*)GetCurrentFiber();
#ifdef RISSE_COROUTINE_DEBUG
	fflush(stdout); fflush(stderr);
	fprintf(stdout, "fiber context %p created\n", p);
	fflush(stdout); fflush(stderr);
#endif

	RISSE_ASSERT(p != reinterpret_cast<void *>(0x1E00));
	return p;
}

//! @brief コルーチンコンテキストの内容をGCに対してプッシュする
struct GC_ms_entry *RisseMarkCoroutineContext(
									tRisseCoroutineContext * co_context,
									struct GC_ms_entry *mark_sp,
									struct GC_ms_entry *mark_sp_limit)
{
	CONTEXT &context = co_context->context;

	ptr_t sp, stack_min, stack_max;

//---------------------------------------------------------------------
// ここの内容は GC の win32_threads.c の GC_push_all_stacks()
// と同じである。

        /* Push all registers that might point into the heap.  Frame	*/
        /* pointer registers are included in case client code was	*/
        /* compiled with the 'omit frame pointer' optimisation.		*/
#       define PUSH1(reg) mark_sp = GC_MARK_AND_PUSH((GC_PTR)context.reg, mark_sp, mark_sp_limit, (GC_PTR*)co_context)
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

	// スタックの push
	stack_min = (ptr_t)(co_context->stack_limit);
	stack_max = (ptr_t)co_context->stack_base;
	if (sp >= stack_min && sp < (ptr_t)co_context->stack_base)
		stack_min = sp;
	else {
		WARN("Fiber stack pointer 0x%lx out of range, pushing everything\n",
			 (unsigned long)sp);
	}

	if(stack_min > stack_max) std::swap(stack_min, stack_max);

	// ptr_t は char* で、GC_PTR は void * であることに注意
//	fprintf(stderr, "pushing from %p to %p (%d words)\n", stack_min, stack_max, ((long)stack_max - (long)stack_min)/sizeof(void*));

	for(ptr_t p = stack_min; p < stack_max; p += sizeof(GC_PTR) / sizeof(*p))
	{
		mark_sp = GC_MARK_AND_PUSH((GC_PTR)(*(void**)p), mark_sp, mark_sp_limit, (GC_PTR*)co_context);
	}

	return mark_sp;
}



} // namespace Risse

//========================================================================
//                        Windowsの場合 - 終わり
//========================================================================



#else

#error No Boehm GC+boost.coroutine support available for this system

#endif




// boost.coroutine の include
// これは、関数の置き換えを #define で行っているためで、この位置で
// include しなければならない
#include "boost/coroutine/coroutine.hpp"
namespace coro = boost::coroutines;





namespace Risse
{

//---------------------------------------------------------------------------
void RisseInitCoroutine()
{
#ifdef RISSE_TRACK_FIBERS
	fibers_critical_section = new tRisseCriticalSection();
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @param		コルーチンの本当の実装クラス
//! @note		注意: このクラスのデストラクタはメインスレッド以外から非同期
//!				に呼ばれる可能性があることに注意
//---------------------------------------------------------------------------
class tRisseCoroutineImpl : public tRisseDestructee /* デストラクタが呼ばれなければならない */
{
public:
	typedef coro::coroutine<tRisseVariant (tRisseCoroutineImpl * coroimpl, tRisseCoroutine * coro, tRisseVariant)> coroutine_type;
	coroutine_type Coroutine;

	coroutine_type::self * CoroutineSelf;
	tRisseCoroutineContext * Context;
	bool Alive;
	bool Running;

	tRisseCoroutineImpl() : Coroutine(Body)
	{
		CoroutineSelf = NULL;
		Context = NULL;
		Alive = true;
		Running = false;
	}

	~tRisseCoroutineImpl()
	{
#ifdef RISSE_COROUTINE_DEBUG
		fflush(stdout); fflush(stderr);
		fprintf(stdout, "tRisseCoroutineImpl %p destructed\n", this);
		fflush(stdout); fflush(stderr);
#endif
	}

#ifdef RISSE_COROUTINE_DEBUG
	void Dump()
	{
		fflush(stdout); fflush(stderr);
		fprintf(stdout, "tRisseCoroutineImpl %p : Alive(%s), Running(%s)\n", this, Alive?"true":"false", Running?"true":"false");
		fflush(stdout); fflush(stderr);
	}
#endif

private:
	static tRisseVariant Body(coroutine_type::self &self, tRisseCoroutineImpl * coroimpl, tRisseCoroutine * coro, tRisseVariant arg)
	{
		coroimpl->CoroutineSelf = &self;
		coroimpl->Context = RisseGetCurrentCoroutineContext();
		tRisseVariant ret;

		try
		{
			if(!coro->Function.IsNull())
			{
				coro->Function.FuncCall(
					coro->Engine, &ret, tRisseString::GetEmptyString(), 0,
					tRisseMethodArgument::New(arg, coro->FunctionArg),
					coro->FunctionArg);
			}
		}
		catch(const tRisseVariant * e)
		{
#ifdef RISSE_COROUTINE_DEBUG
			fflush(stdout); fflush(stderr);
			fprintf(stdout, "tRisseCoroutineImpl %p caught an exception tRisseVariant: ", coroimpl);
			RisseFPrint(stdout, e->operator tRisseString().c_str());
			RisseFPrint(stdout, RISSE_WS("\n"));
			coroimpl->Dump();
			fflush(stdout); fflush(stderr);
#endif
#ifdef RISSE_COROUTINE_DEBUG
			fflush(stdout); fflush(stderr);
			fprintf(stdout, "tRisseCoroutineImpl %p finished\n", coroimpl);
			coroimpl->Dump();
			fflush(stdout); fflush(stderr);
#endif
			coroimpl->Alive = false;
			coroimpl->Running = false;
			coro->ExceptionValue = e;
			coroimpl->Context = NULL;
			throw;
		}
		catch(...)
		{
#ifdef RISSE_COROUTINE_DEBUG
			fflush(stdout); fflush(stderr);
			fprintf(stdout, "tRisseCoroutineImpl %p caught an unknown exception\n", coroimpl);
			coroimpl->Dump();
			fflush(stdout); fflush(stderr);
#endif
#ifdef RISSE_COROUTINE_DEBUG
			fflush(stdout); fflush(stderr);
			fprintf(stdout, "tRisseCoroutineImpl %p finished\n", coroimpl);
			fflush(stdout); fflush(stderr);
#endif
			coroimpl->Alive = false;
			coroimpl->Running = false;
			coro->ExceptionValue = NULL;
			coroimpl->Context = NULL;
			throw;
		}

#ifdef RISSE_COROUTINE_DEBUG
			fflush(stdout); fflush(stderr);
			fprintf(stdout, "tRisseCoroutineImpl %p finished\n", coroimpl);
			coroimpl->Dump();
			fflush(stdout); fflush(stderr);
#endif
		coroimpl->Alive = false;
		coroimpl->Running = false;
		coroimpl->CoroutineSelf = NULL;
		coroimpl->Context = NULL;
		return ret;
	}
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tRisseCoroutineImpl へのポインタを含む特殊な構造体
//---------------------------------------------------------------------------
/*
	この構造体は GC_generic_malloc により割り当てられ、特殊なマーカーである
	RisseMarkCoroutinePtr によりスキャンされる。このマーカーは コルーチンのRisse用
	実装クラスである tRisseCoroutineImpl のポインタと、それが持つコルーチンの
	コンテキスト情報中の有用そうなポインタをマークする。
*/

// forward declaration
struct GC_ms_entry *RisseMarkCoroutinePtr(GC_word *addr,
						struct GC_ms_entry *mark_sp,
						struct GC_ms_entry *mark_sp_limit,
						GC_word env);

//! @brief	GC用kind情報を保持する構造体
struct tRisseCoroutineGCKind : public tRisseCollectee
{
	void ** FreeList;
	int MarkProc;
	int Kind;

	tRisseCoroutineGCKind()
	{
		FreeList = GC_new_free_list();
		MarkProc = GC_new_proc(RisseMarkCoroutinePtr);
		Kind = GC_new_kind(FreeList,
						GC_MAKE_PROC(MarkProc, 0), 0, 0);
	}
};

//! @brief tRisseCoroutineImpl へのポインタを含む特殊な構造体
struct tRisseCoroutinePtr
{
	static tRisseCoroutineGCKind * Kind;
	tRisseCoroutineImpl * Impl; //!< tRisseCoroutineImpl へのポインタ

	//! @brief		コンストラクタ
	tRisseCoroutinePtr()
	{
		Impl = new tRisseCoroutineImpl();
	}

	//! @brief		new 演算子
	void * operator new(size_t size)
	{
		// GC_generic_malloc を使用してメモリを割り当てる

		if(!Kind)
		{
			// スレッド保護はしない。運が悪いとこのインスタンスが
			// 複数個作られることになるが、最終的にはどれかが使われる。
			// 非常に楽観的。
			Kind = new tRisseCoroutineGCKind();
		}

		return GC_generic_malloc(size, Kind->Kind);
	}

	void * operator new [] (size_t size); // not yet implemented
};

tRisseCoroutineGCKind * tRisseCoroutinePtr::Kind = NULL;


//! @brief		tRisseCoroutinePtr 用のマーク関数
struct GC_ms_entry *RisseMarkCoroutinePtr(GC_word *addr,
                                  struct GC_ms_entry *mark_sp,
                                  struct GC_ms_entry *mark_sp_limit,
                                  GC_word env)
{
	tRisseCoroutinePtr * a = (tRisseCoroutinePtr*)addr;

	if(a->Impl)
	{
#ifdef RISSE_COROUTINE_DEBUG
		fflush(stdout); fflush(stderr);
		fprintf(stdout, "marking Ptr %p, tRisseCoroutineImpl %p\n", a, a->Impl);
		a->Impl->Dump();
		fflush(stdout); fflush(stderr);
#endif

		// a->Impl を push する
		mark_sp = GC_MARK_AND_PUSH((GC_PTR)a->Impl, mark_sp, mark_sp_limit, (void**)addr);

		if(a->Impl->Context)
		{
			// Context の内容 (スタックエリアだとかCPUレジスタだとか) を push する
			mark_sp = RisseMarkCoroutineContext(a->Impl->Context, mark_sp, mark_sp_limit);
		}
	}

	return mark_sp;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tRisseCoroutine::tRisseCoroutine(tRisseScriptEngine * engine,
	const tRisseVariant & function, const tRisseVariant arg)
{
#ifdef RISSE_COROUTINE_DEBUG
	fflush(stdout); fflush(stderr);
	fprintf(stdout, "made tRisseCoroutine %p\n", this);
	fflush(stdout); fflush(stderr);
#endif

	Engine = engine;
	Function = function;
	FunctionArg = arg;

	Ptr = new tRisseCoroutinePtr();

#ifdef RISSE_COROUTINE_DEBUG
	fflush(stdout); fflush(stderr);
	fprintf(stdout, "made Ptr %p, tRisseCoroutineImpl %p\n", Ptr, Ptr->Impl);
	fflush(stdout); fflush(stderr);
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseCoroutine::Resume(const tRisseVariant &arg)
{
	if(!GetAlive())
	{
		// コルーチンは無効
		tRisseCoroutineExceptionClass::ThrowCoroutineHasAlreadyExited();
	}

	if(Ptr->Impl->Running)
	{
		// コルーチンはすでに実行中
		tRisseCoroutineExceptionClass::ThrowCoroutineIsRunning();
	}

	Ptr->Impl->Running = true;
	tRisseVariant ret;
	try
	{
		// コルーチンの実行
		ret = Ptr->Impl->Coroutine(Ptr->Impl, this, arg);
	}
	catch(coro::abnormal_exit & e)
	{
		// コルーチン中で例外が発生した場合はこれ。
		Ptr->Impl->Running = false;
		throw ExceptionValue;
	}
	catch(...)
	{
		Ptr->Impl->Running = false;
		throw;
	}
	Ptr->Impl->Running = false;
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseCoroutine::DoYield(const tRisseVariant &arg)
{
#ifdef RISSE_COROUTINE_DEBUG
	fflush(stdout); fflush(stderr);
	fprintf(stdout, "in tRisseCoroutine::DoYield: tRisseCoroutineImpl %p: Alive(%s), Running(%s)\n", Ptr->Impl, Ptr->Impl->Alive?"true":"false", Ptr->Impl->Running?"true":"false");
	fflush(stdout); fflush(stderr);
#endif

	if(!GetAlive())
	{
		// コルーチンは無効
		tRisseCoroutineExceptionClass::ThrowCoroutineHasAlreadyExited();
	}

	if(!Ptr->Impl->CoroutineSelf)
	{
		// コルーチンは開始していない
		tRisseCoroutineExceptionClass::ThrowCoroutineHasNotStartedYet();
	}

	if(!Ptr->Impl->Running)
	{
		// コルーチンは待ちの状態 (実行中でない)
		tRisseCoroutineExceptionClass::ThrowCoroutineIsNotRunning();
	}

	return Ptr->Impl->CoroutineSelf->yield(arg).get<2>();
		// yield の戻りはこの場合tupleになるので、3番目の値を取り出す
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseCoroutine::GetAlive() const
{
	return Ptr && Ptr->Impl->Alive;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCoroutine::Dispose()
{
	if(GetAlive())
	{
		if(Ptr->Impl->Running)
		{
			// コルーチンはすでに実行中
			// 実行中のコルーチンは dispose できない
			tRisseCoroutineExceptionClass::ThrowCoroutineIsRunning();
		}
		delete Ptr->Impl;
		Ptr = NULL; // Ptr のデストラクタは今のところ呼んではならない
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risse


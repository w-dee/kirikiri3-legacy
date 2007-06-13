//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ユーティリティ
//---------------------------------------------------------------------------
#ifndef risseUtilsH
#define risseUtilsH

#include "risseConfig.h"
#include "risseGC.h"

//---------------------------------------------------------------------------
// クリティカルセクション
//---------------------------------------------------------------------------

/*
	Risse のクリティカルセクションは、再突入可能 ( recursive ) であることが
	保証できなくてはならない。
	この条件を満たすのは Win32 のクリティカルセクションや boost の
	recursive_mutex で、wxWidgets の wxCriticalSection はこの保証がないので
	使えない。
*/

#if _WIN32
	// Windows プラットフォームの場合

	#ifdef _MSC_VER
	#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>

	namespace Risse
	{
	//! @brief クリティカルセクションの実装
	class tRisseCriticalSection : public tRisseDestructee
	{
		CRITICAL_SECTION CS; //!< Win32 クリティカルセクションオブジェクト
	public:
		tRisseCriticalSection() { InitializeCriticalSection(&CS); } //!< コンストラクタ
		~tRisseCriticalSection() { DeleteCriticalSection(&CS); } //!< デストラクタ

	private:
		tRisseCriticalSection(const tRisseCriticalSection &); // non-copyable

	private:
		void Enter() { EnterCriticalSection(&CS); } //!< クリティカルセクションに入る
		void Leave() { LeaveCriticalSection(&CS); } //!< クリティカルセクションから出る

	public:
		class tLocker : public tRisseCollectee
		{
			tRisseCriticalSection & CS;
		public:
			tLocker(tRisseCriticalSection & cs) : CS(cs)
			{
				CS.Enter();
			}
			~tLocker()
			{
				CS.Leave();
			}
		private:
			tLocker(const tLocker &); // non-copyable
		};
	};

	} // namespace Risse
#else

	// boost の recursive_mutex をつかう

	#include <boost/thread.hpp>

	namespace Risse
	{
	//! @brief クリティカルセクションの実装
	class tRisseCriticalSection : public tRisseCollectee
	{
		boost::recursive_mutex mutex; //!< boost::recursive_mutex mutexオブジェクト
	public:
		tRisseCriticalSection() { ; } //!< コンストラクタ
		~tRisseCriticalSection() { ; } //!< デストラクタ

	private:
		tRisseCriticalSection(const tRisseCriticalSection &); // non-copyable

	public:
		class tLocker : public tRisseCollectee
		{
			boost::recursive_mutex::scoped_lock lock;
		public:
			tLocker(tRisseCriticalSection & cs) : lock(cs.mutex) {;}
		private:
			tLocker(const tLocker &); // non-copyable
		};
	};

	} // namespace Risse

#endif
//---------------------------------------------------------------------------

#if 0



namespace Risse
{


//---------------------------------------------------------------------------
// tRisseAtExit / tRisseAtStart
//---------------------------------------------------------------------------
class tRisseAtExit
{
	void (*Function)();
public:
	tRisseAtExit(void (*func)()) { Function = func; };
	~tRisseAtExit() { Function(); }
};
//---------------------------------------------------------------------------
class tRisseAtStart
{
public:
	tRisseAtStart(void (*func)()) { func(); };
};
//---------------------------------------------------------------------------
class iRisseDispatch2;
extern iRisseDispatch2 * RisseObjectTraceTarget;

#define RISSE_DEBUG_REFERENCE_BREAK \
	if(RisseObjectTraceTarget == (iRisseDispatch2*)this) RisseNativeDebuggerBreak()
#define RISSE_SET_REFERENCE_BREAK(x) RisseObjectTraceTarget=(x)
//---------------------------------------------------------------------------
const risse_char *RisseVariantTypeToTypeString(tRisseVariantType type);
	// convert given variant type to type string ( "void", "int", "object" etc.)

tRisseString RisseVariantToReadableString(const tRisseVariant &val, risse_int maxlen = 512);
	// convert given variant to human-readable string
	// ( eg. "(string)\"this is a\\nstring\"" )
tRisseString RisseVariantToExpressionString(const tRisseVariant &val);
	// convert given variant to string which can be interpret as an expression.
	// this function does not convert objects ( returns empty string )

//---------------------------------------------------------------------------



/*[*/
//---------------------------------------------------------------------------
// tRisseRefHolder : a object holder for classes that has AddRef and Release methods
//---------------------------------------------------------------------------
template <typename T>
class tRisseRefHolder
{
private:
	T *Object;
public:
	tRisseRefHolder(T * ref) { Object = ref; Object->AddRef(); }
	tRisseRefHolder(const tRisseRefHolder<T> &ref)
	{
		Object = ref.Object;
		Object->AddRef();
	}
	~tRisseRefHolder() { Object->Release(); }

	T* GetObject() { Object->AddRef(); return Object; }
	T* GetObjectNoAddRef() { return Object; }

	const tRisseRefHolder & operator = (const tRisseRefHolder & rhs)
	{
		if(rhs.Object != Object)
		{
			Object->Release();
			Object = rhs.Object;
			Object->AddRef();
		}
		return *this;
	}
};



/*]*/

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// RisseAlignedAlloc : aligned memory allocater
//---------------------------------------------------------------------------
void *RisseAlignedAlloc(risse_uint bytes, risse_uint align_bits);
void RisseAlignedDealloc(void *ptr);
//---------------------------------------------------------------------------
}

#endif


#endif


//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief ユーティリティ
//---------------------------------------------------------------------------
#ifndef risseUtilsH
#define risseUtilsH

#include "risseConfig.h"
#include "risseVariant.h"
#include "risseString.h"

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
	class tRisseCriticalSection
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
		class tLocker
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
	class tRisseCriticalSection
	{
		boost::recursive_mutex mutex; //!< boost::recursive_mutex mutexオブジェクト
	public:
		tRisseCriticalSection() { ; } //!< コンストラクタ
		~tRisseCriticalSection() { ; } //!< デストラクタ

	private:
		tRisseCriticalSection(const tRisseCriticalSection &); // non-copyable

	public:
		class tLocker
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

/*[*/
//---------------------------------------------------------------------------
// floating-point class checker
//---------------------------------------------------------------------------
// constants used in RisseGetFPClass
#define RISSE_FC_CLASS_MASK 7
#define RISSE_FC_SIGN_MASK 8

#define RISSE_FC_CLASS_NORMAL 0
#define RISSE_FC_CLASS_NAN 1
#define RISSE_FC_CLASS_INF 2

#define RISSE_FC_IS_NORMAL(x)  (((x)&RISSE_FC_CLASS_MASK) == RISSE_FC_CLASS_NORMAL)
#define RISSE_FC_IS_NAN(x)  (((x)&RISSE_FC_CLASS_MASK) == RISSE_FC_CLASS_NAN)
#define RISSE_FC_IS_INF(x)  (((x)&RISSE_FC_CLASS_MASK) == RISSE_FC_CLASS_INF)

#define RISSE_FC_IS_NEGATIVE(x) ((bool)((x) & RISSE_FC_SIGN_MASK))
#define RISSE_FC_IS_POSITIVE(x) (!RISSE_FC_IS_NEGATIVE(x))


/*]*/
risse_uint32 RisseGetFPClass(risse_real r);
//---------------------------------------------------------------------------
}

#endif





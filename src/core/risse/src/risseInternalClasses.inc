// このファイルは risseScriptEngine.h / risseScriptEngine.cpp
// から複数回呼ばれる。
// どのようにしてこのファイルが呼ばれているかは 上記ファイルを
// 参照のこと。


#ifdef RISSE_INTERNALCLASSES_INCLUDE
// ここの部分は risseScriptEngine.cpp から呼ばれ、各クラスを
// new するのに必要なインクルードファイルをインクルードするために使われる。

#include "risseObjectClass.h"
#include "risseModuleClass.h"
#include "risseClassClass.h"
#include "risseFunctionClass.h"
#include "rissePropertyClass.h"
#include "risseArrayClass.h"
#include "risseStringClass.h"
#include "rissePrimitiveClass.h"
#include "risseNumberClass.h"
#include "risseIntegerClass.h"
#include "risseRealClass.h"
#include "risseCoroutineClass.h"
#include "risseExceptionClass.h"
#include "risseBindingClass.h"


#else

// ここの部分は RISSE_INTERNALCLASSES_CLASS( ) 内にクラス名を
// 列挙する方法で、組み込みクラスを定義する。
// ここに書いた順番でクラスの初期化が行われることに注意。

RISSE_INTERNALCLASSES_CLASS(Object                          )
RISSE_INTERNALCLASSES_CLASS(Binding                         )
RISSE_INTERNALCLASSES_CLASS(Module                          )
RISSE_INTERNALCLASSES_CLASS(Class                           )
RISSE_INTERNALCLASSES_CLASS(Function                        )
RISSE_INTERNALCLASSES_CLASS(Property                        )
RISSE_INTERNALCLASSES_CLASS(Array                           )
RISSE_INTERNALCLASSES_CLASS(Primitive                       )
RISSE_INTERNALCLASSES_CLASS(String                          )
RISSE_INTERNALCLASSES_CLASS(Number                          )
RISSE_INTERNALCLASSES_CLASS(Integer                         )
RISSE_INTERNALCLASSES_CLASS(Real                            )
RISSE_INTERNALCLASSES_CLASS(Coroutine                       )
RISSE_INTERNALCLASSES_CLASS(SourcePoint                     )
RISSE_INTERNALCLASSES_CLASS(Throwable                       )
RISSE_INTERNALCLASSES_CLASS(Error                           )
RISSE_INTERNALCLASSES_CLASS(AssertionError                  )
RISSE_INTERNALCLASSES_CLASS(Exception                       )
RISSE_INTERNALCLASSES_CLASS(InsufficientResourceException   )
RISSE_INTERNALCLASSES_CLASS(IOException                     )
RISSE_INTERNALCLASSES_CLASS(CharConversionException         )
RISSE_INTERNALCLASSES_CLASS(RuntimeException                )
RISSE_INTERNALCLASSES_CLASS(CompileException                )
RISSE_INTERNALCLASSES_CLASS(ClassDefinitionException        )
RISSE_INTERNALCLASSES_CLASS(InstantiationException          )
RISSE_INTERNALCLASSES_CLASS(UnsupportedOperationException   )
RISSE_INTERNALCLASSES_CLASS(BadContextException             )
RISSE_INTERNALCLASSES_CLASS(MemberAccessException           )
RISSE_INTERNALCLASSES_CLASS(NoSuchMemberException           )
RISSE_INTERNALCLASSES_CLASS(ArgumentException               )
RISSE_INTERNALCLASSES_CLASS(IllegalArgumentException        )
RISSE_INTERNALCLASSES_CLASS(NullObjectException             )
RISSE_INTERNALCLASSES_CLASS(BadArgumentCountException       )
RISSE_INTERNALCLASSES_CLASS(IllegalMemberAccessException    )
RISSE_INTERNALCLASSES_CLASS(CoroutineException              )
RISSE_INTERNALCLASSES_CLASS(BlockExitException              )


#endif

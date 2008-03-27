// このファイルは risseScriptEngine.h / risseScriptEngine.cpp
// から複数回呼ばれる。
// どのようにしてこのファイルが呼ばれているかは 上記ファイルを
// 参照のこと。


#ifdef RISSE_BUILTINCLASSES_INCLUDE
// ここの部分は risseScriptEngine.cpp から呼ばれ、各クラスを
// new するのに必要なインクルードファイルをインクルードするために使われる。

#include "risseObjectClass.h"
#include "risseModuleClass.h"
#include "risseClassClass.h"
#include "risseFunctionClass.h"
#include "rissePropertyClass.h"
#include "risseArrayClass.h"
#include "risseStringClass.h"
#include "risseOctetClass.h"
#include "rissePrimitiveClass.h"
#include "risseNullClass.h"
#include "risseVoidClass.h"
#include "risseBooleanClass.h"
#include "risseNumberClass.h"
#include "risseIntegerClass.h"
#include "risseRealClass.h"
#include "risseDictionaryClass.h"
#include "risseExceptionClass.h"
#include "risseBindingClass.h"
#include "risseScriptBlockClass.h"
#include "risseStreamClass.h"
#include "risse_parser/risseRisseScriptBlockClass.h"

#else

// ここの部分は RISSE_BUILTINCLASSES_CLASS( ) 内にクラス名を
// 列挙する方法で、組み込みクラスを定義する。
// ここに書いた順番でクラスの初期化が行われることに注意。

RISSE_BUILTINCLASSES_CLASS(Object                          )
RISSE_BUILTINCLASSES_CLASS(Module                          )
RISSE_BUILTINCLASSES_CLASS(Class                           )
RISSE_BUILTINCLASSES_CLASS(Binding                         )
RISSE_BUILTINCLASSES_CLASS(ScriptBlock                     )
RISSE_BUILTINCLASSES_CLASS(RisseScriptBlock                )
RISSE_BUILTINCLASSES_CLASS(Function                        )
RISSE_BUILTINCLASSES_CLASS(Property                        )
RISSE_BUILTINCLASSES_CLASS(Array                           )
RISSE_BUILTINCLASSES_CLASS(Primitive                       )
RISSE_BUILTINCLASSES_CLASS(Null                            )
RISSE_BUILTINCLASSES_CLASS(Void                            )
RISSE_BUILTINCLASSES_CLASS(Boolean                         )
RISSE_BUILTINCLASSES_CLASS(String                          )
RISSE_BUILTINCLASSES_CLASS(Octet                           )
RISSE_BUILTINCLASSES_CLASS(Number                          )
RISSE_BUILTINCLASSES_CLASS(Integer                         )
RISSE_BUILTINCLASSES_CLASS(Real                            )
RISSE_BUILTINCLASSES_CLASS(Dictionary                      )
RISSE_BUILTINCLASSES_CLASS(Stream                          )
RISSE_BUILTINCLASSES_CLASS(SourcePoint                     )
RISSE_BUILTINCLASSES_CLASS(Throwable                       )
RISSE_BUILTINCLASSES_CLASS(Error                           )
RISSE_BUILTINCLASSES_CLASS(AssertionError                  )
RISSE_BUILTINCLASSES_CLASS(Exception                       )
RISSE_BUILTINCLASSES_CLASS(InsufficientResourceException   )
RISSE_BUILTINCLASSES_CLASS(IOException                     )
RISSE_BUILTINCLASSES_CLASS(CharConversionException         )
RISSE_BUILTINCLASSES_CLASS(RuntimeException                )
RISSE_BUILTINCLASSES_CLASS(CompileException                )
RISSE_BUILTINCLASSES_CLASS(ImportException                 )
RISSE_BUILTINCLASSES_CLASS(ClassDefinitionException        )
RISSE_BUILTINCLASSES_CLASS(InstantiationException          )
RISSE_BUILTINCLASSES_CLASS(UnsupportedOperationException   )
RISSE_BUILTINCLASSES_CLASS(ArithmeticException             )
RISSE_BUILTINCLASSES_CLASS(BadContextException             )
RISSE_BUILTINCLASSES_CLASS(MemberAccessException           )
RISSE_BUILTINCLASSES_CLASS(NoSuchMemberException           )
RISSE_BUILTINCLASSES_CLASS(ArgumentException               )
RISSE_BUILTINCLASSES_CLASS(IllegalArgumentException        )
RISSE_BUILTINCLASSES_CLASS(NullObjectException             )
RISSE_BUILTINCLASSES_CLASS(IllegalArgumentClassException   )
RISSE_BUILTINCLASSES_CLASS(BadArgumentCountException       )
RISSE_BUILTINCLASSES_CLASS(IllegalMemberAccessException    )
RISSE_BUILTINCLASSES_CLASS(IllegalStateException           )
RISSE_BUILTINCLASSES_CLASS(InaccessibleResourceException   )
RISSE_BUILTINCLASSES_CLASS(BlockExitException              )


#endif

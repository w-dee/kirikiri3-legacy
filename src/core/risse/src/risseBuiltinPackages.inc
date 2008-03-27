// このファイルは risseScriptEngine.h / risseScriptEngine.cpp
// から複数回呼ばれる。
// どのようにしてこのファイルが呼ばれているかは 上記ファイルを
// 参照のこと。


#ifdef RISSE_BUILTINPACKAGES_INCLUDE
// ここの部分は risseScriptEngine.cpp から呼ばれ、各パッケージイニシャライザを
// new するのに必要なインクルードファイルをインクルードするために使われる。

#include "builtin/date/risseDateClass.h"
#include "builtin/thread/risseThreadClass.h"
#include "builtin/coroutine/risseCoroutineClass.h"

#else

// ここの部分は RISSE_BUILTINPACKAGES_PACKAGE( ) 内にクラス名を
// 列挙する方法で、組み込みクラスを定義する。
// ここに書いた順番でクラスの初期化が行われることに注意。

RISSE_BUILTINPACKAGES_PACKAGE(Date                            )
RISSE_BUILTINPACKAGES_PACKAGE(Thread                          )
RISSE_BUILTINPACKAGES_PACKAGE(Coroutine                       )


#endif

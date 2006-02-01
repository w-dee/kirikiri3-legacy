//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief シングルトンを実現する自家製テンプレートクラス
//---------------------------------------------------------------------------
#ifndef RisaSINGLETONH
#define RisaSINGLETONH

/*!
	@note

	コンセプト

	・簡単にシングルトンオブジェクトを利用できること
	・オブジェクトの構築順を依存関係から保証できること
	・オブジェクトの消滅順も依存関係から保証できること
	・オブジェクトの構築時に発生する例外を捕捉できること

	boost::details::pool::singleton_default を参考にしています
*/

#include <vector>
#include <boost/smart_ptr.hpp>
#include <stdio.h>
#include <typeinfo>

//---------------------------------------------------------------------------
//! @brief  シングルトンオブジェクト管理用クラス
//---------------------------------------------------------------------------
class tRisaSingletonManager
{
	typedef void (*tEnsureFunction)(); //!< ensure 関数のtypedef
	static std::vector<tEnsureFunction> * EnsureFunctions; //!< ensure関数の配列
	static unsigned int RefCount; //!< リファレンスカウンタ

public:
	static void Register(tEnsureFunction function);
	static void Unregister();

	static void InitAll();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief  シングルトンオブジェクト用クラス
//---------------------------------------------------------------------------
template <typename T>
class tRisaSingleton
{
	struct object_registerer
	{
		// この構造体は static 領域に配置され、main 関数よりも前に作成される。
		// コンストラクタでは tRisaSingletonManager::Register() が呼ばれ、
		// tRisaSingleton<T>::ensure が tRisaSingletonManager の vector に登録される。
		// これにより、tRisaSingletonManager::InitAll() の呼び出しで全ての
		// シングルトンインスタンスが作成されることを保証する。
		// もしシングルトンインスタンスの構築時に発生する例外を捕捉したいならば、
		// InitAll() を呼び出す際に捕捉することができる。
		// InitAll() を呼び出さなくてもシングルトンオブジェクトは必要時に自動的に
		// 生成されるが、その部分はマルチスレッドアクセスに対応していないので
		// かならず明示的に(他のスレッドが走っていない状態で) InitAll() を呼ぶことを
		// 推奨する。
		object_registerer() { tRisaSingletonManager::Register(&tRisaSingleton<T>::ensure); }
		~object_registerer() { tRisaSingletonManager::Unregister(); }
		inline void do_nothing() const { }
	};
	static object_registerer register_object;

	boost::shared_ptr<T> & referrer;
public:
	// このクラスのオブジェクトを作成することは、
	// このクラスのオブジェクトの存在期間中、T のインスタンスが存在することを
	// 保証する。
	// クラスのメンバとして tRisaSingleton<T> 型のメンバを記述すれば、
	// そのクラスのオブジェクトが存在し続ける間は T のインスタンスが存在
	// することになる。
	// これの寿命管理には shared_ptr を用いた参照カウンタを用いているため、
	// 相互参照があるとオブジェクトが解放されないままになるので注意が必要である。

	tRisaSingleton() : referrer(instance()) {;}

	//! @brief シングルトンオブジェクトのインスタンスを返す
	static boost::shared_ptr<T> & instance()
	{
		static boost::shared_ptr<T> object(new T());
		register_object.do_nothing();
		return object;
	}

	//! @brief オブジェクトが存在することを確かにする
	static void ensure()
	{
		instance();
	}
};
template <typename T>
typename tRisaSingleton<T>::object_registerer tRisaSingleton<T>::register_object;
//---------------------------------------------------------------------------

/*!
@note

  使用例

→ (やじるし) を使い、

依存しているクラス→依存されているクラス

と記述することにすると、


s3 → s2
s3 → s1

s2 → s0
s1 → s0

という関係を例にとる。

依存しているクラスのメンバに、依存されているクラスのシングルトンオブジェクト
をこのように記述することでシングルトンオブジェクトの生成順や消滅順を決定づけ
ることが出来る。


<code>

#include <stdio.h>
#include "Singleton.h"

class s0
{
public:
	s0() { printf("s0 construct\n"); }
	~s0() { printf("s0 destruct\n"); }
};

class s1
{
	tRisaSingleton<s0> referer_s0;
public:
	s1() { printf("s1 construct\n"); }
	~s1() { printf("s1 destruct\n"); }
};

class s2
{
	tRisaSingleton<s0> referer_s0;
public:
	s2() { printf("s2 construct\n"); }
	~s2() { printf("s2 destruct\n"); }
};

class s3
{
	tRisaSingleton<s0> referer_s0;
	tRisaSingleton<s1> referer_s1;
	tRisaSingleton<s2> referer_s2;
	int n;
public:
	s3() { printf("s3 construct\n"); n = 1; }
	~s3() { printf("s3 destruct\n"); n = 0; }
	int getN() const { return n; }
};

int main()
{
	printf("main begin\n");
	printf("InitAll begin\n");
	tRisaSingletonManager::InitAll();
	printf("InitAll end\n");
	printf("n : %d\n", tRisaSingleton<s3>::instance()->getN());
	printf("main end\n");
}


</code>


実行結果は以下の通りとなる。

<pre>

main begin
InitAll begin
s0 construct
s1 construct
s2 construct
s3 construct
InitAll end
n : 1
main end
s3 destruct
s2 destruct
s1 destruct
s0 destruct


</pre>


*/

//---------------------------------------------------------------------------
//! @brief  オブジェクトの生成と消滅のトレース
//! @note   このクラスのオブジェクトを好きな所に書いておくと生成時と消滅時に
//!         メッセージが表示されるというだけ
//---------------------------------------------------------------------------
template <typename T>
class tRisaSingletonObjectLifeTracer
{
public:
	tRisaSingletonObjectLifeTracer()
	{
#ifdef DEBUG
		fprintf(stderr, "class %s created\n", typeid(T).name());
#endif
	}
	~tRisaSingletonObjectLifeTracer()
	{
#ifdef DEBUG
		fprintf(stderr, "class %s deleted\n", typeid(T).name());
#endif
	}
};
//---------------------------------------------------------------------------

#endif


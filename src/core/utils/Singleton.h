//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief シングルトンを実現する自家製テンプレートクラス
//---------------------------------------------------------------------------
#ifndef TVPSINGLETONH
#define TVPSINGLETONH

/*!
	@note

	コンセプト

	・簡単にシングルトンオブジェクトを利用できること
	・オブジェクトの構築順を依存関係から保証できること
	・オブジェクトの消滅順も依存関係から保証できること

	boost::details::pool::singleton_default を参考にしています
*/

#include <boost/smart_ptr.hpp>

//---------------------------------------------------------------------------
//! @brief  シングルトンオブジェクト用クラス
//---------------------------------------------------------------------------
template <typename T>
class tTVPSingleton
{
	struct object_creator
	{
		// この構造体は static 領域に配置され、main 関数よりも前に作成される。
		// コンストラクタでは tTVPSingleton<T>::instance() が呼ばれるため、
		// tTVPSingleton<T>::instance() 中の object が main 関数よりも前に
		// 作成されることを保証する。
		object_creator() { tTVPSingleton<T>::instance(); }
		inline void do_nothing() const { }
	};
	static object_creator create_object;

	boost::shared_ptr<T> & referrer;
public:
	// このクラスのオブジェクトを作成することは、
	// このクラスのオブジェクトの存在期間中、T のインスタンスが存在することを
	// 保証する。
	// クラスのメンバとして tTVPSingleton<T> 型のメンバを記述すれば、
	// そのクラスのオブジェクトが存在し続ける間は T のインスタンスが存在
	// することになる。
	// これの寿命管理には shared_ptr を用いた参照カウンタを用いているため、
	// 相互参照があるとオブジェクトが解放されないままになるので注意が必要である。

	tTVPSingleton() : referrer(instance()) {;}

	static boost::shared_ptr<T> & instance()
	{
		// シングルトンオブジェクトのインスタンスを返す
		static boost::shared_ptr<T> object(new T());
		create_object.do_nothing();
		return object;
	}
};
template <typename T>
typename tTVPSingleton<T>::object_creator
tTVPSingleton<T>::create_object;
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
	tTVPSingleton<s0> referer_s0;
public:
	s1() { printf("s1 construct\n"); }
	~s1() { printf("s1 destruct\n"); }
};

class s2
{
	tTVPSingleton<s0> referer_s0;
public:
	s2() { printf("s2 construct\n"); }
	~s2() { printf("s2 destruct\n"); }
};

class s3
{
	tTVPSingleton<s1> referer_s1;
	tTVPSingleton<s2> referer_s2;
	int n;
public:
	s3() { printf("s3 construct\n"); n = 1; }
	~s3() { printf("s3 destruct\n"); n = 0; }
	int getN() const { return n; }
};

int main()
{
	printf("main begin\n");
	printf("n : %d\n", tTVPSingleton<s3>::instance()->getN());
	printf("main end\n");
}

</code>


実行結果は以下の通りとなる。

<pre>

s0 construct
s1 construct
s2 construct
s3 construct
main begin
n : 1
main end
s3 destruct
s2 destruct
s1 destruct
s0 destruct

</pre>


*/

#endif


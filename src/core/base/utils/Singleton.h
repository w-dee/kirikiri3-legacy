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
	・オブジェクトの構築と消滅をアプリケーションの初期化・終了の
	  任意の時点でできること

	boost::details::pool::singleton_default を参考にしている。


	シングルトンの機構を構成するクラスは３つある。

	一つは内部的に用いられている tRisaSingletonManager、もう一つは
	シングルトンクラスがシングルトンたる動作を規定している
	テンプレートクラスである singleton_base 、最後の一つは
	依存しているシングルトンを表す depends_on 。

	詳しくは例を参照のこと。
*/

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

class s0 : singleton_of<s0>
{
public:
	s0() { printf("s0 construct\n"); }
	~s0() { printf("s0 destruct\n"); }
};

class s1 : singleton_of<s1>, depends_on<s0>
{
public:
	s1() { printf("s1 construct\n"); }
	~s1() { printf("s1 destruct\n"); }
};

class s2 : singleton_of<s2>, depends_on<s0>
{
public:
	s2() { printf("s2 construct\n"); }
	~s2() { printf("s2 destruct\n"); }
};

class s3 : singleton_of<s3>,
		depends_on<s0>,
		depends_on<s1>,
		depends_on<s2>
{
	int n;
public:
	s3() { printf("s3 construct\n"); n = 1; }
	~s3() { printf("s3 destruct\n"); n = 0; }
	int getN() const { return n; }
};

int main()
{
//	s3::referrer s3;
	fprintf(stderr, "main begin\n");
	fprintf(stderr, "InitAll begin\n");
	tRisaSingletonManager::InitAll();
	fprintf(stderr, "InitAll end\n");
	fprintf(stderr, "n : %d\n", s3::instance()->getN());
	fprintf(stderr, "Disconnect begin\n");
	tRisaSingletonManager::DisconnectAll();
	fprintf(stderr, "Disconnect end\n");
	tRisaSingletonManager::ReportAliveObjects();
	fprintf(stderr, "main end\n");
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
Disconnect begin
s3 destruct
s2 destruct
s1 destruct
s0 destruct
Disconnect end
main end


</pre>


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
	typedef void (*tFunction)(); //!< ensure/disconnect 関数のtypedef
	typedef const char * (*tGetNameFunction)(); //!< GetName 関数のtypedef
	typedef bool (*tAliveFunction)(); //!< Alive 関数のtypedef
	struct tRegisterInfo
	{
		tFunction Ensure; //!< ensure関数
		tGetNameFunction GetName; //!< get_name 関数
		tAliveFunction Alive; //!< ailve 関数
	};

	static std::vector<tRegisterInfo> * Functions; //!< ensure関数の配列
	static std::vector<tFunction> * Disconnectors; //!< disconnect関数の配列
	static unsigned int RefCount; //!< リファレンスカウンタ

public:
	static void Register(const tRegisterInfo & info);
	static void RegisterDisconnector(tFunction func);
	static void Unregister();

	static void InitAll();
	static void DisconnectAll();
	static void ReportAliveObjects();

	//! @brief 空の構造体
	struct empty
	{
		struct referrer
		{
		};
	};
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief  シングルトンオブジェクト用クラス
//! @note	テンプレート引数の最初の引数はシングルトンとなるべきクラス、次以降は
//!			このシングルトンが依存しているクラスである。
//---------------------------------------------------------------------------
template <typename T>
class singleton_base
{
	singleton_base(const singleton_base &); //!< non-copyable

	//! @brief シングルトンクラスをマネージャに登録するための構造体
	//! @note
	//! この構造体は static 領域に配置され、main 関数よりも前に作成される。
	//! コンストラクタでは tRisaSingletonManager::Register() が呼ばれ、
	//! シングルトンクラスに関する情報が tRisaSingletonManager の vector に登録される。
	//! この中にはシングルトンインスタンスを作成するための関数へのポインタも
	//! 含まれており、tRisaSingletonManager::InitAll() の呼び出しで全ての
	//! シングルトンインスタンスが作成されることを保証する。
	//! もしシングルトンインスタンスの構築時に発生する例外を捕捉したいならば、
	//! InitAll() を呼び出す際に捕捉することができる。
	//! InitAll() を呼び出さなくてもシングルトンオブジェクトは必要時に自動的に
	//! 生成されるが、その部分はマルチスレッドアクセスに対応していないので
	//! かならず明示的に(他のスレッドが走っていない状態で) InitAll() を呼ぶことを
	//! 推奨する。
	struct object_registerer
	{
		//! @brief コンストラクタ
		object_registerer()
		{
			tRisaSingletonManager::tRegisterInfo info;
			info.Ensure =      &singleton_base<T>::ensure;
			info.GetName =     &singleton_base<T>::get_name;
			info.Alive =       &singleton_base<T>::alive;

			tRisaSingletonManager::Register(info);
		}
		//! @brief デストラクタ
		~object_registerer() { tRisaSingletonManager::Unregister(); }
		//! @brief このクラスが最適化で消されないようにするためのダミー
		inline void do_nothing() const { }
	};
	static object_registerer register_object; //!< object_registerer のstatic変数

	//! @brief オブジェクトを保持する構造体
	//! @note
	//! このクラスのコンストラクタは manipulate_object 内のローカルスコープにある static 変数
	//! が初めて作成される際に呼ばれ、マネージャに disconnect メソッドを登録する。
	//! この関数が呼ばれるのは必ず依存先→依存元の順番となるため、マネージャは
	//! disconnect をこれとは逆順に呼ぶことで依存関係を保ったままオブジェクトの
	//! 消滅を行おうとする。
	struct object_holder
	{
		boost::shared_ptr<T> object; //!< シングルトンオブジェクト
		boost::weak_ptr<T> weak_object; //!< シングルトンオブジェクトへの弱参照
		object_holder() : object(create()), weak_object(object)
		{
			tRisaSingletonManager::RegisterDisconnector(&singleton_base<T>::disconnect);
//			fprintf(stderr, "created %s\n", singleton_base<T>::get_name());
		}
		static T* create()
		{
//			fprintf(stderr, "creating %s\n", singleton_base<T>::get_name());
			return new T();
		}
	};

	//! @brief オブジェクトを操作するメソッド
	//! @note
	//! このメソッドはローカルに disconnector_registerer 型のオブジェクトを
	//! static で持つ。
	//! reset が true の場合は object.object を reset することによって
	//! 参照を切る。is_alive が非nullの場合は、object.weak_object を
	//! 参照することによりシングルトンオブジェクトが有効かどうかを*is_aliveに
	//! 書き込む。
	static boost::shared_ptr<T> & manipulate_object(bool reset=false, bool * is_alive = NULL)
	{
		static object_holder holder;
		register_object.do_nothing();
		if(reset) holder.object.reset();
		if(is_alive) *is_alive = !holder.weak_object.expired();
		return holder.object;
	}

	//! @brief オブジェクトが存在することを確かにする
	static void ensure()
	{
		(void) manipulate_object();
	}

	//! @brief オブジェクトへの参照を切る
	static void disconnect()
	{
		(void) manipulate_object(true);
	}

	//! @brief クラス名を得る
	static const char * get_name()
	{
		return typeid(T).name();
	}

protected:
	//! @brief コンストラクタ
	singleton_base()
	{
		register_object.do_nothing();
			// 何もしないダミーのメソッドである do_nothing を呼ぶことにより、
			// register_object が作成されることを確実にする
	}

	//! @brief デストラクタ
	~singleton_base() {;}

public:

	//! @brief シングルトンオブジェクトのインスタンスを返す
	static boost::shared_ptr<T> & instance()
	{
		return manipulate_object();
	}

	//! @brief オブジェクトが有効かどうかを得る
	static bool alive()
	{
		bool alive;
		(void) manipulate_object(false, &alive);
		return alive;
	}

	//! @brief シングルトン参照用クラス
	//! @note
	//! このクラスのオブジェクトを作成することは、
	//! このクラスのオブジェクトの存在期間中、T のインスタンスが存在することを
	//! 保証する。
	//! クラスのメンバとして singleton_base<T>::referrer 型のメンバを記述すれば、
	//! そのクラスのオブジェクトが存在し続ける間は T のインスタンスが存在
	//! することになる。
	//! これの寿命管理には shared_ptr を用いた参照カウンタを用いているため、
	//! 相互参照があるとオブジェクトが解放されないままになるので注意が必要である。
	class referrer
	{
		boost::shared_ptr<T> object; //!< シングルトンオブジェクトへの参照を保持するメンバ
	public:
		referrer() : object(singleton_base<T>::instance()) {;}
	};
};
template <typename T>
typename singleton_base<T>::object_registerer singleton_base<T>::register_object;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	シングルトン基底クラス用define
//! @note	これは単純に、class A : public singleton_base<T> よりも class A: singleton_of<A> と
//!			書けた方が幸せだろうというだけのこと。混乱の元ならば書かない方が使わない方が
//!			いいかもしれないがそもそもテンプレートを使いまくってる時点で混乱するんだからこれぐらいの
//!			混乱はなんと言うことはないのだろうか→結局これ使ってません
//---------------------------------------------------------------------------
#define singleton_of public singleton_base
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief  シングルトン用依存関係定義テンプレート
//! @note
//! 特定のシングルトンクラスに依存していることを表すためにこれを継承させる。例を参照。
//---------------------------------------------------------------------------
template <typename T>
class depends_on
{
	boost::shared_ptr<T> __referrer_object; //!< シングルトンオブジェクトへの参照を保持するメンバ
public:
	depends_on() : __referrer_object(singleton_base<T>::instance()) {;}
};
//---------------------------------------------------------------------------



#endif


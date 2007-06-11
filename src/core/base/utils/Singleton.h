//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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


	シングルトンの機構を構成するクラスは４つある。

	・シングルトンの管理を行う singleton_manager
	・シングルトンクラスがシングルトンたる動作を規定している
	  テンプレートクラスである singleton_base
	・依存しているシングルトンを表す depends_on
	・singleton_manager::init_all で初期化されずに、
	  初めて必要となった際に初期化されることを示すための
	  manual_start

	詳しくは例を参照のこと。
*/

/*!
@note

  使用例

TODO: 使用例をここに書く
*/


#include <vector>
#include <stdio.h>
#include <typeinfo>
#include "risse/include/risseGC.h"

using namespace Risse;

//---------------------------------------------------------------------------
//! @brief  シングルトンオブジェクト管理用クラス
//---------------------------------------------------------------------------
class singleton_manager : public tRisseCollectee
{
	typedef void (*handler_t)(); //!< ensure/disconnect 関数のtypedef
	typedef const char * (*get_name_function_t)(); //!< get_name 関数のtypedef
	typedef bool (*alive_function_t)(); //!< alive 関数のtypedef
	struct register_info_t
	{
		handler_t ensure; //!< ensure関数
		get_name_function_t get_name; //!< get_name 関数
		alive_function_t alive; //!< ailve 関数
	};

	static gc_vector<register_info_t> * functions; //!< ensure関数の配列
	static gc_vector<handler_t> * disconnectors; //!< disconnect関数の配列
	static gc_vector<handler_t> * manual_starts; //!< 手動起動をするクラスのensure関数の配列

public:
	//! @brief		シングルトン情報を登録する
	static void register_info(const register_info_t & info);

	//! @brief		手動起動を表すシングルトンの情報を登録する
	static void register_manual_start(handler_t func);

	//! @brief		disconnect 関数を登録する
	static void register_disconnector(handler_t func);

	//! @brief		シングルトン情報の登録を解除する
	//! @note		とはなっているが実際にはなにもしない。
	//!				昔はここに参照カウンタをデクリメントし、0になれば
	//!				情報をすべて破棄するようなコードが書かれていたが、
	//!				GC を利用するようになってからそれは無くなった。
	static void unregister_info();

	//! @brief		全てのシングルトンを初期化する
	//! @note		この間に発生した例外は呼び出し元で捕捉できる
	static void init_all();

	//! @brief		全てのシングルトンへの参照を切る
	//! @note		これによりすべてのシングルトンが消滅する保証はない。
	//!				他の場所でこのシングルトンオブジェクトへの参照が残っていた場合は
	//!				その参照が無くなるまでそのシングルトンオブジェクトおよびそれが
	//!				依存しているシングルトンオブジェクトは消滅しないままとなる。
	//!				disconnect関数の呼び出しは、register_disconnector を呼び出した順とは
	//!				逆順となる。
	static void disconnect_all();

	//! @brief		削除されずに残っているオブジェクトを標準エラー出力に表示する
	static void report_alive_objects();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief  シングルトンオブジェクト用クラス
//! @note	テンプレート引数の最初の引数はシングルトンとなるべきクラス。
//!			すべてのシングルトンクラスは同時に tRisseCollectee のサブクラスとなり、
//!			つまり GC reachable である。
//!			destruct という仮想関数があり、これが
//!			singleton_manager::disconnect_all の時点で呼ばれる事になっているので
//!			必要な終了処理などはこのタイミングで行うこと。destruct の規定の動作は
//!			delete this である。つまり、一応 singleton_manager::disconnect_all
//!			が呼ばれる以上はデストラクタが呼ばれると仮定して良い。
//!			ただし、instance() で得たポインタが有効なのは
//!			singleton_manager::disconnect_all が呼ばれるまでの間であることには
//!			注意しなければならない。
//---------------------------------------------------------------------------
template <typename T>
class singleton_base : public tRisseCollectee
{
	singleton_base(const singleton_base &); //!< non-copyable
	void operator = (const singleton_base &); //!< non-copyable

	//! @brief シングルトンクラスをマネージャに登録するための構造体
	//! @note
	//! この構造体は static 領域に配置され、main 関数よりも前に作成される。
	//! コンストラクタでは singleton_manager::register_info() が呼ばれ、
	//! シングルトンクラスに関する情報が singleton_manager の vector に登録される。
	//! この中にはシングルトンインスタンスを作成するための関数へのポインタも
	//! 含まれており、singleton_manager::init_all() の呼び出しで全ての
	//! シングルトンインスタンスが作成されることを保証する。
	//! もしシングルトンインスタンスの構築時に発生する例外を捕捉したいならば、
	//! init_all() を呼び出す際に捕捉することができる。
	//! init_all() を呼び出さなくてもシングルトンオブジェクトは必要時に自動的に
	//! 生成されるが、その部分はマルチスレッドアクセスに対応していないので
	//! かならず明示的に(他のスレッドが走っていない状態で) init_all() を呼ぶことを
	//! 推奨する。
	//! manual_start クラスを作成すると、そのシングルトン
	//! クラスが singleton_manager::init_all() の呼び出しではなく、実際に必要と
	//! なったとき (初回の instance() が呼ばれたときあるいは depends_on クラスの
	//! オブジェクトが作成されたとき) に初めて作成される。
	//! ただし、こちらについてもマルチスレッドによるアクセスは考慮されていないので
	//! 注意すること。サブスレッドからのアクセスは、かならずそのシングルトンインスタンスが
	//! メインスレッドで作成されていることを保証してから、などの考慮が必要となる。
	struct object_registerer
	{
		//! @brief コンストラクタ
		object_registerer()
		{
			singleton_manager::register_info_t info;
			info.ensure =    &singleton_base<T>::ensure;
			info.get_name =  &singleton_base<T>::get_name;
			info.alive =     &singleton_base<T>::alive;

			singleton_manager::register_info(info);
		}
		//! @brief デストラクタ
		~object_registerer() { singleton_manager::unregister_info(); }
		//! @brief このクラスが最適化で消されないようにするためのダミー
		inline void do_nothing() const { }
	};
	static object_registerer register_object; //!< object_registerer のstatic変数


	//! @brief オブジェクトを保持する構造体
	//! @note
	//! このクラスのコンストラクタは manipulate_object 内のローカルスコープにある static 変数
	//! が初めて作成される際に呼ばれ、マネージャに disconnect メソッドを登録する。
	//! この関数が呼ばれるのは必ず依存先→依存元の順番となるため、マネージャは
	//! disconnect をこれとは逆順に呼ぶことで依存関係を保ったまま destruct の
	//! 呼び出しを行おうとする。
	struct object_holder
	{
		T * object; //!< オブジェクトインスタンス
		object_holder() : object(create())
		{
			singleton_manager::register_disconnector(&singleton_base<T>::disconnect);
			singleton_base<T>::object_created = true;
			fprintf(stderr, "created %s\n", singleton_base<T>::get_name());
			fflush(stderr);
		}
		static T* create()
		{
			fprintf(stderr, "creating %s\n", singleton_base<T>::get_name());
			fflush(stderr);
			return new T();
		}
	};

	//! @brief オブジェクトが作成されたかどうか
	static bool object_created;

	//! @brief オブジェクトを操作するメソッド
	//! @note
	//! このメソッドはローカルに object_holder 型のオブジェクトを
	//! static で持つ。
	//! reset が true の場合は object.object を reset することによって
	//! 参照を切る。is_alive が非nullの場合は、object.weak_object を
	//! 参照することによりシングルトンオブジェクトが有効かどうかを*is_aliveに
	//! 書き込む。
	static T * manipulate_object(bool reset=false, bool * is_alive = NULL)
	{
		static object_holder holder;
		register_object.do_nothing();
		if(reset) holder.object = NULL;
		if(is_alive) *is_alive = holder.object;
		return holder.object;
	}

	//! @brief オブジェクトへの参照を切る
	static void disconnect()
	{
		fprintf(stderr, "disconnecting %s\n", get_name());
		fflush(stderr);
		if(alive()) instance()->destruct();
		(void) manipulate_object(true);
	}

	//! @brief	オブジェクトの消滅を行う仮想関数(下位クラスでオーバーライドすること)
	//! @note	デフォルトでは delete this する
	virtual void destruct() {delete this;}

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

	//! @brief	デストラクタ
	virtual ~singleton_base() {;}

public:
	//! @brief オブジェクトが存在することを確かにする
	static void ensure()
	{
		(void) manipulate_object();
	}

	//! @brief シングルトンオブジェクトのインスタンスを返す
	static T* instance()
	{
		return manipulate_object();
	}

	//! @brief オブジェクトが有効かどうかを得る
	//! @note
	//! 当然ながらこの関数が呼ばれた時点の状態を返す。
	//! 次の瞬間にオブジェクトがいまだ有効かどうかは保証がない。
	//! 次の瞬間にオブジェクトが無効になる可能性があるのは、
	//! 次の瞬間までに singleton_manager::disconnect_all が
	//! 呼ばれた場合だけなのであまり考えなくてよいかもしれない
	static bool alive()
	{
		if(!object_created) return false;
		bool alive;
		(void) manipulate_object(false, &alive);
		return alive;
	}
};
template <typename T>
typename singleton_base<T>::object_registerer singleton_base<T>::register_object;
template <typename T>
bool singleton_base<T>::object_created = false;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief  シングルトン用依存関係定義テンプレート
//! @note
//! 特定のシングルトンクラスに依存していることを表すためにこれを継承させる。例を参照。
//---------------------------------------------------------------------------
template <typename T>
class depends_on
{
	T* __referrer_object; //!< シングルトンオブジェクトへの参照を保持するメンバ
public:
	depends_on() : __referrer_object(singleton_base<T>::instance()) {;}

	//! @brief オブジェクトのインスタンスを得る
	T* depend_instance() { return __referrer_object; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief  手動起動のシングルトンを表すためのテンプレートクラス
//! @note
//! このシングルトンは手動起動であることを表すためにこれを継承させる。例を参照。
//---------------------------------------------------------------------------
template <typename T>
class manual_start
{
	struct manual_object_registerer
	{
		//! @brief コンストラクタ
		manual_object_registerer()
		{
			singleton_manager::register_manual_start(&singleton_base<T>::ensure);
		}
		//! @brief このクラスが最適化で消されないようにするためのダミー
		inline void do_nothing() const { }
	};
	static manual_object_registerer manual_register_object; //!< manual_object_registerer のstatic変数
public:
	manual_start()
	{
		manual_register_object.do_nothing();
	}
};
//---------------------------------------------------------------------------
template <typename T>
typename manual_start<T>::manual_object_registerer manual_start<T>::manual_register_object;
//---------------------------------------------------------------------------



#endif


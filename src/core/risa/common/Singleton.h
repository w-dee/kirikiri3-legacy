//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief シングルトンを実現する自家製テンプレートクラス
//---------------------------------------------------------------------------
#ifndef SINGLETONH
#define SINGLETONH

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
#include <stdlib.h>
#include <string.h>
#include <typeinfo>
#include "risseGC.h"
#include "risa/common/RisaThread.h"

#define HAVE_CXXABI_H
#ifdef HAVE_CXXABI_H
	#include <cxxabi.h>
#endif


namespace Risa {
//---------------------------------------------------------------------------


using namespace Risse;


//---------------------------------------------------------------------------
/**
 * 型名のデマングルを行う
 * @param name	マングルされた名前
 * @return	デマングルされた名前(GC管理下あるいはstaticなので明示的に開放しなくてよい)
 */
const char * Demangle(const char * name);

//---------------------------------------------------------------------------
/**
 * シングルトンオブジェクト管理用クラス
 */
class singleton_manager : public tCollectee
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
	static tCriticalSection * CS; //!< いろんなものを保護するためのクリティカルセクション
	static tCriticalSection * DummyCS; //!< メモリバリアを行うためのダミーのCS

public:
	/**
	 * シングルトン情報を登録する
	 */
	static void register_info(const register_info_t & info);

	/**
	 * 手動起動を表すシングルトンの情報を登録する
	 */
	static void register_manual_start(handler_t func);

	/**
	 * disconnect 関数を登録する
	 */
	static void register_disconnector(handler_t func);

	/**
	 * シングルトン情報の登録を解除する
	 * @note	とはなっているが実際にはなにもしない。
	 *			昔はここに参照カウンタをデクリメントし、0になれば
	 *			情報をすべて破棄するようなコードが書かれていたが、
	 *			GC を利用するようになってからそれは無くなった。
	 */
	static void unregister_info();

	/**
	 * 全てのシングルトンを初期化する
	 * @note	この間に発生した例外は呼び出し元で捕捉できる
	 */
	static void init_all();

	/**
	 * 全てのシングルトンへの参照を切る
	 * @note	これによりすべてのシングルトンが消滅する保証はない。
	 *			他の場所でこのシングルトンオブジェクトへの参照が残っていた場合は
	 *			その参照が無くなるまでそのシングルトンオブジェクトおよびそれが
	 *			依存しているシングルトンオブジェクトは消滅しないままとなる。
	 *			disconnect関数の呼び出しは、register_disconnector を呼び出した順とは
	 *			逆順となる。
	 */
	static void disconnect_all();

	/**
	 * 削除されずに残っているオブジェクトを標準エラー出力に表示する
	 */
	static void report_alive_objects();

	/**
	 * クリティカルセクションオブジェクトを得る
	 */
	static tCriticalSection & GetCS() { return *CS; }

	/**
	 * ダミーのクリティカルセクションオブジェクトを得る
	 */
	static tCriticalSection & GetDummyCS() { return *DummyCS; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * シングルトンオブジェクト用クラス
 * @note	テンプレート引数の最初の引数はシングルトンとなるべきクラス。
 *			すべてのシングルトンクラスは同時に tCollectee のサブクラスとなり、
 *			つまり GC reachable である。
 *			destruct という仮想関数があり、これが
 *			singleton_manager::disconnect_all の時点で呼ばれる事になっているので
 *			必要な終了処理などはこのタイミングで行うこと。destruct の規定の動作は
 *			delete this である。つまり、一応 singleton_manager::disconnect_all
 *			が呼ばれる以上はデストラクタが呼ばれると仮定して良い。
 *			ただし、instance() で得たポインタが有効なのは
 *			singleton_manager::disconnect_all が呼ばれるまでの間であることには
 *			注意しなければならない。
 */
template <typename T>
class singleton_base : public tCollectee
{
	singleton_base(const singleton_base &); //!< non-copyable
	void operator = (const singleton_base &); //!< non-copyable

	/**
	 * シングルトンクラスをマネージャに登録するための構造体
	 * この構造体は static 領域に配置され、main 関数よりも前に作成される。
	 * コンストラクタでは singleton_manager::register_info() が呼ばれ、
	 * シングルトンクラスに関する情報が singleton_manager の vector に登録される。
	 * この中にはシングルトンインスタンスを作成するための関数へのポインタも
	 * 含まれており、singleton_manager::init_all() の呼び出しで全ての
	 * シングルトンインスタンスが作成されることを保証する。
	 * もしシングルトンインスタンスの構築時に発生する例外を捕捉したいならば、
	 * init_all() を呼び出す際に捕捉することができる。
	 * manual_start クラスを作成すると、そのシングルトン
	 * クラスが singleton_manager::init_all() の呼び出しではなく、実際に必要と
	 * なったとき (初回の instance() が呼ばれたときあるいは depends_on クラスの
	 * オブジェクトが作成されたとき) に初めて作成される。
	 */
	struct object_registerer
	{
		/**
		 * コンストラクタ
		 */
		object_registerer()
		{
			singleton_manager::register_info_t info;
			info.ensure =    &singleton_base<T>::ensure;
			info.get_name =  &singleton_base<T>::get_name;
			info.alive =     &singleton_base<T>::alive;

			singleton_manager::register_info(info);
		}
		/**
		 * デストラクタ
		 */
		~object_registerer() { singleton_manager::unregister_info(); }
		/**
		 * このクラスが最適化で消されないようにするためのダミー
		 */
		inline void do_nothing() const { }
	};
	static object_registerer register_object; //!< object_registerer のstatic変数


	/**
	 * オブジェクトインスタンス
	 */
	static T * volatile object_instance;

	/**
	 * オブジェクトが消滅したかどうか
	 */
	static bool object_shutdown;

	/**
	 * オブジェクトへの参照を切る
	 */
	static void disconnect()
	{
		fprintf(stderr, "disconnecting %s\n", get_name());
		fflush(stderr);
		if(object_instance)
		{
			volatile tCriticalSection::tLocker lock(singleton_manager::GetCS());

			if(object_instance) instance()->destruct();
			object_instance = NULL;
			object_shutdown = true;
		}
	}

	/**
	 * オブジェクトの消滅を行う仮想関数(下位クラスでオーバーライドすること)
	 * @note	デフォルトでは delete this する
	 */
	virtual void destruct() {delete this;}

	/**
	 * クラス名を得る
	 */
	static const char * get_name()
	{
		return Demangle(typeid(T).name());
	}

protected:
	/**
	 * コンストラクタ
	 */
	singleton_base()
	{
		register_object.do_nothing();
			// 何もしないダミーのメソッドである do_nothing を呼ぶことにより、
			// register_object が作成されることを確実にする
	}

	/**
	 * デストラクタ
	 */
	virtual ~singleton_base() {;}

private:
	/**
	 * object_instance に対して値を書き込む
	 */
	RISSE_NOINLINE static void set_object_instance(T * v)
	{
		object_instance = v;
	}

	/**
	 * オブジェクトが存在することを確かにする (内部関数)
	 */
	RISSE_NOINLINE static void make_instance()
	{
		/*
			double-checked lock アルゴリズムに関する覚え書き

			コンパイラによる最適化により、new T() の実行が完了する前に
			object_instance に値が書き込まれる可能性があるが、これは
			volatile により(おそらく)解決。
			new T() によって初期化された内容が他のプロセッサに見える前に
			object_instance が他のプロセッサに見えてしまう問題
			(memory ordering 問題) は、クリティカルセクションやミュー
			テックスがメモリの可視性の同期を行うことを利用して解決。
			そのため、new T() した直後にいったんダミーのクリティカル
			セクションに入ることにする。
			( x86/windows の実装で言えば、EnterCriticalSection 内で
			使われている lock xchg の lock プリフィックスがメモリの同期を
			行うことになる)
			パフォーマンスを優先するのであればちゃんとメモリバリア命令を
			使うところであろうが、そうでもないのでダミーのクリティカルセクション
			という手に。

			object_instance に値を設定した後に明示的な同期はしていないが、
			もしほかのプロセッサがいったん object_instance が NULL なので
			このメソッドに入ったとしても、このメソッドの最初の
			クリティカルセクションへの侵入で同期が行われ、再度 object_instance
			のチェックが行われるので問題はない。
		*/
		volatile tCriticalSection::tLocker lock(singleton_manager::GetCS());
		if(!object_instance)
		{
			if(object_shutdown)
			{
				fprintf(stderr, "ERROR!! singleton %s is already disconnected!\n", get_name());
				fprintf(stderr, "Returning null as instance pointer, will cause program crash ...\n");
				fflush(stderr);
				set_object_instance(NULL);
			}
			else
			{
				fprintf(stderr, "creating %s\n", get_name());
				fflush(stderr);
				T * volatile _instance = new T();
				{ volatile tCriticalSection::tLocker lock(singleton_manager::GetDummyCS()); }
				set_object_instance(_instance);
				singleton_manager::register_disconnector(&singleton_base<T>::disconnect);
				fprintf(stderr, "created %s\n", get_name());
				fflush(stderr);
			}
		}
	}

public:
	/**
	 * オブジェクトが存在することを確かにする
	 * このクラスのコンストラクタははじめて ensure() が呼ばれる際に作成される。
	 * この際、マネージャに disconnect メソッドを登録する。
	 * この関数が呼ばれるのは必ず依存先→依存元の順番となるため、マネージャは
	 * disconnect をこれとは逆順に呼ぶことで依存関係を保ったまま destruct の
	 * 呼び出しを行おうとする。
	 */
	static void ensure()
	{
		if(!object_instance)
		{
			make_instance();
		}
	}

	/**
	 * シングルトンオブジェクトのインスタンスを返す
	 * @note	シングルトンインスタンスがすでに消滅している場合は NULL が帰るので注意
	 */
	static T* instance()
	{
		ensure();
		return object_instance;
	}

	/**
	 * オブジェクトが有効かどうかを得る
	 * 当然ながらこの関数が呼ばれた時点の状態を返す。
	 * 次の瞬間にオブジェクトがいまだ有効かどうかは保証がない。
	 * 次の瞬間にオブジェクトが無効になる可能性があるのは、
	 * 次の瞬間までに singleton_manager::disconnect_all が
	 * 呼ばれた場合だけなのであまり考えなくてよいかもしれない
	 */
	static bool alive()
	{
		return object_instance;
	}

	/**
	 * シングルトン機構のロック
	 * @note	これが存在する限りは、このシングルトンに関する
	 *			生成や消滅が行われないことを保証する。
	 *			この中で instance() を用いて得たインスタンスは
	 *			これを抜けるまで消滅しない。ただし、現バージョンでは
	 *			このシングルトンに対してのみではなく、すべてのシングルトン
	 *			インスタンスに対してロックを行ってしまうので注意。
	 */
	class tLocker
	{
		tCriticalSection::tLocker lock;
	public:
		tLocker() : lock(singleton_manager::GetCS()){;}
		~tLocker() {;}
	};
};
template <typename T>
typename singleton_base<T>::object_registerer singleton_base<T>::register_object;
template <typename T>
T * volatile singleton_base<T>::object_instance = NULL;
template <typename T>
bool singleton_base<T>::object_shutdown = false;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * シングルトン用依存関係定義テンプレート
 * 特定のシングルトンクラスに依存していることを表すためにこれを継承させる。例を参照。
 */
template <typename T>
class depends_on
{
	T* __referrer_object; //!< シングルトンオブジェクトへの参照を保持するメンバ
public:
	depends_on() : __referrer_object(singleton_base<T>::instance()) {;}

	/**
	 * オブジェクトのインスタンスを得る
	 */
	T* depend_instance() { return __referrer_object; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 手動起動のシングルトンを表すためのテンプレートクラス
 * このシングルトンは手動起動であることを表すためにこれを継承させる。例を参照。
 */
template <typename T>
class manual_start
{
	struct manual_object_registerer
	{
		/**
		 * コンストラクタ
		 */
		manual_object_registerer()
		{
			singleton_manager::register_manual_start(&singleton_base<T>::ensure);
		}
		/**
		 * このクラスが最適化で消されないようにするためのダミー
		 */
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



//---------------------------------------------------------------------------
} // namespace Risa


#endif


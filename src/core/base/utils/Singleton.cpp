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

#include "base/utils/Singleton.h"
#include <algorithm>

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
gc_vector<singleton_manager::register_info_t> * singleton_manager::functions = NULL;
gc_vector<singleton_manager::handler_t> * singleton_manager::disconnectors = NULL;
gc_vector<singleton_manager::handler_t> * singleton_manager::manual_starts = NULL;
tCriticalSection * singleton_manager::CS = NULL;
tCriticalSection * singleton_manager::DummyCS = NULL;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void singleton_manager::register_info(const singleton_manager::register_info_t & info)
{
	// GC の初期化を行う。
	// 必ず。
	GC_init();

	// ここはマルチスレッドからの保護は必要ない (main以前によばれるので)

	if(CS == NULL) CS = new tCriticalSection();
	if(DummyCS == NULL) DummyCS = new tCriticalSection();

	// ここで書かれた CS へのポインタは、どっちみちほかのスレッドが作成される際には
	// 他のプロセッサに対して可視になっているのでたぶん大丈夫

//	fprintf(stderr, "singleton %s\n", info.get_name());

	if(functions == NULL)
		functions = new gc_vector<register_info_t>();

	functions->push_back(info);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void singleton_manager::register_manual_start(handler_t func)
{
	// ここもマルチスレッドからの保護は必要ない (main以前によばれるので)
	if(manual_starts == NULL)
		manual_starts = new gc_vector<handler_t>();
	manual_starts->push_back(func);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void singleton_manager::register_disconnector(handler_t func)
{
	// ここはマルチスレッドからの保護が必要だが、これを呼ぶ
	// singleton_base<T>::make_instance 内ですでに保護されている
	if(disconnectors == NULL)
		disconnectors = new gc_vector<handler_t>();
	disconnectors->push_back(func);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void singleton_manager::unregister_info()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void singleton_manager::init_all()
{
	if(functions)
	{
/*
		fprintf(stderr, "-- singleton list start --\n");
		for(gc_vector<register_info_t>::iterator i = functions->begin();
			i != functions->end(); i++)
		{
			fprintf(stderr, "singleton %s\n", i->get_name());
		}
		fprintf(stderr, "-- singleton list end --\n");
*/
		for(gc_vector<register_info_t>::reverse_iterator i = functions->rbegin();
			i != functions->rend(); i++)
		{
			if(manual_starts)
			{
				// manual_starts の中から i->ensure を探す。
				// 一致した物があった場合、ここでの作成は行わない。
				// 単純な線形検索だが、シングルトンオブジェクトが数千などに
				// ならない限りそれほどパフォーマンスロスにはならないだろう
				gc_vector<handler_t>::iterator p = 
					std::find(manual_starts->begin(), manual_starts->end(), i->ensure);
				if(p != manual_starts->end()) continue;
			}
			i->ensure();
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void singleton_manager::disconnect_all()
{
	if(disconnectors)
	{
		for(gc_vector<handler_t>::reverse_iterator i = disconnectors->rbegin();
			i != disconnectors->rend(); i++)
		{
			(*i)();
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void singleton_manager::report_alive_objects()
{
	if(functions)
	{
		for(gc_vector<register_info_t>::iterator i = functions->begin();
			i != functions->end(); i++)
		{
			if(i->alive())
			{
				fprintf(stderr, "object %s is alive\n", i->get_name());
			}
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


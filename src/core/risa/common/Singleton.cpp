//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief シングルトンを実現する自家製テンプレートクラス
//---------------------------------------------------------------------------

#include "risa/prec.h"
#include "risa/common/Singleton.h"
#include <algorithm>

namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const char * Demangle(const char * name)
{
#ifdef HAVE_CXXABI_H
	// __cxa_demangle を用いて、name のマングリングを解除する
	int status = 0;
	char * demangled = abi::__cxa_demangle(name, NULL, 0, &status);
	if(demangled)
	{
		// GC の管理下のバッファに文字列をコピーする
		// Risse::tSS<...> はちょっと特殊なので別途処理をする
		// Risse::tSS<"パッケージ名"> みたいな感じに変換なのだ
		size_t demangled_len = strlen(demangled);
		char *p = (char*)MallocAtomicCollectee(demangled_len+3);
		char *pp = p;
		for(size_t i = 0; i < demangled_len; i++)
		{
			if(memcmp(demangled + i, "Risse::tSS<", 11) == 0)
			{
				i += 11;
				memcpy(pp, "Risse::tSS<\"", 12); pp += 12;
				while(true)
				{
					while(i < demangled_len &&
							demangled[i] != '>' &&
							!(demangled[i] >= '0' && demangled[i] <= '9')) i++;
					if(i == demangled_len) return NULL; // ??
					if(demangled[i] == '>') { i++; break; }
					int n = 0;
					while(demangled[i] >= '0' && demangled[i] <= '9')
						n*=10, n+=demangled[i]-'0', i++;
					if(n != 0) *(pp++) = (char)n;
					while(demangled[i] != ',' && demangled[i] != '>') i++;
					if(demangled[i] == '>')
					{
						*(pp++) = '"';
						*(pp++) = '>';
						break;
					}
					i++;
				}
			}
			else
			{
				*pp = demangled[i];
				pp++;
			}
		}
		*pp = 0;
		free(demangled);
		return p;
	}
	else
		return name;
#else
	return name;
#endif
}
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
	RISSE_ASSERT_CS_LOCKED(singleton_manager::GetCS());
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


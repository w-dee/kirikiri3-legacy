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

#include "base/utils/Singleton.h"


//---------------------------------------------------------------------------
std::vector<singleton_manager::register_info_t> * singleton_manager::functions = NULL;
std::vector<singleton_manager::handler_t> * singleton_manager::disconnectors = NULL;
std::vector<singleton_manager::handler_t> * singleton_manager::manual_starts = NULL;
unsigned int singleton_manager::ref_count = 0;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		シングルトン情報を登録する
//---------------------------------------------------------------------------
void singleton_manager::register_info(const singleton_manager::register_info_t & info)
{
//	fprintf(stderr, "singleton %s\n", info.get_name());

	if(functions == NULL)
		functions = new std::vector<register_info_t>();

	functions->push_back(info);

	ref_count ++;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		手動起動を表すシングルトンの情報を登録する
//---------------------------------------------------------------------------
void singleton_manager::register_manual_start(handler_t func)
{
	if(manual_starts == NULL)
		manual_starts = new std::vector<handler_t>();
	manual_starts->push_back(func);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		disconnect 関数を登録する
//---------------------------------------------------------------------------
void singleton_manager::register_disconnector(handler_t func)
{
	if(disconnectors == NULL)
		disconnectors = new std::vector<handler_t>();
	disconnectors->push_back(func);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		シングルトン情報の登録を解除する
//---------------------------------------------------------------------------
void singleton_manager::unregister_info()
{
	if(functions)
	{
		ref_count --;
		if(ref_count == 0)
		{
			delete functions, functions = NULL;
			delete disconnectors, disconnectors = NULL;
			delete manual_starts, manual_starts = NULL;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		全てのシングルトンを初期化する
//! @note		この間に発生した例外は呼び出し元で捕捉できる
//---------------------------------------------------------------------------
void singleton_manager::init_all()
{
	if(functions)
	{
/*
		fprintf(stderr, "-- singleton list start --\n");
		for(std::vector<register_info_t>::iterator i = functions->begin();
			i != functions->end(); i++)
		{
			fprintf(stderr, "singleton %s\n", i->get_name());
		}
		fprintf(stderr, "-- singleton list end --\n");
*/
		for(std::vector<register_info_t>::reverse_iterator i = functions->rbegin();
			i != functions->rend(); i++)
		{
			if(manual_starts)
			{
				// manual_starts の中から i->ensure を探す。
				// 一致した物があった場合、ここでの作成は行わない。
				// 単純な線形検索だが、シングルトンオブジェクトが数千などに
				// ならない限りそれほどパフォーマンスロスにはならないだろう
				std::vector<handler_t>::iterator p = 
					std::find(manual_starts->begin(), manual_starts->end(), i->ensure);
				if(p != manual_starts->end()) continue;
			}
			i->ensure();
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		全てのシングルトンへの参照を切る
//! @note		これによりすべてのシングルトンが消滅する保証はない。
//!				他の場所でこのシングルトンオブジェクトへの参照が残っていた場合は
//!				その参照が無くなるまでそのシングルトンオブジェクトおよびそれが
//!				依存しているシングルトンオブジェクトは消滅しないままとなる。
//!				disconnect関数の呼び出しは、register_disconnector を呼び出した順とは
//!				逆順となる。
//---------------------------------------------------------------------------
void singleton_manager::disconnect_all()
{
	if(disconnectors)
	{
		for(std::vector<handler_t>::reverse_iterator i = disconnectors->rbegin();
			i != disconnectors->rend(); i++)
		{
			(*i)();
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		削除されずに残っているオブジェクトを標準エラー出力に表示する
//---------------------------------------------------------------------------
void singleton_manager::report_alive_objects()
{
	if(functions)
	{
		for(std::vector<register_info_t>::iterator i = functions->begin();
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


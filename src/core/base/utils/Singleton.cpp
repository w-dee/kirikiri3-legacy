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
std::vector<tRisaSingletonManager::tRegisterInfo> * tRisaSingletonManager::Functions = NULL;
std::vector<tRisaSingletonManager::tFunction> * tRisaSingletonManager::Disconnectors = NULL;
std::vector<tRisaSingletonManager::tFunction> * tRisaSingletonManager::ManualStarts = NULL;
unsigned int tRisaSingletonManager::RefCount = 0;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ensure 関数を登録する
//---------------------------------------------------------------------------
void tRisaSingletonManager::Register(const tRisaSingletonManager::tRegisterInfo & info)
{
//	fprintf(stderr, "singleton %s\n", info.GetName());

	if(Functions == NULL)
		Functions = new std::vector<tRegisterInfo>();

	Functions->push_back(info);

	RefCount ++;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		手動起動を表すクラスの ensure 関数を登録する
//---------------------------------------------------------------------------
void tRisaSingletonManager::RegisterManualStart(tFunction func)
{
	if(ManualStarts == NULL)
		ManualStarts = new std::vector<tFunction>();
	ManualStarts->push_back(func);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		disconnect 関数を登録する
//---------------------------------------------------------------------------
void tRisaSingletonManager::RegisterDisconnector(tFunction func)
{
	if(Disconnectors == NULL)
		Disconnectors = new std::vector<tFunction>();
	Disconnectors->push_back(func);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ensure 関数の登録を解除する
//---------------------------------------------------------------------------
void tRisaSingletonManager::Unregister()
{
	if(Functions)
	{
		RefCount --;
		if(RefCount == 0)
		{
			delete Functions, Functions = NULL;
			delete Disconnectors, Disconnectors = NULL;
			delete ManualStarts, ManualStarts = NULL;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		全てのシングルトンを初期化する
//! @note		この間に発生した例外は呼び出し元で捕捉できる
//---------------------------------------------------------------------------
void tRisaSingletonManager::InitAll()
{
	if(Functions)
	{
/*
		fprintf(stderr, "-- singleton list start --\n");
		for(std::vector<tRegisterInfo>::iterator i = Functions->begin();
			i != Functions->end(); i++)
		{
			fprintf(stderr, "singleton %s\n", i->GetName());
		}
		fprintf(stderr, "-- singleton list end --\n");
*/
		for(std::vector<tRegisterInfo>::iterator i = Functions->begin();
			i != Functions->end(); i++)
		{
			if(ManualStarts)
			{
				// ManualStarts の中から i->Ensure を探す。
				// 一致した物があった場合、ここでの作成は行わない。
				// 単純な線形検索だが、シングルトンオブジェクトが数千などに
				// ならない限りそれほどパフォーマンスロスにはならないだろう
				std::vector<tFunction>::iterator p = 
					std::find(ManualStarts->begin(), ManualStarts->end(), i->Ensure);
				if(p != ManualStarts->end()) continue;
			}
			i->Ensure();
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
//!				disconnect関数の呼び出しは、RegisterDisconnector を呼び出した順とは
//!				逆順となる。
//---------------------------------------------------------------------------
void tRisaSingletonManager::DisconnectAll()
{
	if(Disconnectors)
	{
		for(std::vector<tFunction>::reverse_iterator i = Disconnectors->rbegin();
			i != Disconnectors->rend(); i++)
		{
			(*i)();
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		削除されずに残っているオブジェクトを標準エラー出力に表示する
//---------------------------------------------------------------------------
void tRisaSingletonManager::ReportAliveObjects()
{
	if(Functions)
	{
		for(std::vector<tRegisterInfo>::iterator i = Functions->begin();
			i != Functions->end(); i++)
		{
			if(i->Alive())
			{
				fprintf(stderr, "object %s is alive\n", i->GetName());
			}
		}
	}
}
//---------------------------------------------------------------------------

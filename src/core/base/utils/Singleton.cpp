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

#include "Singleton.h"


//---------------------------------------------------------------------------
std::vector<tRisaSingletonManager::tEnsureFunction> * tRisaSingletonManager::EnsureFunctions = NULL;
unsigned int tRisaSingletonManager::RefCount = 0;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ensure 関数を登録する
//---------------------------------------------------------------------------
void tRisaSingletonManager::Register(tRisaSingletonManager::tEnsureFunction function)
{
	if(EnsureFunctions == NULL)
		EnsureFunctions = new std::vector<tEnsureFunction>();

	EnsureFunctions->push_back(function);

	RefCount ++;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		ensure 関数の登録を解除する
//---------------------------------------------------------------------------
void tRisaSingletonManager::Unregister()
{
	if(EnsureFunctions)
	{
		RefCount --;
		if(RefCount == 0)
		{
			delete EnsureFunctions, EnsureFunctions = NULL;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		全てのシングルトンを初期化する
//---------------------------------------------------------------------------
void tRisaSingletonManager::InitAll()
{
	if(EnsureFunctions)
	{
		for(std::vector<tEnsureFunction>::iterator i = EnsureFunctions->begin();
			i != EnsureFunctions->end(); i++)
		{
			tEnsureFunction function = *i;
			function();
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 実行コンテキスト管理
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseExecutorContext.h"


namespace Risse
{
//---------------------------------------------------------------------------
RISSE_DEFINE_SOURCE_ID(42398,5962,43064,17612,64140,47331,53821,16458);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseExecutorContext::tRisseExecutorContext()
{
	// 最初のcalleeを登録する
	PushCallee(tRisseVariant(), ocNoOperation);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseExecutorContext::Run()
{
	risse_size stack_size = Stack.size();
	while(stack_size > 1) // スタックは大本のcallerが必ずあるので最低1つは常に存在する
	{
		// スタックトップにある呼び出し先を呼び出す
		GetTop().Info.Callee.Operate(this);

		// スタックのサイズをチェック
		risse_size new_stack_size = Stack.size();
		RISSE_ASSERT(new_stack_size >= stack_size); // スタックサイズが減ることはないはず
		if(new_stack_size == stack_size)
		{
			// 新しいスタックがpushされなかったのでpopする
			Pop();
			new_stack_size --;
		}

		stack_size = new_stack_size;
	}
}
//---------------------------------------------------------------------------




} // namespace Risse

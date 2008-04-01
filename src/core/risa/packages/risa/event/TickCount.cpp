//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TickCount を提供するモジュール
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/event/TickCount.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(37180,40935,47171,16902,29833,29636,3244,55820);
//---------------------------------------------------------------------------


#ifdef RISA_TICKCOUNT_NEED_WRAP_WATCH

//---------------------------------------------------------------------------
tTickCount::tWatcher::tWatcher(tTickCount & owner) : Owner(owner)
{
	Run(); // スレッドの実行を開始
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTickCount::tWatcher::~tWatcher()
{
	Terminate(); // スレッドの終了を伝える
	Event.Signal(); // スレッドをたたき起こす
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTickCount::tWatcher::Execute()
{
	while(!ShouldTerminate())
	{
		// tick count を取得する
		// この際、wrap around も正常に処理される
		(void) Owner.Get();

		// 60秒をまつか、セマフォが取得できるまで待つ。
		// 実際の所これはこれほど短い秒数を待つ必要は全くなく、
		// tick count の wraparound を検出できるほどに長ければよい
		Event.Wait(60*1000); // 60秒待つ
	}
}
//---------------------------------------------------------------------------
#endif














//---------------------------------------------------------------------------
tTickCount::tTickCount()
{
#ifdef RISA_TICKCOUNT_NEED_WRAP_WATCH
	// フィールドの初期化
	Value = 1;
		// 0 でもよいが、0 と ~0 (=InvalidTickCount) だけは予約しておきたいので1
	LastTick = GetTick();

	// スレッドの開始
	Watcher = new tWatcher(*this);
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTickCount::~tTickCount()
{
#ifdef RISA_TICKCOUNT_NEED_WRAP_WATCH
	// スレッドの停止
	delete Watcher;
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tTickCount::Get()
{
	volatile tCriticalSection::tLocker cs_holder(CS);

#ifdef RISA_TICKCOUNT_NEED_WRAP_WATCH
	// 現在の tick と前回取得した tick の差を Value に追加する。
	// 現在の tick と前回取得した tick の差が tTickCountBasicType型 の
	// 最大値を上回らないことは、それを上回らない程度に このメソッドが
	// Watcher のスレッドから定期的に呼ばれることで保証されている。
	tTickCountBasicType nowtick = GetTick();
	Value += nowtick - LastTick;
	LastTick = nowtick;
	return Value;
#else
	return GetTick();
#endif

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


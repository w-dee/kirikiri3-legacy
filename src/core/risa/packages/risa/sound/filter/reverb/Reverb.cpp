//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief リバーブフィルタの実装 (Freeverb)
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/sound/filter/reverb/Reverb.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(8600,37386,12503,16952,7601,59827,19639,49324);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
tReverbInstance::tReverbInstance() :
	tWaveFilterInstance(tPCMTypes::tf32)
{
	Buffer = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tReverbInstance::InputChanged()
{
	Model.mute();

	// Input に PCM 形式を提案する
	tWaveFormat format;
	format.Reset();
	format.PCMType = tPCMTypes::tf32; // float がいい！できれば float にして！！
	Input->SuggestFormat(format);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tReverbInstance::Filter()
{
	// Buffer にデータを読み込む
	tWaveSegmentQueue segmentqueue;

	const risse_uint channels = InputFormat.Channels;
	if(channels != 2) return; // チャンネルは 2 (stereo)以外は今のところ対応していない

	// 入力バッファを確保
	if(!Buffer) Buffer = static_cast<float*>(MallocAtomicCollectee(
					sizeof(float) * NumBufferSampleGranules * InputFormat.Channels));

	// 出力バッファを確保
	float * dest_buf = static_cast<float*>(PrepareQueue(NumBufferSampleGranules));
	if(!dest_buf) return;

	// 入力からデータを読み取る
	risse_uint filled = Fill(Buffer, NumBufferSampleGranules, tPCMTypes::tf32, true, segmentqueue);

	if(filled == 0) return;

	// リバーブエフェクトの実行
	Model.processreplace(Buffer, Buffer+1, dest_buf, dest_buf+1, NumBufferSampleGranules, 2);

	// キューする
	Queue(NumBufferSampleGranules, segmentqueue);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tReverbInstance::construct()
{
	// デフォルトではなにもしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tReverbInstance::initialize(const tNativeCallInfo &info)
{
	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tReverbClass,
		(tSS<'R','e','v','e','r','b'>()),
		tClassHolder<tWaveFilterClass>::instance()->GetClass())
	BindFunction(this, ss_ovulate, &tReverbClass::ovulate);
	BindFunction(this, ss_construct, &tReverbInstance::construct);
	BindFunction(this, ss_initialize, &tReverbInstance::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * Reverb クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','s','o','u','n','d','.','f','i','l','t','e','r'>,
	tReverbClass>;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa



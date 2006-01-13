//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Waveデコーダインターフェースの定義
//---------------------------------------------------------------------------
#ifndef _WAVEDECODERH_
#define _WAVEDECODERH_




//---------------------------------------------------------------------------
//! @brief		スピーカー定義
//---------------------------------------------------------------------------
#ifndef SPEAKER_FRONT_LEFT
// from Windows ksmedia.h
// speaker config

#define     SPEAKER_FRONT_LEFT              0x1
#define     SPEAKER_FRONT_RIGHT             0x2
#define     SPEAKER_FRONT_CENTER            0x4
#define     SPEAKER_LOW_FREQUENCY           0x8
#define     SPEAKER_BACK_LEFT               0x10
#define     SPEAKER_BACK_RIGHT              0x20
#define     SPEAKER_FRONT_LEFT_OF_CENTER    0x40
#define     SPEAKER_FRONT_RIGHT_OF_CENTER   0x80
#define     SPEAKER_BACK_CENTER             0x100
#define     SPEAKER_SIDE_LEFT               0x200
#define     SPEAKER_SIDE_RIGHT              0x400
#define     SPEAKER_TOP_CENTER              0x800
#define     SPEAKER_TOP_FRONT_LEFT          0x1000
#define     SPEAKER_TOP_FRONT_CENTER        0x2000
#define     SPEAKER_TOP_FRONT_RIGHT         0x4000
#define     SPEAKER_TOP_BACK_LEFT           0x8000
#define     SPEAKER_TOP_BACK_CENTER         0x10000
#define     SPEAKER_TOP_BACK_RIGHT          0x20000

#endif
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		PCM データフォーマット (内部用)
//---------------------------------------------------------------------------
struct tTVPWaveFormat
{
	tjs_uint SamplesPerSec;		//!< sample granule per sec
	tjs_uint Channels;			//!< number of channels (1=Mono, 2=Stereo ... etc)
	tjs_uint BitsPerSample;		//!< bits per one sample
	tjs_uint BytesPerSample;	//!< bytes per one sample
	tjs_uint64 TotalSamples;	//!< total samples in sample granule; unknown for zero
	tjs_uint64 TotalTime;		//!< in ms; unknown for zero
	tjs_uint32 SpeakerConfig;	//!< bitwise OR of SPEAKER_* constants
	bool IsFloat;				//!< true if the data is IEEE floating point
	bool Seekable;				//!< true if able to seek, otherwise false
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief	 デコーダインターフェース
//---------------------------------------------------------------------------
class tTVPWaveDecoder
{
public:
	virtual ~tTVPWaveDecoder() {};

	virtual void GetFormat(tTVPWaveFormat & format) = 0;
		/*!< @brief フォーマットを取得する
			@note
			Retrieve PCM format, etc. */

	virtual bool Render(void *buf, tjs_uint bufsamplelen, tjs_uint& rendered) = 0;
		/*!< @brief サウンドをレンダリングする
			@note
			Render PCM from current position.
			where "buf" is a destination buffer, "bufsamplelen" is the buffer's
			length in sample granule, "rendered" is to be an actual number of
			written sample granule.
			returns whether the decoding is to be continued.
			because "redered" can be lesser than "bufsamplelen", the player
			should not end until the returned value becomes false.
		*/

	virtual bool SetPosition(tjs_uint64 samplepos) = 0;
		/*!< @brief デコード位置を変更する
			@note
			Seek to "samplepos". "samplepos" must be given in unit of sample granule.
			returns whether the seeking is succeeded.
		*/
};
//---------------------------------------------------------------------------


#endif


//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief サウンドクラスの Risse バインディング
//---------------------------------------------------------------------------
#include "prec.h"
#include "risse/include/risseNative.h"
#include "sound/SoundBind.h"
#include "base/script/RisseEngine.h"

#if 0

RISSE_DEFINE_SOURCE_ID(38428,3640,56667,17105,48032,55694,30447,43980);
namespace Risa {
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		Soundネイティブインスタンス
//---------------------------------------------------------------------------
class tNI_Sound : public tNativeInstance, public tRisaSound
{
	iRisseDispatch2 * Owner; //!< オーナーとなるRisseオブジェクト

public:
	tNI_Sound();

	risse_error Construct(risse_int numparams,
		tVariant **param, iRisseDispatch2 *risse_obj);
	void Invalidate();

	void OnStatusChanged(tStatus status); //  from tRisaSound

	tString GetStatusString() const;

	static tString StatusToString(tStatus status);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tNI_Sound::tNI_Sound()
{
	// フィールドの初期化
	Owner = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Risse コンストラクタ
//---------------------------------------------------------------------------
risse_error tNI_Sound::Construct(risse_int numparams,
		tVariant **param, iRisseDispatch2 *risse_obj)
{
	/*%
		@fn		Sound.Sound
		@brief	Soundを構築する
	*/
	Owner = risse_obj;
	return RISSE_S_OK;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Risse 無効化関数
//---------------------------------------------------------------------------
void tNI_Sound::Invalidate()
{
	// Owner を NULL に
	Owner = NULL;

	// 内部状態のクリア
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ステータスが変わった
//! @param		status  ステータス
//---------------------------------------------------------------------------
void tNI_Sound::OnStatusChanged(tStatus status)
{
	// イベント関数を呼ぶ
	if(Owner)
	{
		static tString onStatusChanged_name(RISSE_WS("onStatusChanged"));
		tVariant val(StatusToString(status));
		tVariant *params[] = {&val};
		Owner->FuncCall(
			0, // flag
			onStatusChanged_name.c_str(), // name
			onStatusChanged_name.GetHint(), // hint
			NULL, // result
			1, // numparams
			params, // parameters
			Owner // objthis
			);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ステータス文字列を得る
//---------------------------------------------------------------------------
tString tNI_Sound::GetStatusString() const
{
	return StatusToString(GetStatus());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ステータス列挙からステータス文字列に変換を行う
//! @param		status ステータス
//! @return		ステータス文字列
//---------------------------------------------------------------------------
tString tNI_Sound::StatusToString(tStatus status)
{
	static tString unload(RISSE_WS("unload"));
	static tString play  (RISSE_WS("play"));
	static tString stop  (RISSE_WS("stop"));
	static tString pause (RISSE_WS("pause"));
	static tString unknown (RISSE_WS("unknown"));

	switch(status)
	{
	case ssUnload:
		return unload;
	case ssPlay:
		return play;
	case ssStop:
		return stop;
	case ssPause:
		return pause;
	}

	return unknown;
}
//---------------------------------------------------------------------------
















//---------------------------------------------------------------------------
//! @brief		Soundネイティブクラス
//---------------------------------------------------------------------------
class tNC_Sound : public tNativeClass
{
public:
	tNC_Sound();

	static risse_uint32 ClassID;

private:
	tNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		SoundクラスID
//---------------------------------------------------------------------------
risse_uint32 tNC_Sound::ClassID = (risse_uint32)-1;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tNC_Sound::tNC_Sound() :
	tNativeClass(RISSE_WS("Sound"))
{
	// class constructor

	RISSE_BEGIN_NATIVE_MEMBERS(/*Risse class name*/Sound)
	RISSE_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var. name*/_this, /*var. type*/tNI_Sound,
	/*Risse class name*/ Sound)
{
	return RISSE_S_OK;
}
RISSE_END_NATIVE_CONSTRUCTOR_DECL(/*Risse class name*/Sound)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/open)
{
	/*%
		@brief	ファイルを開く
		@param	filename ファイル名
		@note
		<p>
			指定されたファイルを開きます
		</p>
	*/
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tNI_Sound);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	_this->Open(*param[0]);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/open)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/close)
{
	/*%
		@brief	ファイルを閉じる
		@note
		<p>
			ファイルを閉じます。以降、再び open メソッドでファイルを開くまで
			再生は開始できません。
		</p>
	*/
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tNI_Sound);

	_this->Close();

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/close)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/play)
{
	/*%
		@brief	再生を開始する
		@note
		<p>
			再生を開始します。
		</p>
	*/
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tNI_Sound);

	_this->Play();

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/play)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/stop)
{
	/*%
		@brief	再生を停止する
		@note
		<p>
			再生を停止します。
		</p>
	*/
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tNI_Sound);

	_this->Stop();

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/stop)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/pause)
{
	/*%
		@brief	再生を一時停止する
		@note
		<p>
			再生を一時停止します。
		</p>
	*/
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tNI_Sound);

	_this->Pause();

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/pause)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/onStatusChanged)
{
	/*%
		@type event
		@brief	ステータスが変更された
		@param	status  ステータス文字列
		@note
		<p>
			ステータスが変更された際に呼び出されます。
			ステータス文字列は "unload" (メディアが読み込まれていない)、
			"stop" (停止中)、"play" (再生中)、"pause" (一時停止中) の
			いずれかです。
		</p>
	*/
	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/onStatusChanged)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(status)
{
	/*%
		@brief	ステータス
		@note
		<p>
			現在のステータスを表す文字列です。読み出し専用です。
		</p>
		<p>
			ステータス文字列は "unload" (メディアが読み込まれていない)、
			"stop" (停止中)、"play" (再生中)、"pause" (一時停止中) の
			いずれかです。
		</p>
	*/
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tNI_Sound);
		if(result) *result = _this->GetStatusString();
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_DENY_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(status)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(samplePosition)
{
	/*%
		@brief	再生位置
		@note
		<p>
			現在の再生位置を(元のメディア上の)サンプル数単位で表します。
		</p>
	*/
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tNI_Sound);
		if(result) *result = (tTVInteger)_this->GetSamplePosition();
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_BEGIN_NATIVE_PROP_SETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tNI_Sound);
		_this->SetSamplePosition((tTVInteger)*param);
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(samplePosition)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(position)
{
	/*%
		@brief	再生位置
		@note
		<p>
			現在の再生位置を(元のメディア上の)時間で表します。単位はミリ秒です。
		</p>
	*/
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tNI_Sound);
		if(result) *result = _this->GetTimePosition();
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_BEGIN_NATIVE_PROP_SETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tNI_Sound);
		_this->SetTimePosition((double)*param);
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(position)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
	RISSE_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ネイティブインスタンスを作成して返す
//! @return		ネイティブインスタンス
//---------------------------------------------------------------------------
tNativeInstance *tNC_Sound::CreateNativeInstance()
{
	return new tNI_Sound();
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tRisaSoundRegisterer :
			public singleton_base<tRisaSoundRegisterer>,
			protected depends_on<tRisaRisseScriptEngine>
{
public:
	tRisaSoundRegisterer()
	{
		iRisseDispatch2 * cls =  new tNC_Sound();
		try
		{
			depends_on<tRisaRisseScriptEngine>::locked_instance()->
				RegisterGlobalObject(RISSE_WS("Sound"), cls);
		}
		catch(...)
		{
			cls->Release();
			throw;
		}
		cls->Release();
	}
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa



#endif


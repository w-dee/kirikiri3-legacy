//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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

RISSE_DEFINE_SOURCE_ID(38428,3640,56667,17105,48032,55694,30447,43980);


//---------------------------------------------------------------------------
//! @brief		Soundネイティブインスタンス
//---------------------------------------------------------------------------
class tRisseNI_Sound : public tRisseNativeInstance, public tRisaSound
{
	iRisseDispatch2 * Owner; //!< オーナーとなるRisseオブジェクト

public:
	tRisseNI_Sound();

	risse_error Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj);
	void Invalidate();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisseNI_Sound::tRisseNI_Sound()
{
	// フィールドの初期化
	Owner = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Risse コンストラクタ
//---------------------------------------------------------------------------
risse_error tRisseNI_Sound::Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj)
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
void tRisseNI_Sound::Invalidate()
{
	// 内部状態のクリア
	Clear();

	// Owner を NULL に
	Owner = NULL;
}
//---------------------------------------------------------------------------



















//---------------------------------------------------------------------------
//! @brief		Soundネイティブクラス
//---------------------------------------------------------------------------
class tRisseNC_Sound : public tRisseNativeClass
{
public:
	tRisseNC_Sound();

	static risse_uint32 ClassID;

private:
	tRisseNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		SoundクラスID
//---------------------------------------------------------------------------
risse_uint32 tRisseNC_Sound::ClassID = (risse_uint32)-1;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisseNC_Sound::tRisseNC_Sound() :
	tRisseNativeClass(RISSE_WS("Sound"))
{
	// class constructor

	RISSE_BEGIN_NATIVE_MEMBERS(/*Risse class name*/Sound)
	RISSE_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var. name*/_this, /*var. type*/tRisseNI_Sound,
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
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Sound);

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
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Sound);

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
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Sound);

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
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Sound);

	_this->Stop();

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/stop)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
	RISSE_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ネイティブインスタンスを作成して返す
//! @return		ネイティブインスタンス
//---------------------------------------------------------------------------
tRisseNativeInstance *tRisseNC_Sound::CreateNativeInstance()
{
	return new tRisseNI_Sound();
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tRisaSoundRegisterer :
			public singleton_base<tRisaSoundRegisterer>,
			depends_on<tRisaRisseScriptEngine>
{
public:
	tRisaSoundRegisterer()
	{
		iRisseDispatch2 * cls =  new tRisseNC_Sound();
		try
		{
			tRisaRisseScriptEngine::instance()->
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





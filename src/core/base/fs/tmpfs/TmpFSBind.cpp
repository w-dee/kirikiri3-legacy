//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TmpFSのTJS3バインディング
//---------------------------------------------------------------------------
#include "prec.h"
#include "TmpFS.h"
#include "TmpFSBind.h"
#include "TVPException.h"


TJS_DEFINE_SOURCE_ID(2014);



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTJSNI_TmpFS::tTJSNI_TmpFS()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		TJS コンストラクタ
//---------------------------------------------------------------------------
tjs_error tTJSNI_TmpFS::Construct(tjs_int numparams,
		tTJSVariant **param, iTJSDispatch2 *tjs_obj)
{
	/*%
		@fn		TmpFS.TmpFS
		@brief	TmpFSを構築する
	*/

	// filesystem オブジェクトの生成と登録
	RegisterFileSystemNativeInstance(tjs_obj, new tTVPTmpFS());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		TJS 無効化関数
//---------------------------------------------------------------------------
void tTJSNI_TmpFS::Invalidate()
{
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		TmpFS クラスID
//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_TmpFS::ClassID = (tjs_uint32)-1;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTJSNC_TmpFS::tTJSNC_TmpFS() :
	tTJSNativeClass(TJS_WS("TmpFS"))
{
	// class constructor

	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/TmpFS)
	TJS_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var. name*/_this, /*var. type*/tTJSNI_TmpFS,
	/*TJS class name*/ TmpFS)
{
	return TJS_S_OK;
}
TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/TmpFS)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/save)
{
	/*%
		@brief	ファイルシステムの内容を保存する
		@param	filename		保存先ファイル名
		@note
		<p>
			ファイルシステムの内容を単一のファイルに保存します。
			各ディレクトリの構造、ファイルの内容など、すべてが保存されます。
		</p>
		<p>
			ここで保存したファイルは、<code>TmpFS.load</code>メソッドで読み込
			むことが出来ます。
		</p>
	*/

	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_TmpFS);

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;
	ttstr filename = *param[0];

	_this->GetFileSystem()->SerializeTo(filename);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/save)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/load)
{
	/*%
		@brief	ファイルシステムの内容を復元する
		@param	filename		復元する内容が記録されたファイル名
		@note
		<p>
			<code>TmpFS.save</code> メソッドで保存されたファイルシステムの内容を
			読み込みます。
		</p>
		<p>
			このファイルシステムにそれまでに存在していた内容はすべて消去され、
			読み込まれた内容に置き換わりますので注意してください。
		</p>
	*/

	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_TmpFS);

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;
	ttstr filename = *param[0];

	_this->GetFileSystem()->UnserializeFrom(filename);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/load)
//----------------------------------------------------------------------
	TJS_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ネイティブインスタンスを作成して返す
//! @return		ネイティブインスタンス
//---------------------------------------------------------------------------
tTJSNativeInstance *tTJSNC_TmpFS::CreateNativeInstance()
{
	return new tTJSNI_TmpFS();
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPTmpFSRegisterer::tTVPTmpFSRegisterer()
{
	// ファイルシステムクラスオブジェクトの下にクラスオブジェクトを登録する
	iTJSDispatch2 * cls = new tTJSNC_TmpFS();
	try
	{
		tTVPFileSystemRegisterer::instance()->RegisterClassObject(
							TJS_WS("TmpFS"), cls);
	}
	catch(...)
	{
		cls->Release();
		throw;
	}
	cls->Release();
}
//---------------------------------------------------------------------------


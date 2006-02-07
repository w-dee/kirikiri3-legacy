//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TmpFSのRisseバインディング
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/fs/tmpfs/TmpFS.h"
#include "base/fs/tmpfs/TmpFSBind.h"
#include "base/exception/RisaException.h"


RISSE_DEFINE_SOURCE_ID(8315,58560,35490,17582,45491,52576,20915,60482);



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisseNI_TmpFS::tRisseNI_TmpFS()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Risse コンストラクタ
//---------------------------------------------------------------------------
risse_error tRisseNI_TmpFS::Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj)
{
	/*%
		@fn		TmpFS.TmpFS
		@brief	TmpFSを構築する
	*/

	// filesystem オブジェクトの生成と登録
	RegisterFileSystemNativeInstance(risse_obj, new tRisaTmpFS());

	return RISSE_S_OK;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Risse 無効化関数
//---------------------------------------------------------------------------
void tRisseNI_TmpFS::Invalidate()
{
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		TmpFS クラスID
//---------------------------------------------------------------------------
risse_uint32 tRisseNC_TmpFS::ClassID = (risse_uint32)-1;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisseNC_TmpFS::tRisseNC_TmpFS() :
	tRisseNativeClass(RISSE_WS("TmpFS"))
{
	// class constructor

	RISSE_BEGIN_NATIVE_MEMBERS(/*Risse class name*/TmpFS)
	RISSE_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var. name*/_this, /*var. type*/tRisseNI_TmpFS,
	/*Risse class name*/ TmpFS)
{
	return RISSE_S_OK;
}
RISSE_END_NATIVE_CONSTRUCTOR_DECL(/*Risse class name*/TmpFS)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/save)
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

	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_TmpFS);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;
	ttstr filename = *param[0];

	_this->GetFileSystem()->SerializeTo(filename);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/save)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/load)
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

	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_TmpFS);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;
	ttstr filename = *param[0];

	_this->GetFileSystem()->UnserializeFrom(filename);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/load)
//----------------------------------------------------------------------
	RISSE_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ネイティブインスタンスを作成して返す
//! @return		ネイティブインスタンス
//---------------------------------------------------------------------------
tRisseNativeInstance *tRisseNC_TmpFS::CreateNativeInstance()
{
	return new tRisseNI_TmpFS();
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaTmpFSRegisterer::tRisaTmpFSRegisterer()
{
	// ファイルシステムクラスオブジェクトの下にクラスオブジェクトを登録する
	iRisseDispatch2 * cls = new tRisseNC_TmpFS();
	try
	{
		tRisaFileSystemRegisterer::instance()->RegisterClassObject(
							RISSE_WS("TmpFS"), cls);
	}
	catch(...)
	{
		cls->Release();
		throw;
	}
	cls->Release();
}
//---------------------------------------------------------------------------


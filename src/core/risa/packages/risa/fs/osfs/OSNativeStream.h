//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OSFSの実装
//---------------------------------------------------------------------------
#ifndef _OSNativeStreamH_
#define _OSNativeStreamH_

#include "risa/packages/risa/fs/FSManager.h"
#include "base/utils/RisaThread.h"
#include "risse/include/risseWCString.h"
#include "risse/include/builtin/stream/risseStreamClass.h"
#include <wx/file.h>

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		OS ネイティブファイルストリーム(OSNativeStreamクラス)
//---------------------------------------------------------------------------
class tOSNativeStreamInstance : public tStreamInstance
{
protected:
	//! @brief		内部情報用構造体(GCの対象で、回収に際してデストラクタが呼ばれる)
	struct tInternal : public tDestructee
	{
		wxFile File;
	} * Internal;

public:
	//! @brief		コンストラクタ
	tOSNativeStreamInstance() { Internal = NULL; }

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tOSNativeStreamInstance();

public: // risse 用メソッドとか
	void construct() {;}
	void initialize(const tString & path, risse_uint32 flags, const tNativeCallInfo &info);

	//! @brief		ストリームを閉じる
	//! @note		基本的にはこれでストリームを閉じること。
	//!				このメソッドでストリームを閉じなかった場合の動作は
	//!				「未定義」である
	void dispose();

	//! @brief		指定位置にシークする
	//! @param		offset			基準位置からのオフセット (正の数 = ファイルの後ろの方)
	//! @param		whence			基準位置
	//! @return		このメソッドは成功すれば真、失敗すれば偽を返す
	bool seek(risse_int64 offset, tOrigin whence);

	//! @brief		現在位置を取得する
	//! @return		現在位置(先頭からのオフセット)
	risse_uint64 tell();

	//! @brief		ストリームから読み込む
	//! @param		buf		読み込んだデータを書き込む先
	//! @return		実際に読み込まれたサイズ
	risse_size get(const tOctet & buf);

	//! @brief		ストリームに書き込む
	//! @param		buf		書き込むデータ
	//! @return		実際に書き込まれたサイズ
	risse_uint put(const tOctet & buf);

	//! @brief		ストリームを現在位置で切りつめる
	void truncate();

	//! @brief		サイズを得る
	//! @return		このストリームのサイズ
	risse_uint64 get_size();

	//! @brief		ストリームをフラッシュする(書き込みバッファをフラッシュする)
	void flush();
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		"OSNativeStream" クラス
//---------------------------------------------------------------------------
class tOSNativeStreamClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tOSNativeStreamClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public:
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
} // namespace Risa


#endif

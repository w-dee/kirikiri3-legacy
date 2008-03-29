//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief スタンダードIOとの入出力を行うクラス
//---------------------------------------------------------------------------
#ifndef StandardIOH
#define StandardIOH

#include "base/utils/RisaThread.h"
#include "base/gc/RisaGC.h"
#include "base/script/RisseEngine.h"
#include "risse/include/builtin/stream/risseStreamClass.h"
#include "risse/include/risseClass.h"
#include "risse/include/risseObjectClass.h"

namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		標準入出力ストリーム
//---------------------------------------------------------------------------
class tStandardIOStreamInstance : public tStreamInstance
{
protected:
	FILE * Stream; //!< ストリーム

public:
	//! @brief		コンストラクタ
	//! @param		flags アクセスフラグ
	tStandardIOStreamInstance();

	//! @brief		デストラクタ(おそらく呼ばれない)
	~tStandardIOStreamInstance() {;}

public: // risse 用メソッドとか
	void construct() {;}
	void initialize(int index, const tNativeCallInfo &info);

	//! @brief		ストリームを閉じる
	//! @note		基本的にはこれでストリームを閉じること。
	//!				このメソッドでストリームを閉じなかった場合の動作は
	//!				「未定義」である
	void dispose();

	//! @brief		ストリームから読み込む
	//! @param		buf		読み込んだデータを書き込む先
	//! @return		実際に読み込まれたサイズ
	risse_size get(const tOctet & buf);

	//! @brief		ストリームに書き込む
	//! @param		buf		書き込むデータ
	//! @return		実際に書き込まれたサイズ
	risse_size put(const tOctet & buf);

	//! @brief		ストリームをフラッシュする(書き込みバッファをフラッシュする)
	void flush();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		"StandardIOStream" クラス
//---------------------------------------------------------------------------
class tStandardIOStreamClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tStandardIOStreamClass(tScriptEngine * engine);

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

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

#include "risa/common/RisaThread.h"
#include "risa/common/RisaGC.h"
#include "risa/common/RisseEngine.h"
#include "risse/include/builtin/stream/risseStreamClass.h"
#include "risse/include/risseClass.h"
#include "risse/include/risseObjectClass.h"

namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 標準入出力ストリーム
 */
class tStandardIOStreamInstance : public tStreamInstance
{
protected:
	FILE * Stream; //!< ストリーム

public:
	/**
	 * コンストラクタ
	 * @param flags	アクセスフラグ
	 */
	tStandardIOStreamInstance();

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	~tStandardIOStreamInstance() {;}

public: // risse 用メソッドとか
	void construct() {;}
	void initialize(int index, const tNativeCallInfo &info);

	/**
	 * ストリームを閉じる
	 * @note	基本的にはこれでストリームを閉じること。
	 *			このメソッドでストリームを閉じなかった場合の動作は
	 *			「未定義」である
	 */
	void dispose();

	/**
	 * ストリームから読み込む
	 * @param buf	読み込んだデータを書き込む先
	 * @return	実際に読み込まれたサイズ
	 */
	risse_size get(const tOctet & buf);

	/**
	 * ストリームに書き込む
	 * @param buf	書き込むデータ
	 * @return	実際に書き込まれたサイズ
	 */
	risse_size put(const tOctet & buf);

	/**
	 * ストリームをフラッシュする(書き込みバッファをフラッシュする)
	 */
	void flush();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * "StandardIOStream" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tStandardIOStreamClass, tClassBase)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif

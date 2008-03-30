//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オンメモリストリームの実装
//---------------------------------------------------------------------------
#ifndef MemoryStreamH
#define MemoryStreamH

#include "base/utils/RisaThread.h"
#include "risa/packages/risa/fs/FSManager.h"
#include "base/gc/RisaGC.h"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		メモリストリームで用いられるメモリブロック
//! @note		このメモリブロックは、複数のストリームで共有される可能性があり、
//!				複数スレッドから同時アクセスされる可能性がある
//---------------------------------------------------------------------------
class tMemoryStreamBlock : public tDestructee
{
	tCriticalSection CS;	//!< このメモリブロックへのアクセスを保護するクリティカルセクション
	void * Block;			//!< メモリブロック
	risse_size Size;		//!< メモリブロックのデータが入っている部分のサイズ
	risse_size AllocSize;	//!< メモリブロックのアロケートしているサイズ( Size <= AllocSize )

public:
	//! @brief		コンストラクタ
	tMemoryStreamBlock();
protected:
	//! @brief		デストラクタ
	~tMemoryStreamBlock();
public:

	//! @brief		メモリブロックのサイズを変更する
	//! @param		size 新しいサイズ
	//! @note		サイズが拡張される場合、メモリブロックの内容は保たれるが、
	//!				サイズが拡張された部分の内容は不定となる。縮小される場合、
	//!				内容は最後が切りつめられる。
	void ChangeSize(risse_size);

	//! @brief		メモリブロックのサイズをSizeぴったりのサイズに変更する
	void Fit();

	void * GetBlock() { return Block; } //!< ブロックを得る

	tCriticalSection & GetCS( ) { return CS; } //!< クリティカルセクションオブジェクトを得る
	risse_size GetSize() const { return Size; } //!< ブロックのサイズを得る
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		メモリストリーム
//---------------------------------------------------------------------------
/*
	this class provides a tBinaryStream based access method for a memory block.
*/
class tMemoryStreamInstance : public tStreamInstance
{
protected:
	tMemoryStreamBlock * Block; //!< メモリブロック
	risse_size CurrentPos;		//!< 現在のポインタ
	risse_uint32 Flags;			//!< アクセスフラグ

public:
	//! @brief		コンストラクタ
	//! @param		flags アクセスフラグ
	tMemoryStreamInstance();

	//! @brief		デストラクタ(おそらく呼ばれない)
	~tMemoryStreamInstance() {;}

	//! @brief		メモリブロックを設定する
	//! @param		block メモリブロック
	void SetMemoryBlock(tMemoryStreamBlock * block) { Block = block; }

	//! @brief		アクセスフラグを設定する
	//! @param		flags		アクセスフラグ
	void SetAccessFlags(risse_uint32 flags) { Flags = flags; }

	//! @brief		ポインタを最後に移動する
	void SeekEnd();

public: // risse 用メソッドとか
	void construct() {;}
	void initialize(risse_uint32 flags, const tNativeCallInfo &info);

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
	risse_size put(const tOctet & buf);

	//! @brief		ストリームを現在位置で切りつめる
	void truncate();

	//! @brief		サイズを得る
	//! @return		このストリームのサイズ
	risse_uint64 get_size();

	//! @brief		ストリームをフラッシュする(書き込みバッファをフラッシュする)
	void flush();

	// non-tBinaryStream based methods
	void * GetInternalBuffer()  const { return Block->GetBlock(); }
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		"MemoryStream" クラス
//---------------------------------------------------------------------------
class tMemoryStreamClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tMemoryStreamClass(tScriptEngine * engine);

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

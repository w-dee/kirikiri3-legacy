//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 圧縮ブロックを展開された状態でメモリ上に保持するクラス
//---------------------------------------------------------------------------


#ifndef DECOMPRESSEDHOLDERH
#define DECOMPRESSEDHOLDERH

namespace Risa {
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * 圧縮ストリームを展開された状態で保持するオブジェクト
 */
class tDecompressedHolder
{
private:
	risse_size Size; //!< このデータが保持する(無圧縮状態の)サイズ
	risse_uint8 * Data; //!< このデータが保持する(無圧縮状態の)データブロック

	tDecompressedHolder::tDecompressedHolder(const tDecompressedHolder & ref) {;} // コピー禁止

public:
	enum tMethod 
	{
		dhmZLib // zlib 圧縮
	};

	/**
	 * コンストラクタ(メモリ上のデータを展開する場合)
	 * @param method		圧縮メソッド
	 * @param indata		入力データ
	 * @param insize		入力データのサイズ
	 * @param uncomp_size	入力データを展開した際のサイズ
	 */
	tDecompressedHolder(tMethod method, const risse_uint8 * indata, risse_size insize, 
		risse_size uncomp_size);

	/**
	 * コンストラクタ(ストリーム上のデータを展開する場合)
	 * @param method		圧縮メソッド
	 * @param indata		入力ストリーム
	 * @param insize		入力データのサイズ
	 * @param uncomp_size	入力データを展開した際のサイズ
	 */
	tDecompressedHolder(tMethod method, tBinaryStream * instream, risse_size insize, 
		risse_size uncomp_size);

	/**
	 * デストラクタ
	 */
	~tDecompressedHolder();

	risse_size GetSize() const { return Size; } //!< サイズを返す
	risse_uint8 * GetData() const { return Data; } //!< データブロックを返す

private:
	/**
	 * メモリ上のデータを展開する
	 * @param method		圧縮メソッド
	 * @param indata		入力データ
	 * @param insize		入力データのサイズ
	 * @param uncomp_size	入力データを展開した際のサイズ
	 */
	void Decompress(tMethod method, const risse_uint8 * indata, risse_size insize, 
		risse_size uncomp_size);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa



#endif

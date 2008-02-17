//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief イメージコーデッククラス
//---------------------------------------------------------------------------
#ifndef ImageCodecH
#define ImageCodecH

#include "visual/image/Image.h"
#include "risse/include/risseDictionaryClass.h"
#include "risse/include/risseStreamClass.h"
#include "base/utils/ProgressCallback.h"

namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		イメージデコーダクラスの基底クラス
//---------------------------------------------------------------------------
class tImageDecoder : public tCollectee
{
public:
	bool Decoded; //!< デコードが終了したかどうか
	tPixel::tFormat DesiredPixelFormat; //!< 要求されたフォーマット
	tPixel::tFormat DecoderPixelFormat; //!< デコーダから返されるフォーマット
	tImageInstance * Image; //!< 格納先のイメージ
	tImageBuffer * ImageBuffer; //!< 格納先のイメージバッファ
	const tImageBuffer::tBufferPointer * BufferPointer; //!< 格納先のイメージバッファのバッファポインター
	const tImageBuffer::tDescriptor * Descriptor; //!< 格納先イメージバッファの記述子
	void * LastConvertBuffer; //!< 最後に作成した変換用バッファ
	risse_size LastConvertBufferSize; //!< 最後に作成した変換用バッファのサイズ
	risse_offset LastCnvertBufferPitch; //!< 最後に作成した変換用バッファのピッチ
	risse_size LastLineY; //!< 最後に StartLines() で要求された y
	risse_size LastLineH; //!< 最後に StartLines() で要求された h

public:
	tImageDecoder(); //!< コンストラクタ

public:
	//! @brief		stream から image に対してデコードを行う
	//! @param		stream		入力ストリーム
	//! @param		image		イメージ
	//! @param		pixel_format	要求するピクセル形式
	//! @param		callback	進捗コールバック(NULL=イラナイ)
	//! @param		dict		メタデータ用の辞書配列(NULL=メタデータ要らない場合)
	//! @note		例外が発生した場合は stream の位置は元に戻される。
	//!				image にすでにイメージバッファが割り当たっていた場合は
	//!				そのサイズが画像サイズと異なる場合、またはピクセル形式が
	//!				pixel_format と異なる場合は例外が発生する。image にイメージバッファが
	//!				割り当てられていない場合は、サイズにしたがったメモリ上のバッファが
	//!				割り当てられる。
	//!				いったんデコードを行ったらデコーダインスタンスの再利用は行わずに破棄すること。
	//! @note		dict には、もしデコーダに指定したいオプションがある場合はこの中にいれて渡す。
	//!				(その場合は基本的にはキー名の先頭には
	//!				'_' (アンダースコア) をつけること:例 '_subtype' )
	//!				デコーダはこの dict の内容をクリアして新しくメタデータを入れ直さなければならない。
	void Decode(tStreamInstance * stream, tImageInstance * image,
					tPixel::tFormat pixel_format, tProgressCallback * callback,
					tDictionaryInstance * dict);

protected:
	//! @brief		デコードを行う(サブクラスで実装すること)
	//! @param		stream		入力ストリーム
	//! @param		pixel_format	要求するピクセル形式
	//! @param		callback	進捗コールバック(NULL=イラナイ)
	//! @param		dict		メタデータ用の辞書配列(NULL=メタデータ要らない場合)
	//! @note		dict には、もしデコーダに指定したいオプションがある場合はこの中にいれて渡す。
	//!				(その場合は基本的にはキー名の先頭には
	//!				'_' (アンダースコア) をつけること:例 '_subtype' )
	//!				デコーダはこの dict の内容をクリアして新しくメタデータを入れ直さなければならない。
	virtual void Process(tStreamInstance * stream,
					tPixel::tFormat pixel_format, tProgressCallback * callback,
					tDictionaryInstance * dict) = 0;

	//! @brief		サイズなどを設定する(サブクラスから呼ばれる)
	//! @param		w			横幅
	//! @param		h			縦幅
	//! @param		pixel_format	ピクセル形式
	//! @note		image にすでにイメージバッファが割り当たっていた場合、
	//!				サイズが異なる場合は例外が発生する。pixel_format は
	//!				Process() に渡されたピクセルタイプと異なる物を指定してもよいが
	//!				異なる物が渡された場合はこのクラスでの変換がおこなわれる。
	void SetDimensions(risse_size w, risse_size h, tPixel::tFormat pixel_format);

	//! @brief		nライン分のバッファを取得する(サブクラスから呼ばれる)
	//! @param		y		縦座標値(ピクセル単位)
	//! @param		h		縦幅(ピクセル単位)
	//! @param		pitch	ピッチ(次のラインへのバイト数) を格納する変数領域
	//! @return		指定位置から指定縦幅分のバッファ
	//! @note		指定y位置からhライン分のバッファを取得する。このバッファに SetDimensions() で
	//!				指定したピクセル形式でデータを書き込んだら、DoneLines() を呼ぶこと。
	//! @note		できればあまり大きな単位でStartLines() -> DoneLines()をおこなわず、
	//!				大きくとも16ピクセル縦幅などの大きさで StartLines() -> DoneLines() を繰り返した方が効率的。
	void * StartLines(risse_size y, risse_size h, risse_offset & pitch);

	//! @brief		バッファにデータを書き込んだ事を通知する(サブクラスから呼ばれる)
	//! @note		StartLines() で取得したバッファへの書き込みが完了したことを通知するためにサブクラスから呼ばれる
	void DoneLines();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		イメージエンコーダクラスの基底クラス
//---------------------------------------------------------------------------
class tImageEncoder : public tCollectee
{
	bool Encoded; //!< エンコードが終了したかどうか
	tImageBuffer * ImageBuffer; //!< エンコードしたいイメージのイメージバッファ
	const tImageBuffer::tBufferPointer * BufferPointer; //!< エンコードしたいイメージバッファのバッファポインター
	const tImageBuffer::tDescriptor * Descriptor; //!< エンコードしたいイメージバッファの記述子
	void * LastConvertBuffer; //!< 最後に作成した変換用バッファ
	risse_size LastConvertBufferSize; //!< 最後に作成した変換用バッファのサイズ

public:
	tImageEncoder(); //!< コンストラクタ

public:
	//! @brief		image から stream に対してエンコードを行う
	//! @param		stream		出力ストリーム
	//! @param		image		イメージ
	//! @param		callback	進捗コールバック(NULL=イラナイ)
	//! @param		dict		メタデータ用の辞書配列(NULL=メタデータ要らない場合)
	//! @note		例外が発生した場合の stream の状態は不定。
	//!				もしコーデックごとにオプションが必要な場合はdictの中に
	//!				パラメータを指定して渡すこと(その場合は基本的にはキー名の先頭には
	//!				'_' (アンダースコア) をつけること:例 '_subtype' )
	//!				dict の内容はエンコーダ内では変更してはならない。
	void Encode(tStreamInstance * stream, tImageInstance * image,
					tProgressCallback * callback,
					tDictionaryInstance * dict);

protected:
	//! @brief		エンコードを行う(サブクラスで実装すること)
	//! @param		stream		入力ストリーム
	//! @param		pixel_format	要求するピクセル形式
	//! @param		callback	進捗コールバック(NULL=イラナイ)
	//! @param		dict		メタデータ用の辞書配列(NULL=メタデータ要らない場合)
	virtual void Process(tStreamInstance * stream,
					tProgressCallback * callback,
					tDictionaryInstance * dict) = 0;

	//! @brief		サイズなどを取得する(サブクラスから呼ばれる)
	//! @param		w			横幅を格納する変数へのポインタ
	//! @param		h			縦幅を格納する変数へのポインタ
	//! @param		pixel_format	(イメージバッファの元々の)ピクセル形式を格納する変数へのポインタ
	void GetDimensions(risse_size *w, risse_size *h, tPixel::tFormat *pixel_format = NULL);

	//! @brief		nライン分のバッファを取得する(サブクラスから呼ばれる)
	//! @param		buf		格納先バッファ(NULLの場合はこのメソッド内で割り当てられる)
	//! @param		y		縦座標値(ピクセル単位)
	//! @param		h		縦幅(ピクセル単位)
	//! @param		pitch	buf の ピッチ(次のラインへのバイト数)
	//!						bufにNULLを渡した場合は戻りのバッファに対応するpitchが書き込まれる
	//! @param		pixel_format	取得したいピクセル形式
	//! @return		バッファ
	void * GetLines(void * buf, risse_size y, risse_size h, risse_offset & pitch,
		tPixel::tFormat pixel_format);

};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief	 	イメージコーデックファクトリ
//! @note		Risa は 拡張子で画像形式を判断する。ファクトリの登録は
//!				tImageCodecFactoryManager を通して行うこと。
//---------------------------------------------------------------------------
class tImageCodecFactory : public tCollectee
{
public:
	virtual tImageDecoder * CreateDecoder() = 0; //!< デコーダを作成する
	virtual tImageEncoder * CreateEncoder() = 0; //!< エンコーダを作成する
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		イメージコーデックファクトリマネージャ
//---------------------------------------------------------------------------
class tImageCodecFactoryManager : public singleton_base<tImageCodecFactoryManager>
{
	typedef gc_map<tString, tImageCodecFactory *>  tMap; //!< 拡張子→ファクトリのマップの型のtypedef
	tMap Map; //!< 拡張子→ファクトリのマップ

public:
	//! @brief	 	コンストラクタ
	tImageCodecFactoryManager();

	//! @brief	 	デストラクタ
	~tImageCodecFactoryManager();

	//! @brief	 	ファクトリを登録する
	//! @param		extension	拡張子 (小文字を使うこと;ドットも含む)
	//! @param		factory		ファクトリ
	void Register(const tString & extension, tImageCodecFactory * factory);

	//! @brief	 	ファクトリの登録を解除する
	//! @param		extension	拡張子 (小文字を使うこと;ドットも含む)
	void Unregister(const tString & extension);

	//! @brief	 	デコーダを作成する
	//! @param		filename ファイル名
	//! @return		作成されたデコーダ
	tImageDecoder * CreateDecoder(const tString & filename);

	//! @brief	 	エンコーダを作成する
	//! @param		filename ファイル名
	//! @return		作成されたエンコーダ
	tImageEncoder * CreateEncoder(const tString & filename);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risa


#endif

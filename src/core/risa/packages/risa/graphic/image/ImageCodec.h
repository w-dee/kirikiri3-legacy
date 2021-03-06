//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief イメージコーデッククラス
//---------------------------------------------------------------------------
#ifndef ImageCodecH
#define ImageCodecH

#include "risa/packages/risa/graphic/image/Image.h"
#include "risseDictionaryClass.h"
#include "builtin/stream/risseStreamClass.h"
#include "risa/common/ProgressCallback.h"

namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * イメージデコーダクラスの基底クラス
 */
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
	risse_offset LastConvertBufferPitch; //!< 最後に作成した変換用バッファのピッチ
	risse_size LastLineY; //!< 最後に StartLines() で要求された y
	risse_size LastLineH; //!< 最後に StartLines() で要求された h

public:
	tImageDecoder(); //!< コンストラクタ

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tImageDecoder() {}

public:
	/**
	 * stream から image に対してデコードを行う
	 * @param stream		入力ストリーム
	 * @param image			イメージ
	 * @param pixel_format	要求するピクセル形式
	 * @param callback		進捗コールバック(NULL=イラナイ)
	 * @param dict			メタデータ用の辞書配列(NULL=メタデータ要らない場合)
	 * @note	例外が発生した場合は stream の位置は元に戻される。
	 *			image にすでにイメージバッファが割り当たっていた場合は
	 *			そのサイズが画像サイズと異なる場合、またはピクセル形式が
	 *			pixel_format と異なる場合は例外が発生する。image にイメージバッファが
	 *			割り当てられていない場合は、サイズにしたがったメモリ上のバッファが
	 *			割り当てられる。
	 *			いったんデコードを行ったらデコーダインスタンスの再利用は行わずに破棄すること。
	 * @note	dict には、もしデコーダに指定したいオプションがある場合はこの中にいれて渡す。
	 *			(その場合は基本的にはキー名の先頭には
	 *			'_' (アンダースコア) をつけること:例 '_subtype' )
	 *			デコーダはこの dict の内容をクリアして新しくメタデータを入れ直さなければならない。
	 */
	void Decode(tStreamInstance * stream, tImageInstance * image,
					tPixel::tFormat pixel_format, tProgressCallback * callback,
					tDictionaryInstance * dict);

protected:
	/**
	 * デコードを行う(サブクラスで実装すること)
	 * @param stream		入力ストリーム
	 * @param pixel_format	要求するピクセル形式
	 * @param callback		進捗コールバック(NULL=イラナイ)
	 * @param dict			メタデータ用の辞書配列(NULL=メタデータ要らない場合)
	 * @note	dict には、もしデコーダに指定したいオプションがある場合はこの中にいれて渡す。
	 *			(その場合は基本的にはキー名の先頭には
	 *			'_' (アンダースコア) をつけること:例 '_subtype' )
	 *			デコーダはこの dict の内容をクリアして新しくメタデータを入れ直さなければならない。
	 */
	virtual void Process(tStreamInstance * stream,
					tPixel::tFormat pixel_format, tProgressCallback * callback,
					tDictionaryInstance * dict) = 0;

	/**
	 * サイズなどを設定する(サブクラスから呼ばれる)
	 * @param w				横幅
	 * @param h				縦幅
	 * @param pixel_format	ピクセル形式
	 * @note	image にすでにイメージバッファが割り当たっていた場合、
	 *			サイズが異なる場合は例外が発生する。pixel_format は
	 *			Process() に渡されたピクセルタイプと異なる物を指定してもよいが
	 *			異なる物が渡された場合はこのクラスでの変換がおこなわれる。
	 */
	void SetDimensions(risse_size w, risse_size h, tPixel::tFormat pixel_format);

	/**
	 * nライン分のバッファを取得する(サブクラスから呼ばれる)
	 * @param y		縦座標値(ピクセル単位)
	 * @param h		縦幅(ピクセル単位)
	 * @param pitch	ピッチ(次のラインへのバイト数) を格納する変数へのポインタ (NULL=要らない場合)
	 * @return	指定位置から指定縦幅分のバッファ
	 * @note	指定y位置からhライン分のバッファを取得する。このバッファに SetDimensions() で
	 *			指定したピクセル形式でデータを書き込んだら、DoneLines() を呼ぶこと。
	 * @note	できればあまり大きな単位でStartLines() -> DoneLines()をおこなわず、
	 *			大きくとも16ピクセル縦幅などの大きさで StartLines() -> DoneLines() を繰り返した方が効率的。
	 */
	void * StartLines(risse_size y, risse_size h, risse_offset * pitch);

	/**
	 * バッファにデータを書き込んだ事を通知する(サブクラスから呼ばれる)
	 * @note	StartLines() で取得したバッファへの書き込みが完了したことを通知するためにサブクラスから呼ばれる
	 */
	void DoneLines();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * イメージエンコーダクラスの基底クラス
 */
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

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tImageEncoder() {}

public:
	/**
	 * image から stream に対してエンコードを行う
	 * @param stream	出力ストリーム
	 * @param image		イメージ
	 * @param callback	進捗コールバック(NULL=イラナイ)
	 * @param dict		メタデータ用の辞書配列(NULL=メタデータ要らない場合)
	 * @note	例外が発生した場合の stream の状態は不定。
	 *			もしコーデックごとにオプションが必要な場合はdictの中に
	 *			パラメータを指定して渡すこと(その場合は基本的にはキー名の先頭には
	 *			'_' (アンダースコア) をつけること:例 '_subtype' )
	 *			dict の内容はエンコーダ内では変更してはならない。
	 */
	void Encode(tStreamInstance * stream, tImageInstance * image,
					tProgressCallback * callback,
					tDictionaryInstance * dict);

protected:
	/**
	 * エンコードを行う(サブクラスで実装すること)
	 * @param stream		入力ストリーム
	 * @param pixel_format	要求するピクセル形式
	 * @param callback		進捗コールバック(NULL=イラナイ)
	 * @param dict			メタデータ用の辞書配列(NULL=メタデータ要らない場合)
	 */
	virtual void Process(tStreamInstance * stream,
					tProgressCallback * callback,
					tDictionaryInstance * dict) = 0;

	/**
	 * サイズなどを取得する(サブクラスから呼ばれる)
	 * @param w				横幅を格納する変数へのポインタ
	 * @param h				縦幅を格納する変数へのポインタ
	 * @param pixel_format	(イメージバッファの元々の)ピクセル形式を格納する変数へのポインタ
	 */
	void GetDimensions(risse_size *w, risse_size *h, tPixel::tFormat *pixel_format = NULL);

	/**
	 * nライン分のバッファを取得する(サブクラスから呼ばれる)
	 * @param buf			格納先バッファ(NULLの場合はこのメソッド内で割り当てられる)
	 * @param y				縦座標値(ピクセル単位)
	 * @param h				縦幅(ピクセル単位)
	 * @param pitch			buf の ピッチ(次のラインへのバイト数)を格納する変数へのポインタ; 要らない場合はNULL
	 *						bufにNULLを渡した場合は戻りのバッファに対応するpitchが書き込まれる;
	 *						もしhに2以上を渡し、bufに格納先バッファを指定した場合は、pitch の指す先に
	 *						画像のピッチを渡さなければならない。
	 * @param pixel_format	取得したいピクセル形式
	 * @return	バッファ
	 * @note	buf に NULL を指定するとバッファがこのメソッド内で割り当てられるが、
	 *			そのバッファの内容が有効なのは次のこのメソッドの呼び出しまでなので注意
	 *			(つまりこのメソッドの呼び出しごとにバッファの内容は上書きされる)
	 */
	void * GetLines(void * buf, risse_size y, risse_size h, risse_offset * pitch,
		tPixel::tFormat pixel_format);

};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/**
 * イメージデコーダファクトリ
 * @note	Risa は 拡張子で画像形式を判断する。ファクトリの登録は
 *			tImageCodecFactoryManager を通して行うこと。
 */
class tImageDecoderFactory : public tCollectee
{
public:
	virtual ~tImageDecoderFactory() {;} //!< デストラクタ(おそらく呼ばれない)

	virtual tImageDecoder * CreateDecoder() = 0; //!< デコーダを作成する
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * イメージエンコーダファクトリ
 * @note	Risa は 拡張子で画像形式を判断する。ファクトリの登録は
 *			tImageCodecFactoryManager を通して行うこと。
 */
class tImageEncoderFactory : public tCollectee
{
public:
	virtual ~tImageEncoderFactory() {;} //!< デストラクタ(おそらく呼ばれない)

	virtual tImageEncoder * CreateEncoder() = 0; //!< エンコーダを作成する
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * イメージコーデックファクトリマネージャ
 */
class tImageCodecFactoryManager : public singleton_base<tImageCodecFactoryManager>
{
	typedef gc_map<tString, tImageDecoderFactory *>  tDecoderMap; //!< デコーダ:拡張子→ファクトリのマップの型のtypedef
	typedef gc_map<tString, tImageEncoderFactory *>  tEncoderMap; //!< エンコーダ:拡張子→ファクトリのマップの型のtypedef
	tDecoderMap DecoderMap; //!< デコーダ:拡張子→ファクトリのマップ
	tEncoderMap EncoderMap; //!< エンコーダ:拡張子→ファクトリのマップ

public:
	/**
	 * コンストラクタ
	 */
	tImageCodecFactoryManager();

	/**
	 * デストラクタ
	 */
	~tImageCodecFactoryManager();

	/**
	 * デコーダファクトリを登録する
	 * @param extension	拡張子 (小文字を使うこと;ドットも含む)
	 * @param factory	ファクトリ
	 */
	void Register(const tString & extension, tImageDecoderFactory * factory);

	/**
	 * エンコーダファクトリを登録する
	 * @param extension	拡張子 (小文字を使うこと;ドットも含む)
	 * @param factory	ファクトリ
	 */
	void Register(const tString & extension, tImageEncoderFactory * factory);

	/**
	 * デコーダを作成する
	 * @param filename	ファイル名
	 * @return	作成されたデコーダ
	 */
	tImageDecoder * CreateDecoder(const tString & filename);

	/**
	 * エンコーダを作成する
	 * @param filename	ファイル名
	 * @return	作成されたエンコーダ
	 */
	tImageEncoder * CreateEncoder(const tString & filename);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risa


#endif

//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Stream" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseStreamClassH
#define risseStreamClassH

#include "../../risseTypes.h"
#include "../../risseGC.h"
#include "../../risseString.h"
#include "../../risseStream.h"
#include "../../risseClass.h"
#include "../../risseNativeBinder.h"
#include "../risseBuiltinPackageInitializer.h"

namespace Risse
{

//---------------------------------------------------------------------------
/**
 * NativeBinder 用の Variant -> tStreamConstants::tOrigin 変換定義
 */
template <>
inline tStreamConstants::tOrigin FromVariant<tStreamConstants::tOrigin>(const tVariant & v)
{
	return (tStreamConstants::tOrigin)(int)(risse_int64)v;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/**
 * ストリームインスタンス ("Stream" クラスのインスタンス) の基本クラス
 * @note	このクラスではシークが不可能な、本当の意味での「ストリーム」は扱わない
 */
class tStreamInstance : public tObjectBase, public tStreamConstants
{
private:
	tString Name; //!< ストリームの名前

public:
	/**
	 * コンストラクタ
	 */
	tStreamInstance() {;}

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tStreamInstance() {;}

public: // risse 用メソッドとか

	void construct() {;}
	void initialize(const tNativeCallInfo &info) {info.InitializeSuperClass();}

	/**
	 * ストリームを閉じる
	 * @note	基本的にはこれでストリームを閉じること。
	 *			このメソッドでストリームを閉じなかった場合の動作は
	 *			「未定義」である
	 */
	void dispose() {;}

	/**
	 * ストリームの名前を設定する
	 * @param name	名前
	 * @note	このクラス内ではこの名前は例外生成時に使用するだけであるが、
	 *			例外をユーザフレンドリーにするために、なるべく名前は設定するべき
	 */
	void set_name(const tString & name) { Name = name; }

	/**
	 * ストリームの名前を取得する
	 * @return	ストリームの名前を取得する
	 */
	const tString & get_name() { return Name; }


/*
	サブクラスでは以下のメソッドを実装すること ( * = optional )
	seek *
	tell *
	get *
	put *
	truncate *
	flush *
	サブクラスでは以下のプロパティを実装すること ( * = optional )
	size *
*/
	/**
	 * 指定位置にシークする
	 * @param offset	基準位置からのオフセット (正の数 = ファイルの後ろの方)
	 * @param whence	基準位置
	 * @return	このメソッドは成功すれば真、失敗すれば偽を返す
	 * @note	このメソッドは下位クラスで実装しなければならない。
	 *			エラーが発生した場合は、シーク位置を変えず、現在位置を保つべき
	 */
	bool seek(risse_int64 offset, tOrigin whence) { return false; }

	/**
	 * 現在位置を取得する
	 * @return	現在位置(先頭からのオフセット)
	 */
	risse_uint64 tell() { return 0; }

	/**
	 * ストリームから読み込む
	 * @param buf	読み込んだデータを書き込む先
	 * @return	実際に読み込まれたサイズ
	 */
	risse_size get(const tOctet & buf) { return 0; }

	/**
	 * ストリームに書き込む
	 * @param buf	書き込むデータ
	 * @return	実際に書き込まれたサイズ
	 */
	risse_size put(const tOctet & buf) { return 0; }

	/**
	 * ストリームを現在位置で切りつめる
	 * @note	これを実装しない場合は例外が発生する
	 */
	void truncate();

	/**
	 * ストリームのサイズを得る
	 * @return	ストリームのサイズ
	 * @note	実装しなくても良いが、
	 *			実装した方が高いパフォーマンスを得られるようならば実装すべき
	 */
	risse_uint64 get_size();

	/**
	 * ストリームをフラッシュする(書き込みバッファをフラッシュする)
	 */
	void flush();

	/**
	 * 現在位置を得る
	 * @return	現在位置
	 */
	risse_uint64 get_position();

	/**
	 * 現在位置を設定する
	 * @param pos	現在位置
	 * @note	シークに失敗した場合は例外が発生する
	 */
	void set_position(risse_uint64 pos);

	/**
	 * ストリームから読み込む
	 * @param args	引数配列 (optionalな size	読み込むサイズを含む)
	 * @return	読み込まれた内容を含むオクテット列
	 */
	tOctet read(const tMethodArgument &args);

	/**
	 * ストリームに書き込む
	 * @param buf	書き込むデータ
	 */
	risse_size write(const tOctet & buf);

	/**
	 * ストリームから読み込む
	 * @param size	読み込むサイズ
	 * @return	読み込まれた内容を含むオクテット列
	 * @note	読み込みに失敗した場合は例外が発生する
	 */
	tOctet readBuffer(risse_size size);

	/**
	 * ストリームに書き込む
	 * @param buf	書き込むデータ
	 * @note	書き込みに失敗した場合は例外が発生する
	 */
	void writeBuffer(const tOctet & buf);

	/**
	 * 文字列を出力する
	 * @param str	文字列
	 * @note	文字列は現バージョンではUTF-8として出力される
	 */
	void print(const tString & str);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * "Stream" クラス
 */
class tStreamClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	/**
	 * コンストラクタ
	 * @param engine	スクリプトエンジンインスタンス
	 */
	tStreamClass(tScriptEngine * engine);

	/**
	 * 各メンバをインスタンスに追加する
	 */
	void RegisterMembers();

	/**
	 * newの際の新しいオブジェクトを作成して返す
	 */
	static tVariant ovulate();

public:
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * "stream" パッケージイニシャライザ
 */
class tStreamPackageInitializer : public tBuiltinPackageInitializer
{
public:
	tStreamClass * StreamClass;

	/**
	 * コンストラクタ
	 */
	tStreamPackageInitializer();

	/**
	 * パッケージを初期化する
	 * @param engine	スクリプトエンジンインスタンス
	 * @param name		パッケージ名
	 * @param global	パッケージグローバル
	 */
	virtual void Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
#endif

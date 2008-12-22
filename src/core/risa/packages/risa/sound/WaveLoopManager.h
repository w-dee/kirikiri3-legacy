//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Waveループマネージャ
//---------------------------------------------------------------------------

#ifndef WaveLoopManagerH
#define WaveLoopManagerH
//---------------------------------------------------------------------------

#include "risseTypes.h"
#include "risa/common/RisaThread.h"
#include "risa/packages/risa/sound/WaveFilter.h"
#include "risa/packages/risa/sound/WaveDecoder.h"
#include "risa/common/RisaGC.h"
#include <vector>
#include <string>

namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#ifdef RISA_IN_LOOP_TUNER
	typedef AnsiString tLabelStringType;
	typedef char   tLabelCharType;
#else
	typedef tString tLabelStringType;
	typedef risse_char tLabelCharType;
#endif
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * リンクを表す構造体
 */
struct tWaveLoopLink : public tAtomicCollectee
{
	/**
	 * リンクの条件を表す列挙型
	 */
	enum tLinkCondition
	{
		llcNone				/*!< 無条件リンク */,
		llcEqual			/*!< 値が一致したとき */,
		llcNotEqual			/*!< 値が一致しないとき */,
		llcGreater			/*!< 値がフラグより大きいとき */,
		llcGreaterOrEqual	/*!< 値がフラグ以上のとき */,
		llcLesser			/*!< 値がフラグより小さいとき */,
		llcLesserOrEqual	/*!< 値がフラグ以下のとき */
	};

	risse_int64 From;		//!< 'From' in sample position
	risse_int64 To;		//!< 'To' in sample position
	bool Smooth;		//!< Smooth transition (uses short 50ms crossfade)
	tLinkCondition Condition;	//!< Condition
	risse_int RefValue;	//!< リンク条件の「値」
	risse_int CondVar;	//!< Condition variable index
#ifdef RISA_IN_LOOP_TUNER
	// these are only used by the loop tuner
	risse_int FromTier;	//!< display tier of vertical 'from' line
	risse_int LinkTier;	//!< display tier of horizontal link
	risse_int ToTier;		//!< display tier of vertical 'to' allow line
	risse_int Index;		//!< link index

	struct tSortByDistanceFuncObj : public tCollectee
	{
		bool operator()(
			const tWaveLoopLink &lhs,
			const tWaveLoopLink &rhs) const
		{
			risse_int64 lhs_dist = lhs.From - lhs.To;
			if(lhs_dist < 0) lhs_dist = -lhs_dist;
			risse_int64 rhs_dist = rhs.From - rhs.To;
			if(rhs_dist < 0) rhs_dist = -rhs_dist;
			return lhs_dist < rhs_dist;
		}
	};

	struct tSortByIndexFuncObj : public tCollectee
	{
		bool operator()(
			const tWaveLoopLink &lhs,
			const tWaveLoopLink &rhs) const
		{
			return lhs.Index < rhs.Index;
		}
	};
#endif

	/**
	 * コンストラクタ
	 */
	tWaveLoopLink()
	{
		From = To = 0;
		Smooth = false;
		Condition = llcNone;
		RefValue = CondVar = 0;

#ifdef RISA_IN_LOOP_TUNER
		FromTier = LinkTier = ToTier = 0;
		Index = 0;
#endif
	}
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
/**
 * リンクをソートするための比較関数
 * @param lhs	演算子の左側
 * @param rhs	演算子の右側
 * @return	lhs < rhs の場合真、それ以外の場合は偽
 */
bool inline operator < (const tWaveLoopLink & lhs, const tWaveLoopLink & rhs)
{
	if(lhs.From < rhs.From) return true;
	if(lhs.From == rhs.From)
	{
		// give priority to conditional link
		if(lhs.Condition != rhs.Condition)
			return lhs.Condition > rhs.Condition;
		// give priority to conditional expression
		return lhs.CondVar > rhs.CondVar;
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * ラベルを表す構造体
 */
struct tWaveLabel : public tWaveEvent
{
#ifdef RISA_IN_LOOP_TUNER
	// these are only used by the loop tuner
	risse_int NameWidth; //!< display name width
	risse_int Index; //!< index
#endif

	struct tSortByPositionFuncObj
	{
		bool operator()(
			const tWaveLabel &lhs,
			const tWaveLabel &rhs) const
		{
			return lhs.Position < rhs.Position;
		}
	};

#ifdef RISA_IN_LOOP_TUNER
	struct tSortByIndexFuncObj
	{
		bool operator()(
			const tWaveLabel &lhs,
			const tWaveLabel &rhs) const
		{
			return lhs.Index < rhs.Index;
		}
	};
#endif

	/**
	 * コンストラクタ
	 */
	tWaveLabel()
	{
#ifdef RISA_IN_LOOP_TUNER
		NameWidth = 0;
		Index = 0;
#endif
	}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * ラベルをソートするための比較関数
 * @param lhs	演算子の左側
 * @param rhs	演算子の右側
 * @return	lhs < rhs の場合真、それ以外の場合は偽
 */
bool inline operator < (const tWaveLabel & lhs, const tWaveLabel & rhs)
{
	return lhs.Position < rhs.Position;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
/**
 * Wave ループマネージャ
 */
class tWaveDecoder;
class tWaveFileInfo;
class tWaveLoopManager : public tWaveFilter
{
public:
	// 定数
	static const int WaveLoopLinkGiveUpCount = 10;
		/*!< This is for preventing infinite loop caused by recursive links.
		 If the decoding point does not change when the loop manager follows the
		 links, after 'WaveLoopLinkGiveUpCount' times the loop manager
		 will give up the decoding.*/

	static const int MaxFlags = 16; //!< フラグの最大数
	static const int MaxFlagValue = 9999; //!< フラグがとりうる最大値
	static const int SmoothTime = 50; //!< スムースリンクのクロスフェードの時間
	static const int SmoothTimeHalf = SmoothTime / 2; //!< SmoothTime の半分
	static const int MaxIDLen = 16; //!< 識別子の最大長

private:
	tCriticalSection * FlagsCS; //!< CS to protect flags/links/labels
	int Flags[MaxFlags]; //!< フラグ
	bool FlagsModifiedByLabelExpression; //!< true if the flags are modified by EvalLabelExpression
	gc_vector<tWaveLoopLink> Links; //!< リンクの配列
	gc_vector<tWaveLabel> Labels; //!< ラベルの配列
	tCriticalSection * DataCS; // CS to protect other members
	tWaveFileInfo * FileInfo; //!< デコーダのファイル情報
	tWaveDecoder * Decoder; //!< デコーダ
	bool FirstRendered; //!< 最初のサンプルをレンダリングしたかどうか

	risse_int ShortCrossFadeHalfSamples;
		//!< SmoothTimeHalf in sample unit

	bool Looping;
		//!< ループ再生しているときは真 (ただしこれはループ情報を読み込んでいない場合の設定で、
		//   ループ情報を読み込んでいる場合はこの設定に従わない

	risse_int64 Position; //!< decoding position

	risse_uint8 *CrossFadeSamples; //!< sample buffer for crossfading
	risse_int CrossFadeLen; //!< 現在クロスフェード中の場合、そのクロスフェードの長さ
	risse_int CrossFadePosition; //!< 現在クロスフェード中の場合、そのデコード位置

	bool IsLinksSorted; //!< false if links are not yet sorted
	bool IsLabelsSorted; //!< false if labels are not yet sorted

	bool IgnoreLinks; //!< decode the samples with ignoring links

public:
	/**
	 * コンストラクタ
	 */
	tWaveLoopManager(tWaveDecoder * decoder);

private:
	/**
	 * デコーダを設定する
	 * @param decoder	デコーダ
	 */
	void SetDecoder(tWaveDecoder * decoder);

public:
	/**
	 * 指定インデックスのフラグを得る
	 * @param index	フラグのインデックス
	 * @return	そのインデックスに対応するフラグの値
	 */
	int GetFlag(risse_int index);

	/**
	 * フラグをすべて一気にコピーする
	 * @param dest	コピー先
	 * @note	この関数を呼ぶと FlagsModifiedByLabelExpression は偽にリセットされる
	 */
	void CopyFlags(risse_int *dest);

	/**
	 * フラグがラベル式によって変更されたかどうかが返される
	 * @return	フラグがラベル式によって変更されたかどうか
	 * @note	このフラグは CopyFlags により偽にリセットされる
	 */
	bool GetFlagsModifiedByLabelExpression();

	/**
	 * 指定インデックスのフラグを設定する
	 * @param index	フラグのインデックス
	 * @param f		フラグの値
	 */
	void SetFlag(risse_int index, risse_int f);

	/**
	 * フラグをすべて 0 にする
	 */
	void ClearFlags();

	/**
	 * リンクやラベルをすべてクリアする
	 */
	void ClearLinksAndLabels();

	/**
	 * リンクの配列を得る
	 * @return	リンクの配列への参照
	 */
	const gc_vector<tWaveLoopLink> & GetLinks() const;

	/**
	 * ラベルの配列を得る
	 * @return	ラベルの配列への参照
	 */
	const gc_vector<tWaveLabel> & GetLabels() const;

	/**
	 * リンクの配列を設定する
	 * @param links	設定したい配列
	 */
	void SetLinks(const gc_vector<tWaveLoopLink> & links);

	/**
	 * ラベルの配列を設定する
	 * @param links	設定したい配列
	 */
	void SetLabels(const gc_vector<tWaveLabel> & labels);

	/**
	 * リンクを無視しながら再生しているかどうかを返す
	 * @return	リンクを無視しながら再生しているかどうか
	 */
	bool GetIgnoreLinks() const;

	/**
	 * リンクを無視しながら再生しているかどうかを設定する
	 * @param b	リンクを無視しながら再生するかどうか
	 */
	void SetIgnoreLinks(bool b);

	/**
	 * 現在のデコード位置を得る
	 * @return	現在のデコード位置
	 */
	risse_int64 GetPosition() const;

	/**
	 * 現在のデコード位置を設定する
	 * @param pos	現在のデコード位置
	 */
	void SetPosition(risse_int64 pos);

	bool GetLooping() const { return Looping; } //!< ループ情報を読み込んでいないときにループを行うかどうかを得る
	void SetLooping(bool b) { Looping = b; } //!< ループ情報を読み込んでいないときにループを行うかどうかを設定する

	//------- tWaveFilter メソッド  ここから
	void Reset() {;} //!< @note tWaveLoopManager ではなにもしない
	void SetInput(tWaveFilter * input) {;}
		//!< @note tWaveLoopManager には入力するフィルタがないのでこのメソッドはなにもしない

	/**
	 * PCMフォーマットを提案する
	 * @param format	PCMフォーマット
	 */
	void SuggestFormat(const tWaveFormat & format);

	/**
	 * デコードを行う
	 * @param dest			デコード結果を格納するバッファ
	 * @param samples		デコードを行いたいサンプル数
	 * @param written		実際にデコード出来たサンプル数
	 * @param segmentqueue	再生セグメントキュー情報を書き込む先(内容はクリアされずに追加される)
	 * @return	まだデコードすべきデータが残っているかどうか
	 */
	bool Render(void *dest, risse_uint samples, risse_uint &written,
		tWaveSegmentQueue & segmentqueue);

	/**
	 * PCM形式を返す
	 * @return	PCM形式への参照
	 */
	const tWaveFormat & GetFormat();
	//------- tWaveFilter メソッド  ここまで

private:
	/**
	 * 指定位置以降で、指定位置にもっとも近いリンクの始点を探す
	 * @param current			検索開始位置
	 * @param link				見つかったリンクを書き込む先
	 * @param ignore_conditions	リンク条件を無視して検索を行うかどうか
	 * @return	リンクが見つかれば真、見つからなければ偽
	 */
	bool GetNearestLink(risse_int64 current,
		tWaveLoopLink & link, bool ignore_conditions);

	/**
	 * 指定位置以降で指定位置未満の中のイベント(ラベル)を取得する
	 * @param from		検索開始位置
	 * @param to		検索終了位置
	 * @param events	結果を格納する配列
	 */
	void GetEventAt(risse_int64 from, risse_int64 to,
		gc_deque<tWaveEvent> & labels);

	/**
	 * クロスフェードを行う
	 * @param dest			結果格納先
	 * @param src1			ソース1 (消えていく方)
	 * @param src2			ソース2 (入ってくる方)
	 * @param ratiostart	ブレンド率の開始値(%)
	 * @param ratioend		ブレンド率の終了値(%)
	 * @param samples		この関数の呼び出しで処理すべきサンプル数
	 */
	void DoCrossFade(void *dest, void *src1, void *src2, risse_int samples,
		risse_int ratiostart, risse_int ratioend);

	/**
	 * 内部にキャッシュされているクロスフェードの情報をクリアする
	 */
	void ClearCrossFadeInformation();

//--- flag manupulation by label expression
	enum tExpressionToken {
		etUnknown,
		etEOE			/*!< End of the expression */,
		etLBracket		/*!< '[' */,
		etRBracket		/*!< ']' */,
		etInteger		/*!< integer number */,
		etEqual			/*!< '=' */,
		etPlus			/*!< '+' */,
		etMinus			/*!< '-' */,
		etPlusEqual		/*!< '+=' */,
		etMinusEqual	/*!< '-=' */,
		etIncrement		/*!< '++' */,
		etDecrement		/*!< '--' */
	};
public:
	/**
	 * ラベル式を解析する
	 * @param label				ラベル式
	 * @param ope				演算子
	 * @param lv				左辺値
	 * @param rv				右辺値
	 * @param is_rv_indirect	右辺値が間接参照の場合は真、そうでない場合は偽が格納される
	 */
	static bool GetLabelExpression(const tLabelStringType &label,
		tExpressionToken * ope = NULL,
		risse_int *lv = NULL,
		risse_int *rv = NULL, bool *is_rv_indirect = NULL);
private:
	/**
	 * ラベル式を評価する
	 * @param label	ラベル
	 * @return	評価に成功すれば真、失敗すれば偽
	 */
	bool EvalLabelExpression(const tLabelStringType &label);

	/**
	 * ラベル式のトークンを切り出して返す
	 * @param p		切り出し開始位置
	 * @param value	トークンの値を格納する変数へのアドレス
	 * @return	トークンのタイプ
	 */
	static tExpressionToken GetExpressionToken(const tLabelCharType * &  p , risse_int * value);

	/**
	 * ラベル式から整数値を得る
	 * @param s	解析開始位置
	 * @param v	値を格納する変数
	 * @return	解析に成功すれば真
	 */
	static bool GetLabelCharInt(const tLabelCharType *s, risse_int &v);


//--- loop information input/output stuff
private:
	/**
	 * 文字列から整数値を得る
	 * @param s	解析開始位置
	 * @param v	値を格納する変数
	 * @return	解析に成功すれば真
	 */
	static bool GetInt(char *s, risse_int &v);

	/**
	 * 文字列から64bit整数値を得る
	 * @param s	解析開始位置
	 * @param v	値を格納する変数
	 * @return	解析に成功すれば真
	 */
	static bool GetInt64(char *s, risse_int64 &v);

	/**
	 * 文字列から真偽値を得る
	 * @param s	解析開始位置
	 * @param v	値を格納する変数
	 * @return	解析に成功すれば真
	 */
	static bool GetBool(char *s, bool &v);

	/**
	 * 文字列からリンク条件を得る
	 * @param s	解析開始位置
	 * @param v	値を格納する変数
	 * @return	解析に成功すれば真
	 */
	static bool GetCondition(char *s, tWaveLoopLink::tLinkCondition &v);

	/**
	 * 文字列リテラルを取得する
	 * @param s	解析開始位置
	 * @param v	値を格納する変数
	 * @return	解析に成功すれば真
	 */
	static bool GetString(char *s, tLabelStringType &v);

	/**
	 * name=value の形式になっている name と value を取得する
	 * @param p		解析開始位置
	 * @param name	name の部分の最初を表すポインタ
	 * @param value	value の部分の最初を表すポインタ
	 * @return	解析に成功すれば真
	 * @note	このメソッドは、name および value が終了する位置に \0 を書き込む。
	 *			つまり、p を破壊する。
	 */
	static bool GetEntityToken(char * & p, char **name, char **value);

	/**
	 * リンク情報を文字列から読み取る
	 * @param p		読み取り開始位置
	 * @param link	情報格納先クラス
	 * @return	読み取りに成功すれば真
	 */
	static bool ReadLinkInformation(char * & p, tWaveLoopLink &link);

	/**
	 * ラベル情報を文字列から読み取る
	 * @param p		読み取り開始位置
	 * @param link	情報格納先クラス
	 * @return	読み取りに成功すれば真
	 */
	static bool ReadLabelInformation(char * & p, tWaveLabel &label);
public:
	/**
	 * リンク情報やラベル情報を文字列から読み取る
	 * @param p	読み取り開始位置
	 * @return	読み取りに成功すれば真
	 */
	bool ReadInformation(char * p);

#ifdef RISA_IN_LOOP_TUNER
	// output facility (currently only available with VCL interface)
private:
	static void PutInt(AnsiString &s, risse_int v);
	static void PutInt64(AnsiString &s, risse_int64 v);
	static void PutBool(AnsiString &s, bool v);
	static void PutCondition(AnsiString &s, tWaveLoopLink::tLinkCondition v);
	static void PutString(AnsiString &s, tLabelStringType v);
	static void DoSpacing(AnsiString &l, int col);
public:
	void WriteInformation(AnsiString &s);
#endif


public:
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa

#endif



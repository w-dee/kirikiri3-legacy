//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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
#include <vector>
#include <string>


#ifdef TVP_IN_LOOP_TUNER
	#include "WaveReader.h"
#endif

//---------------------------------------------------------------------------
#ifdef TVP_IN_LOOP_TUNER
	typedef AnsiString tTVPLabelStringType;
	typedef char   tTVPLabelCharType;
#else
	typedef ttstr tTVPLabelStringType;
	typedef risse_char tTVPLabelCharType;
#endif
//---------------------------------------------------------------------------


#ifdef TVP_IN_LOOP_TUNER
	//---------------------------------------------------------------------------
	// tRisseCriticalSection ( taken from risseUtils.h )
	//---------------------------------------------------------------------------
	class tRisseCriticalSection
	{
		CRITICAL_SECTION CS;
	public:
		tRisseCriticalSection() { InitializeCriticalSection(&CS); }
		~tRisseCriticalSection() { DeleteCriticalSection(&CS); }

		void Enter() { EnterCriticalSection(&CS); }
		void Leave() { LeaveCriticalSection(&CS); }
	};
	//---------------------------------------------------------------------------
	// tRisseCriticalSectionHolder ( taken from risseUtils.h )
	//---------------------------------------------------------------------------
	class tRisseCriticalSectionHolder
	{
		tRisseCriticalSection *Section;
	public:
		tRisseCriticalSectionHolder(tRisseCriticalSection &cs)
		{
			Section = &cs;
			Section->Enter();
		}

		~tRisseCriticalSectionHolder()
		{
			Section->Leave();
		}
	};
#else
	#include "risseUtils.h"
#endif
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief リンクを表すクラス
//---------------------------------------------------------------------------
class tTVPWaveLoopLink
{
public:
	//! @brief リンクの条件を表す列挙型
	enum tLinkCondition
	{
		llcNone,			//!< 無条件リンク
		llcEqual,			//!< 値が一致したとき
		llcNotEqual,		//!< 値が一致しないとき
		llcGreater,			//!< 値がフラグより大きいとき
		llcGreaterOrEqual,	//!< 値がフラグ以上のとき
		llcLesser,			//!< 値がフラグより小さいとき
		llcLesserOrEqual	//!< 値がフラグ以下のとき
	};

private:
	risse_int64 From;		//!< 'From' in sample position
	risse_int64 To;		//!< 'To' in sample position
	bool Smooth;		//!< Smooth transition (uses short 50ms crossfade)
	tLinkCondition Condition;	//!< Condition
	risse_int RefValue;	//!< リンク条件の「値」
	risse_int CondVar;	//!< Condition variable index
#ifdef TVP_IN_LOOP_TUNER
	// these are only used by the loop tuner
	risse_int FromTier;	//!< display tier of vertical 'from' line
	risse_int LinkTier;	//!< display tier of horizontal link
	risse_int ToTier;		//!< display tier of vertical 'to' allow line
	risse_int Index;		//!< link index

	struct tSortByDistanceFuncObj
	{
		bool operator()(
			const tTVPWaveLoopLink &lhs,
			const tTVPWaveLoopLink &rhs) const
		{
			risse_int64 lhs_dist = lhs.From - lhs.To;
			if(lhs_dist < 0) lhs_dist = -lhs_dist;
			risse_int64 rhs_dist = rhs.From - rhs.To;
			if(rhs_dist < 0) rhs_dist = -rhs_dist;
			return lhs_dist < rhs_dist;
		}
	};

	struct tSortByIndexFuncObj
	{
		bool operator()(
			const tTVPWaveLoopLink &lhs,
			const tTVPWaveLoopLink &rhs) const
		{
			return lhs.Index < rhs.Index;
		}
	};
#endif

	//! @brief コンストラクタ
	tTVPWaveLoopLink()
	{
		From = To = 0;
		Smooth = false;
		Condition = llcNone;
		RefValue = CondVar = 0;

#ifdef TVP_IN_LOOP_TUNER
		FromTier = LinkTier = ToTier = 0;
		Index = 0;
#endif
	}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		リンクをソートするための比較関数
//! @param		lhs		演算子の左側
//! @param		rhs		演算子の右側
//! @return		lhs < rhs の場合真、それ以外の場合は偽
//---------------------------------------------------------------------------
bool inline operator < (const tTVPWaveLoopLink & lhs, const tTVPWaveLoopLink & rhs)
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
//! @brief		ラベルを表すクラス
//---------------------------------------------------------------------------
class tTVPWaveLabel
{
public:
	risse_int64 Position; //!< label position
	tTVPLabelStringType Name; //!< label name
	risse_int Offset;
		/*!< オフセット
			@note
			This member will be set in tTVPWaveLoopManager::Decode,
			and will contain the sample granule offset from first decoding
			point at call of tTVPWaveLoopManager::Decode().
		*/
#ifdef TVP_IN_LOOP_TUNER
	// these are only used by the loop tuner
	risse_int NameWidth; //!< display name width
	risse_int Index; //!< index
#endif

	struct tSortByPositionFuncObj
	{
		bool operator()(
			const tTVPWaveLabel &lhs,
			const tTVPWaveLabel &rhs) const
		{
			return lhs.Position < rhs.Position;
		}
	};

	struct tSortByOffsetFuncObj
	{
		bool operator()(
			const tTVPWaveLabel &lhs,
			const tTVPWaveLabel &rhs) const
		{
			return lhs.Offset < rhs.Offset;
		}
	};

#ifdef TVP_IN_LOOP_TUNER
	struct tSortByIndexFuncObj
	{
		bool operator()(
			const tTVPWaveLabel &lhs,
			const tTVPWaveLabel &rhs) const
		{
			return lhs.Index < rhs.Index;
		}
	};
#endif

	//! @brief コンストラクタ
	tTVPWaveLabel()
	{
		Position = 0;
		Offset = 0;
#ifdef TVP_IN_LOOP_TUNER
		NameWidth = 0;
		Index = 0;
#endif
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ラベルをソートするための比較関数
//! @param		lhs		演算子の左側
//! @param		rhs		演算子の右側
//! @return		lhs < rhs の場合真、それ以外の場合は偽
//---------------------------------------------------------------------------
bool inline operator < (const tTVPWaveLabel & lhs, const tTVPWaveLabel & rhs)
{
	return lhs.Position < rhs.Position;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		Wave ループマネージャ
//---------------------------------------------------------------------------
class tTVPWaveDecoder;
class tTVPWaveFormat;
class tTVPWaveLoopManager
{
public:
	// 定数
	static const int TVPWaveLoopLinkGiveUpCount = 10;
		/*!< This is for preventing infinite loop caused by recursive links.
		 If the decoding point does not change when the loop manager follows the
		 links, after 'TVPWaveLoopLinkGiveUpCount' times the loop manager
		 will give up the decoding.*/

	static const int MaxFlags = 16; //!< フラグの最大数
	static const int MaxFlagValue = 9999; //!< フラグがとりうる最大値
	static const int SmoothTime = 50; //!< スムースリンクのクロスフェードの時間
	static const int SmoothTimeHalf = SmoothTime / 2; //!< SmoothTime の半分
	static const int MaxIDLen = 16; //!< 識別子の最大長

private:

	//! @brief 再生セグメント情報 (tTVPWaveLoopManager::Decodeメソッド で返される)
	struct tSegment
	{
		//! @brief コンストラクタ
		tSegment(risse_int64 start, risse_int64 length)
			{ Start = start; Length = length; }
		risse_int64 Start; //!< スタート位置 (PCM サンプル数単位)
		risse_int64 Length; //!< セグメントの長さ (PCM サンプル数単位)
	};

	tRisseCriticalSection FlagsCS; //!< CS to protect flags/links/labels
	int Flags[MaxFlags]; //!< フラグ
	bool FlagsModifiedByLabelExpression; //!< true if the flags are modified by EvalLabelExpression
	std::vector<tTVPWaveLoopLink> Links; //!< リンクの配列
	std::vector<tTVPWaveLabel> Labels; //!< ラベルの配列
	tRisseCriticalSection DataCS; // CS to protect other members
	tTVPWaveFormat * Format; //!< PCMフォーマット
	tTVPWaveDecoder * Decoder; //!< デコーダ

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
	tTVPWaveLoopManager();
	virtual ~tTVPWaveLoopManager();

	void SetDecoder(tTVPWaveDecoder * decoder);

	int GetFlag(risse_int index);
	void CopyFlags(risse_int *dest);
	bool GetFlagsModifiedByLabelExpression();
	void SetFlag(risse_int index, risse_int f);
	void ClearFlags();
	void ClearLinksAndLabels();

	const std::vector<tTVPWaveLoopLink> & GetLinks() const;
	const std::vector<tTVPWaveLabel> & GetLabels() const;

	void SetLinks(const std::vector<tTVPWaveLoopLink> & links);
	void SetLabels(const std::vector<tTVPWaveLabel> & labels);

	bool GetIgnoreLinks() const;
	void SetIgnoreLinks(bool b);

	risse_int64 GetPosition() const;
	void SetPosition(risse_int64 pos);

	bool GetLooping() const { return Looping; } //!< ループ情報を読み込んでいないときにループを行うかどうかを得る
	void SetLooping(bool b) { Looping = b; } //!< ループ情報を読み込んでいないときにループを行うかどうかを設定する

	void Decode(void *dest, risse_uint samples, risse_uint &written,
		std::vector<tSegment> &segments,
		std::vector<tTVPWaveLabel> &labels);

private:
	bool GetNearestEvent(risse_int64 current,
		tTVPWaveLoopLink & link, bool ignore_conditions);

	void GetLabelAt(risse_int64 from, risse_int64 to,
		std::vector<tTVPWaveLabel> & labels);

	void DoCrossFade(void *dest, void *src1, void *src2, risse_int samples,
		risse_int ratiostart, risse_int ratioend);

	void ClearCrossFadeInformation();

//--- flag manupulation by label expression
	enum tExpressionToken {
		etUnknown,
		etEOE,			// End of the expression
		etLBracket,		// '['
		etRBracket,		// ']'
		etInteger,		// integer number
		etEqual,		// '='
		etPlus,			// '+'
		etMinus,		// '-'
		etPlusEqual,	// '+='
		etMinusEqual,	// '-='
		etIncrement,	// '++'
		etDecrement		// '--'
	};
public:
	static bool GetLabelExpression(const tTVPLabelStringType &label,
		tExpressionToken * ope = NULL,
		risse_int *lv = NULL,
		risse_int *rv = NULL, bool *is_rv_indirect = NULL);
private:
	bool EvalLabelExpression(const tTVPLabelStringType &label);

	static tExpressionToken GetExpressionToken(const tTVPLabelCharType * &  p , risse_int * value);
	static bool GetLabelCharInt(const tTVPLabelCharType *s, risse_int &v);


//--- loop information input/output stuff
private:
	static bool GetInt(char *s, risse_int &v);
	static bool GetInt64(char *s, risse_int64 &v);
	static bool GetBool(char *s, bool &v);
	static bool GetCondition(char *s, tTVPWaveLoopLinkCondition &v);
	static bool GetString(char *s, tTVPLabelStringType &v);

	static bool GetEntityToken(char * & p, char **name, char **value);

	static bool ReadLinkInformation(char * & p, tTVPWaveLoopLink &link);
	static bool ReadLabelInformation(char * & p, tTVPWaveLabel &label);
public:
	bool ReadInformation(char * p);

#ifdef TVP_IN_LOOP_TUNER
	// output facility (currently only available with VCL interface)
private:
	static void PutInt(AnsiString &s, risse_int v);
	static void PutInt64(AnsiString &s, risse_int64 v);
	static void PutBool(AnsiString &s, bool v);
	static void PutCondition(AnsiString &s, tTVPWaveLoopLinkCondition v);
	static void PutString(AnsiString &s, tTVPLabelStringType v);
	static void DoSpacing(AnsiString &l, int col);
public:
	void WriteInformation(AnsiString &s);
#endif


public:
};
//---------------------------------------------------------------------------
#endif


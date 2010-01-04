//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コードブロック
//---------------------------------------------------------------------------
#ifndef risseCodeBlockH
#define risseCodeBlockH

#include "risseGC.h"
#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseVariant.h"
#include "risseObject.h"

//---------------------------------------------------------------------------
namespace Risse
{
class tCodeGenerator;
class tCodeExecutor;
class tScriptBlockBase;
//---------------------------------------------------------------------------
/**
 * コードブロッククラス
 * @note	コードブロックは Risse の実行単位である「関数」などごとに、
 *			VMが実行すべき環境についての情報を保持するクラスである
 */
class tCodeBlock : public tObjectInterface
{
	tScriptBlockInstance * ScriptBlockInstance; //!< スクリプトブロックインスタンス
	risse_uint32 *Code; //!< コード
	risse_size CodeSize; //!< コードのサイズ(ワード単位)
	tVariant *Consts; //!< 定数領域
	risse_size ConstsSize; //!< 定数領域のサイズ(個)
	risse_size NumRegs; //!< 必要なレジスタ数
	risse_size NestLevel; //!< 関数のネストレベル
	risse_size SharedVariableNestCount; //!< 共有変数の最大のネストカウント (risse_size_maxの場合はこの情報が無効のとき)
	risse_size NumSharedVars; //!< 必要な共有変数の数
	std::pair<risse_size, risse_size> * CodeToSourcePosition; //!< コード上の位置からソースコード上の位置へのマッピングの配列
	risse_size CodeToSourcePositionSize; //!< コード上の位置からソースコード上の位置へのマッピングの配列のサイズ

	typedef std::pair<risse_size, risse_size> tRelocation; //!<  コードブロック再配置情報のtypedef
	tRelocation * CodeBlockRelocations; //!< コードブロック再配置情報
	risse_size CodeBlockRelocationSize;

	tRelocation * TryIdentifierRelocations; //!< try識別子再配置情報
	risse_size TryIdentifierRelocationSize;

	tCodeExecutor * Executor; //!< コード実行クラスのインスタンス

public:
	/**
	 * コンストラクタ
	 * @param sb	スクリプトブロックインスタンス
	 */
	tCodeBlock(tScriptBlockInstance * sb);

	/**
	 * スクリプトブロックインスタンスを得る
	 * @return	スクリプトブロックインスタンス
	 */
	tScriptBlockInstance * GetScriptBlockInstance() const { return ScriptBlockInstance; }

	/**
	 * コードを設定する(コードジェネレータから)
	 * @param gen	コードジェネレータ
	 */
	void Assign(const tCodeGenerator *gen);

	/**
	 * 共有変数の最大のネストカウントを設定する
	 * @param level	共有変数の最大のネストカウント
	 */
	void SetSharedVariableNestCount(risse_size level);

	/**
	 * 再配置情報の fixup を行う
	 * @param sb	スクリプトブロック
	 */
	void Fixup();

	/**
	 * このコードブロックのオブジェクトを得る
	 * @return	このコードブロックを表すオブジェクト
	 */
	tVariant GetObject();

	/**
	 * 内容をダンプ(逆アセンブル)する
	 * @return	ダンプした結果
	 */
	tString Dump() const;

 	/**
 	 * コードを得る
 	 * @return	コード
 	 */
 	const risse_uint32 * GetCode() const { return Code; }

	/**
	 * コードのサイズ(ワード単位)を得る
	 * @return	コードのサイズ
	 */
 	risse_size GetCodeSize() const { return CodeSize; }

	/**
	 * 定数領域を得る
	 * @return	定数領域
	 */
	const tVariant * GetConsts() const { return Consts; }

	/**
	 * 定数領域のサイズ(個)を得る
	 * @return	定数領域のサイズ(個)
	 */
	risse_size GetConstsSize() const { return ConstsSize; }

	/**
	 * 必要なレジスタ数を得る
	 * @return	必要なレジスタ数
	 */
	risse_size GetNumRegs() const { return NumRegs; }

	/**
	 * 関数のネストレベルを得る
	 * @return	関数のネストレベル
	 */
	risse_size GetNestLevel() const { return NestLevel; }

	/**
	 * 共有変数の最大のネストレベルを得る
	 * @return	共有変数の最大のネストレベル
	 */
	risse_size GetSharedVariableNestCount() const { return SharedVariableNestCount; }

	/**
	 * 必要な共有変数の数を得る
	 * @return	必要な共有変数の数
	 */
	risse_size GetNumSharedVars() const { return NumSharedVars; }

	/**
	 * コード実行クラスのインスタンスを得る
	 */
	tCodeExecutor * GetExecutor() const { return Executor; }

	/**
	 * VM コード位置からソースコード上の位置へ変換する
	 * @param pos	VMコード位置(ワード単位)
	 * @return	ソースコード上の位置
	 */
	risse_size CodePositionToSourcePosition(risse_size pos) const;

public: // tObjectInterface メンバ

	tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
};
//---------------------------------------------------------------------------


class tSharedVariableFramesOverlay;
//---------------------------------------------------------------------------
/**
 * 共有変数フレーム
 */
class tSharedVariableFrames : public tCollectee
{
protected:
	gc_vector<tVariant *> Frames; //!< 共有変数フレームの配列 (関数のネストレベルによりフレームが異なる)
	tCriticalSection * CS; //!< この共有フレームへのアクセスを保護するための CS

public:
	/**
	 * コンストラクタ
	 * @param max_nest_level	最大の関数のネストレベル(このサイズにて Frames が確保される)
	 */
	tSharedVariableFrames(risse_size max_nest_level)
	{
		CS = new tCriticalSection();
		Frames.resize(max_nest_level);
	}

	/**
	 * コピーコンストラクタ
	 * @param ref	コピー元の共有変数フレーム
	 * @note	ref の Frames がシャローコピーされた後、CS はコピー元と共有されるようになる
	 */
	tSharedVariableFrames(const tSharedVariableFrames & ref) : Frames(ref.Frames), CS(ref.CS)
	{;}

	/**
	 * コンストラクタ
	 * @param ref				コピー元の共有変数フレーム
	 * @param max_nest_level	最大の関数のネストレベル(このサイズにて Frames が確保される)
	 * @note	ref の Frames がシャローコピーされた後、max_nest_level までリサイズされる。
	 *			CS はコピー元と共有されるようになる
	 */
	tSharedVariableFrames(const tSharedVariableFrames & ref,
		risse_size max_nest_level) : Frames(ref.Frames), CS(ref.CS)
	{
		Frames.resize(max_nest_level);
	}

	/**
	 * コンストラクタ(tSharedVariableFramesOverlayから)
	 * @param ref	コピー元のtSharedVariableFramesOverlayオブジェクト
	 */
	tSharedVariableFrames(const tSharedVariableFramesOverlay & ref);

#if 0
	/**
	 * 指定のネストレベルのフレームを確保する
	 * @param level	ネストレベル
	 * @param size	フレームのサイズ
	 * @return	そのネストレベルの位置に以前にあったフレーム
	 */
	tVariant * Set(risse_size level, risse_size size)
	{
		volatile tCriticalSection::tLocker sync(*CS);

		RISSE_ASSERT(level < Frames.size());
		tVariant * prev = Frames[level];
		Frames[level] = size ? new tVariant[size] : NULL;
		return prev;
	}

	/**
	 * 指定のネストレベルのフレームを設定する
	 * @param level	ネストレベル
	 * @param frame	フレーム
	 * @return	そのネストレベルの位置に以前にあったフレーム
	 */
	tVariant * Set(risse_size level, tVariant * frame)
	{
		volatile tCriticalSection::tLocker sync(*CS);

		RISSE_ASSERT(level < Frames.size());
		tVariant * prev = Frames[level];
		Frames[level] = frame;
		return prev;
	}
#endif

	/**
	 * 指定位置の共有変数へ書き込みを行う
	 * @param level	ネストレベル
	 * @param num	位置
	 * @param value	値
	 */
	void Set(risse_size level, risse_size num, const tVariant & val)
	{
		volatile tCriticalSection::tLocker sync(*CS);
		RISSE_ASSERT(level < Frames.size());
		RISSE_ASSERT(Frames[level] != NULL);
		Frames[level][num] = val;
	}

	/**
	 * 指定位置の共有変数からの読み込みを行う
	 * @param level	ネストレベル
	 * @param num	位置
	 * @return	値
	 */
	const tVariant & Get(risse_size level, risse_size num) const
	{
		volatile tCriticalSection::tLocker sync(*CS);
		RISSE_ASSERT(level < Frames.size());
		RISSE_ASSERT(Frames[level] != NULL);
		return Frames[level][num];
	}


	/**
	 * このオブジェクトのクリティカルセクションを得る
	 * @return	クリティカルセクションオブジェクト
	 */
	tCriticalSection & GetCS() const { return *CS; }


	// friend 指定
	friend class tSharedVariableFramesOverlay;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 共有変数フレームオーバーレイ
 * @note	tSharedVariableFrames に1レベルだけ frames をオーバーレイできる
 *			クラス
 */
class tSharedVariableFramesOverlay
{
private:
	tSharedVariableFramesOverlay(const tSharedVariableFramesOverlay &); //!< コピー不可です
	void operator = (const tSharedVariableFramesOverlay &); //!< コピー不可です
	void * operator new(size_t); //!< ヒープ上に置かないでください
	void * operator new [] (size_t); //!< ヒープ上に置かないでください

	const tSharedVariableFrames * Frames; // フレーム
	tVariant * OverlayedFrame; // 新しいフレーム
	risse_size OverlayedFrameLevel; // OverlayedFrame のレベル

public:
	// コンストラクタ
	/**
	 * frames				共有変数フレーム
	 * @param overlayed_frame_level	新しいフレームのレベル
	 * @param overlayed_frame_size	新しいフレームのサイズ
	 */
	tSharedVariableFramesOverlay(const tSharedVariableFrames * frames,
		risse_size overlayed_frame_level, risse_size overlayed_frame_size)
	{
		RISSE_ASSERT(frames != NULL);
		Frames = frames;
		OverlayedFrame = overlayed_frame_size ? new tVariant[overlayed_frame_size] : NULL;
		OverlayedFrameLevel = overlayed_frame_level;
		RISSE_ASSERT(overlayed_frame_size == 0 || OverlayedFrameLevel < Frames->Frames.size());
		if(!OverlayedFrame) OverlayedFrameLevel = risse_size_max;
					// オーバーレイを行わない場合は常に Frames を見に行くように
	}

	/**
	 * 指定位置の共有変数へ書き込みを行う
	 * @param level	ネストレベル
	 * @param num	位置
	 * @param value	値
	 */
	void Set(risse_size level, risse_size num, const tVariant & val)
	{
		volatile tCriticalSection::tLocker sync(*Frames->CS);
		RISSE_ASSERT(level < Frames->Frames.size());
		if(level == OverlayedFrameLevel)
		{
			OverlayedFrame[num] = val;
		}
		else
		{
			RISSE_ASSERT(Frames->Frames[level] != NULL);
			Frames->Frames[level][num] = val;
		}
	}

	/**
	 * 指定位置の共有変数からの読み込みを行う
	 * @param level	ネストレベル
	 * @param num	位置
	 * @return	値
	 */
	const tVariant & Get(risse_size level, risse_size num) const
	{
		volatile tCriticalSection::tLocker sync(*Frames->CS);
		RISSE_ASSERT(level < Frames->Frames.size());
		if(level == OverlayedFrameLevel)
		{
			return OverlayedFrame[num];
		}
		else
		{
			RISSE_ASSERT(Frames->Frames[level] != NULL);
			return Frames->Frames[level][num];
		}
	}

	// friend 指定
	friend class tSharedVariableFrames;
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * コードブロックスタックアダプタ
 * @note	このクラスは tCodeBlock を内部に参照として持つが、
 *			それが実行されるべきスタックフレームや共有フレーム情報をも持つ。
 *			指定のスタックフレームや共有フレームでコードブロックを実行したいときに
 *			このアダプタを利用する。
 *			このインスタンスはコード実行時に動的に作成される。
 */
class tCodeBlockStackAdapter : public tObjectInterface
{
	const tCodeBlock * CodeBlock; //!< コードブロック
	tVariant * Frame; //!< スタックフレーム
	tSharedVariableFrames Shared; //!< 共有フレーム

public:
	/**
	 * コンストラクタ
	 * @param codeblock	コードブロック
	 * @param frame		スタックフレーム
	 * @param shared	共有フレームオーバーレイ
	 */
	tCodeBlockStackAdapter(const tCodeBlock * codeblock,
		tVariant * frame , const tSharedVariableFramesOverlay & shared_overlay):
		 CodeBlock(codeblock), Frame(frame), Shared(shared_overlay) {;}

public: // tObjectInterface メンバ

	tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);

	/**
	 * デストラクタ (コンパイラの警告潰し用;実際には呼ばれることはない)
	 */
	virtual ~tCodeBlockStackAdapter() {;}
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risse
#endif


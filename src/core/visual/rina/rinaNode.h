//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RINA ノード管理
//---------------------------------------------------------------------------
#ifndef RINANODE_H
#define RINANODE_H

#include "visual/rina/rinaIdRegistry.h"
#include "visual/rina/rinaGraph.h"

namespace Rina {
//---------------------------------------------------------------------------


class tInputPin;
class tOutputPin;
class tInputPins;
class tOutputPins;
class tQueueNode;
class tQueueBuilder;
class tGraph;
//---------------------------------------------------------------------------
//! @brief		プロセスノード
//---------------------------------------------------------------------------
class tProcessNode : public Risa::tPolymorphic
{
public:
	typedef Risa::tPolymorphic inherited;

private:
	tGraph * Graph; //!< グラフインスタンス

	risse_size	LongestDistance;
			//!< ルートノードからの最長距離(ステップ数)。
			//!< 依存関係のクイックなチェックに用いる

	risse_size	BuildQueueWaitingOutputPin; //!< BuildQueue にて待つべき出力/入力ピンの数

public:
	//! @brief		グラフをロックするためのクラス
	class tGraphLocker : public Risa::tCriticalSection::tLocker
	{
	public:
		tGraphLocker(const tProcessNode & _this) :
			tCriticalSection::tLocker(_this.GetGraph()->GetCS()) {;}
	};

public:
	//! @brief		プロセスノードを LongestDistance で比較する関数
	struct tLongestDistanceComparator :
		public std::binary_function<bool, tProcessNode*, tProcessNode*>
	{
		bool operator ()(tProcessNode * a, tProcessNode * b)
		{
			risse_offset dis = a->GetLongestDistance() - b->GetLongestDistance();
			if(dis < 0) return true;
			if(dis > 0) return false;
			return a < b;
		}
	};


public:
	//! @brief		コンストラクタ
	//! @param		graph		グラフインスタンス
	tProcessNode(tGraph * graph);

public:
	//! @brief		グラフインスタンスを得る
	tGraph * GetGraph() const { return Graph; }

public:
	//! @brief		ルートノードからの最長距離を得る
	//! @return		ルートノードからの最長距離 (ルートノード = 0)
	//! @note		どの出力ピンも接続されていない状態では返される値の内容は不定
	risse_size GetLongestDistance() const { return LongestDistance; }

	//! @brief		ルートノードからの最長距離を設定する
	//! @note		このメソッドは入力ピン(子ノード)に再帰して、子すべての最長距離を更新する
	void CalcLongestDistance();

public: // サブクラスで実装すべき物
	//! @brief		入力ピンの配列を得る
	//! @return		入力ピンの配列
	virtual tInputPins & GetInputPins() = 0;

	//! @brief		出力ピンの配列を得る
	//! @return		出力ピンの配列
	virtual tOutputPins & GetOutputPins() = 0;

public:
	//! @brief		コマンドキューの組み立てを行う
	//! @param		builder			キュービルダーオブジェクト
	virtual void BuildQueue(tQueueBuilder & builder) { return ; }


};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}

#endif

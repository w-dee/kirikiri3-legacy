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

#include "risa/packages/risa/graphic/rina/rinaIdRegistry.h"
#include "risa/packages/risa/graphic/rina/rinaGraph.h"

namespace Risa {
//---------------------------------------------------------------------------

class tInputPinArrayInstance;
class tOutputPinArrayInstance;
class tQueueNode;
class tQueueBuilder;
class tGraphInstance;
//---------------------------------------------------------------------------
//! @brief		プロセスノードインスタンス
//---------------------------------------------------------------------------
class tNodeInstance : public tObjectBase
{
public:
	typedef tObjectBase inherited;

private:
	tGraphInstance * GraphInstance; //!< グラフインスタンス

	risse_size	LongestDistance;
			//!< ルートノードからの最長距離(ステップ数)。
			//!< 依存関係のクイックなチェックに用いる

	risse_size	BuildQueueWaitingOutputPin; //!< BuildQueue にて待つべき出力/入力ピンの数

public:
	//! @brief		グラフをロックするためのクラス
	class tGraphLocker : public tObjectInterface::tSynchronizer
	{
	public:
		tGraphLocker(const tNodeInstance * _this) :
			tObjectInterface::tSynchronizer(_this->GetGraphInstance()) {;}
	};

public:
	//! @brief		プロセスノードを LongestDistance で比較する関数
	struct tLongestDistanceComparator :
		public std::binary_function<bool, tNodeInstance*, tNodeInstance*>
	{
		bool operator ()(tNodeInstance * a, tNodeInstance * b)
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
	tNodeInstance(tGraphInstance * graph);

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tNodeInstance() {}

public:
	//! @brief		グラフインスタンスを得る
	tGraphInstance * GetGraphInstance() const { return GraphInstance; }

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
	virtual tInputPinArrayInstance & GetInputPinArrayInstance() = 0;

	//! @brief		出力ピンの配列を得る
	//! @return		出力ピンの配列
	virtual tOutputPinArrayInstance & GetOutputPinArrayInstance() = 0;

public:
	//! @brief		コマンドキューの組み立てを行う
	//! @param		builder			キュービルダーオブジェクト
	virtual void BuildQueue(tQueueBuilder & builder) { return ; }

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		"Node" クラス
//---------------------------------------------------------------------------
class tNodeClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tNodeClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public: // Risse 用メソッドなど
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}

#endif

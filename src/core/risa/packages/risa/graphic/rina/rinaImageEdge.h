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
//! @brief RINA ラスタ画像用のピンなど
//---------------------------------------------------------------------------
#ifndef RINAIMAGEEDGE_H
#define RINAIMAGEEDGE_H

#include "risa/packages/risa/graphic/rina/rinaIdRegistry.h"
#include "risa/packages/risa/graphic/rina/rinaQueue.h"
#include "risa/packages/risa/graphic/rina/rinaTypes.h"
#include "risa/packages/risa/graphic/rina/rinaPin.h"

namespace Risa {
//---------------------------------------------------------------------------



static const risse_uint32 ImageEdgeType = tFourCharId<'i','m','a','g'>::value;
	//!< ラスタ画像データのエッジタイプ

//---------------------------------------------------------------------------
//! @brief		画像ピンのエッジタイプを登録するためのシングルトン
//---------------------------------------------------------------------------
class tImageEdgeTypeRegisterer : public Risa::singleton_base<tImageEdgeTypeRegisterer>
{
public:
	//! @brief		コンストラクタ
	tImageEdgeTypeRegisterer();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		親ノードから子ノードへ要求するデータの内容(レンダリング要求)
//---------------------------------------------------------------------------
class tImageRenderRequest : public tRenderRequest, public Risa::tSubmorph<tImageRenderRequest>
{
public:
	typedef tRenderRequest inherited;

private:
	tTexturePolygonList Area; //!< 要求するエリア

public:
	//! @brief		コンストラクタ
	//! @param		parent		親キューノード
	//! @param		index		親キューノード内でのインデックス
	//! @param		area		要求範囲
	tImageRenderRequest(tQueueNode * parent, risse_size index,
		const tTexturePolygonList & area) :
		inherited(parent, index), Area(area) {;}

	//! @brief		要求範囲を得る
	const tTexturePolygonList & GetArea() const { return Area; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		ミキサ(あるいはコンポジッタ)用の親ノードから子ノードへ要求するデータの内容
//---------------------------------------------------------------------------
class tImageMixerRenderRequest : public tImageRenderRequest,
	public Risa::tSubmorph<tImageMixerRenderRequest>
{
public:
	typedef tImageRenderRequest inherited;
private:

	tPolygonXformList		Xforms; //!< 変形情報

public:
	//! @brief		コンストラクタ
	//! @param		parent		親キューノード
	//! @param		index		親キューノード内でのインデックス
	//! @param		area		要求エリア情報
	//! @param		xforms		変形情報
	tImageMixerRenderRequest(tQueueNode * parent, risse_size index,
		const tTexturePolygonList & area,
		const tPolygonXformList & xforms) :
		inherited(parent, index, area), Xforms(xforms) {;}

	//! @brief		変形情報を得る
	//! @return		変形情報
	const tPolygonXformList & GetXforms() const { return Xforms; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		ラスタ画像用の入力ピンインスタンス
//---------------------------------------------------------------------------
class tImageInputPinInstance : public tInputPinInstance
{
public:
	typedef tInputPinInstance inherited;
private:

public:

	//! @brief		コンストラクタ
	tImageInputPinInstance();

	//! @brief		このピンがサポートするタイプの一覧を得る
	//! @return		このピンがサポートするタイプの一覧
	//! @note		返される配列は、最初の物ほど優先度が高い
	virtual const gc_vector<risse_uint32> & GetSupportedTypes();

	//! @brief		内容の更新があったことを上位のノードに伝える
	//! @param		rect		更新のあった矩形
	virtual void NotifyUpdate(const tTexturePolygon & rect);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		ミキサ(あるいはコンポジッタ)用のラスタ画像用の入力ピン
//---------------------------------------------------------------------------
class tImageMixerInputPinInstance : public tImageInputPinInstance
{
public:
	typedef tImageInputPinInstance inherited;
private:

	tPolygonXformList		Xforms; //!< 変形情報

public:
	//! @brief		コンストラクタ
	tImageMixerInputPinInstance() : inherited() { ; }

	//! @brief		変形情報を得る
	//! @return		変形情報
	const tPolygonXformList & GetXforms() const { return Xforms; }

	//! @brief		内容の更新があったことを上位のノードに伝える
	//! @param		rect		更新のあった矩形
	virtual void NotifyUpdate(const tTexturePolygon & rect);
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		ラスタ画像用の出力ピン
//---------------------------------------------------------------------------
class tImageOutputPinInstance : public tOutputPinInstance
{
public:
	typedef tOutputPinInstance inherited;
private:

public:
	//! @brief		コンストラクタ
	tImageOutputPinInstance();

	//! @brief		このピンがサポートするタイプの一覧を得る
	//! @return		このピンがサポートするタイプの一覧
	//! @note		返される配列は、最初の物ほど優先度が高い
	virtual const gc_vector<risse_uint32> & GetSupportedTypes();

	//! @brief		内容の更新があったことを上位のノードに伝える
	//! @param		rect		更新のあった矩形
	void NotifyUpdate(const tTexturePolygon & rect);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		ラスタ画像用のコマンドキュー
//---------------------------------------------------------------------------
class tImageQueueNode : public tQueueNode, public Risa::tSubmorph<tImageQueueNode>
{
public:
	typedef tQueueNode inherited;
private:

public:
	//! @brief		コンストラクタ
	//! @param		request		レンダリング要求
	tImageQueueNode(const tImageRenderRequest * request) :
		inherited(request) {;}

public: //!< サブクラスでオーバーライドして使う物
	//! @brief		画像を得る
	//! @return		画像
	virtual const tString & GetImageBufferNoAddRef() = 0;

	//! @brief		画像のオフセットを得る
	//! @return		オフセット
	//! @brief		GetImageBufferNoAddRef() で得られた画像の左上が、
	//!				要求された座標系におけるどの点なのかを表す。
	virtual t2DPoint GetOffset() = 0;

protected: //!< サブクラスでオーバーライドして使う物
	//! @brief		ノードの処理の最初に行う処理
	virtual void BeginProcess() {;}

	//! @brief		ノードの処理の最後に行う処理
	virtual void EndProcess() {;}
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
}

#endif

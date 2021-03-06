//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RINA 型とか
//---------------------------------------------------------------------------
#ifndef RINATYPES_H
#define RINATYPES_H


namespace Risa {
//---------------------------------------------------------------------------
typedef risse_int32		coord; //!< 整数座標タイプ
typedef float			coord_f; //!< 実数座標タイプ
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 2Dポイントクラス
 */
template <typename VALUE_TYPE>
struct t2DPointBase
{
	typedef VALUE_TYPE tValue; //!< 値の型の typedef

	tValue X; //!< x または u
	tValue Y; //!< y または v

};
//---------------------------------------------------------------------------
/**
 * 整数ポイントクラスのtypedef
 */
typedef t2DPointBase<coord> t2DPoint;
//---------------------------------------------------------------------------
/**
 * 実数ポイントクラスのtypedef
 */
typedef t2DPointBase<coord_f> t2DPointF;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 3次元同次座標による頂点
 */
struct tXYZWVertex : public tCollectee
{
	coord_f X; //!< x
	coord_f Y; //!< y
	coord_f Z; //!< z
	coord_f W; //!< w
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * ポリゴンの変形情報
 */
class tPolygonXform : public tCollectee
{
	tXYZWVertex Vertices[3]; //!< 転送先頂点
	bool IsTriangle; //!< 真の場合3角形、偽の場合4角形
	unsigned char Opacity; //!< 不透明度(0..255)
	// TODO: 合成モードをここに
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * ポリゴンの変形情報のリスト
 */
typedef gc_vector<tPolygonXform> tPolygonXformList;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * テクスチャ上のポリゴン(UV座標系)
 */
struct tTexturePolygon : public tCollectee
{
	t2DPointF Vertices[3]; //!< 頂点
	bool IsTriangle; //!< 真の場合3角形、偽の場合4角形
	/**
	 * コンストラクタ
	 * @param va			3つの頂点配列
	 * @param is_triangle	真の場合3角形、偽の場合4角形
	 */
	tTexturePolygon(const t2DPointF * va,
				bool is_triangle = true)
	{
		Vertices[0] = va[0];
		Vertices[1] = va[1];
		Vertices[2] = va[2];
		IsTriangle = is_triangle;
	}
	/**
	 * コンストラクタ
	 * @param v0			頂点0
	 * @param v1			頂点1
	 * @param v2			頂点2
	 * @param is_triangle	真の場合3角形、偽の場合4角形
	 */
	tTexturePolygon(const t2DPointF & v0, const t2DPointF & v1, const t2DPointF & v2,
				bool is_triangle = true)
	{
		Vertices[0] = v0;
		Vertices[1] = v1;
		Vertices[2] = v2;
		IsTriangle = is_triangle;
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * テクスチャ上のポリゴンリスト
 */
typedef gc_vector<tTexturePolygon> tTexturePolygonList;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}

#endif

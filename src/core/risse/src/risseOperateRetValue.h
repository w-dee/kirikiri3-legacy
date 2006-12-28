//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief tRisseVariantやtRisseObjectInterfaceのOperateメソッドの戻り値処理
//---------------------------------------------------------------------------
#ifndef risseOperateRetValueH
#define risseOperateRetValueH


//---------------------------------------------------------------------------
namespace Risse
{
//---------------------------------------------------------------------------
class tRisseOperateRetValue
{
public:
	//! @brief		Operateメソッドの戻り値
	enum tRetValue
	{
		rvNoError,				//!< エラー無し
		rvMemberNotFound		//!< メンバが見つからないエラー
	};

	//! @brief		例外を発生させる
	//! @param		エラーコード
	//! @param		name	操作を行うメンバ名
	//!						(空文字列の場合はこのオブジェクトそのものに対しての操作)
	static void RaiseError(tRetValue ret, const tRisseString & name);
};
//---------------------------------------------------------------------------
}
#endif


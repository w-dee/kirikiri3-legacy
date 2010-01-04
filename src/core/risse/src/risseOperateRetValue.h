//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief tVariantやtObjectInterfaceのOperateメソッドの戻り値処理
//---------------------------------------------------------------------------
#ifndef risseOperateRetValueH
#define risseOperateRetValueH


//---------------------------------------------------------------------------
namespace Risse
{
//---------------------------------------------------------------------------
class tOperateRetValue
{
public:
	/**
	 * Operateメソッドの戻り値
	 */
	enum tRetValue
	{
		rvNoError					/*!< エラー無し */,
		rvMemberNotFound			/*!< メンバが見つからないエラー */,
		rvMemberIsReadOnly			/*!< メンバは読み込み専用 */,
		rvMemberIsFinal				/*!< メンバはfinalなのでオーバーライドできない */,
		rvPropertyCannotBeRead		/*!< プロパティは書き込み専用 */,
		rvPropertyCannotBeWritten	/*!< プロパティは読み込み専用 */,
	};

	/**
	 * 例外を発生させる
	 * @param name	操作を行うメンバ名
	 *				(空文字列の場合はこのオブジェクトそのものに対しての操作)
	 */
	static void RaiseError(tRetValue ret, const tString & name);

	/**
	 * もし指定された値がエラーならば例外を発生させる
	 * @param name	操作を行うメンバ名
	 *				(空文字列の場合はこのオブジェクトそのものに対しての操作)
	 */
	static void RaiseIfError(tRetValue ret, const tString & name)
	{
		if(ret != rvNoError) RaiseError(ret, name);
	}
};
//---------------------------------------------------------------------------
}
#endif


//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief C++ でモジュールを簡単に実装できるようにするためのヘルパークラス
//---------------------------------------------------------------------------
#ifndef risseModuleH
#define risseModuleH

#include "risseCharUtils.h"
#include "risseObject.h"
#include "risseClass.h"
#include "risseObjectBase.h"

namespace Risse
{
//---------------------------------------------------------------------------
/**
 * モジュール用クラスの基底クラス(C++用)
 * @note	初期化の方式はクラスの場合とかなり違うので注意。
 */
class tModuleBase : public tCollectee
{
	typedef tModuleBase inherited; //!< 親クラスの typedef

	tObjectBase * Instance; //!< モジュールインスタンス
public:
	/**
	 * コンストラクタ
	 * @param name		モジュール名
	 * @param engine	スクリプトエンジンインスタンス
	 */
	tModuleBase(const tString & name, tScriptEngine * engine);

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tModuleBase() {}

	/**
	 * 各メンバをインスタンスに追加する
	 */
	virtual void RegisterMembers();

	/**
	 * モジュールインスタンスを得る
	 */
	tObjectBase * GetInstance() const { return Instance; }

	/**
	 * モジュールインスタンスを登録するためのユーティリティメソッド
	 * @param target	登録先オブジェクト (普通、globalオブジェクト)
	 * @param name		登録名
	 */
	void RegisterInstance(tVariant & target);
};
//---------------------------------------------------------------------------
}
#endif


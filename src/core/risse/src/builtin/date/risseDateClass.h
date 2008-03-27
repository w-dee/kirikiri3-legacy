//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Date" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseDateClassH
#define risseDateClassH

#include "../../risseObject.h"
#include "../../risseClass.h"
#include "../../risseGC.h"
#include "../../risseNativeBinder.h"
#include "../../risseScriptEngine.h"
#include "../risseBuiltinPackageInitializer.h"


#ifndef RISSE_SUPPORT_WX
	#error "wxWidgets support is missing. Currently Risse needs wxWidgets to support Date class."
#endif

#include <wx/datetime.h>

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		"Date" クラスのインスタンス用 C++クラス
//---------------------------------------------------------------------------
class tDateInstance : public tObjectBase
{
private:
	wxDateTime DateTime; //!< DateTime オブジェクト

public:
	//! @brief		DateTimeへの参照を得る
	//! @return		DateTimeへの参照
	wxDateTime & GetDateTime() { return DateTime; }

	//! @brief		ダミーのデストラクタ(おそらく呼ばれない)
	virtual ~tDateInstance() {;}


private:
	//! @brief		日付を RFC 822 フォーマットして帰す
	static tString Format(wxDateTime::Tm & tm, int tzofs);

	//! @brief		日付文字列を parse する
	//! @param		str		parser する文字列
	void Parse(const tString & str);

public: // Risse用メソッドなど

	void construct();
	void initialize(const tNativeCallInfo &info);

	int getYear() const;
	void setYear(int y);
	int getMonth() const;
	void setMonth(int m);
	int getDate() const;
	void setDate(int d);
	int getDay() const;
	int getHours() const;
	void setHours(int h);
	int getMinutes() const;
	void setMinutes(int m);
	int getSeconds() const;
	void setSeconds(int s);
	int getMilliseconds() const;
	void setMilliseconds(int ms);
	risse_int64 getTime() const;
	void setTime(risse_int64 s);
	static int getTimezoneOffset();

	tString toGMTString() const;
	tString toString() const;
	tString toLocaleString() const;

	bool equal(const tVariant & rhs); // 日付同士の比較 TODO: 他の比較演算子の実装
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"Date" クラス
//---------------------------------------------------------------------------
class tDateClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tDateClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"date" パッケージイニシャライザ
//---------------------------------------------------------------------------
class tDatePackageInitializer : public tBuiltinPackageInitializer
{
public:
	tDateClass * DateClass;

	//! @brief		コンストラクタ
	tDatePackageInitializer();

	//! @brief		パッケージを初期化する
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		パッケージ名
	//! @param		global		パッケージグローバル
	virtual void Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global);
};
//---------------------------------------------------------------------------




} // namespace Risse


#endif

//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Array" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseArrayClassH
#define risseArrayClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseSingleton.h"
#include "risseGC.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		"Array" クラスのインスタンス用 C++クラス
//---------------------------------------------------------------------------
class tRisseArrayInstance : public tRisseObjectBase
{
public:
	typedef gc_deque<tRisseVariant> tArray; //!< 配列の中身のtypedef

private:
	tArray Array; //!< 配列の中身

public:
	//! @brief		Arrayへの参照を得る
	//! @return		Arrayへの参照
	tArray & GetArray() { return Array; }

	//! @brief		ダミーのデストラクタ(おそらく呼ばれない)
	virtual ~tRisseArrayInstance() {;}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"Array" クラス
//---------------------------------------------------------------------------
class tRisseArrayClass : public tRisseClassBase, public tRisseClassSingleton<tRisseArrayClass>
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	tRisseArrayClass();

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	tRisseVariant CreateNewObjectBase();

public:
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif

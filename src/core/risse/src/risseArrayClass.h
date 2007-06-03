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
#include "risseGC.h"
#include "risseNativeBinder.h"

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

public: // Risse用メソッドなど

	void construct();
	void initialize(const tRisseNativeBindFunctionCallingInfo &info);
	tRisseVariant iget(risse_offset ofs_index) const;
	void iset(const tRisseVariant & value, risse_offset ofs_index);
	void push(const tRisseMethodArgument & args);
	tRisseVariant pop();
	void unshift(const tRisseMethodArgument & args);
	tRisseVariant shift();
	size_t get_length() const;
	void set_length(size_t new_size);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"Array" クラス
//---------------------------------------------------------------------------
class tRisseArrayClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseArrayClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	tRisseVariant CreateNewObjectBase();

public:
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif

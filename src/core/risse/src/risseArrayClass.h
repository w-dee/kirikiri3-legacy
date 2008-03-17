//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
class tArrayInstance : public tObjectBase
{
public:
	typedef gc_deque<tVariant> tArray; //!< 配列の中身のtypedef

private:
	tArray Array; //!< 配列の中身

public:
	//! @brief		Arrayへの参照を得る
	//! @return		Arrayへの参照
	tArray & GetArray() { return Array; }

	//! @brief		ダミーのデストラクタ(おそらく呼ばれない)
	virtual ~tArrayInstance() {;}

public: // Risse用メソッドなど

	void construct();
	void initialize(const tNativeCallInfo &info);
	tVariant iget(risse_offset ofs_index);
	void iset(const tVariant & value, risse_offset ofs_index);
	void push(const tMethodArgument & args);
	tVariant pop();
	void unshift(const tMethodArgument & args);
	tVariant shift();
	size_t get_length() const;
	void set_length(size_t new_size);
	tString join(const tMethodArgument & args);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"Array" クラス
//---------------------------------------------------------------------------
class tArrayClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tArrayClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public:
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif

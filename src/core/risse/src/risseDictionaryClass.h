//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Dictionary" クラスの実装
//---------------------------------------------------------------------------

#ifndef risseDictionaryClassH
#define risseDictionaryClassH

#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"
#include "risseNativeBinder.h"
#include "risseHashTable.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		"Dictionary" クラスのインスタンス用 C++クラス
//---------------------------------------------------------------------------
class tDictionaryInstance : public tObjectBase
{
public:
	typedef tHashTable<tVariant, tVariant> tMemberHashTable; //!< 辞書配列の中身のtypedef

private:
	tMemberHashTable HashTable; //!< 辞書配列の中身

public:
	//! @brief		HashTableへの参照を得る
	//! @return		HashTableへの参照
	tMemberHashTable & GetHashTable() { return HashTable; }

	//! @brief		ダミーのデストラクタ(おそらく呼ばれない)
	virtual ~tDictionaryInstance() {;}

public: // Risse用メソッドなど

	void construct();
	void initialize(const tNativeCallInfo &info);
	tVariant iget(const tVariant & key);
	void iset(const tVariant & value, const tVariant & key);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		"Dictionary" クラス
//---------------------------------------------------------------------------
class tDictionaryClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tDictionaryClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public:
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif

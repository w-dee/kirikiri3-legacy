//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

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
/**
 * "Dictionary" クラスのインスタンス用 C++クラス
 */
class tDictionaryInstance : public tObjectBase
{
public:
	typedef tHashTable<tVariant, tVariant> tMemberHashTable; //!< 辞書配列の中身のtypedef

private:
	tMemberHashTable HashTable; //!< 辞書配列の中身

public:
	/**
	 * HashTableへの参照を得る
	 * @return	HashTableへの参照
	 */
	tMemberHashTable & GetHashTable() { return HashTable; }

	/**
	 * ダミーのデストラクタ(おそらく呼ばれない)
	 */
	virtual ~tDictionaryInstance() {;}

public: // Risse用メソッドなど

	void construct();
	void initialize(const tNativeCallInfo &info);
	tVariant iget(const tVariant & key);
	void iset(const tVariant & value, const tVariant & key);
	tVariant idelete(const tVariant & key); // delete[]
	void clear();
	risse_size get_count();
	void eachPair(const tNativeCallInfo &info);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * "Dictionary" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tDictionaryClass, tClassBase, tDictionaryInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------
} // namespace Risse


#endif

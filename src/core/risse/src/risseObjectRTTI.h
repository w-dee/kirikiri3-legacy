//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オブジェクトインターフェース用の簡易RTTIの実装
//---------------------------------------------------------------------------

#ifndef risseObjectRTTIH
#define risseObjectRTTIH

#include "risseTypes.h"
#include "risseGC.h"

namespace Risse
{
/*
	Risseのオブジェクトインターフェースは、クラス固有のバイナリデータを
	持つことができ、ネイティブ実装されたメソッドなどから使用することができる。
	しかし、、Risseはコンテキストを自由に変えてクラスのメソッドを
	呼び出すことができるため、間違ったクラスのコンテキストでメソッドが実行されると
	クラッシュする可能性がある。
	これは問題であるため、一応、そのコンテキストが本当に正しいクラスなのかを
	C++ レベルでチェックする機構が必要になる。C++のRTTI(dynamic_cast) を使う
	こともできるだろうが、ここでは自前で非常に簡易的な(そして高速な) RTTIを
	使うことにする。
*/

//---------------------------------------------------------------------------
//! @brief		簡易RTTI
//---------------------------------------------------------------------------
class tRisseRTTI : public tRisseCollectee
{
	gc_vector<void*> Ids; //!< クラスIdの配列

public:
	//! @brief		Ids の中の特定のインデックスにマッチする構造体
	struct tMatcher
	{
		risse_size Index; //!< Classes 内のインデックス
		void * Id; //!< クラスId

		//! @brief		この構造体が RTTI にマッチするかどうかを調べる
		bool Match(const tRisseRTTI * rtti) const
		{
			if(!rtti) return false; // RTTIがない
			if(rtti->Ids.size() <= Index) return false; // インデックスを超えてる
			return rtti->Ids[Index] == Id; // Id の同一性をチェック
		}
	};


public:
	//! @brief		RTTIにId情報を追加する
	//! @param		id		Id
	//! return		RTTIにマッチを行うためにマッチ用構造体
	tMatcher AddId(void * id)
	{
		risse_size index = Ids.size();
		Ids.push_back(id);
		tMatcher matcher;
		matcher.Index = index;
		matcher.Id = id;
		return matcher;
	}
};
//---------------------------------------------------------------------------
} // namespace Risse


#endif
//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 型キャストを行うための簡易RTTI
//---------------------------------------------------------------------------
#ifndef RISATYPECAST_H
#define RISATYPECAST_H

#include "base/utils/Singleton.h"
#include "base/gc/RisaGC.h"

/*!
	ダウンキャストの安全性を、dynamic_cast を使わずに実現するための軽量RTTIの実装。
	Risseでもインスタンスが特定のC++型にダウンキャスト可能かどうかをチェックするために
	似た機構を使っている。


	基本的には

	基底クラスを tPolymorphic から派生し、
	親クラスを public な "inherited" として各クラス内で typedef して使う。
	各クラスは tSubmorph も継承する。

例:
	class tBase : public tPolymorphic
	{ public: typedef tPolymorphic inherited; private:
	};

	class tSubA : public tBase, tSubmorph<tSubA>
	{ public: typedef tBase inherited; private:
	};

	class tSubB : public tSubA, tSubmorph<tSubB>
	{ public: typedef tSubA inherited; private:
	};

	TryDownCast はダウンキャストに成功すればその型にダウンキャストした物が
	得られる。DownCast はダウンキャストに失敗した場合にデバッグビルドの場合に
	assetion failure を発生させる。

	tBase * hoge = ....;
	tSubB * subb = DownCast<tSubB*>(hoge);
		// ↑もし与えられた hoge が tSubB のインスタンスならば成功する
*/


namespace Risa
{
//---------------------------------------------------------------------------
//! @brief  ダウンキャスト可能なクラスの基底クラス
//---------------------------------------------------------------------------
class tPolymorphic : public tCollectee
{
	typedef void * tClassIds;
public:
	tClassIds * ClassIds; // クラスIDの配列への配列
	tPolymorphic() { ClassIds = NULL; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief  各クラスIdを保持するためのクラス
//---------------------------------------------------------------------------
struct tSubmorphIds
{
	enum { MaxIds = 8 };
	void * Ids[MaxIds];

	tSubmorphIds() { for(int i = 0; i < MaxIds; i++) Ids[i] = NULL; }

	tSubmorphIds(const tSubmorphIds * ref) { for(int i = 0; i < MaxIds; i++) Ids[i] = ref?ref->Ids[i]:NULL; }

	int AddId(void * id)
	{
		// 空いているもっともインデックスの小さな場所にIdを設定する
		for(int i = 0; i < MaxIds; i++)
			if(Ids[i] == NULL)
			{
				Ids[i] = id;
				return i;
			}
		RISSE_ASSERT(!"maximum subclass depth exceeded!");
		return 0;
	}
};


//---------------------------------------------------------------------------
// 親クラスのクラスId配列を参照するためのクラス (親クラスにしたがって特殊化させてある)
//---------------------------------------------------------------------------
template <typename T>
struct tSubmorphIdsHolder;

template <typename S>
struct tSubmorphSuperclassSelector
{
	static const tSubmorphIds * Get()
	{ return &(tSubmorphIdsHolder<S>::instance()->Ids); }
};

template <>
struct tSubmorphSuperclassSelector<tPolymorphic>
{
	/*
		こっちのクラスは親クラスが tPolymorphic (つまり基底クラス)の場合に選択され、
		Get() は NULL を返す(つまりクラスId情報がもうないことを表す)
	*/
	static const tSubmorphIds * Get()
	{ return NULL; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	 クラスId配列をクラスごとに保持するためのシングルトンクラス
//---------------------------------------------------------------------------
template <typename T>
struct tSubmorphIdsHolder : public singleton_base<tSubmorphIdsHolder<T> >
{
	tSubmorphIds Ids;
	static int Index;
	static void * Id;
	tSubmorphIdsHolder() : Ids(tSubmorphSuperclassSelector<typename T::inherited>::Get())
	{
		// クラス Id としては、このシングルトンインスタンスのアドレスを用いる
		// (インスタンスごとに必ずユニークになるから)
		Index = Ids.AddId(this);
		Id = this;
	}
};
template <typename T> 
int tSubmorphIdsHolder<T>::Index = 0;
template <typename T> 
void * tSubmorphIdsHolder<T>::Id = NULL;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	 クラスIdを登録するために各 tPolymorphic クラスのサブクラスごとに継承させるテンプレートクラス
//---------------------------------------------------------------------------
template <typename T>
struct tSubmorph
{
	tSubmorph()
	{
		RISSE_ASSERT(dynamic_cast<tPolymorphic*>(static_cast<T*>(static_cast<void*>(this))) != NULL);
		tPolymorphic* base = 
			static_cast<tPolymorphic*>(static_cast<T*>(static_cast<void*>(this)));
		base->ClassIds = tSubmorphIdsHolder<T>::instance()->Ids.Ids;
/*
		fprintf(stderr, "instance %p(%p): initialized id of %s at %p, ", this, base,
			typeid(T).name(),
			base->ClassIds);
		for(int i = 0; i < 8; i++)
			fprintf(stderr, "0x%p ", base->ClassIds[i]);
		fprintf(stderr, "\n");
*/
	}
};

//---------------------------------------------------------------------------
// 参照はずしと cv はずし
template <typename T> struct tDeconstDevolatileDereference                    { typedef T type; };
template <typename T> struct tDeconstDevolatileDereference<T*>                { typedef T type; };
template <typename T> struct tDeconstDevolatileDereference<const T*>          { typedef T type; };
template <typename T> struct tDeconstDevolatileDereference<volatile T*>       { typedef T type; };
template <typename T> struct tDeconstDevolatileDereference<const volatile T*> { typedef T type; };
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		型Tが型Rにダウンキャスト可能かどうかをチェックする関数
//! @param		instance		インスタンス
//! @return		ダウンキャスト可能ならば型Rにダウンキャストされたインスタンス、そうでなければNULL
//---------------------------------------------------------------------------
template <typename R, typename T>
R TryDownCast(T * instance)
{
//	fprintf(stderr, "%p(%p): %s->%s\n", instance, static_cast<const volatile tPolymorphic*>(instance), typeid(T).name(), typeid(R).name());

	typedef typename tDeconstDevolatileDereference<R>::type RAW_R;
	typedef tSubmorphIdsHolder<RAW_R> HOLDER;
	RISSE_ASSERT(dynamic_cast<const volatile tPolymorphic*>(instance) != NULL);
	const volatile tPolymorphic * base = static_cast<const volatile tPolymorphic*>(instance);
	RISSE_ASSERT(base->ClassIds != NULL);

//	fprintf(stderr, "HOLDER::Index:%d, HOLDER::ID:%p, base->ClassIds:%p, base->ClassIds[HOLDER::Index]:%p\n",
//		HOLDER::Index, HOLDER::Id, base->ClassIds, base->ClassIds[HOLDER::Index]);

	/*
		ダウンキャストが可能かどうかは、クラスIdの配列のうち、HOLDER::indexで表される
		インデックス(つまり継承の階層)が HOLDER::Id (つまりクラスId) と同一で
		あるかどうかでチェックする。
	*/

	if(base->ClassIds[HOLDER::Index] == HOLDER::Id)
		return static_cast<R>(instance);


	return NULL;
}
//---------------------------------------------------------------------------
//! @brief		型Tが型Rにダウンキャスト可能かどうかをチェックする関数
//! @param		instance		インスタンス
//! @return		ダウンキャスト可能ならば型Rにダウンキャストされたインスタンス、
//!				そうでなければデバッグビルド時にはassertion failure
//---------------------------------------------------------------------------
template <typename R, typename T>
R DownCast(T * instance)
{
	R result = TryDownCast<R, T>(instance);
#ifdef RISSE_ASSERT_ENABLED
	if(!result) fprintf(stderr, "%p: %s->%s failed\n", instance, typeid(T).name(), typeid(R).name());
#endif
	RISSE_ASSERT(result != NULL);
	return result;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}
#endif

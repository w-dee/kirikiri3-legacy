//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 簡易シングルトン管理用クラス
//---------------------------------------------------------------------------
#ifndef risseSingletonH
#define risseSingletonH

/*
	クラスインスタンスなどはシングルトンとしてプロセス内にただ一つ存在してい
	なければならない。このテンプレートクラスはシングルトンを容易に構築するため
	の物である。
	スレッド保護などの機能はなく、非常にシンプルな実装。
*/

//---------------------------------------------------------------------------


namespace Risse
{

//---------------------------------------------------------------------------
//! @brief  シングルトンオブジェクト用クラス
//! @note	テンプレート引数の最初の引数はシングルトンとなるべきクラス
//!			(デフォルトコンストラクタがprotectedで利用可能でなければならない)
//---------------------------------------------------------------------------
template <typename T>
class tRisseSingleton
{
	tRisseSingleton(const tRisseSingleton &); //!< non-copyable
	void operator = (const tRisseSingleton &); //!< non-copyable

	// インスタンスを保持する変数
	static T * _instance;

public:
	tRisseSingleton(){;} //!< デフォルトコンストラクタ(何もしない)

	//! @brief		インスタンスを得る
	//! @note		インスタンスが作成されていなければ作成を行う
	static T & GetInstance() { if(!_instance) _instance = new T(); return *_instance; }

	//! @brief		インスタンスへのポインタを獲る
	//! @note		インスタンスが作成されていなければ作成を行う
	static T * GetPointer() { if(!_instance) _instance = new T(); return _instance; }

	//! @brief		インスタンスがすでに作成されているかどうかを得る
	static bool GetInstanceAlive() { return _instance != NULL; }
};
template <typename T>
T* tRisseSingleton<T>::_instance = NULL;
//---------------------------------------------------------------------------
}
#endif


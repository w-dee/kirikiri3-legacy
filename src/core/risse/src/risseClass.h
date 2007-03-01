//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief C++ 用 Risse (クラス) の基底クラスの実装
//---------------------------------------------------------------------------
#ifndef risseClassH
#define risseClassH

#include "risseCharUtils.h"
#include "risseObject.h"
#include "risseObjectBase.h"
#include "risseObjectRTTI.h"
#include "risseVariant.h"
#include "risseSingleton.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		Risseクラスの実装用のシングルトンクラス
//! @note		クラスに必要ないくつかの機能をカプセル化するため、このクラスを
//!				継承すると便利。
//---------------------------------------------------------------------------
template <typename T>
class tRisseClassSingleton : public tRisseSingleton<T>
{
public:
	//! @brief		シングルトンクラスインスタンスを登録するためのユーティリティメソッド
	//! @param		target		登録先オブジェクト (普通、globalオブジェクト)
	//! @param		name		登録名
	static void RegisterClassInstance(tRisseVariant & target, const tRisseString &name)
	{
		target.SetPropertyDirect(name,
			tRisseOperateFlags(tRisseMemberAttribute(tRisseMemberAttribute::ocConst)) |
			tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly,
					tRisseVariant(T::GetPointer()));
	}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		Risseクラスの実装
//---------------------------------------------------------------------------
class tRisseClassBase : public tRisseObjectBase
{
	//! @brief		親クラスのtypedef
	typedef tRisseObjectBase inherited;

	tRisseRTTI RTTI; //!< RTTI 参照用データ(このクラスから作成されたインスタンスはこのRTTIを持つ)
	tRisseRTTI::tMatcher RTTIMatcher; //!< RTTI にマッチするための情報

public:
	//! @brief		コンストラクタ
	//! @param		super_class		スーパークラスを表すオブジェクト
	tRisseClassBase(tRisseClassBase * super_class = NULL);

public:
	//! @brief		各メンバをインスタンスに追加する
	//! @note		これは通常コンストラクタ中から呼ばれるが、コンストラクタ中では
	//!				「仮の」メンバが登録される可能性がある(シングルトンインスタンスの
	//!				依存関係が解決できないため)。
	//!				そのため、すべてのクラスのコンストラクタを呼び出した後、
	//!				もう一度このメンバを呼び出し、本来のメンバが登録されるように
	//!				する必要がある (つまりこのメソッドは計２回呼ばれる)。
	//!				子クラスでオーバーライドして、子クラス内では親クラスの同名の
	//!				メソッドを呼ぶこと。
	virtual void RegisterMembers();

public:
	//! @brief		ダミーのデストラクタ(たぶん呼ばれない)
	virtual ~tRisseClassBase() {;}

	//! @brief		RTTIにマッチするための情報を得る
	//! @return		RTTIにマッチするための情報
	const tRisseRTTI::tMatcher & GetRTTIMatcher() const { return RTTIMatcher; }

	//! @brief		オブジェクトに対して操作を行う
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);

protected:
	//! @brief		newの際の新しいオブジェクトを作成して返す(必要ならば下位クラスでオーバーライドすること)
	virtual tRisseVariant CreateNewObjectBase();

public:
	//! @brief		インスタンスの親クラスのメソッドを呼び出すユーティリティメソッド
	//! @param		ret			関数呼び出し結果の格納先(NULL=呼び出し結果は必要なし)
	//! @param		name		呼び出すメソッド名
	//! @param		flags		呼び出しフラグ
	//! @param		args		引数
	//! @param		This		メソッドが実行されるべき"Thisオブジェクト"
	void CallSuperClassMethod(
		tRisseVariantBlock * ret, const tRisseString & name,
		risse_uint32 flags, const tRisseMethodArgument & args, const tRisseVariant & This);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		ただ親クラスを継承するためだけのクラス階層用のテンプレートクラス
//---------------------------------------------------------------------------
#define RISSE_DEFINE_EMPTY_CLASS(classname, super_class, additional) \
class classname : \
	public tRisseClassBase, public tRisseClassSingleton<classname> \
{ \
	typedef tRisseClassBase inherited; \
public: \
	classname()  : tRisseClassBase(super_class::GetPointer()) \
	{ RegisterMembers(); } \
	void RegisterMembers() { \
		RISSE_BEGIN_NATIVE_METHOD(ss_construct) { } RISSE_END_NATIVE_METHOD \
		RISSE_BEGIN_NATIVE_METHOD(ss_initialize) { \
			classname::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This); \
		} RISSE_END_NATIVE_METHOD \
	} \
	additional \
}

//---------------------------------------------------------------------------

}
#endif


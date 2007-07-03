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

namespace Risse
{
class tRisseScriptEngine;
class tRisseNativeCallInfo;
//---------------------------------------------------------------------------
//! @brief		Risseクラスの実装
//---------------------------------------------------------------------------
class tRisseClassBase : public tRisseObjectBase
{
	//! @brief		親クラスのtypedef
	typedef tRisseObjectBase inherited;

private:
	tRisseRTTI ClassRTTI; //!< RTTI 参照用データ(このクラスから作成されたインスタンスはこのRTTIを持つ)
	tRisseRTTI::tMatcher RTTIMatcher; //!< ClassRTTI にマッチするための情報

public:
	//! @brief		コンストラクタ
	//! @param		name			クラス名
	//! @param		super_class		スーパークラスを表すオブジェクト
	//! @param		extensible		extensible の場合に真
	tRisseClassBase(const tRisseString & name, tRisseClassBase * super_class, bool extensible = true);

	//! @brief		コンストラクタ(Objectクラス用)
	//! @param		engine			スクリプトエンジンインスタンス
	//! @param		extensible		extensible の場合に真
	tRisseClassBase(tRisseScriptEngine * engine);

	//! @brief		このインスタンスの RTTI に Class クラスの RTTI を設定する
	//! @param		engine			スクリプトエンジンインスタンス
	void SetClassClassRTTI(tRisseScriptEngine * engine);

	//! @brief		このインスタンスの ClassRTTI を取得する
	//! @return		このインスタンスの ClassRTTI
	tRisseRTTI & GetClassRTTI() { return ClassRTTI; }

	//! @brief		クラスインスタンスを登録するためのユーティリティメソッド
	//! @param		target		登録先オブジェクト (普通、globalオブジェクト)
	//! @param		name		登録名
	void RegisterClassInstance(tRisseVariant & target, const tRisseString &name)
	{
		target.SetPropertyDirect_Object(name,
			tRisseOperateFlags(tRisseMemberAttribute::GetDefault()) |
			tRisseOperateFlags::ofMemberEnsure | tRisseOperateFlags::ofInstanceMemberOnly,
					tRisseVariant(this));
	}


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

public: // Risse用メソッドなど
	static void risse_new(const tRisseNativeCallInfo &info);
	static void fertilize(const tRisseVariant & instance, const tRisseNativeCallInfo &info);
	static void include(const tRisseMethodArgument & args, const tRisseNativeCallInfo &info);
	static tRisseVariant ovulate();
};
//---------------------------------------------------------------------------





}
#endif


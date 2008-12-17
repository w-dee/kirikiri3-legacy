//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
class tScriptEngine;
class tNativeCallInfo;
//---------------------------------------------------------------------------
/**
 * Risseクラスの実装
 */
class tClassBase : public tObjectBase
{
	/**
	 * 親クラスのtypedef
	 */
	typedef tObjectBase inherited;

private:
	tRTTI ClassRTTI; //!< RTTI 参照用データ(このクラスから作成されたインスタンスはこのRTTIを持つ)
	tRTTI::tMatcher RTTIMatcher; //!< ClassRTTI にマッチするための情報

public:
	typedef tClassBase tInstanceClass; //!< このクラスのインスタンスはこのクラス

public:
	/**
	 * コンストラクタ
	 * @param name			クラス名
	 * @param super_class	スーパークラスを表すオブジェクト
	 * @param extensible	extensible の場合に真
	 */
	tClassBase(const tString & name, tClassBase * super_class, bool extensible = true);

	/**
	 * コンストラクタ(Objectクラス用)
	 * @param engine		スクリプトエンジンインスタンス
	 * @param extensible	extensible の場合に真
	 */
	tClassBase(tScriptEngine * engine);

	/**
	 * このインスタンスの RTTI に Class クラスの RTTI を設定する
	 * @param engine	スクリプトエンジンインスタンス
	 */
	void SetClassClassRTTI(tScriptEngine * engine);

	/**
	 * このインスタンスの ClassRTTI を取得する
	 * @return	このインスタンスの ClassRTTI
	 */
	tRTTI & GetClassRTTI() { return ClassRTTI; }

	/**
	 * クラスインスタンスを登録するためのユーティリティメソッド
	 * @param target	登録先オブジェクト (普通、globalオブジェクト)
	 * @param name		登録名
	 */
	void RegisterInstance(const tVariant & target);

public:
	/**
	 * 各メンバをインスタンスに追加する
	 * @note	これは通常コンストラクタ中から呼ばれるが、コンストラクタ中では
	 *			「仮の」メンバが登録される可能性がある(シングルトンインスタンスの
	 *			依存関係が解決できないため)。
	 *			そのため、すべてのクラスのコンストラクタを呼び出した後、
	 *			もう一度このメンバを呼び出し、本来のメンバが登録されるように
	 *			する必要がある (つまりこのメソッドは計２回呼ばれる)。
	 *			子クラスでオーバーライドして、子クラス内では親クラスの同名の
	 *			メソッドを呼ぶこと。
	 */
	virtual void RegisterMembers();

public:
	/**
	 * ダミーのデストラクタ(たぶん呼ばれない)
	 */
	virtual ~tClassBase() {;}

	/**
	 * RTTIにマッチするための情報を得る
	 * @return	RTTIにマッチするための情報
	 */
	const tRTTI::tMatcher & GetRTTIMatcher() const { return RTTIMatcher; }

	/**
	 * オブジェクトに対して操作を行う
	 */
	virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);

public:
	/**
	 * インスタンスの親クラスのメソッドを呼び出すユーティリティメソッド
	 * @param ret	関数呼び出し結果の格納先(NULL=呼び出し結果は必要なし)
	 * @param name	呼び出すメソッド名
	 * @param flags	呼び出しフラグ
	 * @param args	引数
	 * @param This	メソッドが実行されるべき"Thisオブジェクト"
	 */
	void CallSuperClassMethod(
		tVariantBlock * ret, const tString & name,
		risse_uint32 flags, const tMethodArgument & args, const tVariant & This);

protected:
	/**
	 * newされると「このクラスのインスタンスは作成できない」例外を投げるダミーのクラス
	 * @note	このクラスからはインスタンスを生成できないことを表す。
	 */
	class tNoInstanceClass : public tObjectBase
	{
	public:
		/**
		 * コンストラクタ
		 */
		tNoInstanceClass();
	};

	/**
	 * どのようにインスタンス化するかを表す定数
	 */
	enum tInstantiationType
	{
		itNormal /*!< 普通にインスタンス化可能 */,
		itPrimitive /*!< プリミティブ型用 */,
		itNoInstance /*!< インスタンス化できない */
	};

	/**
	 * Tに応じた新規オブジェクトを返す
	 * @return 新規作成されたオブジェクト
	 */
	template <int I, typename T>
	struct tOvulateNewObject
	{
		static T * C() { return new T(); }
	};

	template <typename T>
	struct tOvulateNewObject<itPrimitive, T>
	{
		static T C() { return T(); }
	};

	template <typename T>
	struct tOvulateNewObject<itNoInstance, T>
	{
		static tVariant C() { return new tNoInstanceClass(); }
	};

public: // Risse用メソッドなど
	static void risse_new(const tNativeCallInfo &info);
	static void fertilize(const tVariant & instance, const tNativeCallInfo &info);
	static void include(const tMethodArgument & args, const tNativeCallInfo &info);
	static tVariant ovulate();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// クラスを簡単に定義するためのマクロ
#define RISSE_DEFINE_CLASS_BEGIN(CPPNAME, SUPERCLASS, INSTANCECLASS, INSTANCETYPE) \
	class CPPNAME : public SUPERCLASS \
	{ \
	typedef SUPERCLASS inherited; \
public: \
	typedef INSTANCECLASS tInstanceClass; \
	enum { InstanceType = INSTANCETYPE }; \
public: \
	CPPNAME(tScriptEngine * engine); \
	void RegisterMembers(); \
	static tVariant ovulate(); \
public:

#define RISSE_DEFINE_CLASS_END() \
	};

#define RISSE_DEFINE_CLASS(CPPNAME, SUPERCLASS, INSTANCECLASS, INSTANCETYPE) \
	RISSE_DEFINE_CLASS_BEGIN(CPPNAME, SUPERCLASS, INSTANCECLASS, INSTANCETYPE) \
	RISSE_DEFINE_CLASS_END()

#define RISSE_IMPL_CLASS_BEGIN(CPPNAME, RISSENAME, SUPERCLASS) \
	CPPNAME::CPPNAME(tScriptEngine * engine) : \
		inherited((RISSENAME), (SUPERCLASS)) { RegisterMembers(); } \
	tVariant CPPNAME::ovulate() { return tOvulateNewObject<InstanceType, tInstanceClass>::C(); }\
	void CPPNAME::RegisterMembers() { inherited::RegisterMembers();

#define RISSE_IMPL_CLASS_END() \
	}

#define RISSE_BIND_CONSTRUCTORS /*ovulate, construct, initialize をバインドする */ \
	BindFunction(this, ss_ovulate, &ovulate); \
	BindFunction(this, ss_construct, &tInstanceClass::construct); \
	BindFunction(this, ss_initialize, &tInstanceClass::initialize);
//---------------------------------------------------------------------------




}
#endif


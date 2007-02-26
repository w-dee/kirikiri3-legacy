//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ネイティブプロパティを記述するためのサポート
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseNativeProperty.h"
#include "rissePropertyClass.h"
#include "risseException.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(21996,23593,41879,16445,47022,10284,22019,34197);



//---------------------------------------------------------------------------
tRisseNativePropertyGetter::tRetValue tRisseNativePropertyGetter::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall)
		{
			// このオブジェクトに対するプロパティ読み込みなので Getter を呼ぶ
			if(!Getter) return rvPropertyCannotBeRead;
			Getter(result, flags, This);
			return rvNoError;
		}
	}

	// そのほかの場合はメンバが無い物として扱う
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseNativePropertySetter::tRetValue tRisseNativePropertySetter::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall)
		{
			// このオブジェクトに対するプロパティ書き込みなので Setter を呼ぶ
			if(!Setter) return rvPropertyCannotBeWritten;
			args.ExpectArgumentCount(1);
			Setter(args[0], flags, This);
			return rvNoError;
		}
	}

	// そのほかの場合はメンバが無い物として扱う
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseNativeProperty::tRisseNativeProperty(
	tRisseNativePropertyGetter::tGetter getter,
	tRisseNativePropertySetter::tSetter setter)
{
	Getter = getter;
	Setter = setter;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseNativeProperty::tRetValue tRisseNativeProperty::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall) // このオブジェクトに対するプロパティ読み込みか？
		{
			// このオブジェクトに対するプロパティ読み込みなので Getter を呼ぶ
			if(!Getter) return rvPropertyCannotBeRead;
			Getter(result, flags, This);
			return rvNoError;
		}
		else if(code == ocDSet) // このオブジェクトに対するプロパティ書き込みか？
		{
			// このオブジェクトに対するプロパティ書き込みなので Setter を呼ぶ
			if(!Setter) return rvPropertyCannotBeWritten;
			args.ExpectArgumentCount(1);
			Setter(args[0], flags, This);
			return rvNoError;
		}
	}

	// そのほかの場合はメンバが無い物として扱う
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectInterface * tRisseNativeProperty::New(tRisseNativePropertyGetter::tGetter getter, tRisseNativePropertySetter::tSetter setter)
{
	// tRissePropertyClass がまだ登録されていない場合は仮のメソッドを
	// 作成して登録する (のちに正式なメソッドオブジェクトに置き換えられる)
	if(tRissePropertyClass::GetInstanceAlive())
	{
		tRisseVariant v = tRisseVariant(tRissePropertyClass::GetPointer()).New(
				0, tRisseMethodArgument::New(new tRisseNativePropertyGetter(getter), new tRisseNativePropertySetter(setter)));

		RISSE_ASSERT(v.GetType() == tRisseVariant::vtObject);
		RISSE_ASSERT(dynamic_cast<tRissePropertyInstance*>(v.GetObjectInterface()) != NULL);

		return v.GetObjectInterface();
	}
	else
	{
		// 仮実装
		return (tRissePropertyInstance *)(new tRisseNativeProperty(getter, setter));
	}
}
//---------------------------------------------------------------------------

} // namespace Risse

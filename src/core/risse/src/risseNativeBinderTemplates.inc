
// このファイルは create_binder.rb により再生成されます


	// static関数、非 void の戻り値用
	template <typename CC, typename R >
	class RisseBinderFunctor0s
	{
		typedef R (*tFunc)();

		enum { NumMandatoryArgs = 0 };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
));
			else
				((tFunc)f)(
);
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC >
	class RisseBinderFunctor0s<CC, void >
	{
		typedef void (*tFunc)();

		enum { NumMandatoryArgs = 0 };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			((tFunc)f)(
);
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R >
	class RisseBinderFunctor0
	{
		typedef R (IC::*tFunc)();

		enum { NumMandatoryArgs = 0 };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
));
			else
				(instance->*((tFunc)f))(
);
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC >
	class RisseBinderFunctor0<CC, IC, void  >
	{
		typedef void (IC::*tFunc)();

		enum { NumMandatoryArgs = 0 };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(
);
			if(info.result) info.result->Clear();
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R >
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (*f)(), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindStaticFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&RisseBinderFunctor0s<CC, R >::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R >
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (IC::*f)(), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&RisseBinderFunctor0<CC, IC, R >::Call
					), context), attribute);
	}


	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0>
	class RisseBinderFunctor1s
	{
		typedef R (*tFunc)(T0);
		enum { ArgIndex0=0 };
		enum { NumMandatoryArgs = ArgIndex0+ tRisseIsFuncCallNonMetaType<T0>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info)));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0>
	class RisseBinderFunctor1s<CC, void ,T0>
	{
		typedef void (*tFunc)(T0);
		enum { ArgIndex0=0 };
		enum { NumMandatoryArgs = ArgIndex0+ tRisseIsFuncCallNonMetaType<T0>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0>
	class RisseBinderFunctor1
	{
		typedef R (IC::*tFunc)(T0);
		enum { ArgIndex0=0 };
		enum { NumMandatoryArgs = ArgIndex0+ tRisseIsFuncCallNonMetaType<T0>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info)));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0>
	class RisseBinderFunctor1<CC, IC, void  ,T0>
	{
		typedef void (IC::*tFunc)(T0);
		enum { ArgIndex0=0 };
		enum { NumMandatoryArgs = ArgIndex0+ tRisseIsFuncCallNonMetaType<T0>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (*f)(T0), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindStaticFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&RisseBinderFunctor1s<CC, R ,T0>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (IC::*f)(T0), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&RisseBinderFunctor1<CC, IC, R ,T0>::Call
					), context), attribute);
	}


	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1>
	class RisseBinderFunctor2s
	{
		typedef R (*tFunc)(T0,T1);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { NumMandatoryArgs = ArgIndex1+ tRisseIsFuncCallNonMetaType<T1>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info)));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1>
	class RisseBinderFunctor2s<CC, void ,T0,T1>
	{
		typedef void (*tFunc)(T0,T1);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { NumMandatoryArgs = ArgIndex1+ tRisseIsFuncCallNonMetaType<T1>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1>
	class RisseBinderFunctor2
	{
		typedef R (IC::*tFunc)(T0,T1);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { NumMandatoryArgs = ArgIndex1+ tRisseIsFuncCallNonMetaType<T1>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info)));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1>
	class RisseBinderFunctor2<CC, IC, void  ,T0,T1>
	{
		typedef void (IC::*tFunc)(T0,T1);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { NumMandatoryArgs = ArgIndex1+ tRisseIsFuncCallNonMetaType<T1>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (*f)(T0,T1), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindStaticFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&RisseBinderFunctor2s<CC, R ,T0,T1>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&RisseBinderFunctor2<CC, IC, R ,T0,T1>::Call
					), context), attribute);
	}


	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2>
	class RisseBinderFunctor3s
	{
		typedef R (*tFunc)(T0,T1,T2);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { NumMandatoryArgs = ArgIndex2+ tRisseIsFuncCallNonMetaType<T2>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info)));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2>
	class RisseBinderFunctor3s<CC, void ,T0,T1,T2>
	{
		typedef void (*tFunc)(T0,T1,T2);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { NumMandatoryArgs = ArgIndex2+ tRisseIsFuncCallNonMetaType<T2>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2>
	class RisseBinderFunctor3
	{
		typedef R (IC::*tFunc)(T0,T1,T2);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { NumMandatoryArgs = ArgIndex2+ tRisseIsFuncCallNonMetaType<T2>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info)));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2>
	class RisseBinderFunctor3<CC, IC, void  ,T0,T1,T2>
	{
		typedef void (IC::*tFunc)(T0,T1,T2);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { NumMandatoryArgs = ArgIndex2+ tRisseIsFuncCallNonMetaType<T2>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (*f)(T0,T1,T2), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindStaticFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&RisseBinderFunctor3s<CC, R ,T0,T1,T2>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1,T2), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&RisseBinderFunctor3<CC, IC, R ,T0,T1,T2>::Call
					), context), attribute);
	}


	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3>
	class RisseBinderFunctor4s
	{
		typedef R (*tFunc)(T0,T1,T2,T3);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { NumMandatoryArgs = ArgIndex3+ tRisseIsFuncCallNonMetaType<T3>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info)));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3>
	class RisseBinderFunctor4s<CC, void ,T0,T1,T2,T3>
	{
		typedef void (*tFunc)(T0,T1,T2,T3);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { NumMandatoryArgs = ArgIndex3+ tRisseIsFuncCallNonMetaType<T3>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3>
	class RisseBinderFunctor4
	{
		typedef R (IC::*tFunc)(T0,T1,T2,T3);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { NumMandatoryArgs = ArgIndex3+ tRisseIsFuncCallNonMetaType<T3>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info)));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3>
	class RisseBinderFunctor4<CC, IC, void  ,T0,T1,T2,T3>
	{
		typedef void (IC::*tFunc)(T0,T1,T2,T3);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { NumMandatoryArgs = ArgIndex3+ tRisseIsFuncCallNonMetaType<T3>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (*f)(T0,T1,T2,T3), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindStaticFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&RisseBinderFunctor4s<CC, R ,T0,T1,T2,T3>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1,T2,T3), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&RisseBinderFunctor4<CC, IC, R ,T0,T1,T2,T3>::Call
					), context), attribute);
	}


	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class RisseBinderFunctor5s
	{
		typedef R (*tFunc)(T0,T1,T2,T3,T4);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { NumMandatoryArgs = ArgIndex4+ tRisseIsFuncCallNonMetaType<T4>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info)));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class RisseBinderFunctor5s<CC, void ,T0,T1,T2,T3,T4>
	{
		typedef void (*tFunc)(T0,T1,T2,T3,T4);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { NumMandatoryArgs = ArgIndex4+ tRisseIsFuncCallNonMetaType<T4>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class RisseBinderFunctor5
	{
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { NumMandatoryArgs = ArgIndex4+ tRisseIsFuncCallNonMetaType<T4>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info)));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class RisseBinderFunctor5<CC, IC, void  ,T0,T1,T2,T3,T4>
	{
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { NumMandatoryArgs = ArgIndex4+ tRisseIsFuncCallNonMetaType<T4>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (*f)(T0,T1,T2,T3,T4), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindStaticFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&RisseBinderFunctor5s<CC, R ,T0,T1,T2,T3,T4>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1,T2,T3,T4), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&RisseBinderFunctor5<CC, IC, R ,T0,T1,T2,T3,T4>::Call
					), context), attribute);
	}


	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class RisseBinderFunctor6s
	{
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { NumMandatoryArgs = ArgIndex5+ tRisseIsFuncCallNonMetaType<T5>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info)));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class RisseBinderFunctor6s<CC, void ,T0,T1,T2,T3,T4,T5>
	{
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { NumMandatoryArgs = ArgIndex5+ tRisseIsFuncCallNonMetaType<T5>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class RisseBinderFunctor6
	{
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { NumMandatoryArgs = ArgIndex5+ tRisseIsFuncCallNonMetaType<T5>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info)));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class RisseBinderFunctor6<CC, IC, void  ,T0,T1,T2,T3,T4,T5>
	{
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { NumMandatoryArgs = ArgIndex5+ tRisseIsFuncCallNonMetaType<T5>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (*f)(T0,T1,T2,T3,T4,T5), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindStaticFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&RisseBinderFunctor6s<CC, R ,T0,T1,T2,T3,T4,T5>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1,T2,T3,T4,T5), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&RisseBinderFunctor6<CC, IC, R ,T0,T1,T2,T3,T4,T5>::Call
					), context), attribute);
	}


	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class RisseBinderFunctor7s
	{
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5,T6);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { NumMandatoryArgs = ArgIndex6+ tRisseIsFuncCallNonMetaType<T6>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info)));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class RisseBinderFunctor7s<CC, void ,T0,T1,T2,T3,T4,T5,T6>
	{
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { NumMandatoryArgs = ArgIndex6+ tRisseIsFuncCallNonMetaType<T6>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class RisseBinderFunctor7
	{
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { NumMandatoryArgs = ArgIndex6+ tRisseIsFuncCallNonMetaType<T6>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info)));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class RisseBinderFunctor7<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6>
	{
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { NumMandatoryArgs = ArgIndex6+ tRisseIsFuncCallNonMetaType<T6>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (*f)(T0,T1,T2,T3,T4,T5,T6), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindStaticFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&RisseBinderFunctor7s<CC, R ,T0,T1,T2,T3,T4,T5,T6>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1,T2,T3,T4,T5,T6), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&RisseBinderFunctor7<CC, IC, R ,T0,T1,T2,T3,T4,T5,T6>::Call
					), context), attribute);
	}


	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class RisseBinderFunctor8s
	{
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { ArgIndex7=ArgIndex6 + tRisseIsFuncCallNonMetaType<T6>::value };
		enum { NumMandatoryArgs = ArgIndex7+ tRisseIsFuncCallNonMetaType<T7>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info)));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class RisseBinderFunctor8s<CC, void ,T0,T1,T2,T3,T4,T5,T6,T7>
	{
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { ArgIndex7=ArgIndex6 + tRisseIsFuncCallNonMetaType<T6>::value };
		enum { NumMandatoryArgs = ArgIndex7+ tRisseIsFuncCallNonMetaType<T7>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class RisseBinderFunctor8
	{
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { ArgIndex7=ArgIndex6 + tRisseIsFuncCallNonMetaType<T6>::value };
		enum { NumMandatoryArgs = ArgIndex7+ tRisseIsFuncCallNonMetaType<T7>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info)));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class RisseBinderFunctor8<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6,T7>
	{
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { ArgIndex7=ArgIndex6 + tRisseIsFuncCallNonMetaType<T6>::value };
		enum { NumMandatoryArgs = ArgIndex7+ tRisseIsFuncCallNonMetaType<T7>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (*f)(T0,T1,T2,T3,T4,T5,T6,T7), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindStaticFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&RisseBinderFunctor8s<CC, R ,T0,T1,T2,T3,T4,T5,T6,T7>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1,T2,T3,T4,T5,T6,T7), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&RisseBinderFunctor8<CC, IC, R ,T0,T1,T2,T3,T4,T5,T6,T7>::Call
					), context), attribute);
	}


	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class RisseBinderFunctor9s
	{
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { ArgIndex7=ArgIndex6 + tRisseIsFuncCallNonMetaType<T6>::value };
		enum { ArgIndex8=ArgIndex7 + tRisseIsFuncCallNonMetaType<T7>::value };
		enum { NumMandatoryArgs = ArgIndex8+ tRisseIsFuncCallNonMetaType<T8>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[ArgIndex8]:tRisseVariant::GetVoidObject(),info)));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[ArgIndex8]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class RisseBinderFunctor9s<CC, void ,T0,T1,T2,T3,T4,T5,T6,T7,T8>
	{
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { ArgIndex7=ArgIndex6 + tRisseIsFuncCallNonMetaType<T6>::value };
		enum { ArgIndex8=ArgIndex7 + tRisseIsFuncCallNonMetaType<T7>::value };
		enum { NumMandatoryArgs = ArgIndex8+ tRisseIsFuncCallNonMetaType<T8>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[ArgIndex8]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class RisseBinderFunctor9
	{
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { ArgIndex7=ArgIndex6 + tRisseIsFuncCallNonMetaType<T6>::value };
		enum { ArgIndex8=ArgIndex7 + tRisseIsFuncCallNonMetaType<T7>::value };
		enum { NumMandatoryArgs = ArgIndex8+ tRisseIsFuncCallNonMetaType<T8>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[ArgIndex8]:tRisseVariant::GetVoidObject(),info)));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[ArgIndex8]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class RisseBinderFunctor9<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6,T7,T8>
	{
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { ArgIndex7=ArgIndex6 + tRisseIsFuncCallNonMetaType<T6>::value };
		enum { ArgIndex8=ArgIndex7 + tRisseIsFuncCallNonMetaType<T7>::value };
		enum { NumMandatoryArgs = ArgIndex8+ tRisseIsFuncCallNonMetaType<T8>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[ArgIndex8]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (*f)(T0,T1,T2,T3,T4,T5,T6,T7,T8), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindStaticFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&RisseBinderFunctor9s<CC, R ,T0,T1,T2,T3,T4,T5,T6,T7,T8>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1,T2,T3,T4,T5,T6,T7,T8), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&RisseBinderFunctor9<CC, IC, R ,T0,T1,T2,T3,T4,T5,T6,T7,T8>::Call
					), context), attribute);
	}


	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class RisseBinderFunctor10s
	{
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { ArgIndex7=ArgIndex6 + tRisseIsFuncCallNonMetaType<T6>::value };
		enum { ArgIndex8=ArgIndex7 + tRisseIsFuncCallNonMetaType<T7>::value };
		enum { ArgIndex9=ArgIndex8 + tRisseIsFuncCallNonMetaType<T8>::value };
		enum { NumMandatoryArgs = ArgIndex9+ tRisseIsFuncCallNonMetaType<T9>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[ArgIndex8]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T9>(tRisseIsFuncCallNonMetaType<T9>::value==1?info.args[ArgIndex9]:tRisseVariant::GetVoidObject(),info)));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[ArgIndex8]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T9>(tRisseIsFuncCallNonMetaType<T9>::value==1?info.args[ArgIndex9]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class RisseBinderFunctor10s<CC, void ,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>
	{
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { ArgIndex7=ArgIndex6 + tRisseIsFuncCallNonMetaType<T6>::value };
		enum { ArgIndex8=ArgIndex7 + tRisseIsFuncCallNonMetaType<T7>::value };
		enum { ArgIndex9=ArgIndex8 + tRisseIsFuncCallNonMetaType<T8>::value };
		enum { NumMandatoryArgs = ArgIndex9+ tRisseIsFuncCallNonMetaType<T9>::value };

	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[ArgIndex8]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T9>(tRisseIsFuncCallNonMetaType<T9>::value==1?info.args[ArgIndex9]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class RisseBinderFunctor10
	{
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { ArgIndex7=ArgIndex6 + tRisseIsFuncCallNonMetaType<T6>::value };
		enum { ArgIndex8=ArgIndex7 + tRisseIsFuncCallNonMetaType<T7>::value };
		enum { ArgIndex9=ArgIndex8 + tRisseIsFuncCallNonMetaType<T8>::value };
		enum { NumMandatoryArgs = ArgIndex9+ tRisseIsFuncCallNonMetaType<T9>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[ArgIndex8]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T9>(tRisseIsFuncCallNonMetaType<T9>::value==1?info.args[ArgIndex9]:tRisseVariant::GetVoidObject(),info)));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[ArgIndex8]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T9>(tRisseIsFuncCallNonMetaType<T9>::value==1?info.args[ArgIndex9]:tRisseVariant::GetVoidObject(),info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class RisseBinderFunctor10<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>
	{
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { ArgIndex7=ArgIndex6 + tRisseIsFuncCallNonMetaType<T6>::value };
		enum { ArgIndex8=ArgIndex7 + tRisseIsFuncCallNonMetaType<T7>::value };
		enum { ArgIndex9=ArgIndex8 + tRisseIsFuncCallNonMetaType<T8>::value };
		enum { NumMandatoryArgs = ArgIndex9+ tRisseIsFuncCallNonMetaType<T9>::value };

	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[ArgIndex8]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T9>(tRisseIsFuncCallNonMetaType<T9>::value==1?info.args[ArgIndex9]:tRisseVariant::GetVoidObject(),info));
			if(info.result) info.result->Clear();
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (*f)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindStaticFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&RisseBinderFunctor10s<CC, R ,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&RisseBinderFunctor10<CC, IC, R ,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::Call
					), context), attribute);
	}


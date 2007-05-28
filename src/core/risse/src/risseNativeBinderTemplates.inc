
// このファイルは create_binder.rb により再生成されます


	// static関数、非 void の戻り値用
	template <typename CC, typename R >
	class RisseBinderFunctor0s
	{
		typedef R (*tFunc)();
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0
			);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)());
			else
				((tFunc)f)();
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC >
	class RisseBinderFunctor0s<CC, void >
	{
		typedef void (*tFunc)();
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0
			);
			((tFunc)f)();
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R >
	class RisseBinderFunctor0
	{
		typedef R (IC::*tFunc)();
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))());
			else
				(instance->*((tFunc)f))();
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC >
	class RisseBinderFunctor0<CC, IC, void  >
	{
		typedef void (IC::*tFunc)();
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))();
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
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info)));
			else
				((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0>
	class RisseBinderFunctor1s<CC, void ,T0>
	{
		typedef void (*tFunc)(T0);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			);
			((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0>
	class RisseBinderFunctor1
	{
		typedef R (IC::*tFunc)(T0);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info)));
			else
				(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0>
	class RisseBinderFunctor1<CC, IC, void  ,T0>
	{
		typedef void (IC::*tFunc)(T0);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info));
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
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info)));
			else
				((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1>
	class RisseBinderFunctor2s<CC, void ,T0,T1>
	{
		typedef void (*tFunc)(T0,T1);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			);
			((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1>
	class RisseBinderFunctor2
	{
		typedef R (IC::*tFunc)(T0,T1);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info)));
			else
				(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1>
	class RisseBinderFunctor2<CC, IC, void  ,T0,T1>
	{
		typedef void (IC::*tFunc)(T0,T1);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info));
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
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info)));
			else
				((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2>
	class RisseBinderFunctor3s<CC, void ,T0,T1,T2>
	{
		typedef void (*tFunc)(T0,T1,T2);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			);
			((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2>
	class RisseBinderFunctor3
	{
		typedef R (IC::*tFunc)(T0,T1,T2);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info)));
			else
				(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2>
	class RisseBinderFunctor3<CC, IC, void  ,T0,T1,T2>
	{
		typedef void (IC::*tFunc)(T0,T1,T2);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info));
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
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info)));
			else
				((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3>
	class RisseBinderFunctor4s<CC, void ,T0,T1,T2,T3>
	{
		typedef void (*tFunc)(T0,T1,T2,T3);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			);
			((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3>
	class RisseBinderFunctor4
	{
		typedef R (IC::*tFunc)(T0,T1,T2,T3);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info)));
			else
				(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3>
	class RisseBinderFunctor4<CC, IC, void  ,T0,T1,T2,T3>
	{
		typedef void (IC::*tFunc)(T0,T1,T2,T3);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info));
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
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info)));
			else
				((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class RisseBinderFunctor5s<CC, void ,T0,T1,T2,T3,T4>
	{
		typedef void (*tFunc)(T0,T1,T2,T3,T4);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			);
			((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class RisseBinderFunctor5
	{
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info)));
			else
				(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class RisseBinderFunctor5<CC, IC, void  ,T0,T1,T2,T3,T4>
	{
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info));
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
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info)));
			else
				((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class RisseBinderFunctor6s<CC, void ,T0,T1,T2,T3,T4,T5>
	{
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			);
			((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class RisseBinderFunctor6
	{
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info)));
			else
				(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class RisseBinderFunctor6<CC, IC, void  ,T0,T1,T2,T3,T4,T5>
	{
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info));
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
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info)));
			else
				((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class RisseBinderFunctor7s<CC, void ,T0,T1,T2,T3,T4,T5,T6>
	{
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			);
			((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class RisseBinderFunctor7
	{
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info)));
			else
				(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class RisseBinderFunctor7<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6>
	{
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info));
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
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value
			);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info)));
			else
				((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class RisseBinderFunctor8s<CC, void ,T0,T1,T2,T3,T4,T5,T6,T7>
	{
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value
			);
			((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class RisseBinderFunctor8
	{
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info)));
			else
				(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class RisseBinderFunctor8<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6,T7>
	{
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info));
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
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value+
				tRisseIsFuncCallNonMetaType<T8>::value
			);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info),RisseFromVariantOrCallingInfo<T8>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value
			],info)));
			else
				((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info),RisseFromVariantOrCallingInfo<T8>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value
			],info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class RisseBinderFunctor9s<CC, void ,T0,T1,T2,T3,T4,T5,T6,T7,T8>
	{
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value+
				tRisseIsFuncCallNonMetaType<T8>::value
			);
			((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info),RisseFromVariantOrCallingInfo<T8>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value
			],info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class RisseBinderFunctor9
	{
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value+
				tRisseIsFuncCallNonMetaType<T8>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info),RisseFromVariantOrCallingInfo<T8>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value
			],info)));
			else
				(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info),RisseFromVariantOrCallingInfo<T8>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value
			],info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class RisseBinderFunctor9<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6,T7,T8>
	{
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value+
				tRisseIsFuncCallNonMetaType<T8>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info),RisseFromVariantOrCallingInfo<T8>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value
			],info));
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
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value+
				tRisseIsFuncCallNonMetaType<T8>::value+
				tRisseIsFuncCallNonMetaType<T9>::value
			);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info),RisseFromVariantOrCallingInfo<T8>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value
			],info),RisseFromVariantOrCallingInfo<T9>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value+
				tRisseIsFuncCallNonMetaType<T8>::value
			],info)));
			else
				((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info),RisseFromVariantOrCallingInfo<T8>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value
			],info),RisseFromVariantOrCallingInfo<T9>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value+
				tRisseIsFuncCallNonMetaType<T8>::value
			],info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class RisseBinderFunctor10s<CC, void ,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>
	{
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value+
				tRisseIsFuncCallNonMetaType<T8>::value+
				tRisseIsFuncCallNonMetaType<T9>::value
			);
			((tFunc)f)(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info),RisseFromVariantOrCallingInfo<T8>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value
			],info),RisseFromVariantOrCallingInfo<T9>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value+
				tRisseIsFuncCallNonMetaType<T8>::value
			],info));
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class RisseBinderFunctor10
	{
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value+
				tRisseIsFuncCallNonMetaType<T8>::value+
				tRisseIsFuncCallNonMetaType<T9>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info),RisseFromVariantOrCallingInfo<T8>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value
			],info),RisseFromVariantOrCallingInfo<T9>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value+
				tRisseIsFuncCallNonMetaType<T8>::value
			],info)));
			else
				(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info),RisseFromVariantOrCallingInfo<T8>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value
			],info),RisseFromVariantOrCallingInfo<T9>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value+
				tRisseIsFuncCallNonMetaType<T8>::value
			],info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class RisseBinderFunctor10<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>
	{
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value+
				tRisseIsFuncCallNonMetaType<T8>::value+
				tRisseIsFuncCallNonMetaType<T9>::value
			);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariantOrCallingInfo<T0>(info.args[
				0
			],info),RisseFromVariantOrCallingInfo<T1>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value
			],info),RisseFromVariantOrCallingInfo<T2>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value
			],info),RisseFromVariantOrCallingInfo<T3>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value
			],info),RisseFromVariantOrCallingInfo<T4>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value
			],info),RisseFromVariantOrCallingInfo<T5>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value
			],info),RisseFromVariantOrCallingInfo<T6>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value
			],info),RisseFromVariantOrCallingInfo<T7>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value
			],info),RisseFromVariantOrCallingInfo<T8>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value
			],info),RisseFromVariantOrCallingInfo<T9>(info.args[
				0+
				tRisseIsFuncCallNonMetaType<T0>::value+
				tRisseIsFuncCallNonMetaType<T1>::value+
				tRisseIsFuncCallNonMetaType<T2>::value+
				tRisseIsFuncCallNonMetaType<T3>::value+
				tRisseIsFuncCallNonMetaType<T4>::value+
				tRisseIsFuncCallNonMetaType<T5>::value+
				tRisseIsFuncCallNonMetaType<T6>::value+
				tRisseIsFuncCallNonMetaType<T7>::value+
				tRisseIsFuncCallNonMetaType<T8>::value
			],info));
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


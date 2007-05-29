
// このファイルは create_binder.rb により再生成されます


	// 定数
	
	struct tRisseBinderConsts0
	{

		enum { NumMandatoryArgs = 0 };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R >
	class tRisseBinderFunctor0s
	{
		typedef tRisseBinderConsts0 Consts;
		typedef R (*tFunc)();
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
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
	class tRisseBinderFunctor0s<CC, void >
	{
		typedef tRisseBinderConsts0 Consts;
		typedef void (*tFunc)();
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result) info.result->Clear();
			((tFunc)f)(

				);
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R >
	class tRisseBinderFunctor0
	{
		typedef tRisseBinderConsts0 Consts;
		typedef R (IC::*tFunc)();
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
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
	class tRisseBinderFunctor0<CC, IC, void  >
	{
		typedef tRisseBinderConsts0 Consts;
		typedef void (IC::*tFunc)();
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result) info.result->Clear();
			(instance->*((tFunc)f))(

				);
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R >
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (*f)(), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tRisseBinderFunctor0s<CC, R >::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R >
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (IC::*f)(), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (tRisseObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&tRisseBinderFunctor0<CC, IC, R >::Call
					), context), attribute);
	}


	// 定数
	template <typename T0>
	struct tRisseBinderConsts1
	{
		enum { ArgIndex0=0 };
		enum { NumMandatoryArgs = ArgIndex0+ tRisseIsFuncCallNonMetaType<T0>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0>
	class tRisseBinderFunctor1s
	{
		typedef tRisseBinderConsts1<T0> Consts;
		typedef R (*tFunc)(T0);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info)
				));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0>
	class tRisseBinderFunctor1s<CC, void ,T0>
	{
		typedef tRisseBinderConsts1<T0> Consts;
		typedef void (*tFunc)(T0);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result) info.result->Clear();
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0>
	class tRisseBinderFunctor1
	{
		typedef tRisseBinderConsts1<T0> Consts;
		typedef R (IC::*tFunc)(T0);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info)
				));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0>
	class tRisseBinderFunctor1<CC, IC, void  ,T0>
	{
		typedef tRisseBinderConsts1<T0> Consts;
		typedef void (IC::*tFunc)(T0);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result) info.result->Clear();
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (*f)(T0), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tRisseBinderFunctor1s<CC, R ,T0>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (IC::*f)(T0), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (tRisseObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&tRisseBinderFunctor1<CC, IC, R ,T0>::Call
					), context), attribute);
	}


	// 定数
	template <typename T0,typename T1>
	struct tRisseBinderConsts2
	{
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { NumMandatoryArgs = ArgIndex1+ tRisseIsFuncCallNonMetaType<T1>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1>
	class tRisseBinderFunctor2s
	{
		typedef tRisseBinderConsts2<T0,T1> Consts;
		typedef R (*tFunc)(T0,T1);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info)
				));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1>
	class tRisseBinderFunctor2s<CC, void ,T0,T1>
	{
		typedef tRisseBinderConsts2<T0,T1> Consts;
		typedef void (*tFunc)(T0,T1);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result) info.result->Clear();
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1>
	class tRisseBinderFunctor2
	{
		typedef tRisseBinderConsts2<T0,T1> Consts;
		typedef R (IC::*tFunc)(T0,T1);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info)
				));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1>
	class tRisseBinderFunctor2<CC, IC, void  ,T0,T1>
	{
		typedef tRisseBinderConsts2<T0,T1> Consts;
		typedef void (IC::*tFunc)(T0,T1);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result) info.result->Clear();
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (*f)(T0,T1), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tRisseBinderFunctor2s<CC, R ,T0,T1>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (tRisseObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&tRisseBinderFunctor2<CC, IC, R ,T0,T1>::Call
					), context), attribute);
	}


	// 定数
	template <typename T0,typename T1,typename T2>
	struct tRisseBinderConsts3
	{
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { NumMandatoryArgs = ArgIndex2+ tRisseIsFuncCallNonMetaType<T2>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2>
	class tRisseBinderFunctor3s
	{
		typedef tRisseBinderConsts3<T0,T1,T2> Consts;
		typedef R (*tFunc)(T0,T1,T2);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info)
				));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2>
	class tRisseBinderFunctor3s<CC, void ,T0,T1,T2>
	{
		typedef tRisseBinderConsts3<T0,T1,T2> Consts;
		typedef void (*tFunc)(T0,T1,T2);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result) info.result->Clear();
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2>
	class tRisseBinderFunctor3
	{
		typedef tRisseBinderConsts3<T0,T1,T2> Consts;
		typedef R (IC::*tFunc)(T0,T1,T2);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info)
				));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2>
	class tRisseBinderFunctor3<CC, IC, void  ,T0,T1,T2>
	{
		typedef tRisseBinderConsts3<T0,T1,T2> Consts;
		typedef void (IC::*tFunc)(T0,T1,T2);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result) info.result->Clear();
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (*f)(T0,T1,T2), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tRisseBinderFunctor3s<CC, R ,T0,T1,T2>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1,T2), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (tRisseObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&tRisseBinderFunctor3<CC, IC, R ,T0,T1,T2>::Call
					), context), attribute);
	}


	// 定数
	template <typename T0,typename T1,typename T2,typename T3>
	struct tRisseBinderConsts4
	{
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { NumMandatoryArgs = ArgIndex3+ tRisseIsFuncCallNonMetaType<T3>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3>
	class tRisseBinderFunctor4s
	{
		typedef tRisseBinderConsts4<T0,T1,T2,T3> Consts;
		typedef R (*tFunc)(T0,T1,T2,T3);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info)
				));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3>
	class tRisseBinderFunctor4s<CC, void ,T0,T1,T2,T3>
	{
		typedef tRisseBinderConsts4<T0,T1,T2,T3> Consts;
		typedef void (*tFunc)(T0,T1,T2,T3);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result) info.result->Clear();
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3>
	class tRisseBinderFunctor4
	{
		typedef tRisseBinderConsts4<T0,T1,T2,T3> Consts;
		typedef R (IC::*tFunc)(T0,T1,T2,T3);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info)
				));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3>
	class tRisseBinderFunctor4<CC, IC, void  ,T0,T1,T2,T3>
	{
		typedef tRisseBinderConsts4<T0,T1,T2,T3> Consts;
		typedef void (IC::*tFunc)(T0,T1,T2,T3);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result) info.result->Clear();
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (*f)(T0,T1,T2,T3), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tRisseBinderFunctor4s<CC, R ,T0,T1,T2,T3>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1,T2,T3), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (tRisseObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&tRisseBinderFunctor4<CC, IC, R ,T0,T1,T2,T3>::Call
					), context), attribute);
	}


	// 定数
	template <typename T0,typename T1,typename T2,typename T3,typename T4>
	struct tRisseBinderConsts5
	{
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { NumMandatoryArgs = ArgIndex4+ tRisseIsFuncCallNonMetaType<T4>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class tRisseBinderFunctor5s
	{
		typedef tRisseBinderConsts5<T0,T1,T2,T3,T4> Consts;
		typedef R (*tFunc)(T0,T1,T2,T3,T4);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info)
				));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class tRisseBinderFunctor5s<CC, void ,T0,T1,T2,T3,T4>
	{
		typedef tRisseBinderConsts5<T0,T1,T2,T3,T4> Consts;
		typedef void (*tFunc)(T0,T1,T2,T3,T4);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result) info.result->Clear();
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class tRisseBinderFunctor5
	{
		typedef tRisseBinderConsts5<T0,T1,T2,T3,T4> Consts;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info)
				));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class tRisseBinderFunctor5<CC, IC, void  ,T0,T1,T2,T3,T4>
	{
		typedef tRisseBinderConsts5<T0,T1,T2,T3,T4> Consts;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result) info.result->Clear();
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (*f)(T0,T1,T2,T3,T4), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tRisseBinderFunctor5s<CC, R ,T0,T1,T2,T3,T4>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1,T2,T3,T4), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (tRisseObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&tRisseBinderFunctor5<CC, IC, R ,T0,T1,T2,T3,T4>::Call
					), context), attribute);
	}


	// 定数
	template <typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	struct tRisseBinderConsts6
	{
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { NumMandatoryArgs = ArgIndex5+ tRisseIsFuncCallNonMetaType<T5>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class tRisseBinderFunctor6s
	{
		typedef tRisseBinderConsts6<T0,T1,T2,T3,T4,T5> Consts;
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info)
				));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class tRisseBinderFunctor6s<CC, void ,T0,T1,T2,T3,T4,T5>
	{
		typedef tRisseBinderConsts6<T0,T1,T2,T3,T4,T5> Consts;
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result) info.result->Clear();
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class tRisseBinderFunctor6
	{
		typedef tRisseBinderConsts6<T0,T1,T2,T3,T4,T5> Consts;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info)
				));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class tRisseBinderFunctor6<CC, IC, void  ,T0,T1,T2,T3,T4,T5>
	{
		typedef tRisseBinderConsts6<T0,T1,T2,T3,T4,T5> Consts;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result) info.result->Clear();
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (*f)(T0,T1,T2,T3,T4,T5), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tRisseBinderFunctor6s<CC, R ,T0,T1,T2,T3,T4,T5>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1,T2,T3,T4,T5), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (tRisseObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&tRisseBinderFunctor6<CC, IC, R ,T0,T1,T2,T3,T4,T5>::Call
					), context), attribute);
	}


	// 定数
	template <typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	struct tRisseBinderConsts7
	{
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { NumMandatoryArgs = ArgIndex6+ tRisseIsFuncCallNonMetaType<T6>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class tRisseBinderFunctor7s
	{
		typedef tRisseBinderConsts7<T0,T1,T2,T3,T4,T5,T6> Consts;
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5,T6);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info)
				));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class tRisseBinderFunctor7s<CC, void ,T0,T1,T2,T3,T4,T5,T6>
	{
		typedef tRisseBinderConsts7<T0,T1,T2,T3,T4,T5,T6> Consts;
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result) info.result->Clear();
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class tRisseBinderFunctor7
	{
		typedef tRisseBinderConsts7<T0,T1,T2,T3,T4,T5,T6> Consts;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info)
				));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class tRisseBinderFunctor7<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6>
	{
		typedef tRisseBinderConsts7<T0,T1,T2,T3,T4,T5,T6> Consts;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result) info.result->Clear();
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (*f)(T0,T1,T2,T3,T4,T5,T6), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tRisseBinderFunctor7s<CC, R ,T0,T1,T2,T3,T4,T5,T6>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1,T2,T3,T4,T5,T6), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (tRisseObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&tRisseBinderFunctor7<CC, IC, R ,T0,T1,T2,T3,T4,T5,T6>::Call
					), context), attribute);
	}


	// 定数
	template <typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	struct tRisseBinderConsts8
	{
		enum { ArgIndex0=0 };
		enum { ArgIndex1=ArgIndex0 + tRisseIsFuncCallNonMetaType<T0>::value };
		enum { ArgIndex2=ArgIndex1 + tRisseIsFuncCallNonMetaType<T1>::value };
		enum { ArgIndex3=ArgIndex2 + tRisseIsFuncCallNonMetaType<T2>::value };
		enum { ArgIndex4=ArgIndex3 + tRisseIsFuncCallNonMetaType<T3>::value };
		enum { ArgIndex5=ArgIndex4 + tRisseIsFuncCallNonMetaType<T4>::value };
		enum { ArgIndex6=ArgIndex5 + tRisseIsFuncCallNonMetaType<T5>::value };
		enum { ArgIndex7=ArgIndex6 + tRisseIsFuncCallNonMetaType<T6>::value };
		enum { NumMandatoryArgs = ArgIndex7+ tRisseIsFuncCallNonMetaType<T7>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class tRisseBinderFunctor8s
	{
		typedef tRisseBinderConsts8<T0,T1,T2,T3,T4,T5,T6,T7> Consts;
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info)
				));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class tRisseBinderFunctor8s<CC, void ,T0,T1,T2,T3,T4,T5,T6,T7>
	{
		typedef tRisseBinderConsts8<T0,T1,T2,T3,T4,T5,T6,T7> Consts;
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result) info.result->Clear();
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class tRisseBinderFunctor8
	{
		typedef tRisseBinderConsts8<T0,T1,T2,T3,T4,T5,T6,T7> Consts;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info)
				));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class tRisseBinderFunctor8<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6,T7>
	{
		typedef tRisseBinderConsts8<T0,T1,T2,T3,T4,T5,T6,T7> Consts;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result) info.result->Clear();
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (*f)(T0,T1,T2,T3,T4,T5,T6,T7), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tRisseBinderFunctor8s<CC, R ,T0,T1,T2,T3,T4,T5,T6,T7>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1,T2,T3,T4,T5,T6,T7), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (tRisseObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&tRisseBinderFunctor8<CC, IC, R ,T0,T1,T2,T3,T4,T5,T6,T7>::Call
					), context), attribute);
	}


	// 定数
	template <typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	struct tRisseBinderConsts9
	{
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

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class tRisseBinderFunctor9s
	{
		typedef tRisseBinderConsts9<T0,T1,T2,T3,T4,T5,T6,T7,T8> Consts;
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[Consts::ArgIndex8]:tRisseVariant::GetVoidObject(),info)
				));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[Consts::ArgIndex8]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class tRisseBinderFunctor9s<CC, void ,T0,T1,T2,T3,T4,T5,T6,T7,T8>
	{
		typedef tRisseBinderConsts9<T0,T1,T2,T3,T4,T5,T6,T7,T8> Consts;
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result) info.result->Clear();
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[Consts::ArgIndex8]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class tRisseBinderFunctor9
	{
		typedef tRisseBinderConsts9<T0,T1,T2,T3,T4,T5,T6,T7,T8> Consts;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[Consts::ArgIndex8]:tRisseVariant::GetVoidObject(),info)
				));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[Consts::ArgIndex8]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class tRisseBinderFunctor9<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6,T7,T8>
	{
		typedef tRisseBinderConsts9<T0,T1,T2,T3,T4,T5,T6,T7,T8> Consts;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result) info.result->Clear();
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[Consts::ArgIndex8]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (*f)(T0,T1,T2,T3,T4,T5,T6,T7,T8), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tRisseBinderFunctor9s<CC, R ,T0,T1,T2,T3,T4,T5,T6,T7,T8>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1,T2,T3,T4,T5,T6,T7,T8), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (tRisseObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&tRisseBinderFunctor9<CC, IC, R ,T0,T1,T2,T3,T4,T5,T6,T7,T8>::Call
					), context), attribute);
	}


	// 定数
	template <typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	struct tRisseBinderConsts10
	{
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

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class tRisseBinderFunctor10s
	{
		typedef tRisseBinderConsts10<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9> Consts;
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[Consts::ArgIndex8]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T9>(tRisseIsFuncCallNonMetaType<T9>::value==1?info.args[Consts::ArgIndex9]:tRisseVariant::GetVoidObject(),info)
				));
			else
				((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[Consts::ArgIndex8]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T9>(tRisseIsFuncCallNonMetaType<T9>::value==1?info.args[Consts::ArgIndex9]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class tRisseBinderFunctor10s<CC, void ,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>
	{
		typedef tRisseBinderConsts10<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9> Consts;
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			if(info.result) info.result->Clear();
			((tFunc)f)(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[Consts::ArgIndex8]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T9>(tRisseIsFuncCallNonMetaType<T9>::value==1?info.args[Consts::ArgIndex9]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class tRisseBinderFunctor10
	{
		typedef tRisseBinderConsts10<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9> Consts;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[Consts::ArgIndex8]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T9>(tRisseIsFuncCallNonMetaType<T9>::value==1?info.args[Consts::ArgIndex9]:tRisseVariant::GetVoidObject(),info)
				));
			else
				(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[Consts::ArgIndex8]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T9>(tRisseIsFuncCallNonMetaType<T9>::value==1?info.args[Consts::ArgIndex9]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class tRisseBinderFunctor10<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>
	{
		typedef tRisseBinderConsts10<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9> Consts;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(Consts::NumMandatoryArgs);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result) info.result->Clear();
			(instance->*((tFunc)f))(
						RisseFromVariantOrCallingInfo<T0>(tRisseIsFuncCallNonMetaType<T0>::value==1?info.args[Consts::ArgIndex0]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T1>(tRisseIsFuncCallNonMetaType<T1>::value==1?info.args[Consts::ArgIndex1]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T2>(tRisseIsFuncCallNonMetaType<T2>::value==1?info.args[Consts::ArgIndex2]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T3>(tRisseIsFuncCallNonMetaType<T3>::value==1?info.args[Consts::ArgIndex3]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T4>(tRisseIsFuncCallNonMetaType<T4>::value==1?info.args[Consts::ArgIndex4]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T5>(tRisseIsFuncCallNonMetaType<T5>::value==1?info.args[Consts::ArgIndex5]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T6>(tRisseIsFuncCallNonMetaType<T6>::value==1?info.args[Consts::ArgIndex6]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T7>(tRisseIsFuncCallNonMetaType<T7>::value==1?info.args[Consts::ArgIndex7]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T8>(tRisseIsFuncCallNonMetaType<T8>::value==1?info.args[Consts::ArgIndex8]:tRisseVariant::GetVoidObject(),info),
						RisseFromVariantOrCallingInfo<T9>(tRisseIsFuncCallNonMetaType<T9>::value==1?info.args[Consts::ArgIndex9]:tRisseVariant::GetVoidObject(),info)
				);
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (*f)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tRisseBinderFunctor10s<CC, R ,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (IC::*f)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (tRisseObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&tRisseBinderFunctor10<CC, IC, R ,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::Call
					), context), attribute);
	}


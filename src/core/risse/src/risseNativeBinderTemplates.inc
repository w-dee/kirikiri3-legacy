
// このファイルは create_binder.rb により再生成されます



	// static関数、非 void の戻り値用
	template <typename CC, typename R >
	class RisseBinderFunctor0s
	{
		static const size_t NumArgs = 0;
		typedef R (*tFunc)();
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			if(result)
				*result = RisseToVariant(((tFunc)f)());
			else
				((tFunc)f)();
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC >
	class RisseBinderFunctor0s<CC, void >
	{
		static const size_t NumArgs = 0;
		typedef void (*tFunc)();
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			((tFunc)f)();
			if(result) result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R >
	class RisseBinderFunctor0
	{
		static const size_t NumArgs = 0;
		typedef R (IC::*tFunc)();
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(result)
				*result = RisseToVariant((instance->*((tFunc)f))());
			else
				(instance->*((tFunc)f))();
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC >
	class RisseBinderFunctor0<CC, IC, void  >
	{
		static const size_t NumArgs = 0;
		typedef void (IC::*tFunc)();
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))();
			if(result) result->Clear();
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
		static const size_t NumArgs = 1;
		typedef R (*tFunc)(T0);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			if(result)
				*result = RisseToVariant(((tFunc)f)(RisseFromVariant<T0>(args[0])));
			else
				((tFunc)f)(RisseFromVariant<T0>(args[0]));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0>
	class RisseBinderFunctor1s<CC, void ,T0>
	{
		static const size_t NumArgs = 1;
		typedef void (*tFunc)(T0);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			((tFunc)f)(RisseFromVariant<T0>(args[0]));
			if(result) result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0>
	class RisseBinderFunctor1
	{
		static const size_t NumArgs = 1;
		typedef R (IC::*tFunc)(T0);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(result)
				*result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariant<T0>(args[0])));
			else
				(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0>
	class RisseBinderFunctor1<CC, IC, void  ,T0>
	{
		static const size_t NumArgs = 1;
		typedef void (IC::*tFunc)(T0);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]));
			if(result) result->Clear();
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
		static const size_t NumArgs = 2;
		typedef R (*tFunc)(T0,T1);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			if(result)
				*result = RisseToVariant(((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1])));
			else
				((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1>
	class RisseBinderFunctor2s<CC, void ,T0,T1>
	{
		static const size_t NumArgs = 2;
		typedef void (*tFunc)(T0,T1);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]));
			if(result) result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1>
	class RisseBinderFunctor2
	{
		static const size_t NumArgs = 2;
		typedef R (IC::*tFunc)(T0,T1);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(result)
				*result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1])));
			else
				(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1>
	class RisseBinderFunctor2<CC, IC, void  ,T0,T1>
	{
		static const size_t NumArgs = 2;
		typedef void (IC::*tFunc)(T0,T1);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]));
			if(result) result->Clear();
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
		static const size_t NumArgs = 3;
		typedef R (*tFunc)(T0,T1,T2);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			if(result)
				*result = RisseToVariant(((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2])));
			else
				((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2>
	class RisseBinderFunctor3s<CC, void ,T0,T1,T2>
	{
		static const size_t NumArgs = 3;
		typedef void (*tFunc)(T0,T1,T2);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]));
			if(result) result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2>
	class RisseBinderFunctor3
	{
		static const size_t NumArgs = 3;
		typedef R (IC::*tFunc)(T0,T1,T2);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(result)
				*result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2])));
			else
				(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2>
	class RisseBinderFunctor3<CC, IC, void  ,T0,T1,T2>
	{
		static const size_t NumArgs = 3;
		typedef void (IC::*tFunc)(T0,T1,T2);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]));
			if(result) result->Clear();
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
		static const size_t NumArgs = 4;
		typedef R (*tFunc)(T0,T1,T2,T3);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			if(result)
				*result = RisseToVariant(((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3])));
			else
				((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3>
	class RisseBinderFunctor4s<CC, void ,T0,T1,T2,T3>
	{
		static const size_t NumArgs = 4;
		typedef void (*tFunc)(T0,T1,T2,T3);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]));
			if(result) result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3>
	class RisseBinderFunctor4
	{
		static const size_t NumArgs = 4;
		typedef R (IC::*tFunc)(T0,T1,T2,T3);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(result)
				*result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3])));
			else
				(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3>
	class RisseBinderFunctor4<CC, IC, void  ,T0,T1,T2,T3>
	{
		static const size_t NumArgs = 4;
		typedef void (IC::*tFunc)(T0,T1,T2,T3);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]));
			if(result) result->Clear();
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
		static const size_t NumArgs = 5;
		typedef R (*tFunc)(T0,T1,T2,T3,T4);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			if(result)
				*result = RisseToVariant(((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4])));
			else
				((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class RisseBinderFunctor5s<CC, void ,T0,T1,T2,T3,T4>
	{
		static const size_t NumArgs = 5;
		typedef void (*tFunc)(T0,T1,T2,T3,T4);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]));
			if(result) result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class RisseBinderFunctor5
	{
		static const size_t NumArgs = 5;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(result)
				*result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4])));
			else
				(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class RisseBinderFunctor5<CC, IC, void  ,T0,T1,T2,T3,T4>
	{
		static const size_t NumArgs = 5;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]));
			if(result) result->Clear();
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
		static const size_t NumArgs = 6;
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			if(result)
				*result = RisseToVariant(((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5])));
			else
				((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class RisseBinderFunctor6s<CC, void ,T0,T1,T2,T3,T4,T5>
	{
		static const size_t NumArgs = 6;
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]));
			if(result) result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class RisseBinderFunctor6
	{
		static const size_t NumArgs = 6;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(result)
				*result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5])));
			else
				(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class RisseBinderFunctor6<CC, IC, void  ,T0,T1,T2,T3,T4,T5>
	{
		static const size_t NumArgs = 6;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]));
			if(result) result->Clear();
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
		static const size_t NumArgs = 7;
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5,T6);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			if(result)
				*result = RisseToVariant(((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6])));
			else
				((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class RisseBinderFunctor7s<CC, void ,T0,T1,T2,T3,T4,T5,T6>
	{
		static const size_t NumArgs = 7;
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]));
			if(result) result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class RisseBinderFunctor7
	{
		static const size_t NumArgs = 7;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(result)
				*result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6])));
			else
				(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class RisseBinderFunctor7<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6>
	{
		static const size_t NumArgs = 7;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]));
			if(result) result->Clear();
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
		static const size_t NumArgs = 8;
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			if(result)
				*result = RisseToVariant(((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7])));
			else
				((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7]));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class RisseBinderFunctor8s<CC, void ,T0,T1,T2,T3,T4,T5,T6,T7>
	{
		static const size_t NumArgs = 8;
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7]));
			if(result) result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class RisseBinderFunctor8
	{
		static const size_t NumArgs = 8;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(result)
				*result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7])));
			else
				(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7]));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class RisseBinderFunctor8<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6,T7>
	{
		static const size_t NumArgs = 8;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7]));
			if(result) result->Clear();
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
		static const size_t NumArgs = 9;
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			if(result)
				*result = RisseToVariant(((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7]),RisseFromVariant<T8>(args[8])));
			else
				((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7]),RisseFromVariant<T8>(args[8]));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class RisseBinderFunctor9s<CC, void ,T0,T1,T2,T3,T4,T5,T6,T7,T8>
	{
		static const size_t NumArgs = 9;
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7]),RisseFromVariant<T8>(args[8]));
			if(result) result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class RisseBinderFunctor9
	{
		static const size_t NumArgs = 9;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(result)
				*result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7]),RisseFromVariant<T8>(args[8])));
			else
				(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7]),RisseFromVariant<T8>(args[8]));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class RisseBinderFunctor9<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6,T7,T8>
	{
		static const size_t NumArgs = 9;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7]),RisseFromVariant<T8>(args[8]));
			if(result) result->Clear();
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
		static const size_t NumArgs = 10;
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			if(result)
				*result = RisseToVariant(((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7]),RisseFromVariant<T8>(args[8]),RisseFromVariant<T9>(args[9])));
			else
				((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7]),RisseFromVariant<T8>(args[8]),RisseFromVariant<T9>(args[9]));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class RisseBinderFunctor10s<CC, void ,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>
	{
		static const size_t NumArgs = 10;
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			((tFunc)f)(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7]),RisseFromVariant<T8>(args[8]),RisseFromVariant<T9>(args[9]));
			if(result) result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class RisseBinderFunctor10
	{
		static const size_t NumArgs = 10;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(result)
				*result = RisseToVariant((instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7]),RisseFromVariant<T8>(args[8]),RisseFromVariant<T9>(args[9])));
			else
				(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7]),RisseFromVariant<T8>(args[8]),RisseFromVariant<T9>(args[9]));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class RisseBinderFunctor10<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>
	{
		static const size_t NumArgs = 10;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(RisseFromVariant<T0>(args[0]),RisseFromVariant<T1>(args[1]),RisseFromVariant<T2>(args[2]),RisseFromVariant<T3>(args[3]),RisseFromVariant<T4>(args[4]),RisseFromVariant<T5>(args[5]),RisseFromVariant<T6>(args[6]),RisseFromVariant<T7>(args[7]),RisseFromVariant<T8>(args[8]),RisseFromVariant<T9>(args[9]));
			if(result) result->Clear();
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


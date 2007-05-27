
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


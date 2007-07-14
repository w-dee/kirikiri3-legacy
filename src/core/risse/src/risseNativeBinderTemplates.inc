
// このファイルは create_binder.rb により再生成されます


	// 定数
	
	struct tBinderConsts0
	{

		enum { Man = 0 };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R >
	class tBinderFunctor0s
	{
		typedef tBinderConsts0 Cs;
		typedef R (*tFunc)();
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result)
				*info.result = ToVariant(((tFunc)f)());
			else
				((tFunc)f)();
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC >
	class tBinderFunctor0s<CC, void >
	{
		typedef tBinderConsts0 Cs;
		typedef void (*tFunc)();
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result) *info.result = info.This;
			((tFunc)f)();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R >
	class tBinderFunctor0
	{
		typedef tBinderConsts0 Cs;
		typedef R (IC::*tFunc)();
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result)
				*info.result = ToVariant((instance->*((tFunc)f))());
			else
				(instance->*((tFunc)f))();
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC >
	class tBinderFunctor0<CC, IC, void  >
	{
		typedef tBinderConsts0 Cs;
		typedef void (IC::*tFunc)();
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result) *info.result = info.This;
			(instance->*((tFunc)f))();
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R >
	void BindFunction(CC * _class, const tString & name,
		R (*f)(), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tBinderFunctor0s<CC, R >::Call
					), context), attribute, is_members);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R >
	void BindFunction(CC * _class, const tString & name,
		R (IC::*f)(), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (tObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (tObjectBase::*)()>(f),
				&tBinderFunctor0<CC, IC, R >::Call
					), context), attribute, is_members);
	}


	// 定数
	template <typename T0>
	struct tBinderConsts1
	{
		enum { AI0=0 };
		enum { Man = AI0+ tIsFuncCallNonMetaType<T0>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0>
	class tBinderFunctor1s
	{
		typedef tBinderConsts1<T0> Cs;
		typedef R (*tFunc)(T0);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result)
				*info.result = ToVariant(((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info)));
			else
				((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0>
	class tBinderFunctor1s<CC, void ,T0>
	{
		typedef tBinderConsts1<T0> Cs;
		typedef void (*tFunc)(T0);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result) *info.result = info.This;
			((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info));
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0>
	class tBinderFunctor1
	{
		typedef tBinderConsts1<T0> Cs;
		typedef R (IC::*tFunc)(T0);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result)
				*info.result = ToVariant((instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info)));
			else
				(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0>
	class tBinderFunctor1<CC, IC, void  ,T0>
	{
		typedef tBinderConsts1<T0> Cs;
		typedef void (IC::*tFunc)(T0);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result) *info.result = info.This;
			(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info));
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0>
	void BindFunction(CC * _class, const tString & name,
		R (*f)(T0), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tBinderFunctor1s<CC, R ,T0>::Call
					), context), attribute, is_members);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0>
	void BindFunction(CC * _class, const tString & name,
		R (IC::*f)(T0), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (tObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (tObjectBase::*)()>(f),
				&tBinderFunctor1<CC, IC, R ,T0>::Call
					), context), attribute, is_members);
	}


	// 定数
	template <typename T0,typename T1>
	struct tBinderConsts2
	{
		enum { AI0=0 };
		enum { AI1=AI0 + tIsFuncCallNonMetaType<T0>::value };
		enum { Man = AI1+ tIsFuncCallNonMetaType<T1>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1>
	class tBinderFunctor2s
	{
		typedef tBinderConsts2<T0,T1> Cs;
		typedef R (*tFunc)(T0,T1);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result)
				*info.result = ToVariant(((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info)));
			else
				((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1>
	class tBinderFunctor2s<CC, void ,T0,T1>
	{
		typedef tBinderConsts2<T0,T1> Cs;
		typedef void (*tFunc)(T0,T1);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result) *info.result = info.This;
			((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info));
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1>
	class tBinderFunctor2
	{
		typedef tBinderConsts2<T0,T1> Cs;
		typedef R (IC::*tFunc)(T0,T1);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result)
				*info.result = ToVariant((instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info)));
			else
				(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1>
	class tBinderFunctor2<CC, IC, void  ,T0,T1>
	{
		typedef tBinderConsts2<T0,T1> Cs;
		typedef void (IC::*tFunc)(T0,T1);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result) *info.result = info.This;
			(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info));
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1>
	void BindFunction(CC * _class, const tString & name,
		R (*f)(T0,T1), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tBinderFunctor2s<CC, R ,T0,T1>::Call
					), context), attribute, is_members);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1>
	void BindFunction(CC * _class, const tString & name,
		R (IC::*f)(T0,T1), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (tObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (tObjectBase::*)()>(f),
				&tBinderFunctor2<CC, IC, R ,T0,T1>::Call
					), context), attribute, is_members);
	}


	// 定数
	template <typename T0,typename T1,typename T2>
	struct tBinderConsts3
	{
		enum { AI0=0 };
		enum { AI1=AI0 + tIsFuncCallNonMetaType<T0>::value };
		enum { AI2=AI1 + tIsFuncCallNonMetaType<T1>::value };
		enum { Man = AI2+ tIsFuncCallNonMetaType<T2>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2>
	class tBinderFunctor3s
	{
		typedef tBinderConsts3<T0,T1,T2> Cs;
		typedef R (*tFunc)(T0,T1,T2);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result)
				*info.result = ToVariant(((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info)));
			else
				((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2>
	class tBinderFunctor3s<CC, void ,T0,T1,T2>
	{
		typedef tBinderConsts3<T0,T1,T2> Cs;
		typedef void (*tFunc)(T0,T1,T2);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result) *info.result = info.This;
			((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info));
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2>
	class tBinderFunctor3
	{
		typedef tBinderConsts3<T0,T1,T2> Cs;
		typedef R (IC::*tFunc)(T0,T1,T2);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result)
				*info.result = ToVariant((instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info)));
			else
				(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2>
	class tBinderFunctor3<CC, IC, void  ,T0,T1,T2>
	{
		typedef tBinderConsts3<T0,T1,T2> Cs;
		typedef void (IC::*tFunc)(T0,T1,T2);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result) *info.result = info.This;
			(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info));
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2>
	void BindFunction(CC * _class, const tString & name,
		R (*f)(T0,T1,T2), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tBinderFunctor3s<CC, R ,T0,T1,T2>::Call
					), context), attribute, is_members);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2>
	void BindFunction(CC * _class, const tString & name,
		R (IC::*f)(T0,T1,T2), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (tObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (tObjectBase::*)()>(f),
				&tBinderFunctor3<CC, IC, R ,T0,T1,T2>::Call
					), context), attribute, is_members);
	}


	// 定数
	template <typename T0,typename T1,typename T2,typename T3>
	struct tBinderConsts4
	{
		enum { AI0=0 };
		enum { AI1=AI0 + tIsFuncCallNonMetaType<T0>::value };
		enum { AI2=AI1 + tIsFuncCallNonMetaType<T1>::value };
		enum { AI3=AI2 + tIsFuncCallNonMetaType<T2>::value };
		enum { Man = AI3+ tIsFuncCallNonMetaType<T3>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3>
	class tBinderFunctor4s
	{
		typedef tBinderConsts4<T0,T1,T2,T3> Cs;
		typedef R (*tFunc)(T0,T1,T2,T3);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result)
				*info.result = ToVariant(((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info)));
			else
				((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3>
	class tBinderFunctor4s<CC, void ,T0,T1,T2,T3>
	{
		typedef tBinderConsts4<T0,T1,T2,T3> Cs;
		typedef void (*tFunc)(T0,T1,T2,T3);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result) *info.result = info.This;
			((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info));
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3>
	class tBinderFunctor4
	{
		typedef tBinderConsts4<T0,T1,T2,T3> Cs;
		typedef R (IC::*tFunc)(T0,T1,T2,T3);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result)
				*info.result = ToVariant((instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info)));
			else
				(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3>
	class tBinderFunctor4<CC, IC, void  ,T0,T1,T2,T3>
	{
		typedef tBinderConsts4<T0,T1,T2,T3> Cs;
		typedef void (IC::*tFunc)(T0,T1,T2,T3);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result) *info.result = info.This;
			(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info));
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3>
	void BindFunction(CC * _class, const tString & name,
		R (*f)(T0,T1,T2,T3), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tBinderFunctor4s<CC, R ,T0,T1,T2,T3>::Call
					), context), attribute, is_members);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3>
	void BindFunction(CC * _class, const tString & name,
		R (IC::*f)(T0,T1,T2,T3), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (tObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (tObjectBase::*)()>(f),
				&tBinderFunctor4<CC, IC, R ,T0,T1,T2,T3>::Call
					), context), attribute, is_members);
	}


	// 定数
	template <typename T0,typename T1,typename T2,typename T3,typename T4>
	struct tBinderConsts5
	{
		enum { AI0=0 };
		enum { AI1=AI0 + tIsFuncCallNonMetaType<T0>::value };
		enum { AI2=AI1 + tIsFuncCallNonMetaType<T1>::value };
		enum { AI3=AI2 + tIsFuncCallNonMetaType<T2>::value };
		enum { AI4=AI3 + tIsFuncCallNonMetaType<T3>::value };
		enum { Man = AI4+ tIsFuncCallNonMetaType<T4>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class tBinderFunctor5s
	{
		typedef tBinderConsts5<T0,T1,T2,T3,T4> Cs;
		typedef R (*tFunc)(T0,T1,T2,T3,T4);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result)
				*info.result = ToVariant(((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info)));
			else
				((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class tBinderFunctor5s<CC, void ,T0,T1,T2,T3,T4>
	{
		typedef tBinderConsts5<T0,T1,T2,T3,T4> Cs;
		typedef void (*tFunc)(T0,T1,T2,T3,T4);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result) *info.result = info.This;
			((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info));
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class tBinderFunctor5
	{
		typedef tBinderConsts5<T0,T1,T2,T3,T4> Cs;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result)
				*info.result = ToVariant((instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info)));
			else
				(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4>
	class tBinderFunctor5<CC, IC, void  ,T0,T1,T2,T3,T4>
	{
		typedef tBinderConsts5<T0,T1,T2,T3,T4> Cs;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result) *info.result = info.This;
			(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info));
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4>
	void BindFunction(CC * _class, const tString & name,
		R (*f)(T0,T1,T2,T3,T4), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tBinderFunctor5s<CC, R ,T0,T1,T2,T3,T4>::Call
					), context), attribute, is_members);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4>
	void BindFunction(CC * _class, const tString & name,
		R (IC::*f)(T0,T1,T2,T3,T4), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (tObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (tObjectBase::*)()>(f),
				&tBinderFunctor5<CC, IC, R ,T0,T1,T2,T3,T4>::Call
					), context), attribute, is_members);
	}


	// 定数
	template <typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	struct tBinderConsts6
	{
		enum { AI0=0 };
		enum { AI1=AI0 + tIsFuncCallNonMetaType<T0>::value };
		enum { AI2=AI1 + tIsFuncCallNonMetaType<T1>::value };
		enum { AI3=AI2 + tIsFuncCallNonMetaType<T2>::value };
		enum { AI4=AI3 + tIsFuncCallNonMetaType<T3>::value };
		enum { AI5=AI4 + tIsFuncCallNonMetaType<T4>::value };
		enum { Man = AI5+ tIsFuncCallNonMetaType<T5>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class tBinderFunctor6s
	{
		typedef tBinderConsts6<T0,T1,T2,T3,T4,T5> Cs;
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result)
				*info.result = ToVariant(((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info)));
			else
				((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class tBinderFunctor6s<CC, void ,T0,T1,T2,T3,T4,T5>
	{
		typedef tBinderConsts6<T0,T1,T2,T3,T4,T5> Cs;
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result) *info.result = info.This;
			((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info));
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class tBinderFunctor6
	{
		typedef tBinderConsts6<T0,T1,T2,T3,T4,T5> Cs;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result)
				*info.result = ToVariant((instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info)));
			else
				(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	class tBinderFunctor6<CC, IC, void  ,T0,T1,T2,T3,T4,T5>
	{
		typedef tBinderConsts6<T0,T1,T2,T3,T4,T5> Cs;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result) *info.result = info.This;
			(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info));
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	void BindFunction(CC * _class, const tString & name,
		R (*f)(T0,T1,T2,T3,T4,T5), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tBinderFunctor6s<CC, R ,T0,T1,T2,T3,T4,T5>::Call
					), context), attribute, is_members);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
	void BindFunction(CC * _class, const tString & name,
		R (IC::*f)(T0,T1,T2,T3,T4,T5), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (tObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (tObjectBase::*)()>(f),
				&tBinderFunctor6<CC, IC, R ,T0,T1,T2,T3,T4,T5>::Call
					), context), attribute, is_members);
	}


	// 定数
	template <typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	struct tBinderConsts7
	{
		enum { AI0=0 };
		enum { AI1=AI0 + tIsFuncCallNonMetaType<T0>::value };
		enum { AI2=AI1 + tIsFuncCallNonMetaType<T1>::value };
		enum { AI3=AI2 + tIsFuncCallNonMetaType<T2>::value };
		enum { AI4=AI3 + tIsFuncCallNonMetaType<T3>::value };
		enum { AI5=AI4 + tIsFuncCallNonMetaType<T4>::value };
		enum { AI6=AI5 + tIsFuncCallNonMetaType<T5>::value };
		enum { Man = AI6+ tIsFuncCallNonMetaType<T6>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class tBinderFunctor7s
	{
		typedef tBinderConsts7<T0,T1,T2,T3,T4,T5,T6> Cs;
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5,T6);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result)
				*info.result = ToVariant(((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info)));
			else
				((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class tBinderFunctor7s<CC, void ,T0,T1,T2,T3,T4,T5,T6>
	{
		typedef tBinderConsts7<T0,T1,T2,T3,T4,T5,T6> Cs;
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result) *info.result = info.This;
			((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info));
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class tBinderFunctor7
	{
		typedef tBinderConsts7<T0,T1,T2,T3,T4,T5,T6> Cs;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result)
				*info.result = ToVariant((instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info)));
			else
				(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	class tBinderFunctor7<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6>
	{
		typedef tBinderConsts7<T0,T1,T2,T3,T4,T5,T6> Cs;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result) *info.result = info.This;
			(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info));
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	void BindFunction(CC * _class, const tString & name,
		R (*f)(T0,T1,T2,T3,T4,T5,T6), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tBinderFunctor7s<CC, R ,T0,T1,T2,T3,T4,T5,T6>::Call
					), context), attribute, is_members);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	void BindFunction(CC * _class, const tString & name,
		R (IC::*f)(T0,T1,T2,T3,T4,T5,T6), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (tObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (tObjectBase::*)()>(f),
				&tBinderFunctor7<CC, IC, R ,T0,T1,T2,T3,T4,T5,T6>::Call
					), context), attribute, is_members);
	}


	// 定数
	template <typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	struct tBinderConsts8
	{
		enum { AI0=0 };
		enum { AI1=AI0 + tIsFuncCallNonMetaType<T0>::value };
		enum { AI2=AI1 + tIsFuncCallNonMetaType<T1>::value };
		enum { AI3=AI2 + tIsFuncCallNonMetaType<T2>::value };
		enum { AI4=AI3 + tIsFuncCallNonMetaType<T3>::value };
		enum { AI5=AI4 + tIsFuncCallNonMetaType<T4>::value };
		enum { AI6=AI5 + tIsFuncCallNonMetaType<T5>::value };
		enum { AI7=AI6 + tIsFuncCallNonMetaType<T6>::value };
		enum { Man = AI7+ tIsFuncCallNonMetaType<T7>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class tBinderFunctor8s
	{
		typedef tBinderConsts8<T0,T1,T2,T3,T4,T5,T6,T7> Cs;
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result)
				*info.result = ToVariant(((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info)));
			else
				((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class tBinderFunctor8s<CC, void ,T0,T1,T2,T3,T4,T5,T6,T7>
	{
		typedef tBinderConsts8<T0,T1,T2,T3,T4,T5,T6,T7> Cs;
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result) *info.result = info.This;
			((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info));
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class tBinderFunctor8
	{
		typedef tBinderConsts8<T0,T1,T2,T3,T4,T5,T6,T7> Cs;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result)
				*info.result = ToVariant((instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info)));
			else
				(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	class tBinderFunctor8<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6,T7>
	{
		typedef tBinderConsts8<T0,T1,T2,T3,T4,T5,T6,T7> Cs;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result) *info.result = info.This;
			(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info));
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	void BindFunction(CC * _class, const tString & name,
		R (*f)(T0,T1,T2,T3,T4,T5,T6,T7), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tBinderFunctor8s<CC, R ,T0,T1,T2,T3,T4,T5,T6,T7>::Call
					), context), attribute, is_members);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	void BindFunction(CC * _class, const tString & name,
		R (IC::*f)(T0,T1,T2,T3,T4,T5,T6,T7), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (tObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (tObjectBase::*)()>(f),
				&tBinderFunctor8<CC, IC, R ,T0,T1,T2,T3,T4,T5,T6,T7>::Call
					), context), attribute, is_members);
	}


	// 定数
	template <typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	struct tBinderConsts9
	{
		enum { AI0=0 };
		enum { AI1=AI0 + tIsFuncCallNonMetaType<T0>::value };
		enum { AI2=AI1 + tIsFuncCallNonMetaType<T1>::value };
		enum { AI3=AI2 + tIsFuncCallNonMetaType<T2>::value };
		enum { AI4=AI3 + tIsFuncCallNonMetaType<T3>::value };
		enum { AI5=AI4 + tIsFuncCallNonMetaType<T4>::value };
		enum { AI6=AI5 + tIsFuncCallNonMetaType<T5>::value };
		enum { AI7=AI6 + tIsFuncCallNonMetaType<T6>::value };
		enum { AI8=AI7 + tIsFuncCallNonMetaType<T7>::value };
		enum { Man = AI8+ tIsFuncCallNonMetaType<T8>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class tBinderFunctor9s
	{
		typedef tBinderConsts9<T0,T1,T2,T3,T4,T5,T6,T7,T8> Cs;
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result)
				*info.result = ToVariant(((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info),tFVoC<T8, Cs::AI8>::Cnv(info)));
			else
				((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info),tFVoC<T8, Cs::AI8>::Cnv(info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class tBinderFunctor9s<CC, void ,T0,T1,T2,T3,T4,T5,T6,T7,T8>
	{
		typedef tBinderConsts9<T0,T1,T2,T3,T4,T5,T6,T7,T8> Cs;
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result) *info.result = info.This;
			((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info),tFVoC<T8, Cs::AI8>::Cnv(info));
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class tBinderFunctor9
	{
		typedef tBinderConsts9<T0,T1,T2,T3,T4,T5,T6,T7,T8> Cs;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result)
				*info.result = ToVariant((instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info),tFVoC<T8, Cs::AI8>::Cnv(info)));
			else
				(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info),tFVoC<T8, Cs::AI8>::Cnv(info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	class tBinderFunctor9<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6,T7,T8>
	{
		typedef tBinderConsts9<T0,T1,T2,T3,T4,T5,T6,T7,T8> Cs;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result) *info.result = info.This;
			(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info),tFVoC<T8, Cs::AI8>::Cnv(info));
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	void BindFunction(CC * _class, const tString & name,
		R (*f)(T0,T1,T2,T3,T4,T5,T6,T7,T8), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tBinderFunctor9s<CC, R ,T0,T1,T2,T3,T4,T5,T6,T7,T8>::Call
					), context), attribute, is_members);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	void BindFunction(CC * _class, const tString & name,
		R (IC::*f)(T0,T1,T2,T3,T4,T5,T6,T7,T8), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (tObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (tObjectBase::*)()>(f),
				&tBinderFunctor9<CC, IC, R ,T0,T1,T2,T3,T4,T5,T6,T7,T8>::Call
					), context), attribute, is_members);
	}


	// 定数
	template <typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	struct tBinderConsts10
	{
		enum { AI0=0 };
		enum { AI1=AI0 + tIsFuncCallNonMetaType<T0>::value };
		enum { AI2=AI1 + tIsFuncCallNonMetaType<T1>::value };
		enum { AI3=AI2 + tIsFuncCallNonMetaType<T2>::value };
		enum { AI4=AI3 + tIsFuncCallNonMetaType<T3>::value };
		enum { AI5=AI4 + tIsFuncCallNonMetaType<T4>::value };
		enum { AI6=AI5 + tIsFuncCallNonMetaType<T5>::value };
		enum { AI7=AI6 + tIsFuncCallNonMetaType<T6>::value };
		enum { AI8=AI7 + tIsFuncCallNonMetaType<T7>::value };
		enum { AI9=AI8 + tIsFuncCallNonMetaType<T8>::value };
		enum { Man = AI9+ tIsFuncCallNonMetaType<T9>::value };

	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class tBinderFunctor10s
	{
		typedef tBinderConsts10<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9> Cs;
		typedef R (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result)
				*info.result = ToVariant(((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info),tFVoC<T8, Cs::AI8>::Cnv(info),tFVoC<T9, Cs::AI9>::Cnv(info)));
			else
				((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info),tFVoC<T8, Cs::AI8>::Cnv(info),tFVoC<T9, Cs::AI9>::Cnv(info));
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class tBinderFunctor10s<CC, void ,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>
	{
		typedef tBinderConsts10<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9> Cs;
		typedef void (*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result) *info.result = info.This;
			((tFunc)f)(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info),tFVoC<T8, Cs::AI8>::Cnv(info),tFVoC<T9, Cs::AI9>::Cnv(info));
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class tBinderFunctor10
	{
		typedef tBinderConsts10<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9> Cs;
		typedef R (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result)
				*info.result = ToVariant((instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info),tFVoC<T8, Cs::AI8>::Cnv(info),tFVoC<T9, Cs::AI9>::Cnv(info)));
			else
				(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info),tFVoC<T8, Cs::AI8>::Cnv(info),tFVoC<T9, Cs::AI9>::Cnv(info));
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	class tBinderFunctor10<CC, IC, void  ,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>
	{
		typedef tBinderConsts10<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9> Cs;
		typedef void (IC::*tFunc)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9);
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result) *info.result = info.This;
			(instance->*((tFunc)f))(tFVoC<T0, Cs::AI0>::Cnv(info),tFVoC<T1, Cs::AI1>::Cnv(info),tFVoC<T2, Cs::AI2>::Cnv(info),tFVoC<T3, Cs::AI3>::Cnv(info),tFVoC<T4, Cs::AI4>::Cnv(info),tFVoC<T5, Cs::AI5>::Cnv(info),tFVoC<T6, Cs::AI6>::Cnv(info),tFVoC<T7, Cs::AI7>::Cnv(info),tFVoC<T8, Cs::AI8>::Cnv(info),tFVoC<T9, Cs::AI9>::Cnv(info));
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	void BindFunction(CC * _class, const tString & name,
		R (*f)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tBinderFunctor10s<CC, R ,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::Call
					), context), attribute, is_members);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R ,typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	void BindFunction(CC * _class, const tString & name,
		R (IC::*f)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9), tMemberAttribute attribute = tMemberAttribute(),
		const tVariantBlock * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (tObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (tObjectBase::*)()>(f),
				&tBinderFunctor10<CC, IC, R ,T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::Call
					), context), attribute, is_members);
	}


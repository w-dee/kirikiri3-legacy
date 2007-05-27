#!/usr/bin/env ruby

# 最大の引数の数
COUNT = 10

class Integer
	def join()
		ret = ""
		self.times do |n|
			ret += "," if n != 0
			ret += yield(n)
		end
		ret
	end

	def append()
		ret = ""
		self.times do |n|
			ret += ","
			ret += yield(n)
		end
		ret
	end
end

STDOUT.print "
// このファイルは create_binder.rb により再生成されます


"

# 引数の数の分だけ
(COUNT+1).times do |argnum|
	STDOUT.print "
	// static関数、非 void の戻り値用
	template <typename CC, typename R #{argnum.append{|n|"typename T#{n}"}}>
	class RisseBinderFunctor#{argnum}s
	{
		static const size_t NumArgs = #{argnum};
		typedef R (*tFunc)(#{argnum.join{|n|"T#{n}"}});
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			if(result)
				*result = RisseToVariant(((tFunc)f)(#{argnum.join{|n|"RisseFromVariant<T#{n}>(args[#{n}])"}}));
			else
				((tFunc)f)(#{argnum.join{|n|"RisseFromVariant<T#{n}>(args[#{n}])"}});
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC #{argnum.append{|n|"typename T#{n}"}}>
	class RisseBinderFunctor#{argnum}s<CC, void #{argnum.append{|n|"T#{n}"}}>
	{
		static const size_t NumArgs = #{argnum};
		typedef void (*tFunc)(#{argnum.join{|n|"T#{n}"}});
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			((tFunc)f)(#{argnum.join{|n|"RisseFromVariant<T#{n}>(args[#{n}])"}});
			if(result) result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R #{argnum.append{|n|"typename T#{n}"}}>
	class RisseBinderFunctor#{argnum}
	{
		static const size_t NumArgs = #{argnum};
		typedef R (IC::*tFunc)(#{argnum.join{|n|"T#{n}"}});
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(result)
				*result = RisseToVariant((instance->*((tFunc)f))(#{argnum.join{|n|"RisseFromVariant<T#{n}>(args[#{n}])"}}));
			else
				(instance->*((tFunc)f))(#{argnum.join{|n|"RisseFromVariant<T#{n}>(args[#{n}])"}});
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC #{argnum.append{|n|"typename T#{n}"}}>
	class RisseBinderFunctor#{argnum}<CC, IC, void  #{argnum.append{|n|"T#{n}"}}>
	{
		static const size_t NumArgs = #{argnum};
		typedef void (IC::*tFunc)(#{argnum.join{|n|"T#{n}"}});
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(), RISSE_NATIVEFUNCTION_CALLEE_ARGS)
		{
			args.ExpectArgumentCount(NumArgs);
			IC * instance = This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(#{argnum.join{|n|"RisseFromVariant<T#{n}>(args[#{n}])"}});
			if(result) result->Clear();
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R #{argnum.append{|n|"typename T#{n}"}}>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (*f)(#{argnum.join{|n|"T#{n}"}}), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindStaticFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&RisseBinderFunctor#{argnum}s<CC, R #{argnum.append{|n|"T#{n}"}}>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R #{argnum.append{|n|"typename T#{n}"}}>
	void RisseRegisterBinder(CC * _class, const tRisseString & name,
		R (IC::*f)(#{argnum.join{|n|"T#{n}"}}), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&RisseBinderFunctor#{argnum}<CC, IC, R #{argnum.append{|n|"T#{n}"}}>::Call
					), context), attribute);
	}

"
end






#!/usr/bin/env ruby

# 最大の引数の数
COUNT = 10

class Integer
	def join(sep=',')
		ret = ""
		self.times do |n|
			ret += sep if n != 0
			ret += yield(n)
		end
		ret
	end

	def append(sep=',')
		ret = ""
		self.times do |n|
			ret += sep
			ret += yield(n)
		end
		ret
	end
end

STDOUT.print "
// このファイルは create_binder.rb により再生成されます

"

# Risse関数に渡された引数の数を合計するためのコードを返す
def num_args(n)
	"\n" + "\t\t\t\t0" + n.append('+'){|n| "\n\t\t\t\ttRisseIsFuncCallNonMetaType<T#{n}>::value"} + "\n\t\t\t"
end


# 引数の数の分だけ
(COUNT+1).times do |argnum|


	# RisseFromVariantOrCallingInfo<T0>(info.args[0],info),RisseFromVariantOrCallingInfo<T1>(info.args[1],info), ...
	args_list = argnum.join{|n|"RisseFromVariantOrCallingInfo<T#{n}>(info.args[#{num_args(n)}],info)"}

	STDOUT.print "
	// static関数、非 void の戻り値用
	template <typename CC, typename R #{argnum.append{|n|"typename T#{n}"}}>
	class RisseBinderFunctor#{argnum}s
	{
		typedef R (*tFunc)(#{argnum.join{|n|"T#{n}"}});
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(#{num_args(argnum)});
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(#{args_list}));
			else
				((tFunc)f)(#{args_list});
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC #{argnum.append{|n|"typename T#{n}"}}>
	class RisseBinderFunctor#{argnum}s<CC, void #{argnum.append{|n|"T#{n}"}}>
	{
		typedef void (*tFunc)(#{argnum.join{|n|"T#{n}"}});
	public:
		static void Call(tRisseClassBase * _class,
			void (*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(#{num_args(argnum)});
			((tFunc)f)(#{args_list});
			if(info.result) info.result->Clear();
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R #{argnum.append{|n|"typename T#{n}"}}>
	class RisseBinderFunctor#{argnum}
	{
		typedef R (IC::*tFunc)(#{argnum.join{|n|"T#{n}"}});
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(#{num_args(argnum)});
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(#{args_list}));
			else
				(instance->*((tFunc)f))(#{args_list});
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC #{argnum.append{|n|"typename T#{n}"}}>
	class RisseBinderFunctor#{argnum}<CC, IC, void  #{argnum.append{|n|"T#{n}"}}>
	{
		typedef void (IC::*tFunc)(#{argnum.join{|n|"T#{n}"}});
	public:
		static void Call(tRisseClassBase * _class, void (tRisseObjectBase::*f)(),
			const tRisseNativeBindFunctionCallingInfo & info)
		{
			info.args.ExpectArgumentCount(#{num_args(argnum)});
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)_class);
			(instance->*((tFunc)f))(#{args_list});
			if(info.result) info.result->Clear();
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






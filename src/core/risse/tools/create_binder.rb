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


# 引数の数の分だけ
(COUNT+1).times do |argnum|


	# ターゲット呼び出しの引数
	args_list = argnum.join{|n|"tRisseFVoC<T#{n}, Cs::AI#{n}>::Cnv(info)"}

	# 定数クラス名
	const_class = "tRisseBinderConsts#{argnum}#{argnum > 0 ? "<#{argnum.join{|n|"T#{n}"}}>":""}"

	# 定数 ( AI = ArgumentIndex, Cs = Consts, Man = NumMandatoryArgs
	STDOUT.print "
	// 定数
	#{argnum > 0 ? "template <#{argnum.join{|n|"typename T#{n}"}}>" : ""}
	struct tRisseBinderConsts#{argnum}
	{
#{
	argnum.join("\n") {
		|n|"\t\tenum { AI#{n}=#{n == 0 ? "0":"AI#{n-1} + tRisseIsFuncCallNonMetaType<T#{n-1}>::value"} };"} +
	"\n\t\tenum { Man = #{argnum == 0 ? "0" : "AI#{argnum-1}+ tRisseIsFuncCallNonMetaType<T#{argnum-1}>::value"} };\n"
}
	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R #{argnum.append{|n|"typename T#{n}"}}>
	class tRisseBinderFunctor#{argnum}s
	{
		typedef #{const_class} Cs;
		typedef R (*tFunc)(#{argnum.join{|n|"T#{n}"}});
	public:
		static void Call(void (*f)(),
			const tRisseNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result)
				*info.result = RisseToVariant(((tFunc)f)(#{args_list}));
			else
				((tFunc)f)(#{args_list});
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC #{argnum.append{|n|"typename T#{n}"}}>
	class tRisseBinderFunctor#{argnum}s<CC, void #{argnum.append{|n|"T#{n}"}}>
	{
		typedef #{const_class} Cs;
		typedef void (*tFunc)(#{argnum.join{|n|"T#{n}"}});
	public:
		static void Call(void (*f)(),
			const tRisseNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result) info.result->Clear();
			((tFunc)f)(#{args_list});
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R #{argnum.append{|n|"typename T#{n}"}}>
	class tRisseBinderFunctor#{argnum}
	{
		typedef #{const_class} Cs;
		typedef R (IC::*tFunc)(#{argnum.join{|n|"T#{n}"}});
	public:
		static void Call(void (tRisseObjectBase::*f)(),
			const tRisseNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result)
				*info.result = RisseToVariant((instance->*((tFunc)f))(#{args_list}));
			else
				(instance->*((tFunc)f))(#{args_list});
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC #{argnum.append{|n|"typename T#{n}"}}>
	class tRisseBinderFunctor#{argnum}<CC, IC, void  #{argnum.append{|n|"T#{n}"}}>
	{
		typedef #{const_class} Cs;
		typedef void (IC::*tFunc)(#{argnum.join{|n|"T#{n}"}});
	public:
		static void Call(void (tRisseObjectBase::*f)(),
			const tRisseNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.CheckAndGetObjectInterafce<IC, CC>((CC*)info.Class);
			if(info.result) info.result->Clear();
			(instance->*((tFunc)f))(#{args_list});
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R #{argnum.append{|n|"typename T#{n}"}}>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (*f)(#{argnum.join{|n|"T#{n}"}}), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tRisseBinderFunctor#{argnum}s<CC, R #{argnum.append{|n|"T#{n}"}}>::Call
					), context), attribute);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R #{argnum.append{|n|"typename T#{n}"}}>
	void RisseBindFunction(CC * _class, const tRisseString & name,
		R (IC::*f)(#{argnum.join{|n|"T#{n}"}}), tRisseMemberAttribute attribute = tRisseMemberAttribute(),
		const tRisseVariantBlock * context = tRisseVariant::GetDynamicContext())
	{
		_class->RegisterNormalMember(name,
			tRisseVariant(tRisseNativeBindFunction<void (tRisseObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tRisseClassBase *)_class,
				reinterpret_cast<void (tRisseObjectBase::*)()>(f),
				&tRisseBinderFunctor#{argnum}<CC, IC, R #{argnum.append{|n|"T#{n}"}}>::Call
					), context), attribute);
	}

"
end






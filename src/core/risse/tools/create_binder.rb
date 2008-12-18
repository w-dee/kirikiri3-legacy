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
	args_list = argnum.join{|n|"tFVoC<T#{n}, Cs::AI#{n}>::Cnv(info)"}

	# 定数クラス名
	const_class = "tBinderConsts#{argnum}#{argnum > 0 ? "<#{argnum.join{|n|"T#{n}"}}>":""}"

	# 定数 ( AI = ArgumentIndex, Cs = Consts, Man = NumMandatoryArgs
	STDOUT.print "
	// 定数
	#{argnum > 0 ? "template <#{argnum.join{|n|"typename T#{n}"}}>" : ""}
	struct tBinderConsts#{argnum}
	{
#{
	argnum.join("\n") {
		|n|"\t\tenum { AI#{n}=#{n == 0 ? "0":"AI#{n-1} + tIsFuncCallNonMetaType<T#{n-1}>::value"} };"} +
	"\n\t\tenum { Man = #{argnum == 0 ? "0" : "AI#{argnum-1}+ tIsFuncCallNonMetaType<T#{argnum-1}>::value"} };\n"
}
	};
	// static関数、非 void の戻り値用
	template <typename CC, typename R #{argnum.append{|n|"typename T#{n}"}}>
	class tBinderFunctor#{argnum}s
	{
		typedef #{const_class} Cs;
		typedef R (*tFunc)(#{argnum.join{|n|"T#{n}"}});
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result)
				*info.result = ToVariant(((tFunc)f)(#{args_list}));
			else
				((tFunc)f)(#{args_list});
		}
	};
	// static関数、void の戻り値用の特殊化
	template <typename CC #{argnum.append{|n|"typename T#{n}"}}>
	class tBinderFunctor#{argnum}s<CC, void #{argnum.append{|n|"T#{n}"}}>
	{
		typedef #{const_class} Cs;
		typedef void (*tFunc)(#{argnum.join{|n|"T#{n}"}});
	public:
		static void Call(void (*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			if(info.result) *info.result = info.This;
			((tFunc)f)(#{args_list});
		}
	};
	// 非static関数、非 void の戻り値用
	template <typename CC, typename IC, typename R #{argnum.append{|n|"typename T#{n}"}}>
	class tBinderFunctor#{argnum}
	{
		typedef #{const_class} Cs;
		typedef R (IC::*tFunc)(#{argnum.join{|n|"T#{n}"}});
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.AssertAndGetObjectInterafce((CC*)info.Class);
			if(info.result)
				*info.result = ToVariant((instance->*((tFunc)f))(#{args_list}));
			else
				(instance->*((tFunc)f))(#{args_list});
		}
	};
	// 非static関数、void の戻り値用
	template <typename CC, typename IC #{argnum.append{|n|"typename T#{n}"}}>
	class tBinderFunctor#{argnum}<CC, IC, void  #{argnum.append{|n|"T#{n}"}}>
	{
		typedef #{const_class} Cs;
		typedef void (IC::*tFunc)(#{argnum.join{|n|"T#{n}"}});
	public:
		static void Call(void (tObjectBase::*f)(),
			const tNativeCallInfo & info)
		{
			info.args.ExpectArgumentCount(Cs::Man);
			IC * instance = info.This.AssertAndGetObjectInterafce((CC*)info.Class);
			if(info.result) *info.result = info.This;
			(instance->*((tFunc)f))(#{args_list});
		}
	};
	// static関数用binder登録関数
	template <typename CC, typename R #{argnum.append{|n|"typename T#{n}"}}>
	void BindFunction(CC * _class, const tString & name,
		R (*f)(#{argnum.join{|n|"T#{n}"}}), tMemberAttribute attribute = tMemberAttribute(),
		const tVariant * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (*)()>(f),
				&tBinderFunctor#{argnum}s<CC, R #{argnum.append{|n|"T#{n}"}}>::Call
					), context), attribute, is_members);
	}
	// 非static関数用binder登録関数
	template <typename CC, typename IC, typename R #{argnum.append{|n|"typename T#{n}"}}>
	void BindFunction(CC * _class, const tString & name,
		R (IC::*f)(#{argnum.join{|n|"T#{n}"}}), tMemberAttribute attribute = tMemberAttribute(),
		const tVariant * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (tObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (tObjectBase::*)()>(f),
				&tBinderFunctor#{argnum}<CC, IC, R #{argnum.append{|n|"T#{n}"}}>::Call
					), context), attribute, is_members);
	}
	// const非static関数用binder登録関数
	template <typename CC, typename IC, typename R #{argnum.append{|n|"typename T#{n}"}}>
	void BindFunction(CC * _class, const tString & name,
		R (IC::*f)(#{argnum.join{|n|"T#{n}"}}) const, tMemberAttribute attribute = tMemberAttribute(),
		const tVariant * context = tVariant::GetDynamicContext(), bool is_members = true)
	{
		_class->RegisterNormalMember(name,
			tVariant(tNativeBindFunction<void (tObjectBase::*)()>::New(_class->GetRTTI()->GetScriptEngine(),
				(tClassBase *)_class,
				reinterpret_cast<void (tObjectBase::*)()>(f),
				&tBinderFunctor#{argnum}<CC, IC, R #{argnum.append{|n|"T#{n}"}}>::Call
					), context), attribute, is_members);
	}

"
end






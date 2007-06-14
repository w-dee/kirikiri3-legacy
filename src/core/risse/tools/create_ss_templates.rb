#!/usr/bin/env ruby


# サポートする最大の文字数

COUNT = 32


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

def hash_func(n)
	# ハッシュの計算式を作成する
	e = '0'
	n . times do |i|
		e = "tRisseSS_Shuff<#{e},c#{i}>::result"
	end

	e = "tRisseSS_Norm<#{e}>::result"
	e
end


STDOUT.print "
// このファイルは create_ss_templates.rb により再生成されます

// ハッシュ計算用テンプレート
template <risse_uint32 e, risse_uint32 c>
struct tRisseSS_Shuff { 
	enum { r0 = (e + c)&0xffffffff };
	enum { r1 = (r0 + ((r0<<10)&0xffffffff) ) & 0xffffffff };
	enum { r2 = (r1 ^ ((risse_uint32)(r1&0xffffffff)>>6)) & 0xffffffff };
	enum { result = r2 };
};

template <risse_uint32 e>
struct tRisseSS_Norm { 
	enum { r0 = e };
	enum { r1 = (r0 + ((r0 << 3)&0xffffffff)) & 0xffffffff };
	enum { r2 = r1 ^ ((risse_uint32)(r1&0xffffffff)>>11) };
	enum { r3 = (r2 + ((r2 << 15)&0xffffffff)) & 0xffffffff };
	enum { result =  ((r3&0xffffffff)==0)?0xffffffff:(r3&0xffffffff) };
};


// 文字列リテラルテンプレート
"
	STDOUT.print "
		template <#{(COUNT).join{|n| "risse_char c#{n}=0"}}>
		struct tRisseSS
		{
			static tRisseStringData data;
			static risse_char string[#{COUNT}+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <#{(COUNT).join{|n| "risse_char c#{n}"}}>
		risse_char tRisseSS<#{COUNT.join{|n| "c#{n}"}}>::string[#{COUNT}+3]=
		{tRisseStringData::MightBeShared,#{COUNT.join{|n| "c#{n}"}},0,#{hash_func(COUNT)}};
		template <#{(COUNT).join{|n| "risse_char c#{n}"}}>
		tRisseStringData tRisseSS<#{COUNT.join{|n| "c#{n}"}}>::data =
		{ tRisseSS<#{COUNT.join{|n| "c#{n}"}}>::string + 1, #{COUNT}};
"

def t(cnt)

	STDOUT.print "


		template <#{(cnt).join{|n| "risse_char c#{n}"}}>
		struct tRisseSS<#{(cnt).join{|n| "c#{n}"}}#{(COUNT-cnt).append{"0"}}>
		{
			static tRisseStringData data;
			static risse_char string[#{cnt}+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <#{(cnt).join{|n| "risse_char c#{n}"}}>
		risse_char tRisseSS<#{(cnt).join{|n| "c#{n}"}}#{(COUNT-cnt).append{"0"}}>::string[#{cnt}+3]=
		{tRisseStringData::MightBeShared,#{cnt.join{|n| "c#{n}"}},0,#{hash_func(cnt)}};
		template <#{(cnt).join{|n| "risse_char c#{n}"}}>
		tRisseStringData tRisseSS<#{(cnt).join{|n| "c#{n}"}}#{(COUNT-cnt).append{"0"}}>::data =
		{ tRisseSS<#{(cnt).join{|n| "c#{n}"}}#{(COUNT-cnt).append{"0"}}>::string + 1, #{cnt}};
"


end


(1 .. COUNT-1).each { |n| t(n) }


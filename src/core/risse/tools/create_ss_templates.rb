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

	i = 0

	while(i < n-(8-1))
		e = "tRisseSSS8<#{e},c#{i},c#{i+1},c#{i+2},c#{i+3},c#{i+4},c#{i+5},c#{i+6},c#{i+7}>::r"
		i = i + 8
	end
	while(i < n-(4-1))
		e = "tRisseSSS4<#{e},c#{i},c#{i+1},c#{i+2},c#{i+3}>::r"
		i = i + 4
	end
	while(i < n)
		e = "tRisseSSS<#{e},c#{i}>::r"
		i = i + 1
	end

	e = "tRisseSSN<#{e}>::r"
	e
end


STDOUT.print "
// このファイルは create_ss_templates.rb により再生成されます

// ハッシュ計算用テンプレート
template <risse_uint32 e, risse_uint32 c>
struct tRisseSSS { 
	enum { r0 = (e + c)&0xffffffff };
	enum { r1 = (r0 + ((r0<<10)&0xffffffff) ) & 0xffffffff };
	enum { r2 = (r1 ^ ((risse_uint32)(r1&0xffffffff)>>6)) & 0xffffffff };
	enum { r = r2 };
};

template <risse_uint32 e, risse_uint32 c0,risse_uint32 c1,risse_uint32 c2,risse_uint32 c3>
struct tRisseSSS4 { 
	enum { r0 = tRisseSSS< e,c0>::r };
	enum { r1 = tRisseSSS<r0,c1>::r };
	enum { r2 = tRisseSSS<r1,c2>::r };
	enum { r3 = tRisseSSS<r2,c3>::r };
	enum { r = r3 };
};

template <risse_uint32 e, risse_uint32 c0,risse_uint32 c1,risse_uint32 c2,risse_uint32 c3,
			risse_uint32 c4,risse_uint32 c5,risse_uint32 c6,risse_uint32 c7>
struct tRisseSSS8 { 
	enum { r0 = tRisseSSS4< e,c0,c1,c2,c3>::r };
	enum { r1 = tRisseSSS4<r0,c4,c5,c6,c7>::r };
	enum { r = r1 };
};

template <risse_uint32 e>
struct tRisseSSN { 
	enum { r0 = e };
	enum { r1 = (r0 + ((r0 << 3)&0xffffffff)) & 0xffffffff };
	enum { r2 = r1 ^ ((risse_uint32)(r1&0xffffffff)>>11) };
	enum { r3 = (r2 + ((r2 << 15)&0xffffffff)) & 0xffffffff };
	enum { r =  ((r3&0xffffffff)==0)?0xffffffff:(r3&0xffffffff) };
};


// 文字列リテラルテンプレート
"

def t(cnt)

	STDOUT.print "


		template <#{(cnt).join{|n| "risse_char c#{n}#{(cnt==COUNT)?'=0':''}"}}>
		struct tRisseSS#{(cnt==COUNT)?'':"<#{(cnt).join{|n| "c#{n}"}}#{(COUNT-cnt).append{"0"}}>"}
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

t(COUNT)
(1 .. COUNT-1).each { |n| t(n) }


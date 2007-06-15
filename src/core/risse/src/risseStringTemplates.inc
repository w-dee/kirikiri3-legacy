
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



		template <risse_char c0=0,risse_char c1=0,risse_char c2=0,risse_char c3=0,risse_char c4=0,risse_char c5=0,risse_char c6=0,risse_char c7=0,risse_char c8=0,risse_char c9=0,risse_char c10=0,risse_char c11=0,risse_char c12=0,risse_char c13=0,risse_char c14=0,risse_char c15=0,risse_char c16=0,risse_char c17=0,risse_char c18=0,risse_char c19=0,risse_char c20=0,risse_char c21=0,risse_char c22=0,risse_char c23=0,risse_char c24=0,risse_char c25=0,risse_char c26=0,risse_char c27=0,risse_char c28=0,risse_char c29=0,risse_char c30=0,risse_char c31=0>
		struct tRisseSS
		{
			static tRisseStringData data;
			static risse_char string[32+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28,risse_char c29,risse_char c30,risse_char c31>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,c31>::string[32+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,c31,0,tRisseSSN<tRisseSSS8<tRisseSSS8<tRisseSSS8<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r,c24,c25,c26,c27,c28,c29,c30,c31>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28,risse_char c29,risse_char c30,risse_char c31>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,c31>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,c31>::string + 1, 32};



		template <risse_char c0>
		struct tRisseSS<c0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[1+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0>
		risse_char tRisseSS<c0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[1+3]=
		{tRisseStringData::MightBeShared,c0,0,tRisseSSN<tRisseSSS<0,c0>::r>::r};
		template <risse_char c0>
		tRisseStringData tRisseSS<c0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 1};



		template <risse_char c0,risse_char c1>
		struct tRisseSS<c0,c1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[2+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1>
		risse_char tRisseSS<c0,c1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[2+3]=
		{tRisseStringData::MightBeShared,c0,c1,0,tRisseSSN<tRisseSSS<tRisseSSS<0,c0>::r,c1>::r>::r};
		template <risse_char c0,risse_char c1>
		tRisseStringData tRisseSS<c0,c1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 2};



		template <risse_char c0,risse_char c1,risse_char c2>
		struct tRisseSS<c0,c1,c2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[3+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2>
		risse_char tRisseSS<c0,c1,c2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[3+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,0,tRisseSSN<tRisseSSS<tRisseSSS<tRisseSSS<0,c0>::r,c1>::r,c2>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2>
		tRisseStringData tRisseSS<c0,c1,c2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 3};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3>
		struct tRisseSS<c0,c1,c2,c3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[4+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3>
		risse_char tRisseSS<c0,c1,c2,c3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[4+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,0,tRisseSSN<tRisseSSS4<0,c0,c1,c2,c3>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3>
		tRisseStringData tRisseSS<c0,c1,c2,c3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 4};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4>
		struct tRisseSS<c0,c1,c2,c3,c4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[5+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4>
		risse_char tRisseSS<c0,c1,c2,c3,c4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[5+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,0,tRisseSSN<tRisseSSS<tRisseSSS4<0,c0,c1,c2,c3>::r,c4>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 5};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[6+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[6+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,0,tRisseSSN<tRisseSSS<tRisseSSS<tRisseSSS4<0,c0,c1,c2,c3>::r,c4>::r,c5>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 6};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[7+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[7+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,0,tRisseSSN<tRisseSSS<tRisseSSS<tRisseSSS<tRisseSSS4<0,c0,c1,c2,c3>::r,c4>::r,c5>::r,c6>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 7};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[8+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[8+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,0,tRisseSSN<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 8};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[9+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[9+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,0,tRisseSSN<tRisseSSS<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 9};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[10+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[10+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,0,tRisseSSN<tRisseSSS<tRisseSSS<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8>::r,c9>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 10};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[11+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[11+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,0,tRisseSSN<tRisseSSS<tRisseSSS<tRisseSSS<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8>::r,c9>::r,c10>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 11};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[12+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[12+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,0,tRisseSSN<tRisseSSS4<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 12};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[13+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[13+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,0,tRisseSSN<tRisseSSS<tRisseSSS4<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11>::r,c12>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 13};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[14+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[14+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,0,tRisseSSN<tRisseSSS<tRisseSSS<tRisseSSS4<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11>::r,c12>::r,c13>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 14};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[15+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[15+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,0,tRisseSSN<tRisseSSS<tRisseSSS<tRisseSSS<tRisseSSS4<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11>::r,c12>::r,c13>::r,c14>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 15};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[16+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[16+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,0,tRisseSSN<tRisseSSS8<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 16};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[17+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[17+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,0,tRisseSSN<tRisseSSS<tRisseSSS8<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 17};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[18+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[18+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,0,tRisseSSN<tRisseSSS<tRisseSSS<tRisseSSS8<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16>::r,c17>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 18};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[19+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[19+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,0,tRisseSSN<tRisseSSS<tRisseSSS<tRisseSSS<tRisseSSS8<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16>::r,c17>::r,c18>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 19};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[20+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,0,0,0,0,0,0,0,0,0,0,0,0>::string[20+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,0,tRisseSSN<tRisseSSS4<tRisseSSS8<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 20};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[21+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,0,0,0,0,0,0,0,0,0,0,0>::string[21+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,0,tRisseSSN<tRisseSSS<tRisseSSS4<tRisseSSS8<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19>::r,c20>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 21};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,0,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[22+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,0,0,0,0,0,0,0,0,0,0>::string[22+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,0,tRisseSSN<tRisseSSS<tRisseSSS<tRisseSSS4<tRisseSSS8<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19>::r,c20>::r,c21>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,0,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,0,0,0,0,0,0,0,0,0,0>::string + 1, 22};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,0,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[23+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,0,0,0,0,0,0,0,0,0>::string[23+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,0,tRisseSSN<tRisseSSS<tRisseSSS<tRisseSSS<tRisseSSS4<tRisseSSS8<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19>::r,c20>::r,c21>::r,c22>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,0,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,0,0,0,0,0,0,0,0,0>::string + 1, 23};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,0,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[24+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,0,0,0,0,0,0,0,0>::string[24+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,0,tRisseSSN<tRisseSSS8<tRisseSSS8<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,0,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,0,0,0,0,0,0,0,0>::string + 1, 24};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,0,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[25+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,0,0,0,0,0,0,0>::string[25+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,0,tRisseSSN<tRisseSSS<tRisseSSS8<tRisseSSS8<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r,c24>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,0,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,0,0,0,0,0,0,0>::string + 1, 25};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,0,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[26+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,0,0,0,0,0,0>::string[26+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,0,tRisseSSN<tRisseSSS<tRisseSSS<tRisseSSS8<tRisseSSS8<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r,c24>::r,c25>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,0,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,0,0,0,0,0,0>::string + 1, 26};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,0,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[27+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,0,0,0,0,0>::string[27+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,0,tRisseSSN<tRisseSSS<tRisseSSS<tRisseSSS<tRisseSSS8<tRisseSSS8<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r,c24>::r,c25>::r,c26>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,0,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,0,0,0,0,0>::string + 1, 27};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,0,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[28+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,0,0,0,0>::string[28+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,0,tRisseSSN<tRisseSSS4<tRisseSSS8<tRisseSSS8<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r,c24,c25,c26,c27>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,0,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,0,0,0,0>::string + 1, 28};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,0,0,0>
		{
			static tRisseStringData data;
			static risse_char string[29+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,0,0,0>::string[29+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,0,tRisseSSN<tRisseSSS<tRisseSSS4<tRisseSSS8<tRisseSSS8<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r,c24,c25,c26,c27>::r,c28>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,0,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,0,0,0>::string + 1, 29};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28,risse_char c29>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,0,0>
		{
			static tRisseStringData data;
			static risse_char string[30+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28,risse_char c29>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,0,0>::string[30+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,0,tRisseSSN<tRisseSSS<tRisseSSS<tRisseSSS4<tRisseSSS8<tRisseSSS8<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r,c24,c25,c26,c27>::r,c28>::r,c29>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28,risse_char c29>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,0,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,0,0>::string + 1, 30};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28,risse_char c29,risse_char c30>
		struct tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,0>
		{
			static tRisseStringData data;
			static risse_char string[31+3];
		public:
			operator const tRisseString & ()
			{ return *reinterpret_cast<const tRisseString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28,risse_char c29,risse_char c30>
		risse_char tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,0>::string[31+3]=
		{tRisseStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,0,tRisseSSN<tRisseSSS<tRisseSSS<tRisseSSS<tRisseSSS4<tRisseSSS8<tRisseSSS8<tRisseSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r,c24,c25,c26,c27>::r,c28>::r,c29>::r,c30>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28,risse_char c29,risse_char c30>
		tRisseStringData tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,0>::data =
		{ tRisseSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,0>::string + 1, 31};

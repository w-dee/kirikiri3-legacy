
// このファイルは create_ss_templates.rb により再生成されます

// ハッシュ計算用テンプレート
template <risse_uint32 e, risse_uint32 c>
struct tSSS { 
	enum { r0 = (e + c)&0xffffffff };
	enum { r1 = (r0 + ((r0<<10)&0xffffffff) ) & 0xffffffff };
	enum { r2 = (r1 ^ ((risse_uint32)(r1&0xffffffff)>>6)) & 0xffffffff };
	enum { r = r2 };
};

template <risse_uint32 e, risse_uint32 c0,risse_uint32 c1,risse_uint32 c2,risse_uint32 c3>
struct tSSS4 { 
	enum { r0 = tSSS< e,c0>::r };
	enum { r1 = tSSS<r0,c1>::r };
	enum { r2 = tSSS<r1,c2>::r };
	enum { r3 = tSSS<r2,c3>::r };
	enum { r = r3 };
};

template <risse_uint32 e, risse_uint32 c0,risse_uint32 c1,risse_uint32 c2,risse_uint32 c3,
			risse_uint32 c4,risse_uint32 c5,risse_uint32 c6,risse_uint32 c7>
struct tSSS8 { 
	enum { r0 = tSSS4< e,c0,c1,c2,c3>::r };
	enum { r1 = tSSS4<r0,c4,c5,c6,c7>::r };
	enum { r = r1 };
};

template <risse_uint32 e>
struct tSSN { 
	enum { r0 = e };
	enum { r1 = (r0 + ((r0 << 3)&0xffffffff)) & 0xffffffff };
	enum { r2 = r1 ^ ((risse_uint32)(r1&0xffffffff)>>11) };
	enum { r3 = (r2 + ((r2 << 15)&0xffffffff)) & 0xffffffff };
	enum { r =  ((r3&0xffffffff)==0)?0xffffffff:(r3&0xffffffff) };
};


// 文字列リテラルテンプレート



		template <risse_char c0=0,risse_char c1=0,risse_char c2=0,risse_char c3=0,risse_char c4=0,risse_char c5=0,risse_char c6=0,risse_char c7=0,risse_char c8=0,risse_char c9=0,risse_char c10=0,risse_char c11=0,risse_char c12=0,risse_char c13=0,risse_char c14=0,risse_char c15=0,risse_char c16=0,risse_char c17=0,risse_char c18=0,risse_char c19=0,risse_char c20=0,risse_char c21=0,risse_char c22=0,risse_char c23=0,risse_char c24=0,risse_char c25=0,risse_char c26=0,risse_char c27=0,risse_char c28=0,risse_char c29=0,risse_char c30=0,risse_char c31=0>
		struct tSS
		{
			static tStringData data;
			static risse_char string[32+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28,risse_char c29,risse_char c30,risse_char c31>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,c31>::string[32+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,c31,0,tSSN<tSSS8<tSSS8<tSSS8<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r,c24,c25,c26,c27,c28,c29,c30,c31>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28,risse_char c29,risse_char c30,risse_char c31>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,c31>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,c31>::string + 1, 32};



		template <risse_char c0>
		struct tSS<c0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[1+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0>
		risse_char tSS<c0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[1+3]=
		{tStringData::MightBeShared,c0,0,tSSN<tSSS<0,c0>::r>::r};
		template <risse_char c0>
		tStringData tSS<c0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 1};



		template <risse_char c0,risse_char c1>
		struct tSS<c0,c1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[2+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1>
		risse_char tSS<c0,c1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[2+3]=
		{tStringData::MightBeShared,c0,c1,0,tSSN<tSSS<tSSS<0,c0>::r,c1>::r>::r};
		template <risse_char c0,risse_char c1>
		tStringData tSS<c0,c1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 2};



		template <risse_char c0,risse_char c1,risse_char c2>
		struct tSS<c0,c1,c2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[3+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2>
		risse_char tSS<c0,c1,c2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[3+3]=
		{tStringData::MightBeShared,c0,c1,c2,0,tSSN<tSSS<tSSS<tSSS<0,c0>::r,c1>::r,c2>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2>
		tStringData tSS<c0,c1,c2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 3};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3>
		struct tSS<c0,c1,c2,c3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[4+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3>
		risse_char tSS<c0,c1,c2,c3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[4+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,0,tSSN<tSSS4<0,c0,c1,c2,c3>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3>
		tStringData tSS<c0,c1,c2,c3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 4};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4>
		struct tSS<c0,c1,c2,c3,c4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[5+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4>
		risse_char tSS<c0,c1,c2,c3,c4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[5+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,0,tSSN<tSSS<tSSS4<0,c0,c1,c2,c3>::r,c4>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4>
		tStringData tSS<c0,c1,c2,c3,c4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 5};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5>
		struct tSS<c0,c1,c2,c3,c4,c5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[6+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5>
		risse_char tSS<c0,c1,c2,c3,c4,c5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[6+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,0,tSSN<tSSS<tSSS<tSSS4<0,c0,c1,c2,c3>::r,c4>::r,c5>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5>
		tStringData tSS<c0,c1,c2,c3,c4,c5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 6};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[7+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[7+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,0,tSSN<tSSS<tSSS<tSSS<tSSS4<0,c0,c1,c2,c3>::r,c4>::r,c5>::r,c6>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 7};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[8+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[8+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,0,tSSN<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 8};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[9+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[9+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,0,tSSN<tSSS<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 9};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[10+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[10+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,0,tSSN<tSSS<tSSS<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8>::r,c9>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 10};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[11+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[11+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,0,tSSN<tSSS<tSSS<tSSS<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8>::r,c9>::r,c10>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 11};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[12+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[12+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,0,tSSN<tSSS4<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 12};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[13+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[13+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,0,tSSN<tSSS<tSSS4<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11>::r,c12>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 13};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[14+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[14+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,0,tSSN<tSSS<tSSS<tSSS4<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11>::r,c12>::r,c13>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 14};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[15+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[15+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,0,tSSN<tSSS<tSSS<tSSS<tSSS4<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11>::r,c12>::r,c13>::r,c14>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 15};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[16+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[16+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,0,tSSN<tSSS8<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 16};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[17+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[17+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,0,tSSN<tSSS<tSSS8<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 17};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,0,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[18+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[18+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,0,tSSN<tSSS<tSSS<tSSS8<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16>::r,c17>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,0,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 18};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,0,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[19+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,0,0,0,0,0,0,0,0,0,0,0,0,0>::string[19+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,0,tSSN<tSSS<tSSS<tSSS<tSSS8<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16>::r,c17>::r,c18>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,0,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,0,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 19};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,0,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[20+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,0,0,0,0,0,0,0,0,0,0,0,0>::string[20+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,0,tSSN<tSSS4<tSSS8<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,0,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,0,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 20};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,0,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[21+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,0,0,0,0,0,0,0,0,0,0,0>::string[21+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,0,tSSN<tSSS<tSSS4<tSSS8<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19>::r,c20>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,0,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,0,0,0,0,0,0,0,0,0,0,0>::string + 1, 21};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,0,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[22+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,0,0,0,0,0,0,0,0,0,0>::string[22+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,0,tSSN<tSSS<tSSS<tSSS4<tSSS8<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19>::r,c20>::r,c21>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,0,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,0,0,0,0,0,0,0,0,0,0>::string + 1, 22};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,0,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[23+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,0,0,0,0,0,0,0,0,0>::string[23+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,0,tSSN<tSSS<tSSS<tSSS<tSSS4<tSSS8<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19>::r,c20>::r,c21>::r,c22>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,0,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,0,0,0,0,0,0,0,0,0>::string + 1, 23};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,0,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[24+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,0,0,0,0,0,0,0,0>::string[24+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,0,tSSN<tSSS8<tSSS8<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,0,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,0,0,0,0,0,0,0,0>::string + 1, 24};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,0,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[25+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,0,0,0,0,0,0,0>::string[25+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,0,tSSN<tSSS<tSSS8<tSSS8<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r,c24>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,0,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,0,0,0,0,0,0,0>::string + 1, 25};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,0,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[26+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,0,0,0,0,0,0>::string[26+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,0,tSSN<tSSS<tSSS<tSSS8<tSSS8<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r,c24>::r,c25>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,0,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,0,0,0,0,0,0>::string + 1, 26};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,0,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[27+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,0,0,0,0,0>::string[27+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,0,tSSN<tSSS<tSSS<tSSS<tSSS8<tSSS8<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r,c24>::r,c25>::r,c26>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,0,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,0,0,0,0,0>::string + 1, 27};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,0,0,0,0>
		{
			static tStringData data;
			static risse_char string[28+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,0,0,0,0>::string[28+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,0,tSSN<tSSS4<tSSS8<tSSS8<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r,c24,c25,c26,c27>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,0,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,0,0,0,0>::string + 1, 28};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,0,0,0>
		{
			static tStringData data;
			static risse_char string[29+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,0,0,0>::string[29+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,0,tSSN<tSSS<tSSS4<tSSS8<tSSS8<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r,c24,c25,c26,c27>::r,c28>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,0,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,0,0,0>::string + 1, 29};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28,risse_char c29>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,0,0>
		{
			static tStringData data;
			static risse_char string[30+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28,risse_char c29>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,0,0>::string[30+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,0,tSSN<tSSS<tSSS<tSSS4<tSSS8<tSSS8<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r,c24,c25,c26,c27>::r,c28>::r,c29>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28,risse_char c29>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,0,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,0,0>::string + 1, 30};



		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28,risse_char c29,risse_char c30>
		struct tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,0>
		{
			static tStringData data;
			static risse_char string[31+3];
		public:
			operator const tString & ()
			{ return *reinterpret_cast<const tString *>(&data); }
		};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28,risse_char c29,risse_char c30>
		risse_char tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,0>::string[31+3]=
		{tStringData::MightBeShared,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,0,tSSN<tSSS<tSSS<tSSS<tSSS4<tSSS8<tSSS8<tSSS8<0,c0,c1,c2,c3,c4,c5,c6,c7>::r,c8,c9,c10,c11,c12,c13,c14,c15>::r,c16,c17,c18,c19,c20,c21,c22,c23>::r,c24,c25,c26,c27>::r,c28>::r,c29>::r,c30>::r>::r};
		template <risse_char c0,risse_char c1,risse_char c2,risse_char c3,risse_char c4,risse_char c5,risse_char c6,risse_char c7,risse_char c8,risse_char c9,risse_char c10,risse_char c11,risse_char c12,risse_char c13,risse_char c14,risse_char c15,risse_char c16,risse_char c17,risse_char c18,risse_char c19,risse_char c20,risse_char c21,risse_char c22,risse_char c23,risse_char c24,risse_char c25,risse_char c26,risse_char c27,risse_char c28,risse_char c29,risse_char c30>
		tStringData tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,0>::data =
		{ tSS<c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19,c20,c21,c22,c23,c24,c25,c26,c27,c28,c29,c30,0>::string + 1, 31};

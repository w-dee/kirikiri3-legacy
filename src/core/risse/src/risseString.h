//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 文字列のC++クラス
//---------------------------------------------------------------------------

#ifndef risseStringH
#define risseStringH

/*! @note
Risse 文字列について

Risse 文字列は tRisseStringBlock クラスで表される。
このクラスは、risse_char * 型の文字列を保持する。この文字列は \0 を含むこ
とはできない。

以下の説明ではコードポイントを「文字」と表記する。

risse_char は 32bit サイズであることが求められる。

■ 構造

	tRisseStringBlock は以下の二つのメンバを持っている。

	risse_char  *	Buffer;	// 文字列バッファ
	risse_size		Length;	// 文字列長 (最後の \0 は含めない)

	tRisseStringBlock はこの二つのメンバを用い、「Bufferから始まるLength分の
	長さを持つ文字列」を表す。
	長さのない (空の) 文字列の場合、Length は 0 になる (BufferがNULLになる
	保証などはないので、Length が 0 であることをもって空文字列とすること)。


■ バッファ

	バッファは 最低でも (Length + 3) * sizeof(risse_char) + sizeof(risse_size)
	を持つ。
	tRisseStringBlock が任意の文字列を元に作成される場合、
	(Length + 3) * sizeof(risse_char) + sizeof(size_t) の長さのバッファがま
	ず確保され、文字列中の各文字は以下のように配置される。

	capacity \0 文字0 文字1 文字3 .... \0 hint
	              ↑
                Buffer

	このように、最初は capacity としてバッファに実際に確保されている長さが
	入り、それ以降は 各 CP の両端に \0 がついたバッファ、最後にこの文字列の
	hint が続く。
	capacity は、バッファの長さがぴったりであれば Length と同じになる。

	tRisseStringBlock::Buffer は、最初の capacity や \0 ではなく、その次の
	文字0(つまり文字列の先頭) を指すようになる。

	cpacity の次の \0 は、 Buffer が他の tRisseStringBlock と共有されている
	場合に \0 ではなくなる。最後の \0 は、終端が \0 であることを期待してい
	る C 言語系の関数に渡す際に NULL終端 を表す。

■ バッファの共有

	tRisseStringBlock のコピーコンストラクタや代入演算子は、バッファの中身は
	コピーせず、Buffer と Length だけをコピーする。この際、バッファがすでに
	共有されたことを表すため、Buffer[-1] が \0 でなければ、Buffer[-1] に
	-1 を代入する。これはバッファが共有されている可能性を表す。

■ 部分文字列の共有

	tRisseStringBlock は、他の文字列の一部を指し示すことができる。

	tRisseStringBlock a が以下のバッファを持っている場合、

	4 \0 文字0 文字1 文字2 文字3 \0 hint
	    ↑
	   Buffer
	Length = 4

	文字1 ～ 文字2 の２文字を表す tRisseStringBlock b は以下のように表すこ
	とができる。

	4 -1 文字0 文字1 文字2 文字3 \0 hint
	          ↑
	        Buffer
	Length = 2

	この場合もバッファの共有と同じく、バッファの先頭の \0 が -1 に書き換えら
	れ、バッファが共有されていることを表す。

■ バッファの共有の判定

	tRisseStringBlock が他の tRisseStringBlock とバッファを共有している可能
	性があるかどうかを判断するには、Buffer[-1] が \0 でないかどうかを見れば
	よい。Buffer[-1] が 非 \0 であれはバッファは共有されている可能性がある。

	tRisseStringBlock は文字列を共有する場合、Buffer[-1] が非 \0 の場合に
	 -1 を代入するが、これによりバッファが共有されている可能性を表す。

	tRisseStringBlock は部分文字列を共有する場合、母文字列は Buffer[-1] が
	非 \0の場合に -1 を代入するし、部分文字列は Buffer[-1] は必然的に \0 に
	なる。これは、部分文字列が母文字列の先頭から共有しているならば、Buffer[-1]
	は母文字列と同じ -1 になるし、部分文字列が母文字列の途中から共有している
	ならば、Buffer[-1] は直前の文字を表し、tRisseStringBlock は文字列中に
	\0 を含むことはないから、これは非 \0 となる。

	これらは共有の可能性を表すだけである。可能性があっても実際は共有されてい
	ない場合があり得る。

■ バッファの容量と実際の Length

	+= 演算子などで Length が長くなり、バッファが拡張される際、バッファは実
	際に必要な容量よりもすこし多めに確保され、次の拡張時にバッファを再度確保
	しなくても済むようになる。
	この際、実際にバッファが格納可能な CP 数を表すのが、バッファの先頭に
	size_t 型で確保されている領域である。
	ここの領域は、Buffer[-1] が 0 のとき (共有している可能性がない場合)に
	のみ有効な値を保持していると考えるべきである。実際のところ、Buffer[-1] が
	0 でない場合は += のようなバッファの内容を破壊する操作では直前に
	バッファの内容のコピーが行われるため、この領域が参照されることはない。

■ Independ

	tRisseStringBlock::Independ() メソッドは、文字列がそのバッファを共有
	している可能性がある場合、新たにバッファを確保し、内容をコピーする。
	これにより、バッファに何か変更を書き込んでも、他の文字列に影響が及ばない
	ようにすることができる。

	Independ は新たに確保したバッファの先頭は \0 にするが、元のバッファの
	内容には手を加えない。元のバッファの内容は、さらに他の文字列から共有
	されている可能性があるが、実際に共有されているのか、あるいはされていな
	いのかは、tRisseStringBlock が持っている情報からは判定できないからである。

■ c_str()

	tRisseStringBlock::c_str() は、多くの C 言語系 API が期待するような、
	NULL 終端文字列を返す。文字列が他の文字列の部分文字列を表している場合、
	文字列の最後が \0 である保証はないが、そのような場合は、c_str() は新たに
	バッファを確保し、最後が \0 で終了しているバッファを作り、それを返す。

■ バッファの解放

	参照されなくなったバッファは、GC により自動的に回収される。

■ ヒント

	文字列バッファ中の hint は、この文字列の 32bit ハッシュを表す領域である
	が、常に正しいハッシュを表しているとは限らない。ここが 0 の場合はハッシュ
	が無効であるか、まだ計算されていないことを表し、非 0 の場合はハッシュが
	入っていると考えることができるが、正しいハッシュが入っている保証はない。
	あくまでハッシュ表検索時の「ヒント」として扱うべきである。

	tRisseStringBlock::GetHint() は、このヒント領域へのポインタを返す。
	文字列が部分共有されている場合など、このヒント領域が存在しない場合は
	このメソッドは NULL を返す。その場合はヒントは利用できない。

*/

#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAssert.h"
#include "risseGC.h"


namespace Risse
{
//---------------------------------------------------------------------------
//! @brief	文字列用データ
//! @note	この構造体を直接いじらないこと！
//---------------------------------------------------------------------------
struct tRisseStringData
{
	mutable risse_char  *	Buffer;	//!< 文字列バッファ
	risse_size				Length;	//!< 文字列長 (最後の \0 は含めない)

public:
	const static risse_char MightBeShared  = static_cast<risse_char>(-1L);
		//!< 共有可能性フラグとして Buffer[-1] に設定する値

	//! @brief -1, 0, 0 が入っている配列(空のバッファを表す)
	static risse_char EmptyBuffer[3];

	#define RISSE_STRING_EMPTY_BUFFER (tRisseStringData::EmptyBuffer+1)
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	文字列ブロック
//---------------------------------------------------------------------------
class tRisseStringBlock : protected tRisseStringData, public tRisseCollectee
{
public:
	//! @brief デフォルトコンストラクタ
	tRisseStringBlock()
	{
		Buffer = RISSE_STRING_EMPTY_BUFFER;
		Length = 0;
	}

	//! @brief コピーコンストラクタ
	//! @param ref コピー元オブジェクト
	tRisseStringBlock(const tRisseStringBlock & ref)
	{
		*this = ref;
	}

	//! @brief 部分文字列を作るためのコンストラクタ
	//! @param ref		コピー元オブジェクト
	//! @param offset	切り出す開始位置
	//! @param length	切り出す長さ
	tRisseStringBlock(const tRisseStringBlock & ref,
		risse_size offset, risse_size length);

	//! @brief		コンストラクタ(risse_char * から)
	//! @param		ref		元の文字列
	tRisseStringBlock(const risse_char * ref)
	{
		*this = ref;
	}

	//! @brief		コンストラクタ(risse_char * から、コードポイント数制限付き)
	//! @param		ref		元の文字列
	//! @param		n		コードポイント数
	//! @note		[ref, ref+n) の範囲には \0 がないこと
	tRisseStringBlock(const risse_char * ref, risse_size n);

#ifdef RISSE_WCHAR_T_SIZE_IS_16BIT
	//! @brief		コンストラクタ(wchar_t * から)
	//! @param		ref		元の文字列
	tRisseStringBlock(const wchar_t *ref)
	{
		*this = ref;
	}
#endif

//#ifdef RISSE_SUPPORT_WX
// TODO: パフォーマンス的に問題になりそうならばこれを実装すること
//	tRisseStringBlock(const wxString & ref);
//#endif

	//! @brief		コンストラクタ(char * から)
	//! @param		ref		元の文字列
	tRisseStringBlock(const char * ref)
	{
		*this = ref;
	}

	//! @brief		コンストラクタ(メッセージのビルド)
	//! @param		msg		メッセージ
	//! @param		r1		メッセージ中の '%1' と置き換わる文字列
	tRisseStringBlock(const tRisseStringBlock &msg, const tRisseStringBlock &r1);

	//! @brief		コンストラクタ(メッセージのビルド)
	//! @param		msg		メッセージ
	//! @param		r1		メッセージ中の '%1' と置き換わる文字列
	//! @param		r2		メッセージ中の '%2' と置き換わる文字列
	tRisseStringBlock(const tRisseStringBlock &msg, const tRisseStringBlock &r1,
					const tRisseStringBlock &r2);

	//! @brief		コンストラクタ(メッセージのビルド)
	//! @param		msg		メッセージ
	//! @param		r1		メッセージ中の '%1' と置き換わる文字列
	//! @param		r2		メッセージ中の '%2' と置き換わる文字列
	//! @param		r3		メッセージ中の '%3' と置き換わる文字列
	tRisseStringBlock(const tRisseStringBlock &msg, const tRisseStringBlock &r1,
					const tRisseStringBlock &r2, const tRisseStringBlock &r3);

	//! @brief		コンストラクタ(メッセージのビルド)
	//! @param		msg		メッセージ
	//! @param		r1		メッセージ中の '%1' と置き換わる文字列
	//! @param		r2		メッセージ中の '%2' と置き換わる文字列
	//! @param		r3		メッセージ中の '%3' と置き換わる文字列
	//! @param		r4		メッセージ中の '%4' と置き換わる文字列
	tRisseStringBlock(const tRisseStringBlock &msg, const tRisseStringBlock &r1,
					const tRisseStringBlock &r2, const tRisseStringBlock &r3,
					const tRisseStringBlock &r4);

	//! @brief		デストラクタ
	//! @note		このデストラクタは通常は呼ばれないことに注意すること。
	//!				また、ここに内容の破壊以外に意味のあるコードをかけることを期待しないこと。
	//!				tRisseHashTable などは明示的にin-placeでデストラクタを
	//!				呼ぶことにより内容が破壊され、メンバが参照しているポインタが
	//!				破壊されることにより参照のリンクが切れることを期待するので、それに
	//!				対応する。
	~tRisseStringBlock()
	{
		// 少なくとも、メンバとして持っているポインタが破壊できればよい
		Buffer = NULL; // バッファを null に
	}

	//! @brief	代入演算子
	//! @param	ref	コピー元オブジェクト
	//! @return	このオブジェクトへの参照
	tRisseStringBlock & operator = (const tRisseStringBlock & ref)
	{
		if(ref.Buffer[-1] == 0)
			ref.Buffer[-1] = MightBeShared; // 共有可能性フラグをたてる
		Buffer = ref.Buffer;
		Length = ref.Length;
		return *this;
	}

	//! @brief		代入演算子(risse_char * から)
	//! @param		ref		元の文字列
	//! @return		このオブジェクトへの参照
	tRisseStringBlock & operator = (const risse_char * ref);

	//! @brief		代入演算子(risse_char から)
	//! @param		ref		元の文字列
	//! @return		このオブジェクトへの参照
	tRisseStringBlock & operator = (const risse_char ref);

#ifdef RISSE_WCHAR_T_SIZE_IS_16BIT
	//! @brief		代入演算子(wchar_t * から)
	//! @param		ref		元の文字列
	//! @return		このオブジェクトへの参照
	tRisseStringBlock & operator = (const wchar_t *ref);
#endif

//#ifdef RISSE_SUPPORT_WX
// TODO: パフォーマンス的に問題になりそうならばこれを実装すること
//	tRisseStringBlock & operator = (const wxString & ref);
//#endif

	//! @brief		代入演算子(char * から)
	//! @param		ref		元の文字列
	//! @return		このオブジェクトへの参照
	tRisseStringBlock & operator = (const char * ref);

	//! @brief	バッファをコピーし、新しい tRisseStringBlock を返す
	//! @param	ref	コピー元オブジェクト
	//! @return	新しい tRisseStringBlock オブジェクト
	//! @note	このメソッドは、必ずバッファをコピーして返し、
	//!			元の文字列 (この文字列) の共有状態などはいっさい
	//!			変更しない。破壊を前提とした文字列を他の文字列
	//!			から作成する場合などに効率的。
	tRisseStringBlock MakeBufferCopy(const tRisseStringBlock & ref) const
	{
		return tRisseStringBlock(Buffer, Length);
	}


private: // buffer management
	//! @brief		n個のコードポイントからなるバッファを割り当てる
	//! @param		n	コードポイント数
	//! @param		prevbuf	以前のバッファ(バッファを再確保する場合のみ;prevbuf[-1] は 0 =非共有であること)
	//! @return		割り当てられたバッファ
	//! @note		実際には (n+3)*sizeof(risse_char) + sizeof(risse_size) が割り当
	//! てられ、2番目の文字を指すポインタが帰る。共有可能性フラグはクリアされ、
	//! 容量も書き込まれるが、null終端とヒントは書き込まれないので注意。
	static risse_char * AllocateInternalBuffer(risse_size n, risse_char *prevbuf = NULL);


	//! @brief		バッファに割り当てられているコードポイント数(容量)を得る
	//! @param		buffer バッファ
	//! @return		コードポイント数
	//! @note		Buffer[-1] が 0 の時のみにこのメソッドを呼ぶこと。
	//!				それ以外の場合は返値は信用してはならない。
	static risse_size GetBufferCapacity(const risse_char * buffer)
	{
		return
			*reinterpret_cast<const risse_size *>(
				reinterpret_cast<const char *>(buffer) -
					(sizeof(risse_char) + sizeof(risse_size)));
	}

public: // pointer
	//! @brief バッファを割り当てる
	//! @param  n バッファに割り当てる文字数 (最後の \0 は含まない)
	//! @return	文字列バッファ
	//! @note このメソッドを使った後、もし n と異なる
	//! 長さを書き込んだ場合は、FixLength あるいは
	//! SetLength を呼ぶこと。
	risse_char * Allocate(risse_size n)
	{
		if(n)
			Buffer = AllocateInternalBuffer(n), Buffer[n] = Buffer[n+1] = 0;
		else
			Buffer = RISSE_STRING_EMPTY_BUFFER;
		Length = n;
		return Buffer;
	}

	//! @brief 内部で持っている文字列の長さを、実際の長さに合わせる
	void FixLength()
	{
		if((Length = Risse_strlen(Buffer)) == 0)
			Buffer = RISSE_STRING_EMPTY_BUFFER;
		Buffer[Length + 1] = 0; // hint をクリア
	}

	//! @brief C 言語スタイルのポインタを得る
	//! @note  tRisseStringBlock は内部に保持しているバッファの最後が \0 で
	//! 終わってない場合は、バッファを新たにコピーして \0 で終わらせ、その
	//! バッファのポインタを返す。また、空文字列の場合は NULL を返さずに
	//! "" へのポインタを返す。
	const risse_char * c_str() const
	{
		if(Buffer[Length]) return Independ();
		return Buffer;
	}

	//! @brief 内部バッファのポインタを返す
	//! @return 内部バッファのポインタ
	//! @note  このメソッドで返されるポインタは、しばしば
	//! 期待した位置に \0 がない (null終結している保証がない)
	const risse_char * Pointer() const { return Buffer; }

	//! @brief 文字列バッファをコピーし、独立させる
	//! @return 内部バッファ
	//! @note tRisseStringBlock は一つのバッファを複数の文字列インスタンスが
	//! 共有する場合があるが、このメソッドは共有を切り、文字列バッファを
	//! 独立する。Risse の GC の特性上、その文字列がすでに独立しているかどうかを
	//! 確実に知るすべはなく、このメソッドはかなりの確率でバッファをコピーするため、
	//! 実行が高価になる場合があることに注意すること。
	//! このメソッドは内部バッファへのポインタを返すが、このバッファに、もしもとの
	//! 長さよりも短い長さの文字列を書き込んだ場合は、FixLength あるいは
	//! SetLength を呼ぶこと。
	//! このメソッドは、内容が空の時は独立を行わなずに NULL を返す
	risse_char * Independ() const
	{
		if(Buffer[-1]) // 共有可能性フラグが立っている？
			return InternalIndepend();
		return Buffer;
	}

	//! @brief	文字列バッファを文字列と同じサイズにする
	//! @note	Append や += 演算子などは、後のサイズの増加に備えて内部バッファを
	//!			余分に取るが、その余分を切り捨てる。すでにぴったりなサイズ
	//!			だった場合はなにもしない。また、共有中の場合は共有を断ち切る。
	void Fit() const
	{
		if(Buffer[-1]) // 共有可能性フラグが立っている？
			InternalIndepend();
		if(GetBufferCapacity(Buffer) != Length) // 長さがぴったりではない
			InternalIndepend();
	}

private:
	//! @brief		文字列バッファをコピーし、独立させる
	//! @return		内部バッファ
	risse_char * InternalIndepend() const;

public:
	//! @brief		内部バッファのサイズを予約する
	//! @param		capacity  容量
	//! @note		内部バッファを最低でも capacity で指定された
	//!				コードポイント数にする。すでに内部バッファが
	//!				指定された容量分だけある場合は何もしない。
	void Reserve(risse_size capacity) const;

public: // hint/hash
	//! @brief	ヒントへのポインタを得る
	//! @return ヒントへのポインタ
	//! @note
	//! ここで返されるヒントのポインタは、この文字列オブジェクトの他の
	//! 破壊的メソッドを呼ぶと無効になる。<br>
	//! 破壊的メソッドは const メソッドでも内部バッファを破壊する場合がある
	//! ので注意。これにはc_str()も含む。Pointer() や GetLength(),
	//! operator [] は大丈夫。 <br>
	//! 使用可能ならば常に GetHint() か SetHint() を用いること。<br>
	//! このポインタは、ヒントが使用不可の場合は NULL が帰る。
	risse_uint32 * GetHintPointer() const
	{
		if(!Buffer[Length])
		{
			// バッファの期待した位置に \0終端がある。
			// この場合はその次をヒントへのポインタと見なすことができる。
			return reinterpret_cast<risse_uint32*>(Buffer + Length + 1);
		}
		return NULL;
	}

	//! @brief ヒントを得る
	//! @return ヒント (0 = ヒントが無効)
	risse_uint32 GetHint() const
	{
		if(!Buffer[Length])
			return *reinterpret_cast<risse_uint32*>(Buffer + Length + 1);
		return 0;
	}

	//! @brief 現在の文字列のハッシュに従ってヒントを設定する
	//! @note このメソッドはバッファの内容を変更するにもかかわらず const
	//! メソッドである。
	void SetHint() const
	{
		if(!Buffer[Length])
			*reinterpret_cast<risse_uint32*>(Buffer + Length + 1) = GetHash();
	}

	//! @brief ヒントを設定する
	//! @brief hint  ハッシュ値
	//! @note このメソッドはバッファの内容を変更するにもかかわらず const
	//! メソッドである。
	void SetHint(risse_uint32 hint) const
	{
		if(!Buffer[Length])
			*reinterpret_cast<risse_uint32*>(Buffer + Length + 1) = hint;
	}

	//! @brief 文字列のハッシュを計算して返す
	//! @return ハッシュ値
	//! @note	戻り値は必ず0以外の値になる
	risse_uint32 GetHash() const;

public: // object property
	//! @brief 文字列の長さを得る
	//! @return	文字列の長さ(コードポイント単位) (\0 は含まれない)
	risse_size GetLength() const { return Length; }

	//! @brief 文字列の長さを設定する(切りつめのみ可)
	//! @param	n 新しい長さ(コードポイント単位)
	void SetLength(risse_size n)
	{
		RISSE_ASSERT(n <= Length);
		Independ();
		Length = n;
		if(n)
			Buffer[n] = Buffer[n+1] = 0; // null終端と hint をクリア
		else
			Buffer = RISSE_STRING_EMPTY_BUFFER; // Buffer を解放
	}

	//! @brief 文字列が空かどうかを得る @return 文字列が空かどうか
	bool IsEmpty() const { return Length == 0; }

public: // comparison
	//! @brief < 演算子
	//! @param	ref		比較するオブジェクト
	//! @return	*this<refかどうか
	bool operator <  (const tRisseStringBlock & ref) const
	{
		return Risse_strcmp(Buffer, ref.Buffer) < 0;
	}

	//! @brief > 演算子
	//! @param	ref		比較するオブジェクト
	//! @return	*this>refかどうか
	bool operator >  (const tRisseStringBlock & ref) const
		{ return ref < *this; }

	//! @brief <= 演算子
	//! @param	ref		比較するオブジェクト
	//! @return	*this<=refかどうか
	bool operator <= (const tRisseStringBlock & ref) const
		{ return ! (*this > ref); }

	//! @brief >= 演算子
	//! @param	ref		比較するオブジェクト
	//! @return	*this>=refかどうか
	bool operator >= (const tRisseStringBlock & ref) const
		{ return ! (*this < ref); }

	//! @brief 同一比較
	//! @param	ref		比較するオブジェクト
	//! @return	*this==refかどうか
	bool operator == (const tRisseStringBlock & ref) const
	{
		if(this == &ref) return true; // 同じポインタ
		if(Length != ref.Length) return false; // 違う長さ
		if(Buffer == ref.Buffer) return true; // 同じバッファ
		return !Risse_strcmp(Buffer, ref.Buffer);
	}

	//! @brief 同一比較
	//! @param	ref		比較する文字列
	//! @return	*this==ptrかどうか
	bool operator == ( const risse_char *ptr ) const;

	//! @brief 不一致判定
	//! @param	ref		比較するオブジェクト
	//! @return	*this!=refかどうか
	bool operator != (const tRisseStringBlock & ref) const
		{ return ! (*this == ref); }

	//! @brief 不一致判定
	//! @param	ref		比較する文字列
	//! @return	*this!=ptrかどうか
	bool operator != (const risse_char *ptr) const
		{ return ! (*this == ptr); }

public: // operators
	//! @brief		文字列の追加
	//! @param		buffer		追加する文字列 (length中には \0 が無いこと)
	//! @param		length		追加する文字列の長さ
	void Append(const risse_char * buffer, risse_size length);

	//! @brief		文字列の連結
	//! @param		ref		連結する文字列
	//! @return		このオブジェクトへの参照
	tRisseStringBlock & operator += (const tRisseStringBlock & ref)
	{
		if(Length == 0) { *this = ref; return *this; }
		Append(ref.Buffer, ref.Length);
		return *this;
	}

	//! @brief		文字の連結
	//! @param		ref		連結する文字
	//! @return		このオブジェクトへの参照
	tRisseStringBlock & operator += (risse_char ref)
	{
		if(ref == 0) { return *this; } // やることなし
		if(Length == 0) { *this = ref; return *this; }
		Append(&ref, 1); // 文字を追加
		return *this;
	}

	//! @brief		文字列の連結
	//! @param		ref		連結する文字列
	//! @return		新しく連結された文字列
	tRisseStringBlock operator + (const tRisseStringBlock & ref) const;

	//! @brief		文字列の連結
	//! @param		ref		連結する文字列
	//! @return		新しく連結された文字列
	tRisseStringBlock operator + (const risse_char * ref) const;

	//! @brief		文字の連結
	//! @param		ref		連結する文字
	//! @return		新しく連結された文字列
	tRisseStringBlock operator + (const risse_char ref) const
	{
		risse_char tmp[2];
		tmp[0] = ref; tmp[1] = 0;
		return *this + tmp;
	}

	// + 演算子 (risse_char * と tRisseStringBlockの連結) を friend に
	// (定義と実装は下の方)
	friend tRisseStringBlock operator +(const risse_char *lhs, const tRisseStringBlock &rhs);

	//! @brief [] 演算子
	//! @param		n		位置
	//! @return		nの位置にあるコード
	risse_char operator [] (risse_size n) const
	{
		RISSE_ASSERT(n < Length);
		return Buffer[n];
	}

public: // conversion

#ifdef RISSE_SUPPORT_WX
	wxString AsWxString() const
		{ return RisseCharToWxString(Buffer, Length); }
	operator wxString() const
		{ return AsWxString(); }
#endif

	//! @brief		数値を文字列に変換(整数から)
	//! @param		v		値
	//! @return		このオブジェクトへの参照
	//! @note		このクラスにはrisse_charがint型と見誤りやすいという理由で
	//!				整数型からの直接の構築や整数型の代入は定義されていない。
	//!				整数型から文字列への変換はこのメソッドを使うと楽。
	static tRisseStringBlock AsString(risse_int v);

	//! @brief		数値を文字列に変換(64bit整数から)
	//! @param		v		値
	//! @return		このオブジェクトへの参照
	//! @note		このクラスにはrisse_charがint型と見誤りやすいという理由で
	//!				整数型からの直接の構築や整数型の代入は定義されていない。
	//!				整数型から文字列への変換はこのメソッドを使うと楽。
	static tRisseStringBlock AsString(risse_int64 v);

public: // other utilities
	//! @brief		文字列の置き換え
	//! @param		old_str			置き換え元の文字列
	//! @param		new_str			置き換え先の文字列
	//! @parma		replace_all		すべての一致を置き換えるかどうか
	//! @return		置き換えられた文字列
	tRisseStringBlock Replace(const tRisseStringBlock &old_str,
		const tRisseStringBlock &new_str, bool replace_all = true) const;

	//! @brief		文字列の繰り返しを生成して返す
	//! @param		count 繰り返し回数
	//! @return		繰り返された文字列
	tRisseStringBlock Times(risse_size count);

	//! @brief		文字列リテラルとして解釈できるようなエスケープを行う
	//! @param		maxlen		返値文字列のおおよその最大コードポイント数(risse_size_maxの場合は無制限)
	//! @param		quote		両端を "" で囲むかどうか
	//! @return		エスケープされた文字列
	//! @note		返値の両端にクオート ( "" ) は付かない。つけたい場合は
	//!				自分でつけるか ToTokenString を使うこと。
	//!				返値 は maxlen 付近で切られるが、maxlen ぴったりである保証はない。
	//!				maxlen 付近で切られた場合の省略記号 '...' は、quote が true の場合のみ付く。
	tRisseStringBlock Escape(risse_size maxlen = risse_size_max, bool quote = false) const;

	//! @brief		値を再パース可能な文字列に変換する
	//! @return		再パース可能な文字列
	tRisseStringBlock AsTokenString() const
	{ return Escape(risse_size_max, true); }

	//! @brief		値を人間が読み取り可能な文字列に変換する
	//! @param		maxlen		おおよその最大コードポイント数; 収まり切らない場合は 
	//!							省略記号 '...' が付く(risse_size_maxの場合は無制限)
	//! @return		人間が読み取り可能な文字列
	tRisseStringBlock AsHumanReadable(risse_size maxlen = risse_size_max) const
	{ return Escape(maxlen, true); }

private:
	//! @brief		static な空文字列を表すデータ
	static tRisseStringData EmptyStringData;

public:
	//! @brief		static な空文字列を得る
	//! @return		static な空文字列
	//! @note		tRisseString() は空文字列になるがstaticではない。
	//!				このメソッドは空文字列をstaticに保持しているデータへの
	//!				参照を返す。よって単に空文字列が欲しい場合には tRisseString() 
	//!				と比べて効率的。ただし、あくまでこれは参照を返すので、
	//!				たとえば tRisseString GetXXX() { return tRisseString::GetEmptyString(); }
	//!				などとすると参照から実体が作られて、それが帰ることになるので非効率的。
	//!				あくまで const tRisseString & の参照が求められている文脈でのみ使うこと。
	static const tRisseStringBlock & GetEmptyString()
		{ return *(const tRisseStringBlock*)(&EmptyStringData); }

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列の連結
//! @param		lhs		連結する文字列(左側)
//! @param		rhs		連結する文字列(右側)
//! @return		新しく連結された文字列
//---------------------------------------------------------------------------
tRisseStringBlock operator +(const risse_char *lhs, const tRisseStringBlock &rhs);
//---------------------------------------------------------------------------

typedef tRisseStringBlock tRisseString; //!< いまのところ tRisseString は tRisseStringBlock と同じ

//---------------------------------------------------------------------------
} // namespace Risse


#endif

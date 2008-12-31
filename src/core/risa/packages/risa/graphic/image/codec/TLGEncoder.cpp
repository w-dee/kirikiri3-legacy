//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TLGエンコーダ
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/graphic/image/codec/TLGEncoder.h"
#include "risa/packages/risa/graphic/image/codec/TLGDecoder.h"
#include "risa/common/RisaException.h"



namespace Risa {
RISSE_DEFINE_SOURCE_ID(10011,40455,5786,18693,50305,8218,39140,14309);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
#define SLIDE_N 4096
#define SLIDE_M (18+255)
/**
 * スライド辞書法 圧縮クラス
 */
class SlideCompressor : public tCollectee
{
	// TODO: 誰かこれを高速化してくんね
	struct Chain
	{
		int Prev;
		int Next;
	};

	risse_uint8 Text[SLIDE_N + SLIDE_M - 1];
	int Map[256*256];
	Chain Chains[SLIDE_N];


	risse_uint8 Text2[SLIDE_N + SLIDE_M - 1];
	int Map2[256*256];
	Chain Chains2[SLIDE_N];


	int S;
	int S2;

public:
	SlideCompressor();
	virtual ~SlideCompressor();

private:
	int GetMatch(const risse_uint8*cur, int curlen, int &pos, int s);
	void AddMap(int p)
	{
		int place = Text[p] + ((int)Text[(p + 1) & (SLIDE_N - 1)] << 8);

		if(Map[place] == -1)
		{
			// first insertion
			Map[place] = p;
		}
		else
		{
			// not first insertion
			int old = Map[place];
			Map[place] = p;
			Chains[old].Prev = p;
			Chains[p].Next = old;
			Chains[p].Prev = -1;
		}
	}

	void DeleteMap(int p)
	{
		int n;
		if((n = Chains[p].Next) != -1)
			Chains[n].Prev = Chains[p].Prev;

		if((n = Chains[p].Prev) != -1)
		{
			Chains[n].Next = Chains[p].Next;
		}
		else if(Chains[p].Next != -1)
		{
			int place = Text[p] + ((int)Text[(p + 1) & (SLIDE_N - 1)] << 8);
			Map[place] = Chains[p].Next;
		}
		else
		{
			int place = Text[p] + ((int)Text[(p + 1) & (SLIDE_N - 1)] << 8);
			Map[place] = -1;
		}

		Chains[p].Prev = -1;
		Chains[p].Next = -1;
	}


public:
	void Encode(const risse_uint8 *in, risse_size inlen,
		risse_uint8 *out, risse_size & outlen);

	void Store();
	void Restore();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
SlideCompressor::SlideCompressor()
{
	S = 0;
	for(int i = 0; i < SLIDE_N + SLIDE_M; i++) Text[i] = 0;
	for(int i = 0; i < 256*256; i++)
		Map[i] = -1;
	for(int i = 0; i < SLIDE_N; i++)
		Chains[i].Prev = Chains[i].Next = -1;
	for(int i = SLIDE_N - 1; i >= 0; i--)
		AddMap(i);
}
//---------------------------------------------------------------------------
SlideCompressor::~SlideCompressor()
{
}
//---------------------------------------------------------------------------
int SlideCompressor::GetMatch(const risse_uint8*cur, int curlen, int &pos, int s)
{
	// get match length
	if(curlen < 3) return 0;

	int place = cur[0] + ((int)cur[1] << 8);

	int maxlen = 0;
	if((place = Map[place]) != -1)
	{
		int place_org;
		curlen -= 1;
		do
		{
			place_org = place;
			if(s == place || s == ((place + 1) & (SLIDE_N -1))) continue;
			place += 2;
			int lim = (SLIDE_M < curlen ? SLIDE_M : curlen) + place_org;
			const risse_uint8 *c = cur + 2;
			if(lim >= SLIDE_N)
			{
				if(place_org <= s && s < SLIDE_N)
					lim = s;
				else if(s < (lim&(SLIDE_N-1)))
					lim = s + SLIDE_N;
			}
			else
			{
				if(place_org <= s && s < lim)
					lim = s;
			}
			while(Text[place] == *(c++) && place < lim) place++;
			int matchlen = place - place_org;
			if(matchlen > maxlen) pos = place_org, maxlen = matchlen;
			if(matchlen == SLIDE_M) return maxlen;

		} while((place = Chains[place_org].Next) != -1);
	}
	return maxlen;
}
//---------------------------------------------------------------------------
void SlideCompressor::Encode(const risse_uint8 *in, risse_size inlen,
		risse_uint8 *out, risse_size & outlen)
{
	risse_uint8 code[40], codeptr, mask;

	if(inlen == 0) return;

	outlen = 0;
	code[0] = 0;
	codeptr = mask = 1;

	int s = S;
	while(inlen > 0)
	{
		int pos = 0;
		int len = GetMatch(in, inlen, pos, s);
		if(len >= 3)
		{
			code[0] |= mask;
			if(len >= 18)
			{
				code[codeptr++] = pos & 0xff;
				code[codeptr++] = ((pos &0xf00)>> 8) | 0xf0;
				code[codeptr++] = len - 18;
			}
			else
			{
				code[codeptr++] = pos & 0xff;
				code[codeptr++] = ((pos&0xf00)>> 8) | ((len-3)<<4);
			}
			while(len--)
			{
				risse_uint8 c = 0[in++];
				DeleteMap((s - 1) & (SLIDE_N - 1));
				DeleteMap(s);
				if(s < SLIDE_M - 1) Text[s + SLIDE_N] = c;
				Text[s] = c;
				AddMap((s - 1) & (SLIDE_N - 1));
				AddMap(s);
				s++;
				inlen--;
				s &= (SLIDE_N - 1);
			}
		}
		else
		{
			risse_uint8 c = 0[in++];
			DeleteMap((s - 1) & (SLIDE_N - 1));
			DeleteMap(s);
			if(s < SLIDE_M - 1) Text[s + SLIDE_N] = c;
			Text[s] = c;
	 		AddMap((s - 1) & (SLIDE_N - 1));
			AddMap(s);
			s++;
			inlen--;
			s &= (SLIDE_N - 1);
			code[codeptr++] = c;
		}
		mask <<= 1;

		if(mask == 0)
		{
			for(int i = 0; i < codeptr; i++)
				out[outlen++] = code[i];
			mask = codeptr = 1;
			code[0] = 0;
		}
	}

	if(mask != 1)
	{
		for(int i = 0; i < codeptr; i++)
			out[outlen++] = code[i];
	}

	S = s;
}
//---------------------------------------------------------------------------
void SlideCompressor::Store()
{
	S2 = S;
	int i;
	for(i = 0; i < SLIDE_N + SLIDE_M - 1; i++)
		Text2[i] = Text[i];

	for(i = 0; i < 256*256; i++)
		Map2[i] = Map[i];

	for(i = 0; i < SLIDE_N; i++)
		Chains2[i] = Chains[i];
}
//---------------------------------------------------------------------------
void SlideCompressor::Restore()
{
	S = S2;
	int i;
	for(i = 0; i < SLIDE_N + SLIDE_M - 1; i++)
		Text[i] = Text2[i];

	for(i = 0; i < 256*256; i++)
		Map[i] = Map2[i];

	for(i = 0; i < SLIDE_N; i++)
		Chains[i] = Chains2[i];
}
//---------------------------------------------------------------------------












#define BLOCK_HEIGHT 4
//---------------------------------------------------------------------------
static void WriteInt32(risse_uint32 num, tStreamAdapter *out)
{
	risse_uint8 buf[4];
	buf[0] = num & 0xff;
	buf[1] = (num >> 8) & 0xff;
	buf[2] = (num >> 16) & 0xff;
	buf[3] = (num >> 24) & 0xff;
	out->WriteBuffer(buf, 4);
}
//---------------------------------------------------------------------------
void tTLGImageEncoder::EncodeTLG5(tStreamInstance * stream,
				tProgressCallback * callback,
				int compos)
{
	tStreamAdapter out(stream);

	int colors = compos;

	// header
	risse_size width = 0;
	risse_size height = 0;
	GetDimensions(&width, &height);
	{
		out.WriteBuffer("TLG5.0\x00raw\x1a\x00", 11);
		out.WriteBuffer(&colors, 1);
		WriteInt32(width, &out);
		WriteInt32(height, &out);
		int blockheight = BLOCK_HEIGHT;
		WriteInt32(blockheight, &out);
	}

	int blockcount = (int)((height - 1) / BLOCK_HEIGHT) + 1;


	// buffers/compressors
	SlideCompressor compressor;
	risse_uint8 *cmpinbuf[4];
	risse_uint8 *cmpoutbuf[4];
	for(int i = 0; i < colors; i++)
		cmpinbuf[i] = cmpoutbuf[i] = NULL;
	risse_size written[4];
	int *blocksizes;
	risse_uint32 * upper;
	risse_uint32 * current = NULL;

	// allocate buffers/compressors
	for(int i = 0; i < colors; i++)
	{
		cmpinbuf[i] = static_cast<risse_uint8*>(
			AlignedMallocAtomicCollectee(sizeof(risse_uint8) * width * BLOCK_HEIGHT, 4));
		cmpoutbuf[i] = static_cast<risse_uint8*>(
			AlignedMallocAtomicCollectee(sizeof(risse_uint8) * width * BLOCK_HEIGHT * 9 / 4, 4));
		written[i] = 0;
	}
	blocksizes = static_cast<int *>(
		AlignedMallocAtomicCollectee(sizeof(int) * blockcount, 4));
	upper = static_cast<risse_uint32*>(
		AlignedMallocAtomicCollectee(sizeof(risse_uint32) * width, 4));

	risse_uint64 blocksizepos = out.GetPosition();

	// write block size header
	// (later fill this)
	for(int i = 0; i < blockcount; i++)
	{
		out.WriteBuffer("    ", 4);
	}

	//
	risse_size block = 0;
	for(risse_size blk_y = 0; blk_y < height; blk_y += BLOCK_HEIGHT, block++)
	{
		if(callback) callback->CallOnProgress(height, blk_y);

		risse_size ylim = blk_y + BLOCK_HEIGHT;
		if(ylim > height) ylim = height;


		risse_size inp = 0;
		for(risse_size y = blk_y; y < ylim; y++)
		{

			// current を upper にコピー
			if(current)
				memcpy(upper, current, sizeof(risse_uint32) * width);
			else
				memset(upper, 0, sizeof(risse_uint32) * width);

			// 入力ピクセル列を得る
			current = static_cast<risse_uint32*>(
				GetLines(NULL, y, 1, NULL, tPixel::pfARGB32));

			// prepare buffer
			int prevcl[4];
			int val[4];

			for(int c = 0; c < colors; c++) prevcl[c] = 0;

			for(risse_size x = 0; x < width; x++)
			{
				for(int c = 0; c < colors; c++)
				{
					int cl;
					cl = (current[x]>>(c*8)&0xff) - (upper[x]>>(c*8)&0xff);
					val[c] = cl - prevcl[c];
					prevcl[c] = cl;
				}
				// composite colors
				switch(colors)
				{
				case 1:
					cmpinbuf[0][inp] = val[0];
					break;
				case 3:
					cmpinbuf[0][inp] = val[0] - val[1];
					cmpinbuf[1][inp] = val[1];
					cmpinbuf[2][inp] = val[2] - val[1];
					break;
				case 4:
					cmpinbuf[0][inp] = val[0] - val[1];
					cmpinbuf[1][inp] = val[1];
					cmpinbuf[2][inp] = val[2] - val[1];
					cmpinbuf[3][inp] = val[3];
					break;
				}

				inp++;
			}

		}


		// compress buffer and write to the file

		// LZSS
		int blocksize = 0;
		for(int c = 0; c < colors; c++)
		{
			risse_size wrote = 0;
			compressor.Store();
			compressor.Encode(cmpinbuf[c], inp,
				cmpoutbuf[c], wrote);
			if(wrote < inp)
			{
				out.WriteBuffer("\x00", 1);
				WriteInt32(wrote, &out);
				out.WriteBuffer(cmpoutbuf[c], wrote);
				blocksize += wrote + 4 + 1;
			}
			else
			{
				compressor.Restore();
				out.WriteBuffer("\x01", 1);
				WriteInt32(inp, &out);
				out.WriteBuffer(cmpinbuf[c], inp);
				blocksize += inp + 4 + 1;
			}
			written[c] += wrote;
		}

		blocksizes[block] = blocksize;
	}


	// write block sizes
	risse_uint64 pos_save = out.GetPosition();
	out.SetPosition(blocksizepos);
	for(int i = 0; i < blockcount; i++)
		WriteInt32(blocksizes[i], &out);
	out.SetPosition(pos_save);

}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------

// TLG6.0 bitstream output implementation


class TLG6BitStream
{
	gc_vector<risse_uint8> & OutStream; // output stream
	int BufferBitPos; // bit position of output buffer
	risse_size BufferBytePos; // byte position of output buffer
	risse_uint8 *Buffer; // output buffer
	risse_size BufferCapacity; // output buffer capacity

public:
	TLG6BitStream(gc_vector<risse_uint8> & outstream) :
		OutStream(outstream),
		BufferBitPos(0),
		BufferBytePos(0),
		Buffer(NULL),
		BufferCapacity(0)
	{
	}

	~TLG6BitStream()
	{
		Flush();
	}

public:
	int GetBitPos() const { return BufferBitPos; }
	risse_size GetBytePos() const { return BufferBytePos; }

	void Flush()
	{
		if(Buffer && (BufferBitPos || BufferBytePos))
		{
			if(BufferBitPos) BufferBytePos ++;
			size_t pos = OutStream.size();
			OutStream.resize(pos + BufferBytePos);
			memcpy(&(OutStream[0]) + pos, Buffer, BufferBytePos);
			BufferBytePos = 0;
			BufferBitPos = 0;
		}
		FreeCollectee(Buffer);
		BufferCapacity = 0;
		Buffer = NULL;
	}

	risse_size GetBitLength() const { return BufferBytePos * 8 + BufferBitPos; }

	void Put1Bit(bool b)
	{
		if(BufferBytePos == BufferCapacity)
		{
			// need more bytes
			risse_size org_cap = BufferCapacity;
			BufferCapacity += 0x1000;
			if(Buffer)
				Buffer = (risse_uint8 *)ReallocCollectee(Buffer, BufferCapacity);
			else
				Buffer = (risse_uint8 *)MallocAtomicCollectee(BufferCapacity);
			memset(Buffer + org_cap, 0, BufferCapacity - org_cap);
		}

		if(b) Buffer[BufferBytePos] |= 1 << BufferBitPos;
		BufferBitPos ++;
		if(BufferBitPos == 8)
		{
			BufferBitPos = 0;
			BufferBytePos ++;
		}
	}

	void PutGamma(int v)
	{
		// Put a gamma code.
		// v must be larger than 0.
		int t = v;
		t >>= 1;
		int cnt = 0;
		while(t)
		{
			Put1Bit(0);
			t >>= 1;
			cnt ++;
		}
		Put1Bit(1);
		while(cnt--)
		{
			Put1Bit(v&1);
			v >>= 1;
		}
	}

	void PutInterleavedGamma(int v)
	{
		// Put a gamma code, interleaved.
		// interleaved gamma codes are:
		//     1 :                   1
		//   <=3 :                 1x0
		//   <=7 :               1x0x0
		//  <=15 :             1x0x0x0
		//  <=31 :           1x0x0x0x0
		// and so on.
		// v must be larger than 0.
		
		v --;
		while(v)
		{
			v >>= 1;
			Put1Bit(0);
			Put1Bit(v&1);
		}
		Put1Bit(1);
	}

	static int GetGammaBitLengthGeneric(int v)
	{
		int needbits = 1;
		v >>= 1;
		while(v)
		{
			needbits += 2;
			v >>= 1;
		}
		return needbits;
	}

	static int GetGammaBitLength(int v)
	{
		// Get bit length where v is to be encoded as a gamma code.
		if(v<=1) return 1;    //                   1
		if(v<=3) return 3;    //                 x10
		if(v<=7) return 5;    //               xx100
		if(v<=15) return 7;   //             xxx1000
		if(v<=31) return 9;   //          x xxx10000
		if(v<=63) return 11;  //        xxx xx100000
		if(v<=127) return 13; //      xxxxx x1000000
		if(v<=255) return 15; //    xxxxxxx 10000000
		if(v<=511) return 17; //   xxxxxxx1 00000000
		return GetGammaBitLengthGeneric(v);
	}

	void PutNonzeroSigned(int v, int len)
	{
		// Put signed value into the bit pool, as length of "len".
		// v must not be zero. abs(v) must be less than 257.
		if(v > 0) v--;
		while(len --)
		{
			Put1Bit(v&1);
			v >>= 1;
		}
	}

	static int GetNonzeroSignedBitLength(int v)
	{
		// Get bit (minimum) length where v is to be encoded as a non-zero signed value.
		// v must not be zero. abs(v) must be less than 257.
		if(v == 0) return 0;
		if(v < 0) v = -v;
		if(v <= 1) return 1;
		if(v <= 2) return 2;
		if(v <= 4) return 3;
		if(v <= 8) return 4;
		if(v <= 16) return 5;
		if(v <= 32) return 6;
		if(v <= 64) return 7;
		if(v <= 128) return 8;
		if(v <= 256) return 9;
		return 10;
	}

	void PutValue(long v, int len)
	{
		// put value "v" as length of "len"
		while(len --)
		{
			Put1Bit(v&1);
			v >>= 1;
		}
	}

};


//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
#define MAX_COLOR_COMPONENTS 4

#define FILTER_TRY_COUNT 16

#define W_BLOCK_SIZE 8
#define H_BLOCK_SIZE 8

//------------------------------ FOR DEBUG
//#define FILTER_TEST
//#define WRITE_ENTROPY_VALUES
//#define WRITE_VSTXT
//------------------------------


/*
	shift-jis

	TLG6 圧縮アルゴリズム(概要)

	TLG6 は吉里吉里２で用いられる画像圧縮フォーマットです。グレースケール、
	24bitビットマップ、8bitアルファチャンネル付き24bitビットマップに対応し
	ています。デザインのコンセプトは「高圧縮率であること」「画像展開において
	十分に高速な実装ができること」です。

	TLG6 は以下の順序で画像を圧縮します。

	1.   MED または 平均法 による輝度予測
	2.   リオーダリングによる情報量削減
	3.   色相関フィルタによる情報量削減
	4.   ゴロム・ライス符号によるエントロピー符号化



	1.   MED または 平均法 による輝度予測

		TLG6 は MED (Median Edge Detector) あるいは平均法による輝度予測を行
		い、予測値と実際の値の誤差を記録します。

		MED は、予測対象とするピクセル位置(下図x位置)に対し、左隣のピクセル
		の輝度をa、上隣のピクセルの輝度をb、左上のピクセルの輝度をcとし、予
		測対象のピクセルの輝度 x を以下のように予測します。

			+-----+-----+
			|  c  |  b  |
			+-----+-----+
			|  a  |  x  |
			+-----+-----+

			x = min(a,b)    (c > max(a,b) の場合)
			x = max(a,b)    (c < min(a,b) の場合)
			x = a + b - c   (その他の場合)

		MEDは、単純な、周囲のピクセルの輝度の平均による予測などと比べ、画像
		の縦方向のエッジでは上のピクセルを、横方向のエッジでは左のピクセルを
		参照して予測を行うようになるため、効率のよい予測が可能です。

		平均法では、a と b の輝度の平均から予測対象のピクセルの輝度 x を予
		測します。以下の式を用います。

			x = (a + b + 1) >> 1

		MED と 平均法は、8x8 に区切られた画像ブロック内で両方による圧縮率を
		比較し、圧縮率の高かった方の方法が選択されます。

		R G B 画像や A R G B 画像に対してはこれを色コンポーネントごとに行い
		ます。

	2.   リオーダリングによる情報量削減

		誤差のみとなったデータは、リオーダリング(並び替え)が行われます。
		TLG6では、まず画像を8×8の小さなブロックに分割します。ブロックは横
		方向に、左のブロックから順に右のブロックに向かい、一列分のブロック
		の処理が完了すれば、その一列下のブロック、という方向で処理を行いま
		す。
		その個々のブロック内では、ピクセルを以下の順番に並び替えます。

		横位置が偶数のブロック      横位置が奇数のブロック
		 1  2  3  4  5  6  7  8     57 58 59 60 61 62 63 64
		16 15 14 13 12 11 10  9     56 55 54 53 52 51 50 49
		17 18 19 20 21 22 23 24     41 42 43 44 45 46 47 48
		32 31 30 29 28 27 26 25     40 39 38 37 36 35 34 33
		33 34 35 36 37 38 39 40     25 26 27 28 29 30 31 32
		48 47 46 45 44 43 42 41     24 23 22 21 20 19 18 17
		49 50 51 52 53 54 55 56      9 10 11 12 13 14 15 16
		64 63 62 61 60 59 58 57      8  7  6  5  4  3  2  1

		このような「ジグザグ」状の並び替えを行うことにより、性質(輝度や色相、
		エッジなど)の似たピクセルが連続する可能性が高くなり、後段のエントロ
		ピー符号化段でのゼロ・ラン・レングス圧縮の効果を高めることができます。

	3.   色相関フィルタによる情報量削減

		R G B 画像 ( A R G B 画像も同じく ) においては、色コンポーネント間
		の相関性を利用して情報量を減らせる可能性があります。TLG6では、以下の
		16種類のフィルタをブロックごとに適用し、後段のゴロム・ライス符号によ
		る圧縮後サイズを試算し、もっともサイズの小さくなるフィルタを適用しま
		す。フィルタ番号と MED/平均法の別は LZSS 法により圧縮され、出力され
		ます。

		0          B        G        R              (フィルタ無し)
		1          B-G      G        R-G
		2          B        G-B      R-B-G
		3          B-R-G    G-R      R
		4          B-R      G-B-R    R-B-R-G
		5          B-R      G-B-R    R
		6          B-G      G        R
		7          B        G-B      R
		8          B        G        R-G
		9          B-G-R-B  G-R-B    R-B
		10         B-R      G-R      R
		11         B        G-B      R-B
		12         B        G-R-B    R-B
		13         B-G      G-R-B-G  R-B-G
		14         B-G-R    G-R      R-B-G-R
		15         B        G-(B<<1) R-(B<<1)

		これらのフィルタは、多くの画像でテストした中、フィルタの使用頻度を調
		べ、もっとも適用の多かった16個のフィルタを選出しました。

	4.   ゴロム・ライス符号によるエントロピー符号化

		最終的な値はゴロム・ライス符号によりビット配列に格納されます。しかし
		最終的な値には連続するゼロが非常に多いため、ゼロと非ゼロの連続数を
		ガンマ符号を用いて符号化します(ゼロ・ラン・レングス)。そのため、ビッ
		ト配列中には基本的に

		1 非ゼロの連続数(ラン・レングス)のガンマ符号
		2 非ゼロの値のゴロム・ライス符号 (連続数分 繰り返される)
		3 ゼロの連続数(ラン・レングス)のガンマ符号

		が複数回現れることになります。

		ガンマ符号は以下の形式で、0を保存する必要がないため(長さ0はあり得な
		いので)、表現できる最低の値は1となっています。

		                  MSB ←    → LSB
		(v=1)                            1
		(2<=v<=3)                      x10          x = v-2
		(4<=v<=7)                    xx100         xx = v-4
		(8<=v<=15)                 xxx1000        xxx = v-8
		(16<=v<=31)             x xxx10000       xxxx = v-16
		(32<=v<=63)           xxx xx100000      xxxxx = v-32
		(64<=v<=127)        xxxxx x1000000     xxxxxx = v-64
		(128<=v<=255)     xxxxxxx 10000000    xxxxxxx = v-128
		(256<=v<=511)    xxxxxxx1 00000000   xxxxxxxx = v-256
		      :                    :                  :
		      :                    :                  :

		ゴロム・ライス符号は以下の方法で符号化します。まず、符号化しようとす
		る値をeとします。eには1以上の正の整数と-1以下の負の整数が存在します。
		これを0以上の整数に変換します。具体的には以下の式で変換を行います。
		(変換後の値をmとする)

		m = ((e >= 0) ? 2*e : -2*e-1) - 1

		この変換により、下の表のように、負のeが偶数のmに、正のeが奇数のmに変
		換されます。

		 e     m
		-1     0
		 1     1
		-2     2
		 2     3
		-3     4
		 3     5
		 :     :
		 :     :

		この値 m がどれぐらいのビット長で表現できるかを予測します。このビッ
		ト長を k とします。k は 0 以上の整数です。しかし予測がはずれ、m が k
		ビットで表現できない場合があります。その場合は、k ビットで表現でき
		なかった分の m、つまり k>>m 個分のゼロを出力し、最後にその終端記号と
		して 1 を出力します。
		m が kビットで表現できた場合は、ただ単に 1 を出力します。そのあと、
		m を下位ビットから k ビット分出力します。

		たとえば e が 66 で k が 5 の場合は以下のようになります。

		m は m = ((e >= 0) ? 2*e : -2*e-1) - 1 の式により 131 となります。
		予測したビット数 5 で表現できる値の最大値は 31 ですので、131 である
		m は k ビットで表現できないということになります。ですので、m >> k
		である 4 個の 0 を出力します。

		0000

		次に、その4個の 0 の終端記号として 1 を出力します。

		10000

		最後に m を下位ビットから k ビット分出力します。131 を２進数で表現
		すると 10000011 となりますが、これの下位から 5 ビット分を出力します。


        MSB←  →LSB
		00011 1 0000      (計10ビット)
		~~~~~ ~ ~~~~
		  c   b   a

		a   (m >> k) 個の 0
		b   a の 終端記号
		c   m の下位 k ビット分

		これが e=66 k=5 を符号化したビット列となります。

		原理的には k の予測は、今までに出力した e の絶対値の合計とそれまでに
		出力した個数を元に、あらかじめ実際の画像の統計から作成されたテーブル
		を参照することで算出します。動作を C++ 言語風に記述すると以下のよう
		になります。

		n = 3; // カウンタ
		a = 0; // e の絶対値の合計

		while(e を読み込む)
		{
			k = table[a][n]; // テーブルから k を参照
			m = ((e >= 0) ? 2*e : -2*e-1) - 1;
			e を m と k で符号化;
			a += m >> 1; // e の絶対値の合計 (ただし m >>1 で代用)
			if(--n < 0) a >>= 1, n = 3;
		}

		4 回ごとに a が半減され n がリセットされるため、前の値の影響は符号化
		が進むにつれて薄くなり、直近の値から効率的に k を予測することができ
		ます。


	参考 :
		Golomb codes
		http://oku.edu.mie-u.ac.jp/~okumura/compression/golomb/
		三重大学教授 奥村晴彦氏によるゴロム符号の簡単な解説
*/

//---------------------------------------------------------------------------
#ifdef WRITE_VSTXT
FILE *vstxt = fopen("vs.txt", "wt");
#endif
#define GOLOMB_GIVE_UP_BYTES 4
static void CompressValuesGolomb(TLG6BitStream &bs, char *buf, int size)
{
	// golomb encoding, -- http://oku.edu.mie-u.ac.jp/~okumura/compression/golomb/

	// run-length golomb method
	bs.PutValue(buf[0]?1:0, 1); // initial value state

	int count;

	int n = TLG6_GOLOMB_N_COUNT - 1; // 個数のカウンタ
	int a = 0; // 予測誤差の絶対値の和

	count = 0;
	for(int i = 0; i < size; i++)
	{
		if(buf[i])
		{
			// write zero count
			if(count) bs.PutGamma(count);

			// count non-zero values
			count = 0;
			int ii;
			for(ii = i; ii < size; ii++)
			{
				if(buf[ii]) count++; else break;
			}

			// write non-zero count
			bs.PutGamma(count);

			// write non-zero values
			for(; i < ii; i++)
			{
				int e = buf[i];
#ifdef WRITE_VSTXT
				fprintf(vstxt, "%d ", e);
#endif
				int k = TLG6GolombBitLengthTable[a][n];
				int m = ((e >= 0) ? 2*e : -2*e-1) - 1;
				risse_size store_limit = bs.GetBytePos() + GOLOMB_GIVE_UP_BYTES;
				bool put1 = true;
				for(int c = (m >> k); c > 0; c--)
				{
					if(store_limit == bs.GetBytePos())
					{
						bs.PutValue(m >> k, 8);
#ifdef WRITE_VSTXT
						fprintf(vstxt, "[ %d ] ", m>>k);
#endif
						put1 = false;
						break;
					}
					bs.Put1Bit(0);
				}
				if(store_limit == bs.GetBytePos())
				{
					bs.PutValue(m >> k, 8);
#ifdef WRITE_VSTXT
					fprintf(vstxt, "[ %d ] ", m>>k);
#endif
					put1 = false;
				}
				if(put1) bs.Put1Bit(1);
				bs.PutValue(m, k);
				a += (m>>1);
				if (--n < 0) {
					a >>= 1; n = TLG6_GOLOMB_N_COUNT - 1;
				}
			}

#ifdef WRITE_VSTXT
			fprintf(vstxt, "\n");
#endif

			i = ii - 1;
			count = 0;
		}
		else
		{
			// zero
			count ++;
		}
	}

	if(count) bs.PutGamma(count);
}
//---------------------------------------------------------------------------
class TryCompressGolomb
{
	risse_size TotalBits; // total bit count
	int Count; // running count
	int N;
	int A;
	bool LastNonZero;

public:
	TryCompressGolomb()
	{
		Reset();
	}

	TryCompressGolomb(const TryCompressGolomb &ref)
	{
		Copy(ref);
	}

	~TryCompressGolomb() { ; }

	void Copy(const TryCompressGolomb &ref)
	{
		TotalBits = ref.TotalBits;
		Count = ref.Count;
		N = ref.N;
		A = ref.A;
		LastNonZero = ref.LastNonZero;
	}

	void Reset()
	{
		TotalBits = 1;
		Count = 0;
		N = TLG6_GOLOMB_N_COUNT - 1;
		A = 0;
		LastNonZero = false;
	}

	int Try(char *buf, int size)
	{
		for(int i = 0; i < size; i++)
		{
			if(buf[i])
			{
				// write zero count
				if(!LastNonZero)
				{
					if(Count)
						TotalBits +=
							TLG6BitStream::GetGammaBitLength(Count);

					// count non-zero values
					Count = 0;
				}

				// write non-zero values
				for(; i < size; i++)
				{
					int e = buf[i];
					if(!e) break;
					Count ++;
					int k = TLG6GolombBitLengthTable[A][N];
					int m = ((e >= 0) ? 2*e : -2*e-1) - 1;
					int unexp_bits = (m>>k);
					if(unexp_bits >= (GOLOMB_GIVE_UP_BYTES*8-8/2))
						unexp_bits = (GOLOMB_GIVE_UP_BYTES*8-8/2)+8;
					TotalBits += unexp_bits + 1 + k;
					A += (m>>1);
					if (--N < 0) {
						A >>= 1; N = TLG6_GOLOMB_N_COUNT - 1;
					}
				}

				// write non-zero count

				i--;
				LastNonZero = true;
			}
			else
			{
				// zero
				if(LastNonZero)
				{
					if(Count)
					{
						TotalBits += TLG6BitStream::GetGammaBitLength(Count);
						Count = 0;
					}
				}

				Count ++;
				LastNonZero = false;
			}
		}
		return TotalBits;
	}

	risse_size Flush()
	{
		if(Count)
		{
			TotalBits += TLG6BitStream::GetGammaBitLength(Count);
			Count = 0;
		}
		return TotalBits;
	}
};
//---------------------------------------------------------------------------
#define DO_FILTER \
	len -= 4; \
	for(d = 0; d < len; d+=4) \
	{ FILTER_FUNC(0); FILTER_FUNC(1); FILTER_FUNC(2); FILTER_FUNC(3); } \
	len += 4; \
	for(; d < len; d++) \
	{ FILTER_FUNC(0); }

static void ApplyColorFilter(char * bufb, char * bufg, char * bufr, int len, int code)
{
	int d;
	unsigned char t;
	switch(code)
	{
	case 0:
		break;
	case 1:
		#define FILTER_FUNC(n) \
			bufr[d+n] -= bufg[d+n], \
			bufb[d+n] -= bufg[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 2:
		#define FILTER_FUNC(n) \
			bufr[d+n] -= bufg[d+n],\
			bufg[d+n] -= bufb[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 3:
		#define FILTER_FUNC(n) \
			bufb[d+n] -= bufg[d+n],\
			bufg[d+n] -= bufr[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 4:
		#define FILTER_FUNC(n) \
			bufr[d+n] -= bufg[d+n],\
			bufg[d+n] -= bufb[d+n],\
			bufb[d+n] -= bufr[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 5:
		#define FILTER_FUNC(n) \
			bufg[d+n] -= bufb[d+n],\
			bufb[d+n] -= bufr[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 6:
		#define FILTER_FUNC(n) \
			bufb[d+n] -= bufg[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 7:
		#define FILTER_FUNC(n) \
			bufg[d+n] -= bufb[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 8:
		#define FILTER_FUNC(n) \
			bufr[d+n] -= bufg[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 9:
		#define FILTER_FUNC(n) \
			bufb[d+n] -= bufg[d+n],\
			bufg[d+n] -= bufr[d+n],\
			bufr[d+n] -= bufb[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 10:
		#define FILTER_FUNC(n) \
			bufg[d+n] -= bufr[d+n], \
			bufb[d+n] -= bufr[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 11:
		#define FILTER_FUNC(n) \
			bufr[d+n] -= bufb[d+n], \
			bufg[d+n] -= bufb[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 12:
		#define FILTER_FUNC(n) \
			bufg[d+n] -= bufr[d+n],\
			bufr[d+n] -= bufb[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 13:
		#define FILTER_FUNC(n) \
			bufg[d+n] -= bufr[d+n],\
			bufr[d+n] -= bufb[d+n],\
			bufb[d+n] -= bufg[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 14:
		#define FILTER_FUNC(n) \
			bufr[d+n] -= bufb[d+n],\
			bufb[d+n] -= bufg[d+n],\
			bufg[d+n] -= bufr[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 15:
		#define FILTER_FUNC(n) \
			t = bufb[d+n]<<1; \
			bufr[d+n] -= t, \
			bufg[d+n] -= t;
		DO_FILTER
		#undef FILTER_FUNC
		break;
//-------------------------------------- ここまでが標準フィルタ
	case 16:
		#define FILTER_FUNC(n) \
			bufg[d+n] -= bufr[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 17:
		#define FILTER_FUNC(n) \
			bufr[d+n] -= bufb[d+n],\
			bufb[d+n] -= bufg[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 18:
		#define FILTER_FUNC(n) \
			bufr[d+n] -= bufb[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 19:
		#define FILTER_FUNC(n) \
			bufb[d+n] -= bufr[d+n],\
			bufr[d+n] -= bufg[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 20:
		#define FILTER_FUNC(n) \
			bufb[d+n] -= bufr[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 21:
		#define FILTER_FUNC(n) \
			bufb[d+n] -= bufg[d+n]>>1;
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 22:
		#define FILTER_FUNC(n) \
			bufg[d+n] -= bufb[d+n]>>1;
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 23:
		#define FILTER_FUNC(n) \
			bufg[d+n] -= bufb[d+n],\
			bufb[d+n] -= bufr[d+n],\
			bufr[d+n] -= bufg[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 24:
		#define FILTER_FUNC(n) \
			bufb[d+n] -= bufr[d+n],\
			bufr[d+n] -= bufg[d+n],\
			bufg[d+n] -= bufb[d+n];
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 25:
		#define FILTER_FUNC(n) \
			bufg[d+n] -= bufr[d+n]>>1;
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 26:
		#define FILTER_FUNC(n) \
			bufr[d+n] -= bufg[d+n]>>1;
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 27:
		#define FILTER_FUNC(n) \
			t = bufr[d+n]>>1; \
			bufg[d+n] -= t, \
			bufb[d+n] -= t;
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 28:
		#define FILTER_FUNC(n) \
			bufr[d+n] -= bufb[d+n]>>1;
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 29:
		#define FILTER_FUNC(n) \
			t = bufg[d+n]>>1; \
			bufr[d+n] -= t, \
			bufb[d+n] -= t;
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 30:
		#define FILTER_FUNC(n) \
			t = bufb[d+n]>>1; \
			bufr[d+n] -= t, \
			bufg[d+n] -= t;
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 31:
		#define FILTER_FUNC(n) \
			bufb[d+n] -= bufr[d+n]>>1;
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 32:
		#define FILTER_FUNC(n) \
			bufr[d+n] -= bufb[d+n]<<1;
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 33:
		#define FILTER_FUNC(n) \
			bufb[d+n] -= bufg[d+n]<<1;
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 34:
		#define FILTER_FUNC(n) \
			t = bufr[d+n]<<1; \
			bufg[d+n] -= t, \
			bufb[d+n] -= t;
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 35:
		#define FILTER_FUNC(n) \
			bufg[d+n] -= bufb[d+n]<<1;
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 36:
		#define FILTER_FUNC(n) \
			bufr[d+n] -= bufg[d+n]<<1;
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 37:
		#define FILTER_FUNC(n) \
			bufr[d+n] -= bufg[d+n]<<1,\
			bufb[d+n] -= bufg[d+n]<<1;
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 38:
		#define FILTER_FUNC(n) \
			bufg[d+n] -= bufr[d+n]<<1;
		DO_FILTER
		#undef FILTER_FUNC
		break;
	case 39:
		#define FILTER_FUNC(n) \
			bufb[d+n] -= bufr[d+n]<<1;
		DO_FILTER
		#undef FILTER_FUNC
		break;

	}

}
//---------------------------------------------------------------------------
static int DetectColorFilter(char *b, char *g, char *r, int size, risse_size &outsize)
{
#ifndef FILTER_TEST
	risse_size minbits = risse_size_max;
	int mincode = -1;

	char bbuf[H_BLOCK_SIZE*W_BLOCK_SIZE];
	char gbuf[H_BLOCK_SIZE*W_BLOCK_SIZE];
	char rbuf[H_BLOCK_SIZE*W_BLOCK_SIZE];
	TryCompressGolomb bc, gc, rc;

	for(int code = 0; code < FILTER_TRY_COUNT; code++)   // 17..27 are currently not used
	{
		// copy bbuf, gbuf, rbuf into b, g, r.
		memcpy(bbuf, b, sizeof(char)*size);
		memcpy(gbuf, g, sizeof(char)*size);
		memcpy(rbuf, r, sizeof(char)*size);

		// copy compressor
		bc.Reset();
		gc.Reset();
		rc.Reset();

		// Apply color filter
		ApplyColorFilter(bbuf, gbuf, rbuf, size, code);

		// try to compress
		risse_size bits;
		bits  = (bc.Try((char*)bbuf, size), bc.Flush());
		if(minbits != risse_size_max && minbits < bits) continue;
		bits += (gc.Try((char*)gbuf, size), gc.Flush());
		if(minbits != risse_size_max && minbits < bits) continue;
		bits += (rc.Try((char*)rbuf, size), rc.Flush());

		if(minbits == risse_size_max || minbits > bits)
			minbits = bits, mincode = code;
	}

	outsize = minbits;

	return mincode;
#else
	static int filter = 0;

	int f = filter;

	filter++;
	if(filter >= FILTER_TRY_COUNT) filter = 0;

	outsize = 0;

	return f;
#endif
}
//---------------------------------------------------------------------------
static void TLG6InitializeColorFilterCompressor(SlideCompressor &c)
{
	risse_uint8 code[4096];
	risse_uint8 dum[4096];
	risse_uint8 *p = code;
	for(int i = 0; i < 32; i++)
	{
		for(int j = 0; j < 16; j++)
		{
			p[0] = p[1] = p[2] = p[3] = i;
			p += 4;
			p[0] = p[1] = p[2] = p[3] = j;
			p += 4;
		}
	}

	risse_size dumlen;
	c.Encode(code, 4096, dum, dumlen);
}
//---------------------------------------------------------------------------
// int ftfreq[256] = {0};
void tTLGImageEncoder::EncodeTLG6(tStreamInstance * stream,
					tProgressCallback * callback,
					int compos)
{
	tStreamAdapter out(stream);

#ifdef WRITE_ENTROPY_VALUES
	FILE *vs = fopen("vs.bin", "wb");
#endif

	int colors = compos;

	risse_size width = 0;
	risse_size height = 0;
	GetDimensions(&width, &height);

	// output stream header
	{
		out.WriteBuffer("TLG6.0\x00raw\x1a\x00", 11);
		out.WriteBuffer(&colors, 1);
		risse_uint32 n = 0;
		out.WriteBuffer(&n, 3); // data flag (0), color type (0), external golomb table (0)
		WriteInt32(width, &out);
		WriteInt32(height, &out);
	}

	// prepare pointers to buffers
	risse_size max_bit_length = 0;

	char *buf[MAX_COLOR_COMPONENTS];
	for(int i = 0; i < MAX_COLOR_COMPONENTS; i++) buf[i] = NULL;
	char *block_buf[MAX_COLOR_COMPONENTS];
	for(int i = 0; i < MAX_COLOR_COMPONENTS; i++) block_buf[i] = NULL;
	risse_uint8 *filtertypes = NULL;

	risse_uint32 * input_lines[H_BLOCK_SIZE+1]; // 入力データへのポインタ,1ブロック高さ分+1
	for(int i = 0; i < H_BLOCK_SIZE+1; i++) input_lines[i] = 0;
	risse_uint32 * prev_line; // 入力データ, 各ブロックの直前のライン

	// allocate buffer
	for(int c = 0; c < colors; c++)
	{
		buf[c] = static_cast<char *>(
			AlignedMallocAtomicCollectee(sizeof(char *)* W_BLOCK_SIZE * H_BLOCK_SIZE * 3, 4));
		block_buf[c] = static_cast<char *>(
			AlignedMallocAtomicCollectee(sizeof(char) *H_BLOCK_SIZE * width, 4));
	}
	int w_block_count = (int)((width - 1) / W_BLOCK_SIZE) + 1;
	int h_block_count = (int)((width - 1) / H_BLOCK_SIZE) + 1;
	filtertypes = static_cast<risse_uint8 *>(
		AlignedMallocAtomicCollectee(sizeof(risse_uint8) * w_block_count * h_block_count, 4));
	prev_line = static_cast<risse_uint32 *>(
		AlignedMallocAtomicCollectee(sizeof(risse_uint32) * width, 4));


	gc_vector<risse_uint8> temp_storage; // エントロピーコーディング用
	TLG6BitStream bs(temp_storage);

	int fc = 0;
	for(risse_size y = 0; y < height; y += H_BLOCK_SIZE)
	{
		if(callback) callback->CallOnProgress(height, y);

		risse_size ylim = y + H_BLOCK_SIZE;
		if(ylim > height) ylim = height;

		// 入力データを受け取る
		//   - そのまえに直前データの 最後のラインをprev_lineにコピー
		if(input_lines[H_BLOCK_SIZE])
			memcpy(prev_line, input_lines[H_BLOCK_SIZE], sizeof(risse_uint32) * width);
		else
			memset(prev_line, 0, sizeof(risse_uint32) * width);
		risse_offset pitch = 0;
		void * lines = GetLines(NULL, y, ylim-y, &pitch, tPixel::pfARGB32);
		input_lines[0] = prev_line; // input_lines 0 は直前のライン
		for(int i = 0; i < H_BLOCK_SIZE; i++)
			input_lines[i+1] = (risse_uint32*)((risse_uint8*)lines + pitch*i); // 各ラインへの先頭

		//
		int gwp = 0;
		int xp = 0;
		for(risse_size x = 0; x < width; x += W_BLOCK_SIZE, xp++)
		{
			risse_size xlim = x + W_BLOCK_SIZE;
			if(xlim > width) xlim = width;
			risse_size bw = xlim - x;

			risse_size p0size; // size of MED method (p=0)
			int minp = 0; // most efficient method (0:MED, 1:AVG)
			int ft; // filter type
			int wp; // write point
			for(int p = 0; p < 2; p++)
			{
				int dbofs = (p+1) * (H_BLOCK_SIZE * W_BLOCK_SIZE);

				// do med(when p=0) or take average of upper and left pixel(p=1)
				for(int c = 0; c < colors; c++)
				{
					int wp = 0;
					for(risse_size yy = y; yy < ylim; yy++)
					{
						const risse_uint32 * sl  =
							(const risse_uint32 *)input_lines[yy-y+1];
						const risse_uint32 * usl = 
							(const risse_uint32 *)input_lines[yy-y  ];

						for(risse_size xx = x; xx < xlim; xx++)
						{
							risse_uint8 pa =
								xx > 0 ? ((sl[xx-1] >> (c*8)) & 0xff) : 0;
							risse_uint8 pb = (usl[xx] >> (c*8)) & 0xff  ;
							risse_uint8 px = (sl[xx] >> (c*8)) & 0xff;

							risse_uint8 py;

//								py = 0;
							if(p == 0)
							{
								risse_uint8 pc =
									xx > 0 ? ((usl[xx-1] >> (c*8)) & 0xff) : 0;
								risse_uint8 min_a_b = pa>pb?pb:pa;
								risse_uint8 max_a_b = pa<pb?pb:pa;

								if(pc >= max_a_b)
									py = min_a_b;
								else if(pc < min_a_b)
									py = max_a_b;
								else
									py = pa + pb - pc;
							}
							else
							{
								py = (pa+pb+1)>>1;
							}
							
							buf[c][wp] = (risse_uint8)(px - py);

							wp++;
						}
					}
				}

				// reordering
				// Transfer the data into block_buf (block buffer).
				// Even lines are stored forward (left to right),
				// Odd lines are stored backward (right to left).

				wp = 0;
				for(risse_size yy = y; yy < ylim; yy++)
				{
					int ofs;
					if(!(xp&1))
						ofs = (yy - y)*bw;
					else
						ofs = (ylim - yy - 1) * bw;
					bool dir; // false for forward, true for backward
					if(!((ylim-y)&1))
					{
						// vertical line count per block is even
						dir = (yy&1) ^ (xp&1);
					}
					else
					{
						// otherwise;
						if(xp & 1)
							dir = (yy&1);
						else
							dir = (yy&1) ^ (xp&1);
					}

					if(!dir)
					{
						// forward
						for(risse_size xx = 0; xx < bw; xx++)
						{
							for(int c = 0; c < colors; c++)
								buf[c][wp + dbofs] =
								buf[c][ofs + xx];
							wp++;
						}
					}
					else
					{
						// backward
						for(int xx = bw - 1; xx >= 0; xx--)
						{
							for(int c = 0; c < colors; c++)
								buf[c][wp + dbofs] =
								buf[c][ofs + xx];
							wp++;
						}
					}
				}
			}


			for(int p = 0; p < 2; p++)
			{
				int dbofs = (p+1) * (H_BLOCK_SIZE * W_BLOCK_SIZE);
				// detect color filter
				risse_size size = 0;
				int ft_;
				if(colors >= 3)
					ft_ = DetectColorFilter(
						buf[0] + dbofs,
						buf[1] + dbofs,
						buf[2] + dbofs, wp, size);
				else
					ft_ = 0;

				// select efficient mode of p (MED or average)
				if(p == 0)
				{
					p0size = size;
					ft = ft_;
				}
				else
				{
					if(p0size >= size)
						minp = 1, ft = ft_;
				}
			}

			// Apply most efficient color filter / prediction method
			wp = 0;
			int dbofs = (minp + 1)  * (H_BLOCK_SIZE * W_BLOCK_SIZE);
			for(risse_size yy = y; yy < ylim; yy++)
			{
				for(risse_size xx = 0; xx < bw; xx++)
				{
					for(int c = 0; c < colors; c++)
						block_buf[c][gwp + wp] = buf[c][wp + dbofs];
					wp++;
				}
			}

			ApplyColorFilter(block_buf[0] + gwp,
				block_buf[1] + gwp, block_buf[2] + gwp, wp, ft);

			filtertypes[fc++] = (ft<<1) + minp;
			gwp += wp;
		}

		// compress values (entropy coding)
		for(int c = 0; c < colors; c++)
		{
			int method;
			CompressValuesGolomb(bs, (char *)(block_buf[c]), gwp);
			method = 0;
#ifdef WRITE_ENTROPY_VALUES
			fwrite(block_buf[c], 1, gwp, vs);
#endif
			risse_size bitlength = bs.GetBitLength();
			if(bitlength & 0xc0000000)
				tIOExceptionClass::Throw(
					RISSE_WS_TR("error on saving TLG6: given image may be too large"));
			// two most significant bits of bitlength are
			// entropy coding method;
			// 00 means Golomb method,
			// 01 means Gamma method (implemented but not used),
			// 10 means modified LZSS method (not yet implemented),
			// 11 means raw (uncompressed) data (not yet implemented).
			if(max_bit_length < bitlength) max_bit_length = bitlength;
			bitlength |= (method << 30);

			// temp_storage に bitlength を書き出す
			temp_storage.resize(temp_storage.size() + 4);
			risse_uint8 * p = &(temp_storage[0]) + temp_storage.size() - 4;
			p[0] = (bitlength >>  0) & 0xff;
			p[1] = (bitlength >>  8) & 0xff;
			p[2] = (bitlength >> 16) & 0xff;
			p[3] = (bitlength >> 24) & 0xff;

			// temp_storage に ビットストリームを書き出す
			bs.Flush();
		}

	}

	// write max bit length
	WriteInt32(max_bit_length, &out);

	// output filter types
	{
		SlideCompressor comp;
		TLG6InitializeColorFilterCompressor(comp);
		risse_uint8 *outbuf = static_cast<risse_uint8 *>(
			AlignedMallocAtomicCollectee(sizeof(risse_uint8)* fc * 2, 4));

		risse_size outlen;
		comp.Encode(filtertypes, fc, outbuf, outlen);
		WriteInt32(outlen, &out);
		out.WriteBuffer(outbuf, outlen);
/*
		{
			int n = 0;
			for(int y = 0; y < h_block_count; y++)
			{
				for(int x = 0; x < w_block_count; x++)
				{
					int t = filtertypes[n++];
					risse_uint8 b;
					if(t & 1) b = 'A'; else b = 'M';
					t >>= 1;
					fprintf(stderr, "%c%x", b, t);
				}
				fprintf(stderr, "\n");
			}
		}
*/
	}

	// write entropy values (temp_storage の中身を出力ストリームに書き出す)
	out.WriteBuffer(&(temp_storage[0]), temp_storage.size());


#ifdef WRITE_ENTROPY_VALUES
	fclose(vs);
#endif
//	printf("med : %d ms\n", medend - medstart);
//	printf("reorder : %d ms\n", reordertick);
/*
	for(int i = 0; i < 256; i++)
	{
		if(ftfreq[i])
		{
			printf("%3d : %d\n", i, ftfreq[i]);
		}
	}
*/
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tTLGImageEncoder::tCallback::tRetValue
	tTLGImageEncoder::tCallback::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(code == ocFuncCall && name.IsEmpty())
	{
		// このオブジェクトに対する関数呼び出し
		args.ExpectArgumentCount(2);

		tString key = args[0].operator tString();
		if(key.GetLength() != 0 && key[0] != RISSE_WC('_'))
		{
			// 先頭が _ で始まるキー名は格納しません
			risse_size key_len = 0;
			char * key_utf8 = key.AsNarrowString(&key_len);
			char key_len_s[21]; key_len_s[20] = '\0';
			snprintf(key_len_s, 20, "%lu", (unsigned long)key_len);
			size_t key_len_s_len = strlen(key_len_s);

			tString value = args[1].operator tString();
			risse_size value_len = 0;
			char * value_utf8 = value.AsNarrowString(&value_len);
			char value_len_s[21]; value_len_s[20] = '\0';
			snprintf(value_len_s, 20, "%lu", (unsigned long)value_len);
			size_t value_len_s_len = strlen(value_len_s);

			// MetaData のサイズを拡張
			size_t org_size = Encoder.MetaData.size();
			Encoder.MetaData.resize(org_size +
				key_len_s_len + 1 + key_len + // key_len:key
				1 + // =
				value_len_s_len + 1 + value_len + // value_len:value
				1 // ,
				);

			// MetaData に追加
			char * p = &(Encoder.MetaData[0]) + org_size;
			strcpy(p, key_len_s);
			p += key_len_s_len;
			strcpy(p, ":");
			p ++;
			strcpy(p, key_utf8);
			p += key_len;
			strcpy(p, "=");
			p ++;
			strcpy(p, value_len_s);
			p += value_len_s_len;
			strcpy(p, ":");
			p ++;
			strcpy(p, value_utf8);
			p += value_len;
			strcpy(p, ",");
		}

		return rvNoError;
	}

	// それ以外の機能はこのインターフェースにはない
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTLGImageEncoder::tTLGImageEncoder() : DictCallback(*this)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTLGImageEncoder::Process(tStreamInstance * stream,
					tProgressCallback * callback,
					tDictionaryInstance * dict)
{
	// 今のところ dict の '_type' は '24' (別名'R8G8B8') と '32'
	// (別名 'A8R8G8B8') を受け付ける
	// (デフォルトは 24)
	int pixel_bytes = 3;
	if(dict)
	{
		tVariant val = dict->Invoke(tSS<'[',']'>(), tVariant(tSS<'_','t','y','p','e'>()));
		if(!val.IsVoid())
		{
			tString str = val.operator tString();
			if(str == tSS<'2','4'>() ||
				str == tSS<'R','8','G','8','B','8'>())
				pixel_bytes = 3;
			else if(str == tSS<'3','2'>() ||
				str == tSS<'A','8','R','8','G','8','B','8'>())
				pixel_bytes = 4;
			else
				tIllegalArgumentExceptionClass::Throw(
					RISSE_WS_TR("unknown TLG pixel format in '_type' parameter"));
		}
	}

	// dict の '_version' は 'tlg6' か 'tlg5' (デフォルトは tlg5)
	int version = 500;
	if(dict)
	{
		tVariant val = dict->Invoke(tSS<'[',']'>(), tVariant(tSS<'_','v','e','r','s','i','o','n'>()));
		if(!val.IsVoid())
		{
			tString str = val.operator tString();
			if(str == tSS<'t','l','g','6'>()) version = 600;
			else if(str == tSS<'t','l','g','5'>()) version = 500;
			else
				tIllegalArgumentExceptionClass::Throw(
					RISSE_WS_TR("unknown TLG version in '_version' parameter"));
		}
	}

	// メタデータを作成するため dict の eachPair を呼び出す
	tMethodArgument & args = tMethodArgument::Allocate(0, 1);
	tVariant block_arg0(&DictCallback);
	args.SetBlockArgument(0, &block_arg0);
	dict->Do(ocFuncCall, NULL, tSS<'e','a','c','h','P','a','i','r'>(), 0, args);

	// TLG sds のヘッダを書き出す
	tStreamAdapter src(stream);
	src.WriteBuffer("TLG0.0\x00sds\x1a\x00    ", 15);
	risse_uint64 rawlen_pos = src.GetPosition() - 4; // 後でここに TLG の本体サイズを書き込む

	// バージョンにしたがって本体を書き出す
	if(version == 500)
		EncodeTLG5(stream, callback, pixel_bytes);
	else if(version == 600)
		EncodeTLG6(stream, callback, pixel_bytes);

	// rawlen を書き込む
	risse_uint64 sds_start = src.GetPosition();
	src.SetPosition(rawlen_pos);
	WriteInt32(static_cast<risse_uint32>(sds_start - rawlen_pos + 4), &src);

	// tags チャンクを書き込む
	src.SetPosition(sds_start);
	src.WriteBuffer("tags", 4); // tags chunk
	WriteInt32(static_cast<risse_uint32>(MetaData.size()), &src);
	if(MetaData.size() > 0) src.WriteBuffer(&(MetaData[0]), MetaData.size());

	// ends チャンクを書き込む
	src.WriteBuffer("ends\0\0\0\0", 8); // ends chunk
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
/**
 * TLGイメージエンコーダファクトリ
 */
class tTLGImageEncoderFactory : public tImageEncoderFactory,
								public singleton_base<tTLGImageEncoderFactory>
{
	virtual tImageEncoder * CreateEncoder() { return new tTLGImageEncoder; }
public:
	/**
	 * コンストラクタ
	 */
	tTLGImageEncoderFactory()
	{
		tImageCodecFactoryManager::instance()->Register(tSS<'.','t','l','g'>(), (tImageEncoderFactory*)this);
		tImageCodecFactoryManager::instance()->Register(tSS<'.','t','l','g','5'>(), (tImageEncoderFactory*)this);
		tImageCodecFactoryManager::instance()->Register(tSS<'.','t','l','g','6'>(), (tImageEncoderFactory*)this);
	}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa



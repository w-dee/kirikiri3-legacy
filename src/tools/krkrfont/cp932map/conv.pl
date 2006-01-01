

$sjis_leading = (0x81 .. 0x9f, 0xe0 .. 0xFC);

open FH, "CP932.TXT" or die;

%cmap = ();

# ファイルを全部読み込んで SJIS -> UNICODE のマッピングを作成

while($line = <FH>)
{
	chomp $line;

	if($line =~ /^0x([A-Za-z0-9]+)\t0x([A-Za-z0-9]+)/)
	{
		$from = sprintf("%04X", hex($1));
		$to   = sprintf("%04X", hex($2));
		$cmap{$from} = $to;
	}
}

open FH, ">../cp932_uni.cpp" or die;
open HH, ">../cp932_uni.h" or die;

$head =  <<EOF;
//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee\@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! \@file
//! \@brief CP932(≒SJIS)->UNICODE 変換
//---------------------------------------------------------------------------
// UNICODEとSJIS(cp932)の変換マップについては unicode.org を参照のこと

EOF

print FH $head;
print HH $head;

print FH <<EOF;
#include "prec.h"


EOF


@submap = ();

@high = ();
@low = ();


for($i = 0; $i < 256; $i ++)
{
	push @high , -1;
	push @low , -1;
}

foreach $key (sort keys %cmap)
{
	$group = hex($key) >> 8;
	$sub = hex($key) & 0xff;
	if($high[$group] == -1)
	{
		$high[$group] = $sub;
		$low[$group] = $sub;
	}
	else
	{
		$high[$group] = $sub if($high[$group] < $sub);
		$low[$group]  = $sub if($low[$group]  > $sub);
	}
}

$high[0] = 255;
$low[0] = 0;

for($i = 0; $i < 256; $i++)
{
	if($high[$i] != -1)
	{
		for($c = $low[$i]; $c <= $high[$i]; $c++)
		{
			$key = sprintf("%02X%02X", $i, $c);
			$cmap{$key} = '0000' if(!defined($cmap{$key}));
		}
	}
}


$prev_group = -1;
foreach $key (sort keys %cmap)
{
	$group = hex($key) >> 8;
	$sub = hex($key) & 0xff;
	if($prev_group != $group)
	{
		printf FH ("};\n\n") if($prev_group != -1);
		printf FH "static const tjs_uint16 TVPSJIS2UNICODE_Submap_map_%02X[%d]={\n/* 0x%02X%02X - 0x%02X%02X */\n",
			$group, $high[$group] - $low[$group] + 1,
			$group, $low[$group] , $group, $high[$group];
		$count = 0;
	}
	printf FH "0x%04XU,", hex($cmap{$key});
	$prev_group = $group;
	
	$count ++;
	if($count == 8)
	{
		print FH "\n";
		$count = 0;
	}
}
printf FH  ("};\n\n");


print FH <<EOF;

//---------------------------------------------------------------------------
//! \@brief		CP932 ( = Shift-JIS) -> UNICODE table lookup structure
//---------------------------------------------------------------------------
struct tTVPSJIS2UNICODE_Submap
{
	tjs_uint8	low;
	tjs_uint8	high;
	const tjs_uint16	*submap;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! \@brief		CP932 ( = Shift-JIS) -> UNICODE table
//---------------------------------------------------------------------------
static const tTVPSJIS2UNICODE_Submap TVPSJIS2UNICODE_Submap[128] = {
EOF


for($i = 128; $i < 256; $i++)
{
	printf FH "/*0x%02X*/ " , $i;
	if($high[$i] == -1)
	{
		printf FH ("{ 0x00, 0x00, NULL },\n");
	}
	else
	{
		printf FH "{ 0x%02X, 0x%02X, TVPSJIS2UNICODE_Submap_map_%02X },\n",
			$low[$i], $high[$i], $i;
	}
}

print FH <<EOF;


};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! \@brief		CP932一文字をUNICODEに変換する
//! \@param		in 入力 MBCS
//! \@param		out 出力 UNICODE (wchar_t)
//! \@return		変換に成功すれば真、失敗すれば偽
//---------------------------------------------------------------------------
static bool inline _TVPSJISToUnicode(const char * & in, tjs_char *out)
{
	const unsigned char * & p = (const unsigned char * &)in;

	// 1 byte 文字をチェック
	tjs_uint16 ch = TVPSJIS2UNICODE_Submap_map_00[p[0]];
	if(ch != 0x0000U)
	{
		// 1byte
		if(out) *out = static_cast<tjs_char>(ch);
		in ++;
		return true;
	}

	if(p[0] >= 0x80)
	{
		 const tTVPSJIS2UNICODE_Submap & submap =
		 	TVPSJIS2UNICODE_Submap[p[0]-0x80];
		if(submap.submap && submap.low <= p[1] && submap.high >= p[1])
		{
			ch = submap.submap[p[1]-submap.low];
			if(ch != 0x0000U)
			{
				// 2 byte map found
				if(out) *out = static_cast<tjs_char>(ch);
				in += 2;
				return true;
			}
		}
	}

	// no conversion available
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! \@brief		CP932一文字をUNICODEに変換する
//! \@param		in 入力 sjisコード  例: '漢' = 0x8abf  '0' = 0x0030
//! \@return		出力 UNICODE (wchar_t) 変換に失敗すれば 0
//---------------------------------------------------------------------------
tjs_char TVPSJISToUnicode(tjs_uint sjis)
{
	char buf[3];
	const char * p = buf;
	if(sjis >= 0x100)
	{
		buf[0] = static_cast<char>(static_cast<unsigned char>((sjis >> 8) & 0xff));
		buf[1] = static_cast<char>(static_cast<unsigned char>((sjis     ) & 0xff));
		buf[2] = '\\0';
	}
	else
	{
		buf[0] = static_cast<char>(static_cast<unsigned char>(sjis));
		buf[1] = '\\0';
	}
	tjs_char out;
	if(_TVPSJISToUnicode(p, &out)) return out;
	return 0;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! \@brief		CP932文字列をUNICODEに変換する
//! \@param		in 入力 MBCS 文字列
//! \@param		out 出力 UNICODE (wchar_t) 文字列 (NULLの場合は書き込まれない)
//! \@return		出力された文字数
//!				(最後に\\0は書き込まれないしその文字数も含まれないので注意)
//!				(tjs_size)-1 = 異常な文字が見つかった
//---------------------------------------------------------------------------
tjs_size TVPSJISToUnicodeString(const char * in, tjs_char *out)
{
	// convert input Shift-JIS (CP932) string to output wide string
	int count = 0;
	while(*in)
	{
		tjs_char c;
		if(out)
		{
			if(!_TVPSJISToUnicode(in, &c))
				return static_cast<tjs_size>(-1); // invalid character found
			*out++ = c;
		}
		else
		{
			if(!_TVPSJISToUnicode(in, NULL))
				return static_cast<tjs_size>(-1); // invalid character found
		}
		count ++;
	}
	return count;
}
//---------------------------------------------------------------------------

EOF


print HH <<EOF;

#ifndef _CP932_UNI_
#define _CP932_UNI_

tjs_size TVPSJISToUnicodeString(const char * in, tjs_char *out);
tjs_char TVPSJISToUnicode(tjs_uint sjis);

#endif

EOF


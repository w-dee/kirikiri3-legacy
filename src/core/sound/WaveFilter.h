//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Waveフィルタインターフェースの定義
//---------------------------------------------------------------------------
#ifndef _WAVEFILTERH_
#define _WAVEFILTERH_

#include "WaveDecoder.h"

/*

RisaのWaveフィルタについて

■ アーキテクチャ

  RisaのWaveフィルタは、複数のフィルタを直列して接続することができるインサー
  ションフィルタである。すべてのフィルタは tRisaWaveFilter インターフェース
  を実装する。

  フィルタは、デコーダを駆動してPCMデータを読み出し、ループ等の制御を行う
  tRisaWaveLoopManager と、OpenAL の Buffer にデータを流し込む
  tRisaOpenALBuffer の中間に挟まれて動作する。

  tRisaWaveLoopManager も tRisaWaveFilter インターフェースを実装しており、
  ほかの tRisaWaveFilter や tRisaOpenALBuffer からは一つのフィルタとして見
  える。

■ データの流れ

  この一連のフィルタがつながった物をフィルタチェーンと呼ぶ。典型的なフィル
  タチェーンは下図のようになる。

  +----------------------+    +-----------------+    +------------------+
  | tRisaWaveLoopManager | -> | tRisaWaveFilter | -> | tRisaOpenALBuffer|
  +----------------------+    +-----------------+    +------------------+

  tRisaOpenBuffer はバッファに流し込むデータの要求が発生すると、データを
  直前にある tRisaWaveFilter のインスタンスに要求する。tRisaWaveFilter の
  インスタンスは、より直前にある tRisaWaveFilter のインスタンスに要求を
  渡す。このようにして、要求はチェーンをさかのぼる形で(データの流れとは逆に)
  順次行われる。

  データは、tRisaWaveLoopManager から順にチェーンをたどる方向で渡され、
  最終的に tRisaOpenALBuffer に渡される。

  フィルタはすべて SetInputメソッドをもち、どのフィルタにデータを要求すれば
  よいかはこのメソッドで設定される。

■ PCMフォーマットについて

  フィルタチェーン間を流れるPCMのフォーマットは、すべて「そのデータを送り出
  す側の」フィルタが決定してよい。どのようなPCMフォーマットになるかは、
  そのフィルタのGetFormatメソッドを呼ぶことにより知ることが出来る。
  フィルタや tRisaOpenALBuffer は、Render メソッドによりデータを要求する
  前には必ず 直前のフィルタに対して GetFormat を呼び出して形式をチェック
  しなければならない。もしFormatの変更に対応できないならば、Render メソッド
  でfalseを返し、それ以上処理を続行できないことを示してよい。その場合は再生
  は中断する。

  tRisaOpenALBuffer は、OpenAL 側の制限により、再生中に周波数やチャンネル数
  を変えることが出来ない。そのため、これらが変わったときは再生は中断する。
  ただし、PCMフォーマットの形式 (8bit か16bitか、整数かfloatか等) は変わっ
  てもtRisaOpenALBuffer側で変換を行うため、再生が中断されることはない。

■ Renderメソッドについて

  tRisaOpenALBuffer は、ストリーミング再生時は約0.125秒ごと、非ストリーミング
  再生時は 約512KBごとのPCMデータを要求する。
  フィルタの Render メソッドには、要求するサンプルグラニュール数としてsamples
  パラメータが渡され、実際に書き込まれたサンプルグラニュール数としてwritten
  を返す。フィルタは、要求されたサンプルグラニュールよりも少ない数のみをデ
  コードしてもよい。この際、あとに続くデータが無いならば偽を返し、あるならば
  真を返す。データを要求する側は、Render メソッドが偽を返さない限りは、デ
  コードが終了したと見なすことは出来ない。必要な量のデータになるまで繰り返し
  チェーンの前にあるフィルタに対してRenderメソッドを通じて要求を行ってよい。
  その際、前述の通り、Renderメソッドを呼ぶ前には毎回 GetFormat メソッドを呼
  び、PCM形式に変更があるかどうかをチェックしなければならない。
  逆に言えば、1回の Render メソッドの呼び出しで返されるデータの途中でPCM
  フォーマットを変えることは許されない。PCMフォーマットを変えたい場合は、
  writtenを samples よりも少なく返し、後ろのフィルタが再度 GetFormat と
  Render メソッドを呼ぶようにしなければならない。

■ tRisaWaveEvent

  tRisaWaveEvent は、サウンド上に設定されたイベントを表す構造体である。
  このイベントとは Loop Tuner でいう「ラベル」で、Risa はこのラベルを通過する
  際にイベントを発生することが出来る。

  tRisaWaveEvent::Position はオリジナルのデコーダ上での PCM 位置を表し、
  tRisaOpenALBuffer や tRisaOpenALSource がこの値を使うことはないが、
  tRisaWaveEvent::Offset は "一回のRenderメソッドの呼び出により返された
  PCM の先頭からのサンプルグラニュール数" を表している。
  
                               v ここにイベント offset=400sg
  +------------++-------++------------+
  |  A  800sg  ||B 400sg||  C 800sg   |
  +------------++-------++------------+
  |<--------------2000sg------------->|

  たとえば、あるフィルタで 2000sg  (sg=サンプルグラニュール) の PCM データを
  要求されたが、前のフィルタが 800sg, 400sg, 800sg の３回に分けて PCM デー
  タを返し、また３回目の呼び出しで返された PCM データ C の中のオフセット
  400sg にイベントがあった場合、このフィルタは、後のフィルタに対して
  一気に2000sg のデータを返してもよいが、その場合はこのイベントのオフセッ
  トは1600sg に修正されなければならないことに注意が必要である。

  フィルタによっては入力として受け取る PCM サンプル数と、出力する PCM サン
  プル数が、PCMの圧縮や延長によって変わる物がある ( Resampling や
  Time Stretch のようなフィルタ )。
  この場合は、正しい位置でイベントが発生するように、フィルタ中で
  tRisaWaveEvent::Offset の値を修正してから次のフィルタに渡さなければならない。

  フィルタは、Render メソッドに渡された events 配列に、必要であれば上記のよ
  うな変換を行った後、追加を行う。
  フィルタに渡す events 配列の内容をクリアするのは呼び出し側の責任である。
  クリアしなければ、すでに存在する内容に追加される。

■ tRisaWaveSegment

  tRisaWaveSegment は、そのフィルタが返すデータが、オリジナルのデコーダ上の
  もともとどこの位置にあったデータであるか、を表している。
  これらはセグメントと呼ばれ、オリジナルのデコーダ上のどの位置からどれだけの
  長さのデータが渡されたか、を示している。
  ループが行われている状態では、ループによりオリジナルのデコーダ上のデコード
  位置が変更されるため、一回の Render の呼び出しで複数のセグメントが返される
  可能性がある。

  tRisaWaveSegment には Length と FilteredLength という二つの長さを表す
  メンバがある。フィルタによっては入力として受け取る PCM サンプル数と、出力
  するPCMのサンプル数が、圧縮や延長によって変わる物がある ( Resampling や
  Time Stretch のようなフィルタ )。このような場合は、FilteredLength は、
  実際に出力した長さに修正しなければならない。そうでない場合は、FilteredLength
  はLengthと同じ長さになる。

  フィルタは、Render メソッドに渡された segments 配列に、必要であれば上記のよ
  うな変換を行った後、追加を行う。
  フィルタに渡す segments 配列の内容をクリアするのは呼び出し側の責任である。
  クリアしなければ、すでに存在する内容に追加される。


*/


//---------------------------------------------------------------------------
//! @brief 再生セグメント情報 (tRisaWaveFilter::Renderメソッド で返される)
//---------------------------------------------------------------------------
struct tRisaWaveSegment
{
	//! @brief コンストラクタ
	tRisaWaveSegment(risse_int64 start, risse_int64 length)
		{ Start = start; Length = FilteredLength = length; }
	tRisaWaveSegment(risse_int64 start, risse_int64 length, risse_int64 filteredlength)
		{ Start = start; Length = length; FilteredLength = filteredlength; }
	risse_int64 Start; //!< オリジナルデコーダ上でのセグメントのスタート位置 (PCM サンプルグラニュール数単位)
	risse_int64 Length; //!< オリジナルデコーダ上でのセグメントの長さ (PCM サンプルグラニュール数単位)
	risse_int64 FilteredLength; //!< フィルタ後の長さ (PCM サンプルグラニュール数単位)
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief 再生イベント情報 (tRisaWaveFilter::Renderメソッド で返される)
//---------------------------------------------------------------------------
struct tRisaWaveEvent
{
	//! @brief コンストラクタ
	risse_int64 Position; //!< オリジナルデコーダ上でのラベル位置 (PCM サンプルグラニュール数単位)
	ttstr Name; //!< イベント名
	risse_int Offset;
		/*!< オフセット
			@note
			This member will be set in tRisaWaveLoopManager::Render,
			and will contain the sample granule offset from first decoding
			point at call of tRisaWaveLoopManager::Render().
		*/
	//! @brief コンストラクタ
	tRisaWaveEvent()
	{
		Position = 0;
		Offset = 0;
	}
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief	 フィルタインターフェース
//---------------------------------------------------------------------------
class tRisaWaveFilter
{
public:
	virtual ~tRisaWaveFilter() {;}

	virtual void SetInput(boost::shared_ptr<tRisaWaveFilter> input) = 0;
		/*!<
			@brief	入力フィルタの設定
		*/

	virtual bool Render(void *dest, risse_uint samples, risse_uint &written,
		std::vector<tRisaWaveSegment> &segments,
		std::vector<tRisaWaveEvent> &events) = 0;
		/*!<
			@brief	デコードを行う
			@return まだデータが残っているかどうか
			@note
				このメソッドを呼ぶ前に毎回 GetFormat を呼んで、フォーマットが
				変わっていないかどうかを検証すること。
		*/
	virtual const tRisaWaveFormat & GetFormat() = 0;
		/*!<
			@brief PCM形式を取得する
			@return PCM形式への参照
			@note ここで返されるのは PCM 形式の参照だが、その値の有効性は
				次にこのフィルタの別のメソッドが呼ばれか、あるいは
				このフィルタが消滅するかのどちらか早い方までである。
		*/
};
//---------------------------------------------------------------------------


#endif


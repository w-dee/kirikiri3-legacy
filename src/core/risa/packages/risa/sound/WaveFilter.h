//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Waveフィルタインターフェースの定義
//---------------------------------------------------------------------------
#ifndef _WAVEFILTERH_
#define _WAVEFILTERH_

#include "risa/packages/risa/sound/WaveDecoder.h"
#include "risa/packages/risa/sound/WaveSegmentQueue.h"

namespace Risa {
//---------------------------------------------------------------------------

/*

※ 名称に注意
   Risse 用クラスの WaveFilter は filter/BasicWaveFilter.h 内で定義されている。
   Risse 用クラスで WaveFilter と言った場合は、実際には tWaveFilterInstance の
   派生クラスである。
   C++ クラスの tWaveFilter はより上位のインターフェースであり、PhaseVocoder や
   Reverb にとどまらずに WaveLoopManager もこれを継承するものである。


RisaのWaveフィルタについて

■ アーキテクチャ

  RisaのWaveフィルタは、複数のフィルタを直列して接続することができるインサー
  ションフィルタである。すべてのフィルタは tWaveFilter インターフェース
  を実装する。

  フィルタは、デコーダを駆動してPCMデータを読み出し、ループ等の制御を行う
  tWaveLoopManager と、OpenAL の Buffer にデータを流し込む
  tOpenALBuffer の中間に挟まれて動作する。

  tWaveLoopManager も tWaveFilter インターフェースを実装しており、
  ほかの tWaveFilter や tOpenALBuffer からは一つのフィルタとして見
  える。

■ データの流れ

  この一連のフィルタがつながった物をフィルタチェーンと呼ぶ。典型的なフィル
  タチェーンは下図のようになる。

  +------------------+    +-------------+    +---------------+
  | tWaveLoopManager | -> | tWaveFilter | -> | tOpenALBuffer |
  +------------------+    +-------------+    +---------------+

  tOpenALBuffer はバッファに流し込むデータの要求が発生すると、データを
  直前にある tWaveFilter のインスタンスに要求する。tWaveFilter の
  インスタンスは、より直前にある tWaveFilter のインスタンスに要求を
  渡す。このようにして、要求はチェーンをさかのぼる形で(データの流れとは逆に)
  順次行われる。

  データは、tWaveLoopManager から順にチェーンをたどる方向で渡され、
  最終的に tOpenALBuffer に渡される。

  フィルタはすべて SetInputメソッドをもち、どのフィルタにデータを要求すれば
  よいかはこのメソッドで設定される。

■ PCMフォーマットについて

  フィルタチェーン間を流れるPCMのフォーマットは、すべて「そのデータを送り出
  す側の」フィルタが決定してよい。どのようなPCMフォーマットになるかは、
  そのフィルタのGetFormatメソッドを呼ぶことにより知ることが出来る。
  フィルタや tOpenALBuffer は、Render メソッドによりデータを要求する
  前には必ず 直前のフィルタに対して GetFormat を呼び出して形式をチェック
  しなければならない。もしFormatの変更に対応できないならば、Render メソッド
  でfalseを返し、それ以上処理を続行できないことを示してよい。その場合は再生
  は中断する。

  tOpenALBuffer は、OpenAL 側の制限により、再生中に周波数やチャンネル数
  を変えることが出来ない。そのため、これらが変わったときは再生は中断する。
  ただし、PCMフォーマットの形式 (8bit か16bitか、整数かfloatか等) は変わっ
  てもtOpenALBuffer側で変換を行うため、再生が中断されることはない。

■ Renderメソッドについて

  tOpenALBuffer は、ストリーミング再生時は約0.125秒ごと、非ストリーミング
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

■ Resetメソッドについて

  再生開始前には必ずResetメソッドが呼ばれる。各フィルタでは、このResetメソッ
  ドをトリガにして、内部状態の初期化などを行うことができる。Resetメソッドは
  SetInputメソッドより後に呼ばれる。
  フィルタは、Resetメソッドで自己のフィルタのリセット処理が完了したら、
  SetInputメソッドで指定された入力フィルタのResetメソッドも同様に呼びださな
  ければならない。


■ tWaveSegmentQueue

  tWaveSegmentQueue はサウンドの断片が、それがもともとのPCMファイルの
  どの位置にあったデータなのかや、どの位置にイベント (Waveサウンド上に
  自由に設定できる、ループチューナで言うところのラベル)があるかを
  保持している。
  tOpenALBuffer はサウンドの再生位置情報を取得するため、
  tWaveSegmentQueueを利用する。この情報はtWaveLoopManagerで生成され、
  フィルタを通って最終的にtOpenALBufferに行き着く。

  

*/





//---------------------------------------------------------------------------
/**
 * フィルタインターフェース
 */
class tWaveFilter : public tCollectee
{
public:
	virtual ~tWaveFilter() {;}

	virtual void Reset() = 0;
		/*!<
			@brief	フィルタのリセット
		*/

	virtual void SetInput(tWaveFilter * input) = 0;
		/*!<
			@brief	入力フィルタの設定
		*/

	virtual void SuggestFormat(const tWaveFormat & format) = 0;
		/*!<
			@brief PCM形式を提案する
			@note フィルタを利用する側にとって利用しやすいタイプのPCM形式を
			提案する。フィルタはこれに必ずしも従う必要はないし、
			フィルタを利用する側がこの形式に実際に変更されることを期待
			することもできない。
		*/

	virtual bool Render(void *dest, risse_uint samples, risse_uint &written,
		tWaveSegmentQueue & segmentqueue) = 0;
		/*!<
			@brief	デコードを行う
			@param	dest		PCMサンプル格納先バッファ
			@param	samples		格納するサンプルグラニュール数
			@param	written		実際にどれだけのサンプルグラニュール数が書き込まれたのかを返す
			@param	segmentqueue	セグメントキューの書き込み先。内容はクリアされずに追加される。
			@return まだデータが残っているかどうか
			@note
				このメソッドを呼ぶ前に毎回 GetFormat を呼んで、フォーマットが
				変わっていないかどうかを検証すること。
		*/

	virtual const tWaveFormat & GetFormat() = 0;
		/*!<
			@brief PCM形式を取得する
			@return PCM形式への参照
			@note ここで返されるのは PCM 形式の参照だが、その値の有効性は
				次にこのフィルタの別のメソッドが呼ばれか、あるいは
				このフィルタが消滅するかのどちらか早い方までである。
		*/
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


#endif


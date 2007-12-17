//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RINA ID管理
//---------------------------------------------------------------------------
#ifndef RINAIDREGISTRY_H
#define RINAIDREGISTRY_H

#include "base/utils/Singleton.h"

namespace Rina {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		４つの文字から 32bit id を生成するためのテンプレートクラス
//---------------------------------------------------------------------------
template <int a, int b, int c, int d>
struct tFourCharId
{
	enum { value = 
		((a&0xff) << 24) + 
		((b&0xff) << 16) +
		((c&0xff) <<  8) +
		((d&0xff)      ) };
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		IDレジストリ
//---------------------------------------------------------------------------
class tIdRegistry : public Risa::singleton_base<tIdRegistry>, Risa::manual_start<tIdRegistry>
{
	typedef Risa::singleton_base<tIdRegistry> inherited;

	tCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

public:
	//! @brief		エッジを流れるデータの型の情報
	struct tEdgeData
	{
		risse_uint32 Id;	//!< ID
		tString ShortName;	//!< 短い名前(Risseの識別子として使えるようなもの; i18n不可)
		tString ShortDesc;	//!< 短い説明(i18n可)
		tString LongDesc;	//!< 長い説明(i18n可)
	};

	typedef risse_uint32 tRenderGeneration;
	static const tRenderGeneration UndefinedRenderGeneration = 0;

private:
	//! @brief		エッジを流れるデータの型のマップ
	typedef gc_map<risse_uint32, tEdgeData> tEdgeDataMap;

	tEdgeDataMap EdgeDataMap; //!< エッジを流れるデータの型のマップ

	risse_uint32 RenderGeneration; //!< レンダリングの世代(普通1ずつふえ、前のレンダリングと次のレンダリングを区別する)

public:
	//! @brief		コンストラクタ
	tIdRegistry();

	//! @brief		エッジを流れるデータの型を追加する
	//! @param		data		エッジを流れるデータの型の情報
	//! @return		登録に成功したかどうか(真=成功,偽=すでにデータがあった)
	bool RegisterEdgeData(const tEdgeData & data);

	//! @brief		レンダリング世代をインクリメントして得る
	//! @return		新しいレンダリング世代
	risse_uint32 GetNewRenderGeneration();

};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
}

#endif

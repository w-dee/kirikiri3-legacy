//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief スタンダードIOとの入出力を行うクラス
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/common/RisaException.h"
#include "risse/include/risseExceptionClass.h"
#include "risa/packages/risa/stdio/StandardIO.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(36650,50749,64532,16674,7599,41607,50003,48070);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
tStandardIOStreamInstance::tStandardIOStreamInstance()
{
	Stream = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStandardIOStreamInstance::initialize(int index, const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	info.InitializeSuperClass(); // スーパークラスのコンストラクタを呼ぶ

	switch(index)
	{
	case 0:		Stream = stdin;		break;
	case 1:		Stream = stdout;	break;
	case 2:		Stream = stderr;	break;
	default:
		tIllegalArgumentExceptionClass::Throw(RISSE_WS_TR("stream index must be 0, 1 or 2"));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStandardIOStreamInstance::dispose()
{
	volatile tSynchronizer sync(this); // sync

	Stream = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tStandardIOStreamInstance::get(const tOctet & buf)
{
	volatile tSynchronizer sync(this); // sync

	if(!Stream) tInaccessibleResourceExceptionClass::Throw();

	return fread(const_cast<risse_uint8 *>(buf.Pointer()), 1, buf.GetLength(), Stream);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tStandardIOStreamInstance::put(const tOctet & buf)
{
	volatile tSynchronizer sync(this); // sync

	if(!Stream) tInaccessibleResourceExceptionClass::Throw();

	return fwrite(buf.Pointer(), 1, buf.GetLength(), Stream);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStandardIOStreamInstance::flush()
{
	volatile tSynchronizer sync(this); // sync

	if(!Stream) tInaccessibleResourceExceptionClass::Throw();

	fflush(Stream);
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tStandardIOStreamClass,
		(tSS<'S','t','a','n','d','a','r','d','I','O','S','t','r','e','a','m'>()),
		static_cast<tClassBase*>(engine->GetPackageGlobal(tSS<'s','t','r','e','a','m'>()).
				GetPropertyDirect(engine, tSS<'S','t','r','e','a','m'>()).GetObjectInterface()))
	RISSE_BIND_CONSTRUCTORS
	BindFunction(this, ss_get, &tStandardIOStreamInstance::get);
	BindFunction(this, ss_put, &tStandardIOStreamInstance::put);
	BindFunction(this, ss_flush, &tStandardIOStreamInstance::flush);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * StandardIOStream クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','s','t','d','i','o'>,
	tStandardIOStreamClass>;
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
/**
 * risa.stdio のパッケージのメンバを初期化するためのシングルトンインスタンス
 */
class tRisaStdioPackageMemberInitializer : public tPackageMemberInitializer,
	public singleton_base<tRisaStdioPackageMemberInitializer>
{
public:
	/**
	 * コンストラクタ
	 */
	tRisaStdioPackageMemberInitializer()
	{
		tPackageRegisterer<tSS<'r','i','s','a','.','s','t','d','i','o'> >::instance()->
			AddInitializer(this);
	}

	/**
	 * パッケージを初期化する
	 * @param engine	スクリプトエンジンインスタンス
	 * @param name		パッケージ名
	 * @param global	パッケージグローバル
	 */
	void Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global)
	{
		tStandardIOStreamClass * cls = tClassHolder<tStandardIOStreamClass>::instance()->GetClass();
		// stdin, stdout, stderr を作成して登録する
		tVariant stdin_v  = cls->Invoke(ss_new, (risse_int64)0);
		tVariant stdout_v = cls->Invoke(ss_new, (risse_int64)1);
		tVariant stderr_v = cls->Invoke(ss_new, (risse_int64)2);

		global.RegisterFinalConstMember(tSS<'s','t','d','i','n'>(),     stdin_v );
		global.RegisterFinalConstMember(tSS<'s','t','d','o','u','t'>(), stdout_v);
		global.RegisterFinalConstMember(tSS<'s','t','d','e','r','r'>(), stderr_v);
	}
};
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
} // namespace Risa




// Risse テスト用プログラム

#include "prec.h"
#include "risseString.h"
#include "risseVariant.h"
#include "gc_cpp.h"
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/dir.h>



#include "risseScriptEngine.h"
#include "risseClass.h"
#include "risseStaticStrings.h"
#include "risseNativeBinder.h"
#include "risseStringTemplate.h"
#include "risseObjectClass.h"
#include "risseExceptionClass.h"
#include "risseDataClass.h"

RISSE_DEFINE_SOURCE_ID(1760,7877,28237,16679,32159,45258,11038,1907);


using namespace Risse;

//---------------------------------------------------------------------------
//! @brief		パッケージ検索のためのインターフェース
//---------------------------------------------------------------------------
class tPackageFileSystemInterfaceImpl : public tPackageFileSystemInterface
{
public:
	virtual void List(const tString & dir, gc_vector<tString> & files)
	{
		wxString native_name(ConvertToNativePathDelimiter(dir.AsWxString()));
		wxDir dir_object;
		if(!dir_object.Open(native_name)) return; // ディレクトリを開けなかった
		wxString filename;
		bool cont;

		// ファイルを列挙
		cont = dir_object.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
		while(cont)
		{
			if(!filename.StartsWith(wxT(".")))
			{
				files.push_back(tString(filename));
			}
			cont = dir_object.GetNext(&filename);
		}
		// ディレクトリを列挙
		cont = dir_object.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
		while(cont)
		{
			if(!filename.StartsWith(wxT(".")))
			{
				files.push_back(tString(filename + wxT("/")));
			}
			cont = dir_object.GetNext(&filename);
		}
	}

	virtual int GetType(const tString & file)
	{
		wxString native_name(ConvertToNativePathDelimiter(file.AsWxString()));
		if(wxDirExists(native_name)) return 2;
		if(wxFileExists(native_name)) return 1;
		return 0;
	}

	virtual tString ReadFile(const tString & file)
	{
		// name を取ってきて eval する
		wxFile file_object;
		wxString native_name(ConvertToNativePathDelimiter(file.AsWxString()));
		if(file_object.Open(native_name))
		{
			// 内容を読み込む
			size_t length = file_object.Length();
			char *buf = new (PointerFreeGC) char [length + 1];
			file_object.Read(buf, length);
			buf[length] = 0;

			return tString(buf);
		}
		else
		{
			// TODO: IOException
			return tString();
		}
	}

	static wxString ConvertToNativePathDelimiter(const wxString & path)
	{
		wxString ret(path);
		if(ret.Length() > 0)
		{
			wxChar pathsep = static_cast<wxChar>(wxFileName::GetPathSeparator());
			for(size_t n = 0; ret.GetChar(n); n++)
			{
				if(ret.GetChar(n) == wxT('/'))
					ret.GetWritableChar(n) = pathsep;
			}
		}
		return ret;
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		テスト用ネイティブ実装インスタンス
//---------------------------------------------------------------------------
class tTestInstance : public tObjectBase
{
	tString str;

	virtual ~tTestInstance() {;} //!< おそらく呼ばれないデストラクタ

public:
	void construct() {;}
	void initialize(const tNativeCallInfo & info)
	{
		// 親クラスの同名メソッドを呼び出す
		info.InitializeSuperClass();
	}

	const tString & getString() { return str; }
	const tString & get_propString() const { return str; }
	void set_propString(const tString & v) { str = v; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		"Test" クラス
//---------------------------------------------------------------------------
RISSE_DEFINE_CLASS_BEGIN(tTestClass, tClassBase, tTestInstance, itNormal)
RISSE_DEFINE_CLASS_END()

RISSE_IMPL_CLASS_BEGIN(tTestClass, (tSS<'T','e','s','t'>()), engine->ObjectClass)
	RISSE_BIND_CONSTRUCTORS
	BindFunction(this, tSS<'g','e','t','S','t','r','i','n','g'>(), &tTestInstance::getString);
	BindProperty(this, tSS<'p','r','o','p','S','t','r','i','n','g'>(),
		&tTestInstance::get_propString, &tTestInstance::set_propString);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		暫定 Script クラス
//---------------------------------------------------------------------------
class tScriptClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tScriptClass(tScriptEngine * engine) :
		tClassBase(tSS<'S','c','r','i','p','t'>(), engine->ObjectClass)
	{
		RegisterMembers();
	}

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers()
	{
		BindFunction(this, ss_construct, &tScriptClass::construct);
		BindFunction(this, ss_initialize, &tScriptClass::initialize);
		BindFunction(this, tSS<'p','r','i','n','t'>(), &tScriptClass::print);
	}

	static void construct()
	{
	}

	static void initialize()
	{
	}

	static void print(const tMethodArgument & args)
	{
		fflush(stderr);
		fflush(stdout);
		for(risse_size i = 0; i < args.GetArgumentCount(); i++)
			FPrint(stdout, args[i].operator tString().c_str());
		fflush(stderr);
		fflush(stdout);
	}

public:
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		暫定 Data の派生クラス "Pixel"
//---------------------------------------------------------------------------
class tPixelClass : public tDataClassBase
{
	typedef tDataClassBase inherited; //!< 親クラスの typedef
public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tPixelClass(tScriptEngine * engine) :
		inherited(tSS<'P','i','x','e','l'>(), engine->DataClass)
	{
		RegisterMembers();
	}

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers()
	{
		// 親クラスの RegisterMembers を呼ぶ
		inherited::RegisterMembers();

		// クラスに必要なメソッドを登録する
		// 基本的に ss_construct と ss_initialize は各クラスごとに
		// 記述すること。たとえ construct の中身が空、あるいは initialize の
		// 中身が親クラスを呼び出すだけだとしても、記述すること。

		// construct は tPrimitiveClass 内ですでに登録されている

		BindFunction(this, ss_ovulate, &tPixelClass::ovulate,
			tMemberAttribute().Set(tMemberAttribute::mcConst).Set(tMemberAttribute::ocFinal));
		BindFunction(this, ss_initialize, &tPixelClass::initialize,
			tMemberAttribute().Set(tMemberAttribute::mcConst).Set(tMemberAttribute::ocFinal));
		BindProperty(this, tSS<'b'>(), &tPixelClass::get_b, &tPixelClass::set_b,
			tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
									tMemberAttribute(tMemberAttribute::ocFinal)) );
		BindProperty(this, tSS<'g'>(), &tPixelClass::get_g, &tPixelClass::set_g,
			tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
									tMemberAttribute(tMemberAttribute::ocFinal)) );
		BindProperty(this, tSS<'r'>(), &tPixelClass::get_r, &tPixelClass::set_r,
			tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
									tMemberAttribute(tMemberAttribute::ocFinal)) );
		BindProperty(this, tSS<'a'>(), &tPixelClass::get_a, &tPixelClass::set_a,
			tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
									tMemberAttribute(tMemberAttribute::ocFinal)) );
		BindProperty(this, tSS<'v','a','l','u','e'>(), &tPixelClass::get_value, &tPixelClass::set_value,
			tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
									tMemberAttribute(tMemberAttribute::ocFinal)) );

	}

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate()
	{
		// このクラスのインスタンスは作成できないので例外を投げる
		tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
		return tVariant();
	}

public: // Risse用メソッドなど
	static void initialize(const tNativeCallInfo & info)
	{
	}


	static void get_b(const tNativePropGetInfo & info)
	{
		if(info.result) *info.result = (risse_int64)*static_cast<risse_uint32*>(info.This.GetData()) & 0x000000ff;
	}

	static void set_b(const tNativePropSetInfo & info)
	{
		risse_uint32 * p = static_cast<risse_uint32*>(info.This.GetData());
		*p  &= 0xffffff00;
		*p |= (risse_uint32)(risse_int64)info.value;
	}

	static void get_g(const tNativePropGetInfo & info)
	{
		if(info.result) *info.result = (risse_int64)(*static_cast<risse_uint32*>(info.This.GetData()) & 0x0000ff00) >> 8;
	}

	static void set_g(const tNativePropSetInfo & info)
	{
		risse_uint32 * p = static_cast<risse_uint32*>(info.This.GetData());
		*p  &= 0xffff00ff;
		*p |= (risse_uint32)(risse_int64)info.value << 8;
	}

	static void get_r(const tNativePropGetInfo & info)
	{
		if(info.result) *info.result = (risse_int64)(*static_cast<risse_uint32*>(info.This.GetData()) & 0x00ff0000) >> 16;
	}

	static void set_r(const tNativePropSetInfo & info)
	{
		risse_uint32 * p = static_cast<risse_uint32*>(info.This.GetData());
		*p  &= 0xff00ffff;
		*p |= (risse_uint32)(risse_int64)info.value << 16;
	}

	static void get_a(const tNativePropGetInfo & info)
	{
		if(info.result) *info.result = (risse_int64)(*static_cast<risse_uint32*>(info.This.GetData()) & 0xff000000) >> 24;
	}

	static void set_a(const tNativePropSetInfo & info)
	{
		risse_uint32 * p = static_cast<risse_uint32*>(info.This.GetData());
		*p  &= 0x00ffffff;
		*p |= ((risse_uint32)(risse_int64)info.value) << 24;
	}


	static void get_value(const tNativePropGetInfo & info)
	{
		if(info.result) *info.result = (risse_int64)*static_cast<risse_uint32*>(info.This.GetData());
	}

	static void set_value(const tNativePropSetInfo & info)
	{
		risse_uint32 * p = static_cast<risse_uint32*>(info.This.GetData());
		*p = (risse_int64)info.value;
	}


};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		警告情報の出力先インターフェース
//---------------------------------------------------------------------------
class tWarningOutput : public tLineOutputInterface
{
	void Output(const tString & info)
	{
		fflush(stderr);
		fflush(stdout);
		FPrint(stderr, RISSE_WS("warning: "));
		FPrint(stderr, info.c_str());
		FPrint(stderr, RISSE_WS("\n"));
		fflush(stderr);
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		アプリケーションクラス
//---------------------------------------------------------------------------
class Application : public wxAppConsole
{
public:
	virtual bool OnInit();
	virtual int OnRun();

private:
	wxLocale locale;
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// アプリケーションメインルーチン定義
//---------------------------------------------------------------------------
IMPLEMENT_APP_CONSOLE(Application)
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		アプリケーションが開始するとき
//! @return		成功すれば真
//---------------------------------------------------------------------------
bool Application::OnInit()
{
	return true;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		アプリケーションメインルーチン
//! @return		終了コード
//---------------------------------------------------------------------------
int Application::OnRun()
{
	if(argc < 2)
	{
		fprintf(stderr, "Specify a file name to read.\n");
		return 0;
	}

	// Risse スクリプトエンジンを作成する
	try
	{
		tScriptEngine engine;
		engine.SetAssertionEnabled(true);
		engine.SetWarningOutput(new tWarningOutput());
		engine.SetPackageFileSystem(new tPackageFileSystemInterfaceImpl());

		// risse パッケージの packagePath に "lib" を追加する
		tVariant risse_package = engine.GetRissePackageGlobal();
		tVariant packagePath = risse_package.GetPropertyDirect_Object(
			tSS<'p','a','c','k','a','g','e','P','a','t','h'>());
		packagePath.Invoke_Object(tSS<'p','u','s','h'>(), tVariant(tString(tSS<'l','i','b'>())));

		// Script クラスを追加する
		(new tScriptClass(&engine))->
				RegisterInstance(engine.GetMainPackageGlobal());

		// Test クラスを追加する
		(new tTestClass(&engine))->
				RegisterInstance(engine.GetMainPackageGlobal());

		// Pixel クラスを追加する
		tPixelClass * pixel_class;
		(pixel_class = new tPixelClass(&engine))->
				RegisterInstance(engine.GetMainPackageGlobal());

		// なんかひとつ Pixel クラスのインスタンスを作ってみる
		risse_uint32 pixel_value = 0xaa112233;
		tVariant pixel_instance = tVariant(pixel_class, (void*)&pixel_value);
		engine.GetMainPackageGlobal().RegisterMember(tSS<'p','i','x','e','l'>(), pixel_instance);


		// 入力ファイルを開く
		wxFile file;
		if(file.Open(argv[1]))
		{
			// スクリプトのある場所をカレントディレクトリに指定する
			wxFileName filename(argv[1]);
			wxFileName::SetCwd(filename.GetPath());

			// 内容を読み込む
			size_t length = file.Length();
			char *buf = new (PointerFreeGC) char [length + 1];
			file.Read(buf, length);
			buf[length] = 0;

			// 内容を評価する
			tVariant result;
			engine.Evaluate((tString)(buf), argv[1], 0, &result);
			FPrint(stderr,(RISSE_WS("========== Result ==========\n")));
			fflush(stderr);
			fflush(stdout);
			FPrint(stdout, result.AsHumanReadable().c_str());
		}

	}
	catch(const tTemporaryException * te)
	{
		te->Dump();
	}
	catch(const tVariant * e)
	{
		fflush(stderr);
		fflush(stdout);
		wxFprintf(stdout, wxT("exception: %s\n"), e->operator tString().AsWxString().c_str());
	}
	return 0;
}
//---------------------------------------------------------------------------


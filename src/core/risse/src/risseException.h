//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse 例外処理
//---------------------------------------------------------------------------


#ifndef risseExceptionH
#define risseExceptionH

#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAssert.h"
#include "risseString.h"
#include "risseGC.h"

namespace Risse
{

class tRisseVariantBlock;
typedef tRisseVariantBlock tRisseVariant;

#if 0
//---------------------------------------------------------------------------
// macro
//---------------------------------------------------------------------------
#ifdef RISSE_SUPPORT_VCL
	#define RISSE_CONVERT_TO_RISSE_EXCEPTION_ADDITIONAL \
		catch(const Exception &e) \
		{ \
			Risse_eRisseError(e.Message.c_str()); \
		}
#else
	#define RISSE_CONVERT_TO_RISSE_EXCEPTION_ADDITIONAL
#endif


#define RISSE_CONVERT_TO_RISSE_EXCEPTION \
	catch(const eRisseSilent &e) \
	{ \
		throw e; \
	} \
	catch(const eRisseScriptException &e) \
	{ \
		throw e; \
	} \
	catch(const eRisseScriptError &e) \
	{ \
		throw e; \
	} \
	catch(const eRisseError &e) \
	{ \
		throw e; \
	} \
	catch(const eRisse &e) \
	{ \
		Risse_eRisseError(e.GetMessageString()); \
	} \
	catch(const std::exception &e) \
	{ \
		Risse_eRisseError(e.what()); \
	} \
	catch(const wchar_t *text) \
	{ \
		Risse_eRisseError(text); \
	} \
	catch(const char *text) \
	{ \
		Risse_eRisseError(text); \
	} \
	RISSE_CONVERT_TO_RISSE_EXCEPTION_ADDITIONAL
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// RisseGetExceptionObject : retrieves risse 'Exception' object
//---------------------------------------------------------------------------
extern void RisseGetExceptionObject(tRisse *risse, tRisseVariant *res, tRisseVariant &msg,
	tRisseVariant *trace/* trace is optional */ = NULL);
//---------------------------------------------------------------------------

#ifdef RISSE_SUPPORT_VCL
	#define RISSE_CONVERT_TO_RISSE_EXCEPTION_OBJECT_ADDITIONAL(_risse, _result_condition, _result_addr, _before_catched, _when_catched) \
	catch(EAccessViolation &e) \
	{ \
		_before_catched; \
		if(_result_condition) \
		{ \
			tRisseVariant msg(e.Message.c_str()); \
			RisseGetExceptionObject((_risse), (_result_addr), msg, NULL); \
		} \
		_when_catched; \
	} \
	catch(Exception &e) \
	{ \
		_before_catched; \
		if(_result_condition) \
		{ \
			tRisseVariant msg(e.Message.c_str()); \
			RisseGetExceptionObject((_risse), (_result_addr), msg, NULL); \
		} \
		_when_catched; \
	}
#else
	#define RISSE_CONVERT_TO_RISSE_EXCEPTION_OBJECT_ADDITIONAL(_risse, _result_condition, _result_addr, _before_catched, _when_catched)
#endif


#define RISSE_CONVERT_TO_RISSE_EXCEPTION_OBJECT(risse, result_condition, result_addr, before_catched, when_catched) \
	catch(eRisseSilent &e) \
	{ \
		throw e; \
	} \
	catch(eRisseScriptException &e) \
	{ \
		before_catched \
		if(result_condition) *(result_addr) = e.GetValue(); \
		when_catched; \
	} \
	catch(eRisseScriptError &e) \
	{ \
		before_catched \
		if(result_condition) \
		{ \
			tRisseVariant msg(e.GetMessageString()); \
			tRisseVariant trace(e.GetTrace()); \
			RisseGetExceptionObject((risse), (result_addr), msg, &trace); \
		} \
		when_catched; \
	} \
	catch(eRisse &e)  \
	{  \
		before_catched \
		if(result_condition) \
		{ \
			tRisseVariant msg(e.GetMessageString()); \
			RisseGetExceptionObject((risse), (result_addr), msg, NULL); \
		} \
		when_catched; \
	} \
	catch(exception &e) \
	{ \
		before_catched \
		if(result_condition) \
		{ \
			tRisseVariant msg(e.what()); \
			RisseGetExceptionObject((risse), (result_addr), msg, NULL); \
		} \
		when_catched; \
	} \
	RISSE_CONVERT_TO_RISSE_EXCEPTION_OBJECT_ADDITIONAL(risse, result_condition, result_addr, before_catched, when_catched) \
	catch(...) \
	{ \
		before_catched \
		if(result_condition) (result_addr)->Clear(); \
		when_catched; \
	}


#endif

//---------------------------------------------------------------------------
//! @brief		Risseの例外の基本クラス
//---------------------------------------------------------------------------
class eRisse : public tRisseCollectee
{
public:
	//! @brief		デフォルトコンストラクタ
	eRisse() {;}

	//! @brief		コピーコンストラクタ
	eRisse(const eRisse&) {;}

	//! @brief		代入演算子
	eRisse& operator= (const eRisse& e) { return *this; }

	//! @brief		デストラクタ
	virtual ~eRisse() {;}

	//! @brief		例外メッセージを得る
	//! @return		例外メッセージ
	virtual const tRisseString & GetMessageString() const;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Risseの汎用例外クラス
//---------------------------------------------------------------------------
class eRisseError : public eRisse
{
public:
	//! @brief		コンストラクタ
	eRisseError(const tRisseString & Msg) :
		Message(Msg) {;}

	//! @brief		メッセージを取得	@return		例外メッセージ
	const tRisseString & GetMessageString() const { return Message; }

	//! @brief		メッセージを追加	@param msg 追加するメッセージ
	void AppendMessage(const tRisseString & msg) { Message += msg; }

private:
	tRisseString Message; //!< 例外メッセージ

public:
	//! @brief		例外を投げる
	//! @param		msg		メッセージ
	static void Throw(const tRisseString & msg);
};
//---------------------------------------------------------------------------


class tRisseScriptBlockBase;
//---------------------------------------------------------------------------
//! @brief		スクリプトエラーの基本クラス
//---------------------------------------------------------------------------
class eRisseScriptError : public eRisseError
{
	tRisseScriptBlockBase * Block; //!< スクリプトブロック
	risse_size Position; //!< 例外が発生したコードポイント位置

public:
	tRisseScriptBlockBase * GetScriptBlock() const { return Block; } //!< スクリプトブロックを得る
	risse_size GetPosition() const { return Position; } //!< コードポイント位置を得る

	//! @brief		コンストラクタ
	//! @param		Msg		例外メッセージ
	//! @param		block	例外の発生したスクリプトブロック
	//! @param		pos		例外の発生した位置
	eRisseScriptError(const tRisseString &  msg,
		tRisseScriptBlockBase *block, risse_size pos):
			eRisseError(BuildMessage(msg, block, pos)), Block(block), Position(pos) {;}

	//! @brief		コピーコンストラクタ
	eRisseScriptError(const eRisseScriptError &ref) :
		eRisseError(ref), Block(ref.Block), Position(ref.Position) {;}

	//! @brief		"error at script.rs line 2" の "at XXXX line YYYY" を付け足した
	//!				メッセージを生成して返す
	//! @param		msgbase		メッセージ本体
	//! @param		block		スクリプトブロック
	//! @param		pos			スクリプト上の位置
	//! @return		組み立てられたメッセージ
	static tRisseString BuildMessage(const tRisseString & msgbase,
		tRisseScriptBlockBase *block, risse_size pos);

	//! @brief		例外を投げる
	//! @param		msg		例外メッセージ
	//! @param		block	例外の発生したスクリプトブロック
	//! @param		pos		例外の発生した位置
	static void Throw(const tRisseString &  msg,
		tRisseScriptBlockBase *block, risse_size pos);
};
//---------------------------------------------------------------------------

/*
//---------------------------------------------------------------------------
//! @brief		コンパイルエラークラス
//---------------------------------------------------------------------------
class eRisseCompileError : public eRisseScriptError
{
public:
	//! @brief		コンストラクタ
	//! @param		Msg		例外メッセージ
	//! @param		block	例外の発生したスクリプトブロック
	//! @param		pos		例外の発生した位置
	eRisseCompileError(const tRisseString &  msg,
		tRisseScriptBlockBase *block, risse_size pos) :
		eRisseScriptError(msg, block, pos) {;}

	//! @brief		コピーコンストラクタ
	eRisseCompileError(const eRisseCompileError &ref) : eRisseScriptError(ref) {;}

public:
	//! @brief		例外を投げる
	//! @param		msg		例外メッセージ
	//! @param		block	例外の発生したスクリプトブロック
	//! @param		pos		例外の発生した位置
	static void Throw(const tRisseString & msg,
			tRisseScriptBlockBase *block, risse_size pos);
};
//---------------------------------------------------------------------------
*/

//---------------------------------------------------------------------------
//! @brief		スクリプトで発生させた例外の基本クラス(暫定)
//---------------------------------------------------------------------------
class eRisseScriptException : public eRisseScriptError
{
	const tRisseVariant * Value; // 例外の値

public:
	//! @brief		コンストラクタ
	//! @param		Msg		例外メッセージ
	//! @param		block	例外の発生したスクリプトブロック
	//! @param		pos		例外の発生した位置
	//! @param		value		例外の値
	eRisseScriptException(const tRisseString &  msg,
		tRisseScriptBlockBase *block, risse_size pos, const tRisseVariant & value);

	//! @brief		コピーコンストラクタ
	eRisseScriptException(const eRisseScriptException &ref);

	//! @brief		例外の値を得る	@return	例外の値
	const tRisseVariant & GetValue() const { return *Value; }

public:
	//! @brief		例外を投げる
	//! @param		msg		例外メッセージ
	//! @param		block	例外の発生したスクリプトブロック
	//! @param		pos		例外の発生した位置
	//! @param		value	例外の値
	static void Throw(const tRisseString & msg,
			tRisseScriptBlockBase *block, risse_size pos, const tRisseVariant & value);
};
//---------------------------------------------------------------------------


















#if 0

void Risse_eRisseError(const tRisseString & msg);
void Risse_eRisseError(const risse_char* msg);
//---------------------------------------------------------------------------
class eRisseVariantError : public eRisseError
{
public:
	eRisseVariantError(const tRisseString & Msg) :
		eRisseError(Msg) {;}

	eRisseVariantError(const eRisseVariantError &ref) :
		eRisseError(ref) {;}
};
//---------------------------------------------------------------------------
void Risse_eRisseVariantError(const tRisseString & msg);
void Risse_eRisseVariantError(const risse_char * msg);
//---------------------------------------------------------------------------
class tRisseScriptBlock;
class tRisseInterCodeContext;
class eRisseScriptError : public eRisseError
{
	class tScriptBlockHolder
	{
	public:
		tScriptBlockHolder(tRisseScriptBlock *block);
		~tScriptBlockHolder();
		tScriptBlockHolder(const tScriptBlockHolder &holder);
		tRisseScriptBlock *Block;
	} Block;

	risse_int Position;

	tRisseString Trace;

public:
	tRisseScriptBlock * GetBlockNoAddRef() { return Block.Block; }

	risse_int GetPosition() const { return Position; }

	risse_int GetSourceLine() const;

	const risse_char * GetBlockName() const;

	const tRisseString & GetTrace() const { return Trace; }

	bool AddTrace(tRisseScriptBlock *block, risse_int srcpos);
	bool AddTrace(tRisseInterCodeContext *context, risse_int codepos);
	bool AddTrace(const tRisseString & data);

	eRisseScriptError(const tRisseString &  Msg,
		tRisseScriptBlock *block, risse_int pos);

	eRisseScriptError(const eRisseScriptError &ref) :
		eRisseError(ref), Block(ref.Block), Position(ref.Position), Trace(ref.Trace) {;}
};
//---------------------------------------------------------------------------
void Risse_eRisseScriptError(const tRisseString &msg, tRisseScriptBlock *block, risse_int srcpos);
void Risse_eRisseScriptError(const risse_char *msg, tRisseScriptBlock *block, risse_int srcpos);
void Risse_eRisseScriptError(const tRisseString &msg, tRisseInterCodeContext *context, risse_int codepos);
void Risse_eRisseScriptError(const risse_char *msg, tRisseInterCodeContext *context, risse_int codepos);
//---------------------------------------------------------------------------
class eRisseScriptException : public eRisseScriptError
{
	tRisseVariant Value;
public:
	tRisseVariant & GetValue() { return Value; }

	eRisseScriptException(const tRisseString & Msg,
		tRisseScriptBlock *block, risse_int pos, tRisseVariant &val)
			: eRisseScriptError(Msg, block, pos), Value(val) {}

	eRisseScriptException(const eRisseScriptException &ref) :
		eRisseScriptError(ref), Value(ref.Value) {;}
};
//---------------------------------------------------------------------------
void Risse_eRisseScriptException(const tRisseString &msg, tRisseScriptBlock *block,
	risse_int srcpos, tRisseVariant &val);
void Risse_eRisseScriptException(const risse_char *msg, tRisseScriptBlock *block,
	risse_int srcpos, tRisseVariant &val);
void Risse_eRisseScriptException(const tRisseString &msg, tRisseInterCodeContext *context,
	risse_int codepos, tRisseVariant &val);
void Risse_eRisseScriptException(const risse_char *msg, tRisseInterCodeContext *context,
	risse_int codepos, tRisseVariant &val);
//---------------------------------------------------------------------------
class eRisseCompileError : public eRisseScriptError
{
public:
	eRisseCompileError(const tRisseString &  Msg, tRisseScriptBlock *block, risse_int pos) :
		eRisseScriptError(Msg, block, pos) {;}

	eRisseCompileError(const eRisseCompileError &ref) : eRisseScriptError(ref) {;}

};
//---------------------------------------------------------------------------
void Risse_eRisseCompileError(const tRisseString & msg, tRisseScriptBlock *block, risse_int srcpos);
void Risse_eRisseCompileError(const risse_char * msg, tRisseScriptBlock *block, risse_int srcpos);
//---------------------------------------------------------------------------
void RisseThrowFrom_risse_error(risse_error hr, const risse_char *name = NULL);
#define RISSE_THROW_IF_ERROR(x) { \
	risse_error ____er; ____er = (x); if(RISSE_FAILED(____er)) RisseThrowFrom_risse_error(____er); }
//---------------------------------------------------------------------------
} // namespace Risse
//---------------------------------------------------------------------------


#endif

//---------------------------------------------------------------------------
} // namespace Risse
//---------------------------------------------------------------------------


#endif // #ifndef risseExceptionH





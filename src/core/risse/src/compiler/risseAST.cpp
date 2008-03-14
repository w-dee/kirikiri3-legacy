//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief AST(抽象構文木) の操作
//---------------------------------------------------------------------------
#include "../prec.h"
#include "risseAST.h"
#include "risseSSAForm.h"
#include "risseSSAStatement.h"
#include "risseSSAVariable.h"
#include "risseCompilerNS.h"
#include "risseCompiler.h"
#include "risseSSABlock.h"
#include "../risseExceptionClass.h"
#include "../risseScriptBlockClass.h"
#include "../risseStaticStrings.h"
#include "../risseScriptEngine.h"

// 名前表の読み込み
#undef risseASTH
#define RISSE_AST_DEFINE_NAMES
#include "risseAST.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(29091,1243,20617,17999,61570,21800,19479,2186);


/*
	ここから AST のダンプに関わる部分
	(後ろのほうに SSA 形式の生成に関わる部分がある)
*/

//---------------------------------------------------------------------------
void tASTNode::Dump(tString & result, risse_int level)
{
	result += ASTNodeTypeNames[Type];
	tString comment = GetDumpComment();
	if(!comment.IsEmpty())
		result += tString(RISSE_WS(" (%1)"), comment);
#ifdef RISSE_TEXT_OUT_CRLF
	result += RISSE_WS("\r\n");
#else
	result += RISSE_WS("\n");
#endif

	level ++;

	risse_size child_count = GetChildCount();
	for(risse_size i = 0; i < child_count; i++)
	{
		result += tString(RISSE_WS(" ")).Times(level) + GetChildNameAt(i);
		result += RISSE_WC(':');
		tASTNode * child = GetChildAt(i);
		if(child)
			child->Dump(result, level);
		else
		#ifdef RISSE_TEXT_OUT_CRLF
			result += RISSE_WS("(null)\r\n");
		#else
			result += RISSE_WS("(null)\n");
		#endif
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_List::GetChildNameAt(risse_size index) const
{
	if(index < Array.size())
	{
		risse_char buf[40];
		return tString(RISSE_WS("node")) + ::Risse::int64_to_str(index, buf);
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_OneExpression::GetChildNameAt(risse_size index) const
{
	if(index == 0) return RISSE_WS("expression"); else return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_Context::GetDumpComment() const
{
	return ASTContextTypeNames[ContextType];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_Import::GetChildNameAt(risse_size index) const
{
	if(index == 0) return RISSE_WS("package");
	if(index == 1) return RISSE_WS("id");
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_ImportAs::GetChildNameAt(risse_size index) const
{
	if(index == 0) return RISSE_WS("name");
	if(index == 1) return RISSE_WS("as");
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_ImportList::GetChildNameAt(risse_size index) const
{
	if(index < inherited::GetChildCount())
	{
		risse_char buf[40];
		return tString(RISSE_WS("item")) + ::Risse::int64_to_str(index, buf);
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_ImportLoc::GetChildNameAt(risse_size index) const
{
	if(index < inherited::GetChildCount())
	{
		risse_char buf[40];
		return tString(RISSE_WS("item")) + ::Risse::int64_to_str(index, buf);
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_Factor::GetDumpComment() const
{
	tString ret = ASTFactorTypeNames[FactorType];
	if(FactorType == aftConstant)
	{
		ret += RISSE_WS(" ");
		ret += Value.AsHumanReadable();
	}
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_Id::GetDumpComment() const
{
	if(IsPrivate)
		return RISSE_WS("@ ") + Name.AsHumanReadable();
	else
		return Name.AsHumanReadable();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_Trinary::GetDumpComment() const
{
	return ASTTrinaryTypeNames[TrinaryType];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_VarDecl::GetChildNameAt(risse_size index) const
{
	if(index < inherited::GetChildCount())
	{
		risse_char buf[40];
		return tString(RISSE_WS("item")) + ::Risse::int64_to_str(index, buf);
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_VarDeclPair::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("name");
	case 1: return RISSE_WS("initializer");
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_MemberSel::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("object");
	case 1: return RISSE_WS("membername");
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_MemberSel::GetDumpComment() const
{
	tString str = ASTMemberAccessTypeNames[AccessType];
	tString flags = Flags.AsString();
	if(!flags.IsEmpty())
	{
		str += RISSE_WS(" flags=<");
		str += flags;
		str += RISSE_WS(">");
	}
	tString attribs = Attribute.AsString();
	if(!attribs.IsEmpty())
	{
		str += RISSE_WS(" attribute=<");
		str += attribs;
		str += RISSE_WS(">");
	}
	return str;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_Unary::GetChildNameAt(risse_size index) const
{
	if(index == 0)
		return RISSE_WS("child");
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_Unary::GetDumpComment() const
{
	return ASTUnaryTypeNames[UnaryType];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_Binary::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("child0");
	case 1: return RISSE_WS("child1");
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_Binary::GetDumpComment() const
{
	return ASTBinaryTypeNames[BinaryType];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_Trinary::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("child0");
	case 1: return RISSE_WS("child1");
	case 2: return RISSE_WS("child2");
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_InContextOf::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("instance");
	case 1: return RISSE_WS("context");
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_InContextOf::GetDumpComment() const
{
	tString ret;
	if(Context == NULL) ret = RISSE_WS("dynamic");
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tASTNode_Array::Strip()
{
	while(GetChildCount() > 0 && GetLastChild() == NULL) PopChild();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_Array::GetChildNameAt(risse_size index) const
{
	if(index < inherited::GetChildCount())
	{
		risse_char buf[40];
		return tString(RISSE_WS("item")) + ::Risse::int64_to_str(index, buf);
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_Dict::GetChildNameAt(risse_size index) const
{
	if(index < inherited::GetChildCount())
	{
		risse_char buf[40];
		return tString(RISSE_WS("item")) + ::Risse::int64_to_str(index, buf);
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_DictPair::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("name");
	case 1: return RISSE_WS("value");
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_RegExp::GetDumpComment() const
{
	return tString(RISSE_WS("pattern=")) + Pattern.AsHumanReadable() +
		RISSE_WS(", flags=") + Flags.AsHumanReadable();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_If::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("condition");
	case 1: return RISSE_WS("true");
	case 2: return RISSE_WS("false");
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_While::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("condition");
	case 1: return RISSE_WS("body");
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_While::GetDumpComment() const
{
	if(SkipFirstCheck)
		return RISSE_WS("SkipFirstCheck");
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_For::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("initializer");
	case 1: return RISSE_WS("condition");
	case 2: return RISSE_WS("iterator");
	case 3: return RISSE_WS("body");
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_ExpressionBlock::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("object");
	case 1: return RISSE_WS("body");
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_Case::GetDumpComment() const
{
	if(GetExpression() == NULL) return RISSE_WS("default");
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_Try::GetChildNameAt(risse_size index) const
{
	if(index == 0) return RISSE_WS("body");
	if(index == inherited::GetChildCount() + 1) return RISSE_WS("finally");
	index --;
	if(index < inherited::GetChildCount())
	{
		risse_char buf[40];
		return tString(RISSE_WS("catch")) + ::Risse::int64_to_str(index, buf);
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_Catch::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("condition");
	case 1: return RISSE_WS("body");
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_Catch::GetDumpComment() const
{
	return tString(RISSE_WS("variable=")) + Name.AsHumanReadable();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_FuncCall::GetChildNameAt(risse_size index) const
{
	if(index == 0) return RISSE_WS("function");
	index --;
	if(index < inherited::GetChildCount())
	{
		risse_char buf[40];
		return tString(RISSE_WS("argument")) + ::Risse::int64_to_str(index, buf);
	}
	index -= inherited::GetChildCount();
	if(index < Blocks.size())
	{
		risse_char buf[40];
		return tString(RISSE_WS("block")) + ::Risse::int64_to_str(index, buf);
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_FuncCall::GetDumpComment() const
{
	tString ret;
	if(CreateNew) ret += RISSE_WS("create_new");
	if(Omit) { if(!ret.IsEmpty()) ret += RISSE_WC(' '); ret += RISSE_WS("omit_arg"); }
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_FuncCallArg::GetChildNameAt(risse_size index) const
{
	if(index == 0)
		return RISSE_WS("expression");
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_FuncCallArg::GetDumpComment() const
{
	if(Expand)
		return RISSE_WS("expand");
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_FuncDecl::GetChildNameAt(risse_size index) const
{
	if(index == inherited::GetChildCount() + Blocks.size()) return RISSE_WS("body");
	if(index < inherited::GetChildCount())
	{
		risse_char buf[40];
		return tString(RISSE_WS("argument")) + ::Risse::int64_to_str(index, buf);
	}
	index -= inherited::GetChildCount();
	if(index < Blocks.size())
	{
		risse_char buf[40];
		return tString(RISSE_WS("block")) + ::Risse::int64_to_str(index, buf);
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_FuncDecl::GetDumpComment() const
{
	tString attrib = Attribute.AsString();
	if(!attrib.IsEmpty()) attrib += RISSE_WC(' ');
	if(Name.IsEmpty()) attrib += RISSE_WS("anonymous"); else attrib += Name;
	if(IsBlock) attrib += RISSE_WS(" block");
	return attrib;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_FuncDeclArg::GetChildNameAt(risse_size index) const
{
	if(index == 0)
		return RISSE_WS("initializer");
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_FuncDeclArg::GetDumpComment() const
{
	tString ret = Name.AsHumanReadable();
	if(Collapse)
		ret += RISSE_WS(", collapse");
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_FuncDeclBlock::GetDumpComment() const
{
	return Name.AsHumanReadable();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_PropDecl::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return tString(RISSE_WS("setter(argument=")) +
			SetterArgumentName.AsHumanReadable() + RISSE_WS(")");
	case 1: return RISSE_WS("getter");
	}
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_PropDecl::GetDumpComment() const
{
	tString attrib = Attribute.AsString();
	if(!attrib.IsEmpty()) attrib += RISSE_WC(' ');
	if(Name.IsEmpty()) return attrib + RISSE_WS("anonymous");
	return attrib + Name;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_ClassDecl::GetChildNameAt(risse_size index) const
{
	if(index == 0) return RISSE_WS("super");
	if(index == 1) return RISSE_WS("body");
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tASTNode_ClassDecl::GetDumpComment() const
{
	tString type = IsModule ?
					RISSE_WS("module ") :
					RISSE_WS("class ");
	tString attrib = Attribute.AsString();
	if(!attrib.IsEmpty()) attrib += RISSE_WC(' ');
	if(Name.IsEmpty()) return type + attrib + RISSE_WS("anonymous");
	return type + attrib + Name;
}
//---------------------------------------------------------------------------














/*
	ここから SSA 形式の生成に関わる部分
*/














//---------------------------------------------------------------------------
tString tASTNode::GetAccessTargetId()
{
	if(!this) return tString();
	if(GetType() == antId) return static_cast<tASTNode_Id*>(this)->GetName();

	// obj.mem.mem.mem ... 等の . 演算子 / :: 演算子をどんどんと右にたどっていき、
	// id を見つける。見つかったらそれを返す。
	tASTNode * target = this;
	while(target->GetType() == antMemberSel)
	{
		tASTNode * membername = static_cast<tASTNode_MemberSel*>(target)->GetMemberName();
		if(membername->GetType() == antId) return static_cast<tASTNode_Id*>(membername)->GetName();
		target = membername;
	}

	// 特定できない
	return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tASTNode::SearchEndPosition() const
{
	if(Type == antContext)
		return ((const tASTNode_Context*)this)->GetEndPosition();

	risse_size pos = Position;
	risse_size child_count = GetChildCount();
	for(risse_size i = 0; i < child_count; i++)
	{
		tASTNode * child = GetChildAt(i);
		if(child)
		{
			risse_size child_end_pos = child->SearchEndPosition();
			if(pos < child_end_pos) pos = child_end_pos;
		}
	}

	return pos;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Context::DoReadSSA(
			tSSAForm *form, void * param) const
{
	// actTopLevel がトップレベルの場合は、SSA 生成においては、おおかたこのノードが
	// 一番最初に呼ばれることになる。

	// ローカル変数スコープの生成  - コンテキストの種類に従って分岐
	switch(ContextType)
	{
	case actTopLevel: // トップレベル
		break; // 何もしない
	case actBlock: // 基本ブロック
		form->GetLocalNamespace()->Push(); // スコープを push
		break;
	}

	// すべての子ノードに再帰する
	for(risse_size i = 0; i < GetChildCount(); i++)
		GetChildAt(i)->GenerateReadSSA(form);

	// ローカル変数スコープの消滅  - コンテキストの種類に従って分岐
	switch(ContextType)
	{
	case actTopLevel: // トップレベル
		break; // 何もしない
	case actBlock: // 基本ブロック
		form->GetLocalNamespace()->Pop(); // スコープを pop
		break;
	}

	// このノードは ss_lastEvalResultHiddenVarName を返す
	return form->GetLocalNamespace()->Read(GetEndPosition(), ss_lastEvalResultHiddenVarName);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Assert::DoReadSSA(tSSAForm *form, void * param) const
{
	// このノードは、スクリプトエンジンインスタンスの AssertionEnabled が
	// 真の場合のみ、assert 用コードを出力する。
	tScriptEngine * engine = form->GetFunction()->GetFunctionGroup()->
						GetCompiler()->GetScriptBlockInstance()->GetScriptEngine();
	if(engine->GetAssertionEnabled())
	{
		// 式を評価する
		tSSAVariable * res = GetExpression()->GenerateReadSSA(form);

		// assert 命令を置く
		tSSAStatement * stmt = form->AddStatement(GetPosition(), ocAssert, NULL, res);
		stmt->SetMessage(ExpressionString); // メッセージを設定
	}

	return NULL; // このノードは答えを返さない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_ExprStmt::DoReadSSA(
			tSSAForm *form, void * param) const
{
	// このノードは式を保持しているだけなので子ノードに処理をさせるだけ
	// ただし、node は NULL の可能性がある(空文の場合)
	tASTNode * node = GetChildAt(0);
	tSSAVariable * res = NULL;
	if(node) res = node->GenerateReadSSA(form);

	// このノードは子の答えを返す
	form->WriteLastEvalResult(GetPosition(), res);
	return res;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Factor::DoReadSSA(
			tSSAForm *form, void * param) const
{
	// "項"
	switch(FactorType)
	{
	case aftConstant:	// 定数
			return form->AddConstantValueStatement(GetPosition(), Value);

	case aftThis:		// "this"
			// 文を作成して戻る
			return form->AddVariableWithStatement(GetPosition(), ocAssignThis);

	case aftThisProxy:		// this-proxy
			// form が保持している ThisProxy を返す
			return form->GetThisProxy(GetPosition());

	case aftSuper:		// "super"
		{
			// 文を作成して戻る
			// ローカル変数から "super" を探す
			tSSAVariable * ret_var =
				form->GetLocalNamespace()->Read(GetPosition(), RISSE_WS("super"));
			if(!ret_var)
			{
				// "super" がみつからない、すなわちそこには super キーワードをおけない
				tCompileExceptionClass::Throw(
					form->GetFunction()->GetFunctionGroup()->
						GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
					tString(
					RISSE_WS_TR("cannot use 'super' keyword here")),
						form->GetScriptBlockInstance(), GetPosition());
			}
			return ret_var;
		}

	case aftGlobal:		// "global"
		{
			// 文を作成して戻る
			tSSAVariable * ret_var =
				form->AddVariableWithStatement(GetPosition(), ocAssignGlobal);
			return ret_var;
		}

	case aftBinding:		// "(@)"
		{
			// 文を作成して戻る
			tSSAVariable * ret_var =
				form->AddVariableWithStatement(GetPosition(), ocAssignNewBinding);

			// bindingオブジェクトに変数とそのレジスタ番号の対応を追加する
			form->AddBindingMap(GetPosition(), ret_var);

			return ret_var;
		}
	}
	// ありえん
	RISSE_ASSERT(!"at last at tASTNode_Factor::DoReadSSA");
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tASTNode_VarDecl::SetAttribute(tDeclAttribute attrib)
{
	// 子に再帰する
	for(risse_size i = 0; i < GetChildCount(); i++)
	{
		tASTNode_VarDeclPair * pair = static_cast<tASTNode_VarDeclPair*>(GetChildAt(i));
		RISSE_ASSERT(pair->GetType() == antVarDeclPair);
		pair->SetAttribute(attrib);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_VarDecl::DoReadSSA(
			tSSAForm *form, void * param) const
{
	// 変数宣言
	// 子に再帰する
	tSSAVariable * value;
	for(risse_size i = 0; i < GetChildCount(); i++)
		value = GetChildAt(i)->GenerateReadSSA(form);

	// このノードは最後に宣言された子の値を返す
	form->WriteLastEvalResult(GetPosition(), value);
	return value;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tASTNode_VarDeclPair::tASTNode_VarDeclPair(risse_size position,
	tASTNode * name, tASTNode * initializer) :
	tASTNode(position, antVarDeclPair),
		Name(name), Initializer(initializer)
{
	if(Name) Name->SetParent(this);
	if(Initializer) Initializer->SetParent(this);

	if(Name->GetType() == antMemberSel)
	{
		// Name がメンバ選択演算子だった場合
		tASTNode_MemberSel * memsel =
			static_cast<tASTNode_MemberSel*>(Name);
		// ofMemberEnsure を指定する
		memsel->SetFlags(memsel->GetFlags() | tOperateFlags::ofMemberEnsure);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tASTNode_VarDeclPair::SetAttribute(tDeclAttribute attrib)
{
	Attribute = attrib;
	if(Name->GetType() == antMemberSel)
	{
		// Name がメンバ選択演算子だった場合
		static_cast<tASTNode_MemberSel*>(Name)->SetAttribute(attrib);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_VarDeclPair::DoReadSSA(
			tSSAForm *form, void * param) const
{
	// 変数宣言の準備
	PrepareVarDecl(form, Name);

	// 初期化値の準備
	tSSAVariable * init;
	if(Initializer)
	{
		// 初期化値がある
		init = Initializer->GenerateReadSSA(form);
	}
	else
	{
		// void の定数値の作成
		init = form->AddConstantValueStatement(GetPosition(), tVariant());
	}

	// 変数宣言のSSA表現を生成する
	GenerateVarDecl(form, GetPosition(), Name, init, Attribute);

	// このノードは初期化値を返す
	return init;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tASTNode_VarDeclPair::PrepareVarDecl(tSSAForm * form, const tASTNode * name)
{
	if(name->GetType() == antId && !static_cast<const tASTNode_Id*>(name)->GetIsPrivate())
	{
		// name ノードに id が直接来ている場合、かつプライベート (@付き) 変数ではない場合

		// グローバル変数として作成すべきかどうかをチェック
		tString str_name = static_cast<const tASTNode_Id*>(name)->GetName();
		if(form->GetLocalNamespace()->GetHasScope())
		{
			// ローカル変数として作成する

			// 変数のローカル名前空間への登録
			form->GetLocalNamespace()->Add(str_name, NULL);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tASTNode_VarDeclPair::GenerateVarDecl(tSSAForm * form,
	risse_size position,
	const tASTNode * name, tSSAVariable * init, tDeclAttribute attrib)
{
	if(name->GetType() == antId && !static_cast<const tASTNode_Id*>(name)->GetIsPrivate())
	{
		// name ノードに id が直接来ている場合、かつプライベート (@付き) 変数ではない場合
		tString str_name = static_cast<const tASTNode_Id*>(name)->GetName();

		// グローバル変数として作成すべきかどうかをチェック
		if(form->GetLocalNamespace()->GetHasScope())
		{
			// ローカル変数として作成する

			// ローカル変数への書き込み
			form->GetLocalNamespace()->Write(position, str_name, init);
		}
		else
		{
			// グローバル変数(あるいはクラス変数など)として作成する
			// this 上に変数を作成するノードを一時的に作成
			tASTNode_MemberSel * write_node =
				new tASTNode_MemberSel(position,
				new tASTNode_Factor(position, aftThis),
				new tASTNode_Factor(position, aftConstant, str_name), matDirectThis,

					tOperateFlags(
						tDeclAttribute(tDeclAttribute::pcField)|
						tOperateFlags::ofMemberEnsure)
						// 普通の変数アクセスかつメンバの作成
					);
			write_node->SetAttribute(attrib);
			write_node->GenerateWriteSSA(form, init);
		}
	}
	else
	{
		// name ノードが id じゃない場合
		// そこに代入を行う式を生成する
		name->GenerateWriteSSA(form, init);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tASTNode_MemberSel::PrepareSSA(
		tSSAForm *form, tPrepareMode mode) const
{
	tPrepareSSA * pws = new tPrepareSSA;
	// オブジェクトの式の値を得る
	pws->ObjectVar = Object->GenerateReadSSA(form);
	// メンバ名の式の値を得る
	pws->MemberNameVar = MemberName->GenerateReadSSA(form);

	return pws;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_MemberSel::DoReadSSA(
			tSSAForm *form, void * param) const
{
	// メンバ選択演算子
	tPrepareSSA * pws = static_cast<tPrepareSSA *>(param);

	// 文の作成
	tSSAVariable * ret_var = NULL;
	tOpCode code = ocNoOperation;
	tOperateFlags new_flags = Flags;
	switch(AccessType)
	{
	case matDirect:		code = ocDGet;		break;
	case matDirectThis:	code = ocDGet;
			new_flags = new_flags | tOperateFlags::ofUseClassMembersRule;	break;
	case matIndirect:	code = ocIGet;		break;
	}
	RISSE_ASSERT(code != ocNoOperation);

	tSSAStatement * stmt =
		form->AddStatement(GetPosition(), code, &ret_var,
							pws->ObjectVar, pws->MemberNameVar);
	stmt->SetAccessFlags(new_flags);

	// 戻りの変数を返す
	return ret_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tASTNode_MemberSel::DoWriteSSA(
		tSSAForm *form, void * param, tSSAVariable * value) const
{
	tPrepareSSA * pws = static_cast<tPrepareSSA *>(param);

	// 文の作成
	tOperateFlags new_flags = Flags;
	tOperateFlags attrib_flags;
	tOpCode code = ocNoOperation;
	switch(AccessType)
	{
	case matDirect:		code = ocDSet;		break;
	case matDirectThis:	code = ocDSet;
			new_flags = new_flags | tOperateFlags::ofUseClassMembersRule;
			attrib_flags = attrib_flags | tOperateFlags::ofUseClassMembersRule;
			break;
	case matIndirect:	code = ocISet;		break;
	}
	RISSE_ASSERT(code != ocNoOperation);

	tSSAStatement * stmt =
		form->AddStatement(GetPosition(), code, NULL,
							pws->ObjectVar, pws->MemberNameVar, value);
	stmt->SetAccessFlags(new_flags);

	if(Attribute.HasAny())
	{
		RISSE_ASSERT(AccessType == matDirect || AccessType == matDirectThis);
		// 属性を持っている場合はそれを設定する文を作成する
		tSSAStatement * stmt =
			form->AddStatement(GetPosition(), ocDSetAttrib, NULL,
							pws->ObjectVar, pws->MemberNameVar);
		stmt->SetAccessFlags(attrib_flags| Attribute);
	}

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tASTNode_Id::PrepareSSA(
			tSSAForm *form, tPrepareMode mode) const
{
	tPrepareSSA * pws = new tPrepareSSA;

	if(IsPrivate)
	{
		// private 変数の場合, this へアクセスするためのノードを作成する
		pws->MemberSel = CreatePrivateAccessNodeOnThis(
			form->GetFunction()->GetFunctionGroup()->GetClassName() + RISSE_WC('_') ,
			(mode == pmWrite || mode == pmReadWrite));
		pws->MemberSelParam = pws->MemberSel->PrepareSSA(form, mode);
	}
	else
	{
		// 普通の変数の場合
		bool need_access_this_proxy; // this-proxy 上のメンバにアクセスする必要があるかどうか
		need_access_this_proxy = !form->GetLocalNamespace()->IsAvailable(Name);

		if(need_access_this_proxy)
		{
			// this-proxy 上のメンバにアクセスする必要がある
			pws->MemberSel = CreateAccessNodeOnThisProxy();
			pws->MemberSelParam = pws->MemberSel->PrepareSSA(form, mode);
		}
		else
		{
			// ローカル変数に見つかった
			pws->MemberSel = NULL;
			pws->MemberSelParam = NULL;
		}
	}
	return pws;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Id::DoReadSSA(
			tSSAForm *form, void * param) const
{
	// PrepareSSA と このメソッドの間でローカル変数の状態が変わると意図した
	// 動作をしない可能性があるので注意
	tPrepareSSA * pws = static_cast<tPrepareSSA *>(param);

	// 識別子
	if(pws->MemberSel)
	{
		// ローカル変数に見つからない
		// ローカル変数に見つからない場合は、this-proxy / this へのアクセスを行う物として
		// 置き換えて処理を行う

		// this-proxy あるいは this 上のメンバをアクセスするためだけに新規に作成した AST ノードに処理をさせる
		return pws->MemberSel->DoReadSSA(form, pws->MemberSelParam);
	}
	else
	{
		// ローカル変数に見つかった
		return form->GetLocalNamespace()->Read(GetPosition(), Name);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tASTNode_Id::DoWriteSSA(
		tSSAForm *form, void * param,
		tSSAVariable * value) const
{
	// PrepareSSA と このメソッドの間でローカル変数の状態が変わると意図した
	// 動作をしない可能性があるので注意
	tPrepareSSA * pws = static_cast<tPrepareSSA *>(param);

	if(pws->MemberSel)
	{
		// ローカル変数には見つからなかった
		// tASTNode_MemberSel::DoWriteSSA を呼ぶ
		pws->MemberSel->DoWriteSSA(form, pws->MemberSelParam, value);
		return true;
	}
	else
	{
		// ローカル変数に見つかった;ローカル変数に上書きする
		bool result = form->GetLocalNamespace()->Write(GetPosition(), Name, value);
		RISSE_ASSERT(result == true);
		return result;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const tASTNode_MemberSel * tASTNode_Id::CreatePrivateAccessNodeOnThis(const tString & prefix, bool write) const
{
	// private (@つき) 変数の場合は、クラス名 + '_' + 変数名を this 上において
	// アクセスこととなる。
	// 匿名クラス/モジュールの場合や、そもそも関数グループがクラスやモジュールでない
	// 場合は GetClassName は空文字列が帰るため、その場合は単に '_' がプリフィクスと
	// なる
	return
		new tASTNode_MemberSel(GetPosition(),
		new tASTNode_Factor(GetPosition(), aftThis),
		new tASTNode_Factor(GetPosition(), aftConstant, prefix + Name), matDirectThis,
			(
			write ?	tOperateFlags(tOperateFlags::ofMemberEnsure) :
					tOperateFlags() )
					);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const tASTNode_MemberSel * tASTNode_Id::CreateAccessNodeOnThisProxy() const
{
	return
		new tASTNode_MemberSel(GetPosition(),
		new tASTNode_Factor(GetPosition(), aftThisProxy),
		new tASTNode_Factor(GetPosition(), aftConstant, Name), matDirectThis);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Unary::DoReadSSA(
			tSSAForm *form, void * param) const
{

	// 単項演算子
	switch(UnaryType)
	{
	case autLogNot:		// "!" logical not
	case autBitNot:		// "~" bit not
	case autPlus:		// "+"
	case autMinus:		// "-"
		{
			// 子の計算結果を得る
			tSSAVariable * child_var = Child->GenerateReadSSA(form);
			RISSE_ASSERT(child_var != NULL);

			// オペコードを決定
			tOpCode oc;
			switch(UnaryType)
			{
			case autLogNot:		oc = ocLogNot;	break;
			case autBitNot:		oc = ocBitNot;	break;
			case autPlus:		oc = ocPlus;	break;
			case autMinus:		oc = ocMinus;	break;
			default: oc = ocNoOperation;
			}
			// 文の作成
			tSSAVariable * ret_var = NULL;
			form->AddStatement(GetPosition(), oc, &ret_var, child_var);

			//////////////////////////////////////////////////////////////////////////////////////////
			// 演算子のオーバーロードによっては ! 演算子は bool を返さない可能性がある
			// この仕様は後に変更の可能性アリ (! 演算子をオーバーロードできないようにする可能性がある)
			// 他の ~ や + などの演算子についてもそうなる可能性がある
			//////////////////////////////////////////////////////////////////////////////////////////

			// 戻る
			return ret_var;
		}

	// キャスト演算子の類
	case autString:		//!< "string" cast to string
	case autBoolean:	//!< "boolean" cast to boolean
	case autReal:		//!< "real" cast to real
	case autInteger:	//!< "integer" cast to integer
	case autOctet:		//!< "octet" cast to octet
		{
			// 子の計算結果を得る
			tSSAVariable * child_var = Child->GenerateReadSSA(form);
			RISSE_ASSERT(child_var != NULL);

			// オペコードを決定
			tOpCode oc;
			tVariant::tType rettype = tVariant::vtVoid;
			switch(UnaryType)
			{
			case autString:		oc = ocString;	rettype = tVariant::vtString;		break;
			case autBoolean:	oc = ocBoolean;	rettype = tVariant::vtBoolean;		break;
			case autReal:		oc = ocReal;	rettype = tVariant::vtReal;		break;
			case autInteger:	oc = ocInteger;	rettype = tVariant::vtInteger;		break;
			case autOctet:		oc = ocOctet;	rettype = tVariant::vtOctet;		break;
			default: oc = ocNoOperation;
			}
			// 文の作成
			tSSAVariable * ret_var = NULL;
			form->AddStatement(GetPosition(), oc, &ret_var, child_var);

			// 戻る
			return ret_var;
		}

	case autPreDec:		// "--" pre-positioned decrement
	case autPreInc:		// "++" pre-positioned increment
	case autPostDec:	// "--" post-positioned decrement
	case autPostInc:	// "++" post-positioned increment
		{
			// オペレーションコードと前置、後置を得る
			bool is_prepositioned; // 前置？
			tOpCode code; // オペレーションコード
			switch(UnaryType)
			{
			case autPreDec:
				is_prepositioned = true;  code = ocSub;
				break;
			case autPreInc:
				is_prepositioned = true;  code = ocAdd;
				break;
			case autPostDec:
				is_prepositioned = false; code = ocSub;
				break;
			case autPostInc:
				is_prepositioned = false; code = ocAdd;
				break;
			default:
				// ここには来ないがこれを書いておかないと
				// コンパイラが文句をたれるので
				is_prepositioned = false; code = ocNoOperation;
				break;
			}

			// インクリメント、デクリメント演算子は、整数値 1 を加算あるいは
			// 減算するものとして扱われる
			// (実際にそれを inc や dec の VM 命令にするのは CG や optimizer の仕事)
			// 定数 1 を生成
			tSSAVariable * one_var =
				form->AddConstantValueStatement(
						GetPosition(), tVariant((risse_int64)1));

			// 子ノードを準備
			void * child_param = Child->PrepareSSA(form, pmReadWrite);

			// 子ノードの値を取得
			tSSAVariable * child_var = Child->DoReadSSA(form, child_param);

			// 演算を行う文を生成
			tSSAVariable * processed_var = NULL;
			form->AddStatement(GetPosition(), code, &processed_var, child_var, one_var);

			// その結果を識別子に書き込む文を生成
			if(!Child->DoWriteSSA(form, child_param, processed_var))
			{
				tCompileExceptionClass::Throw(
					form->GetFunction()->GetFunctionGroup()->
						GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
					tString(
					RISSE_WS_TR("writable expression expected as an increment/decrement operand")),
						form->GetScriptBlockInstance(), GetPosition());
			}

			// 演算結果を返す
			return is_prepositioned ? processed_var : child_var;
		}

	case autDelete:		// "delete"
		;
	}
	// ありえない
	RISSE_ASSERT(!"at last at tASTNode_Unary::DoReadSSA");
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Binary::DoReadSSA(
			tSSAForm *form, void * param) const
{
	// 演算子のタイプに従って分岐
	switch(BinaryType)
	{
	case abtAssign:				// =
		{
			// 単純代入
			// 右辺の値を得る
			tSSAVariable * rhs_var = Child2->GenerateReadSSA(form);

			// 左辺に書き込む
			if(!Child1->GenerateWriteSSA(form, rhs_var))
			{
				tCompileExceptionClass::Throw(
					form->GetFunction()->GetFunctionGroup()->
						GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
					tString(RISSE_WS_TR("writable expression expected at left side of '='")),
						form->GetScriptBlockInstance(), GetPosition());
			}

			return rhs_var; // 右辺値を返す
		}

	case abtBitAndAssign:		// &=
	case abtBitOrAssign:		// |=
	case abtBitXorAssign:		// ^=
	case abtSubAssign:			// -=
	case abtAddAssign:			// +=
	case abtModAssign:			// %=
	case abtDivAssign:			// /=
	case abtIdivAssign:			// \=
	case abtMulAssign:			// *=
	case abtLogOrAssign:		// ||=
	case abtLogAndAssign:		// &&=
	case abtRBitShiftAssign:	// >>>=
	case abtLShiftAssign:		// <<=
	case abtRShiftAssign:		// >>=
		{
			// 複合代入演算子
			// オペレーションコードを得る
			tOpCode code; // オペレーションコード
			switch(BinaryType)
			{
			case abtBitAndAssign:		code = ocBitAnd;		break; // &=
			case abtBitOrAssign:		code = ocBitOr;			break; // |=
			case abtBitXorAssign:		code = ocBitXor;		break; // ^=
			case abtSubAssign:			code = ocSub;			break; // -=
			case abtAddAssign:			code = ocAdd;			break; // +=
			case abtModAssign:			code = ocMod;			break; // %=
			case abtDivAssign:			code = ocDiv;			break; // /=
			case abtIdivAssign:			code = ocIdiv;			break; // \=
			case abtMulAssign:			code = ocMul;			break; // *=
			case abtLogOrAssign:		code = ocLogOr;			break; // ||=
			case abtLogAndAssign:		code = ocLogAnd;		break; // &&=
			case abtRBitShiftAssign:	code = ocRBitShift;		break; // >>>=
			case abtLShiftAssign:		code = ocLShift;		break; // <<=
			case abtRShiftAssign:		code = ocRShift;		break; // >>=
			default:
				// ここには来ないがこれを書いておかないと
				// コンパイラが文句をたれるので
				code = ocNoOperation;
				break;
			}


			// 右辺の値を得る
			tSSAVariable * rhs_var = Child2->GenerateReadSSA(form);

			// 左辺を準備
			void * lhs_param = Child1->PrepareSSA(form, pmReadWrite);

			// 左辺の値を取得
			tSSAVariable * lhs_var = Child1->DoReadSSA(form, lhs_param);

			// 演算を行う文を生成
			tSSAVariable * ret_var = NULL;
			form->AddStatement(GetPosition(), code, &ret_var, lhs_var, rhs_var);

			// その結果を識別子に書き込む文を生成
			if(!Child1->DoWriteSSA(form, lhs_param, ret_var))
			{
				tCompileExceptionClass::Throw(
					form->GetFunction()->GetFunctionGroup()->
						GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
					tString(
	RISSE_WS_TR("writable expression expected at left side of compound assignment operator")),
						form->GetScriptBlockInstance(), GetPosition());

			}

			// 演算結果を返す
			return ret_var;
		}

	case abtSwap:				// <->
		{
			// 交換演算子

			// これはちょっと厄介。
			// a.b <-> c.d と記述された場合、a と c 、 b と d
			// は１回のみの評価としたいので、先に評価を行わなければならない。

			// 評価の準備を行う
			void * lhs_param = Child1->PrepareSSA(form, pmReadWrite);
			void * rhs_param = Child2->PrepareSSA(form, pmReadWrite);

			// 左辺の値を取得
			tSSAVariable * lhs_var = Child1->DoReadSSA(form, lhs_param);

			// 右辺の値を取得
			tSSAVariable * rhs_var = Child2->DoReadSSA(form, rhs_param);

			// 右辺の値を左辺に代入
			if(!Child1->DoWriteSSA(form, lhs_param, rhs_var))
			{
				tCompileExceptionClass::Throw(
					form->GetFunction()->GetFunctionGroup()->
						GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
					tString(
					RISSE_WS_TR("writable expression expected at left side of '<->'")),
						form->GetScriptBlockInstance(), GetPosition());
			}

			// 左辺の値を右辺に代入
			if(!Child2->DoWriteSSA(form, rhs_param, lhs_var))
			{
				tCompileExceptionClass::Throw(
					form->GetFunction()->GetFunctionGroup()->
						GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
					tString(
					RISSE_WS_TR("writable expression expected at right side of '<->'")),
						form->GetScriptBlockInstance(), GetPosition());
			}

			// このノードは答えを返さない
			return NULL;
		}

	case abtComma:				// ,
		Child1->GenerateReadSSA(form); // 左辺値は捨てる
		return Child2->GenerateReadSSA(form); // 右辺値を返す

	case abtIf:					// if
		return NULL;

	case abtLogOr:				// ||
	case abtLogAnd:				// &&
		return GenerateLogicalAndOr(form);

	case abtBitOr:				// |
	case abtBitXor:				// ^
	case abtBitAnd:				// &
	case abtNotEqual:			// !=
	case abtEqual:				// ==
	case abtDiscNotEqual:		// !==
	case abtDiscEqual:			// ===
	case abtLesser:				// <
	case abtGreater:			// >
	case abtLesserOrEqual:		// <=
	case abtGreaterOrEqual:		// >=
	case abtInstanceOf:			// instanceof
	case abtRBitShift:			// >>>
	case abtLShift:				// <<
	case abtRShift:				// >>
	case abtMod:				// %
	case abtDiv:				// /
	case abtIdiv:				// \ (integer div)
	case abtMul:				// *
	case abtAdd:				// +
	case abtSub:				// -
		{
			// 普通の２項演算子

			// オペレーションコードを得る
			tOpCode code; // オペレーションコード
			switch(BinaryType)
			{
			case abtBitOr:				code = ocBitOr;				break; // |
			case abtBitXor:				code = ocBitXor;			break; // ^
			case abtBitAnd:				code = ocBitAnd;			break; // &
			case abtNotEqual:			code = ocNotEqual;			break; // !=
			case abtEqual:				code = ocEqual;				break; // ==
			case abtDiscNotEqual:		code = ocDiscNotEqual;		break; // !==
			case abtDiscEqual:			code = ocDiscEqual;			break; // ===
			case abtLesser:				code = ocLesser;			break; // <
			case abtGreater:			code = ocGreater;			break; // >
			case abtLesserOrEqual:		code = ocLesserOrEqual;		break; // <=
			case abtGreaterOrEqual:		code = ocGreaterOrEqual;	break; // >=
			case abtInstanceOf:			code = ocInstanceOf;		break; // instanceof
			case abtRBitShift:			code = ocRBitShift;			break; // >>>
			case abtLShift:				code = ocLShift;			break; // <<
			case abtRShift:				code = ocRShift;			break; // >>
			case abtMod:				code = ocMod;				break; // %
			case abtDiv:				code = ocDiv;				break; // /
			case abtIdiv:				code = ocIdiv;				break; // \ (integer div)
			case abtMul:				code = ocMul;				break; // *
			case abtAdd:				code = ocAdd;				break; // +
			case abtSub:				code = ocSub;				break; // -
			default:
				// ここには来ないがこれを書いておかないと
				// コンパイラが文句をたれるので
				code = ocNoOperation;
				break;
			}

			// 左辺の値を得る
			tSSAVariable * lhs_var = Child1->GenerateReadSSA(form);
			// 右辺の値を得る
			tSSAVariable * rhs_var = Child2->GenerateReadSSA(form);

			// 演算を行う文を生成
			tSSAVariable * ret_var = NULL;
			form->AddStatement(GetPosition(), code, &ret_var, lhs_var, rhs_var);

			// 戻る
			return ret_var;
		}

	default:
		return NULL;
	}
	RISSE_ASSERT(!"at last at tASTNode_Binary::DoReadSSA");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Binary::GenerateLogicalAndOr(tSSAForm *form) const
{
	// && or || ?
	RISSE_ASSERT(BinaryType == abtLogAnd || BinaryType == abtLogOr);
	bool is_and = BinaryType == abtLogAnd;

	// 左辺の値を得る
	tSSAVariable * lhs_var = Child1->GenerateReadSSA(form);

	// 分岐文を作成
	tSSAStatement * lhs_branch_stmt =
		form->AddStatement(GetPosition(), ocBranch, NULL, lhs_var);

	// ショートカットしないとき用の新しい基本ブロックを作成
	// && の場合は 真の時にここに来て、右辺の評価を続行する
	// || の場合は 偽の時にここにきて、右辺の評価を続行する
	tSSABlock * non_shortcut_block = form->CreateNewBlock(RISSE_WS("log_nonshortcut"));

	// 右辺の値を得る
	tSSAVariable * rhs_var = Child2->GenerateReadSSA(form);

	// 分岐文を作成
	tSSAStatement * rhs_branch_stmt =
		form->AddStatement(GetPosition(), ocBranch, NULL, rhs_var);

	//1: 結果が真の時にくる基本ブロックを作成
	tSSABlock * true_block_1 = form->CreateNewBlock(RISSE_WS("log_true_1"));
	//1: true の値を作成
	tSSAVariable * true_var_1 = form->AddConstantValueStatement(GetPosition(), tVariant(true));
	//1: ジャンプ文を作成
	tSSAStatement * true_jump_stmt_1 = form->AddStatement(GetPosition(), ocJump, NULL);

	//2: 結果が真の時にくる基本ブロックを作成
	tSSABlock * true_block_2 = form->CreateNewBlock(RISSE_WS("log_true_2"));
	//2: true の値を作成
	tSSAVariable * true_var_2 = form->AddConstantValueStatement(GetPosition(), tVariant(true));
	//2: ジャンプ文を作成
	tSSAStatement * true_jump_stmt_2 = form->AddStatement(GetPosition(), ocJump, NULL);

	//1: 結果が偽の時にくる基本ブロックを作成
	tSSABlock * false_block_1 = form->CreateNewBlock(RISSE_WS("log_false_1"));
	//1: true の値を作成
	tSSAVariable * false_var_1 = form->AddConstantValueStatement(GetPosition(), tVariant(false));
	//1: ジャンプ文を作成
	tSSAStatement * false_jump_stmt_1 = form->AddStatement(GetPosition(), ocJump, NULL);

	//2: 結果が偽の時にくる基本ブロックを作成
	tSSABlock * false_block_2 = form->CreateNewBlock(RISSE_WS("log_false_2"));
	//2: true の値を作成
	tSSAVariable * false_var_2 = form->AddConstantValueStatement(GetPosition(), tVariant(false));
	//2: ジャンプ文を作成
	tSSAStatement * false_jump_stmt_2 = form->AddStatement(GetPosition(), ocJump, NULL);

	// 文を抜けるための基本ブロックを作成
	tSSABlock * exit_block = form->CreateNewBlock(RISSE_WS("log_exit"));

	// 各ブロックの配線と、答えを返すための φ関数を作成する
	tSSAVariable * ret_var = NULL;
	if(is_and)
	{
		// &&
		lhs_branch_stmt->SetTrueBranch(non_shortcut_block);
		lhs_branch_stmt->SetFalseBranch(false_block_1);
		rhs_branch_stmt->SetTrueBranch(true_block_1);
		rhs_branch_stmt->SetFalseBranch(false_block_2);

		true_jump_stmt_1->SetJumpTarget(exit_block);
		false_jump_stmt_1->SetJumpTarget(exit_block);
		false_jump_stmt_2->SetJumpTarget(exit_block);
		form->AddStatement(GetPosition(), ocPhi, &ret_var, true_var_1, false_var_1, false_var_2);
	}
	else
	{
		// ||
		lhs_branch_stmt->SetTrueBranch(true_block_1);
		lhs_branch_stmt->SetFalseBranch(non_shortcut_block);
		rhs_branch_stmt->SetTrueBranch(true_block_2);
		rhs_branch_stmt->SetFalseBranch(false_block_1);

		true_jump_stmt_1->SetJumpTarget(exit_block);
		true_jump_stmt_2->SetJumpTarget(exit_block);
		false_jump_stmt_1->SetJumpTarget(exit_block);
		form->AddStatement(GetPosition(), ocPhi, &ret_var, true_var_1, true_var_2, false_var_1);
	}

	return ret_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Trinary::DoReadSSA(
			tSSAForm *form, void * param) const
{
	RISSE_ASSERT(TrinaryType == attCondition);
	return tASTNode_If::InternalDoReadSSA(form, GetPosition(),
		RISSE_WS("cond"), Child1, Child2, Child3, true);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_InContextOf::DoReadSSA(
			tSSAForm *form, void * param) const
{

	// 左辺の値を得る
	tSSAVariable * lhs_var = Instance->GenerateReadSSA(form);
	// 右辺の値を得る
	tSSAVariable * rhs_var = Context ?  Context->GenerateReadSSA(form) : NULL;

	// 演算を行う文を生成
	tSSAVariable * ret_var = NULL;
	form->AddStatement(GetPosition(), ocInContextOf, &ret_var, lhs_var, rhs_var);
		// Context が NULL の場合は rhs_var も NULL になるので注意

	// 戻る
	return ret_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tASTNode_Array::PrepareSSA(tSSAForm *form, tPrepareMode mode) const
{
	// この数値を超える量の インデックス用数値定数はここでは作成しない
	// (大量のインデックス用定数が変数領域を埋め尽くすのを避けるため)
	const risse_size max_prepare_index = 4; 

	// 配列の要素それぞれに対してprepareを行う
	tPrepareSSA * data = new tPrepareSSA();
	data->Mode = mode;

	data->Elements.reserve(GetChildCount());
	data->Indices.reserve(std::max(max_prepare_index, GetChildCount()));

	for(risse_size i = 0; i < GetChildCount(); i++)
	{
		// 各要素の準備
		tASTNode * child = GetChildAt(i);
		if(child)
		{
			// 本質的に 読み込みと書き込みが行われなければ先に
			// 内容を prepare しておく必要はない
			if(mode == pmReadWrite)
				data->Elements.push_back(child->PrepareSSA(form, mode));
			else
				data->Elements.push_back(NULL);
		}
		else
		{
			data->Elements.push_back(NULL);
		}

		// インデックスの準備
		if(mode == pmReadWrite)
		{
			if(child && i < max_prepare_index)
			{
				tSSAVariable * index_var =
					form->AddConstantValueStatement(GetPosition(), (risse_int64)i);

				data->Indices.push_back(index_var);
			}
			else
			{
				data->Indices.push_back(NULL);
			}
		}
	}

	return data;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Array::DoReadSSA(tSSAForm *form, void * param) const
{
	// インライン配列からの読み出し (配列オブジェクトを作成し、初期化して返す)
	tPrepareSSA * data = static_cast<tPrepareSSA*>(param);

	// 配列オブジェクトを作成
	tSSAVariable * array_var = NULL;
	form->AddStatement(GetPosition(), ocAssignNewArray, &array_var);

#if 0
XXXX: Array は内部的に deque を使っているので配列の予約はできない(必要ない)
	// 配列の reserve メソッドを呼び出し、配列数を予約する
	if(GetChildCount() != 0)
	{
		tSSAVariable * element_count_var =
			form->AddConstantValueStatement(GetPosition(), (risse_int64)GetChildCount());
		array_var->GenerateFuncCall(GetPosition(), RISSE_WS("reserve"), element_count_var);
	}
#endif

	RISSE_ASSERT(data->Elements.size() == GetChildCount());
	for(risse_size i = 0; i < GetChildCount(); i++)
	{
		// 内容
		tSSAVariable * element_var;
		tASTNode * child = GetChildAt(i);
		if(child)
		{
			if(data->Elements[i])
				element_var = child->DoReadSSA(form, data->Elements[i]);
			else
				element_var = child->GenerateReadSSA(form);
		}
		else
		{
			element_var =
				form->AddConstantValueStatement(GetPosition(), tVariant());
		}

		// インデックス用数値定数
		tSSAVariable * index_var = NULL;
		if(i < data->Indices.size())
			index_var = data->Indices[i]; // あらかじめインデックス用定数が用意されている

		if(!index_var)
			index_var =
				form->AddConstantValueStatement(GetPosition(), (risse_int64)i);

		// 代入文を生成
		form->AddStatement(GetPosition(), ocISet, NULL,
								array_var, index_var, element_var);
	}

	// 配列を返す
	return array_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tASTNode_Array::DoWriteSSA(tSSAForm *form, void * param,
		tSSAVariable * value) const
{
	// インライン配列への書き込み
	tPrepareSSA * data = static_cast<tPrepareSSA*>(param);

	RISSE_ASSERT(data->Mode != pmRead);
	RISSE_ASSERT(data->Elements.size() == GetChildCount());
	for(risse_size i = 0; i < GetChildCount(); i++)
	{
		tASTNode * child = GetChildAt(i);
		if(child)
		{
			// インデックス用数値定数
			tSSAVariable * index_var = NULL;
			if(i < data->Indices.size())
				index_var = data->Indices[i]; // あらかじめインデックス用定数が用意されている

			if(!index_var)
				index_var =
					form->AddConstantValueStatement(GetPosition(), (risse_int64)i);

			// 要素の値を配列から得る
			tSSAVariable * elm_var = NULL;
			form->AddStatement(GetPosition(), ocIGet, &elm_var, value, index_var);

			// 各要素に対する代入文を生成
			if(!data->Elements[i])
				data->Elements[i] = child->PrepareSSA(form, pmWrite);
			if(!child->DoWriteSSA(form, data->Elements[i], elm_var))
			{
				// 書き込みに失敗
				risse_char i_str[40];
				::Risse::int_to_str(i, i_str);
				tCompileExceptionClass::Throw(
					form->GetFunction()->GetFunctionGroup()->
						GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
					tString(
					RISSE_WS_TR("writable expression expected at array index %1"), i_str),
						form->GetScriptBlockInstance(), GetPosition());
			}
		}
	}
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tASTNode_Dict::PrepareSSA(tSSAForm *form, tPrepareMode mode) const
{
	// 辞書配列用の名前と値を準備
	tPrepareSSA * data = new tPrepareSSA();
	data->Names.reserve(GetChildCount());
	data->Values.reserve(GetChildCount());

	// 各要素に対して ...
	for(risse_size i = 0; i < GetChildCount(); i++)
	{
		RISSE_ASSERT(inherited::GetChildAt(i)->GetType() == antDictPair);
		tASTNode_DictPair * pair_node =
			static_cast<tASTNode_DictPair*>(GetChildAt(i));

		if(mode == pmReadWrite)
		{
			// 名前と値を準備
			void * name_prep_data = pair_node->GetName()->PrepareSSA(form, pmRead);
														// 名前は常に読み込み扱い
			void * value_prep_data = pair_node->GetValue()->PrepareSSA(form, mode);

			data->Names.push_back(name_prep_data);
			data->Values.push_back(value_prep_data);
		}
		else
		{
			data->Names.push_back(NULL);
			data->Values.push_back(NULL);
		}
	}
	return data;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Dict::DoReadSSA(tSSAForm *form, void * param) const
{
	// インライン辞書配列からの読み出し (辞書配列オブジェクトを作成し、初期化して返す)
	tPrepareSSA * data = static_cast<tPrepareSSA*>(param);

	// 辞書配列オブジェクトを作成
	tSSAVariable * dict_var = NULL;
	form->AddStatement(GetPosition(), ocAssignNewDict, &dict_var);

	RISSE_ASSERT(data->Names.size() == GetChildCount());
	RISSE_ASSERT(data->Values.size() == GetChildCount());

	// 各要素に対して ...
	for(risse_size i = 0; i < GetChildCount(); i++)
	{
		tASTNode_DictPair * pair_node =
			static_cast<tASTNode_DictPair*>(GetChildAt(i));

		// 名前の値を得る
		tSSAVariable * name_var;
		if(data->Names[i])
			name_var = pair_node->GetName()->DoReadSSA(form, data->Names[i]);
		else
			name_var = pair_node->GetName()->GenerateReadSSA(form);

		// 値の値を得る
		tSSAVariable * value_var;
		if(data->Values[i])
			value_var = pair_node->GetValue()->DoReadSSA(form, data->Values[i]);
		else
			value_var = pair_node->GetValue()->GenerateReadSSA(form);

		// 代入文を生成
		form->AddStatement(GetPosition(), ocISet, NULL,
								dict_var, name_var, value_var);
	}
	return dict_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tASTNode_Dict::DoWriteSSA(tSSAForm *form, void * param,
		tSSAVariable * value) const
{
	// インライン辞書配列への書き込み (右辺値を辞書配列と見なし、値を設定する)
	tPrepareSSA * data = static_cast<tPrepareSSA*>(param);

	RISSE_ASSERT(data->Names.size() == GetChildCount());
	RISSE_ASSERT(data->Values.size() == GetChildCount());

	// 各要素に対して ...
	for(risse_size i = 0; i < GetChildCount(); i++)
	{
		tASTNode_DictPair * pair_node =
			static_cast<tASTNode_DictPair*>(GetChildAt(i));

		// 名前の値を得る
		tSSAVariable * name_var;
		if(data->Names[i])
			name_var = pair_node->GetName()->DoReadSSA(form, data->Names[i]);
		else
			name_var = pair_node->GetName()->GenerateReadSSA(form);

		// その名前に対する値を得る
		tSSAVariable * value_var = NULL;
		form->AddStatement(GetPosition(), ocIGet, &value_var,
								value, name_var);

		// 値を設定する
		if(!data->Values[i]) data->Values[i] = pair_node->GetValue()->PrepareSSA(form, pmWrite);
		if(!pair_node->GetValue()->DoWriteSSA(form, data->Values[i], value_var))
		{
			// 書き込みに失敗
			risse_char i_str[40];
			::Risse::int_to_str(i, i_str);
			tCompileExceptionClass::Throw(
				form->GetFunction()->GetFunctionGroup()->
					GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
				tString(
				RISSE_WS_TR("writable expression expected at value of dictionary element index %1"), i_str),
					form->GetScriptBlockInstance(), GetPosition());
		}
	}
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_RegExp::DoReadSSA(tSSAForm *form, void * param) const
{
	// 文字列定数を作成
	tSSAVariable * pattern_var =
			form->AddConstantValueStatement(GetPosition(), Pattern);
	tSSAVariable * flags_var =
			form->AddConstantValueStatement(GetPosition(), Flags);
	// 正規表現オブジェクトを作成する文を生成してその結果を返す
	tSSAVariable * regexp_var = NULL;
	form->AddStatement(GetPosition(), ocAssignNewRegExp, &regexp_var,
		pattern_var, flags_var);
	return regexp_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_If::InternalDoReadSSA(tSSAForm *form,
		risse_size pos,
		const tString &basename,
		tASTNode * condition,
		tASTNode * truenode,
		tASTNode * falsenode,
		bool needresult
		)
{
	// 条件式の結果を得る
	tSSAVariable * cond_var = condition->GenerateReadSSA(form);

	// 分岐文を作成
	tSSAStatement * branch_stmt =
		form->AddStatement(pos, ocBranch, NULL, cond_var);

	// 新しい基本ブロックを作成(真の場合)
	tSSABlock * true_block = form->CreateNewBlock(basename + RISSE_WS("_true"));

	// 真の場合に実行する内容を作成
	tSSAVariable * true_var = truenode->GenerateReadSSA(form);

	// もし 値を得られないような物であれば void を代わりに使う
	if(!true_var) true_var = form->AddConstantValueStatement(pos, tVariant());

	// ジャンプ文を作成
	tSSAStatement * true_exit_jump_stmt =
		form->AddStatement(pos, ocJump, NULL);

	// 偽の場合に実行するノードがある場合のみ
	tSSABlock * false_last_block = NULL;
	tSSABlock * false_block = NULL;
	tSSAStatement * false_exit_jump_stmt = NULL;
	tSSAVariable * false_var = NULL;

	if(falsenode)
	{
		// 新しい基本ブロックを作成(偽の場合)
		false_block = form->CreateNewBlock(basename + RISSE_WS("_false"));

		// 偽の場合に実行する内容を作成
		false_var = falsenode->GenerateReadSSA(form);

		// もし 値を得られないような物であれば void を代わりに使う
		if(!false_var) false_var = form->AddConstantValueStatement(pos, tVariant());

		false_last_block = form->GetCurrentBlock();

		// ジャンプ文を作成
		false_exit_jump_stmt =
			form->AddStatement(pos, ocJump, NULL);
	}
	else
	{
		// false ノードがない場合
		if(needresult)
		{
			// 結果が必要な場合は void を代入する式を作成する

			// 新しい基本ブロックを作成(偽の場合)
			false_block = form->CreateNewBlock(basename + RISSE_WS("_pseudo_false"));

			// 偽の場合に実行する内容を作成
			false_var = form->AddConstantValueStatement(pos, tVariant());
			false_last_block = form->GetCurrentBlock();

			// ジャンプ文を作成
			false_exit_jump_stmt =
				form->AddStatement(pos, ocJump, NULL);
		}
	}


	// 新しい基本ブロックを作成(if文からの脱出)
	tSSABlock * if_exit_block;
	if_exit_block = form->CreateNewBlock(basename + RISSE_WS("_exit"));

	// 分岐/ジャンプ文のジャンプ先を設定
	branch_stmt->SetTrueBranch(true_block);
	branch_stmt->SetFalseBranch(false_block ? false_block : if_exit_block);
	true_exit_jump_stmt->SetJumpTarget(if_exit_block);
	if(false_exit_jump_stmt) false_exit_jump_stmt->SetJumpTarget(if_exit_block);

	// もし答えが必要ならば、答えを返すための φ関数を作成する
	tSSAVariable * ret_var = NULL;
	if(needresult)
		form->AddStatement(pos, ocPhi, &ret_var, true_var, false_var);

	return ret_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_If::DoReadSSA(
			tSSAForm *form, void * param) const
{
	// if文を作成
	tSSAVariable * res = 
		InternalDoReadSSA(form, GetPosition(), RISSE_WS("if"), Condition,
			True, False, true);

	// 結果は _ に書き込まれるとともにこの関数の戻り値となる
	form->WriteLastEvalResult(GetPosition(), res);
	return res;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_While::DoReadSSA(tSSAForm *form, void * param) const
{
	// while ループ または do ～ while ループ

	// break に関する情報を生成
	tBreakInfo * break_info = new tBreakInfo(form);

	// break に関する情報を form に設定
	tBreakInfo * old_break_info = form->SetCurrentBreakInfo(break_info);

	// continue に関する情報を生成
	tContinueInfo * continue_info = new tContinueInfo(form);

	// continue に関する情報を form に設定
	tContinueInfo * old_continue_info = form->SetCurrentContinueInfo(continue_info);

	// 条件式または body にジャンプするための文を作成
	tSSAStatement * entry_jump_stmt =
		form->AddStatement(SkipFirstCheck?Body->GetPosition():Condition->GetPosition(), ocJump, NULL);

	// 条件式を格納する基本ブロックを作成
	tSSABlock * while_cond_block =
		form->CreateNewBlock(RISSE_WS("while_cond"));

	// 条件式の結果を得る
	tSSAVariable * cond_var = Condition->GenerateReadSSA(form);

	// 分岐文を作成
	tSSAStatement * branch_stmt =
		form->AddStatement(Condition->GetPosition(), ocBranch, NULL, cond_var);

	// 新しい基本ブロックを作成(条件式が真の場合)
	tSSABlock * while_body_block =
		form->CreateNewBlock(RISSE_WS("while_body"));

	// while 文の body を生成
	Body->GenerateReadSSA(form);

	// ジャンプ文を作成
	tSSAStatement * while_body_jump_stmt =
		form->AddStatement(GetPosition(), ocJump, NULL);

	// 新しい基本ブロックを作成(while文からの脱出)
	tSSABlock * while_exit_block =
		form->CreateNewBlock(RISSE_WS("while_exit"));

	// _ に void を代入 (値付き break で終了しないループの値は void )
	form->WriteLastEvalResult(GetPosition(),
		form->AddConstantValueStatement(GetPosition(), tVariant()));
	tSSAStatement * while_exit_jump_stmt = form->AddStatement(GetPosition(), ocJump, NULL);

	// 分岐/ジャンプ文のジャンプ先を設定
	entry_jump_stmt->SetJumpTarget(SkipFirstCheck?while_body_block:while_cond_block);
	while_body_jump_stmt->SetJumpTarget(while_cond_block);
	branch_stmt->SetTrueBranch(while_body_block);
	branch_stmt->SetFalseBranch(while_exit_block);

	// 新しい基本ブロックを作成(break文のターゲット)
	tSSABlock * while_break_target_block =
		form->CreateNewBlock(RISSE_WS("while_break_target"));
	while_exit_jump_stmt->SetJumpTarget(while_break_target_block);

	// break の処理
	break_info->BindAll(while_break_target_block);

	// continue の処理
	continue_info->BindAll(while_cond_block);

	// break に関する情報を元に戻す
	form->SetCurrentBreakInfo(old_break_info);

	// continue に関する情報を元に戻す
	form->SetCurrentContinueInfo(old_continue_info);

	// このノードは _ を返す。
	// _ への書き込みは上の form->WriteLastEvalResult ですでに行われているか、
	// あるいは break 文によって tSSAForm::AddBreakOrContinueStatement() 内で
	// _ への書き込みが行われている。
	tSSAVariable * res = form->GetLocalNamespace()->Read(
					GetPosition(), ss_lastEvalResultHiddenVarName);
	return res;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_For::DoReadSSA(tSSAForm *form, void * param) const
{
	// for ループ
	// for ループの第１節ではローカル変数を宣言する可能性があるので
	// スコープを作っておく
	form->GetLocalNamespace()->Push(); // スコープを push

	// 初期化ノードを生成する
	if(Initializer) Initializer->GenerateReadSSA(form);


	// 条件式にジャンプするための文を作成
	tSSAStatement * entry_jump_stmt =
		form->AddStatement(GetPosition(), ocJump, NULL);

	// 条件式を格納する基本ブロックを作成
	tSSABlock * for_cond_block =
		form->CreateNewBlock(RISSE_WS("for_cond"));

	// break に関する情報を生成
	tBreakInfo * break_info = new tBreakInfo(form);

	// break に関する情報を form に設定
	tBreakInfo * old_break_info = form->SetCurrentBreakInfo(break_info);

	// continue に関する情報を生成
	tContinueInfo * continue_info = new tContinueInfo(form);

	// continue に関する情報を form に設定
	tContinueInfo * old_continue_info = form->SetCurrentContinueInfo(continue_info);

	// 条件式の結果を得る (条件式が省略されている場合は常に真であると見なす)
	tSSAVariable * cond_var = NULL;
	if(Condition)
		cond_var = Condition->GenerateReadSSA(form);
	else
		cond_var = form->AddConstantValueStatement(GetPosition(), tVariant(true));

	// 分岐文を作成
	tSSAStatement * branch_stmt =
		form->AddStatement(GetPosition(), ocBranch, NULL, cond_var);

	// 新しい基本ブロックを作成(条件式が真の場合)
	tSSABlock * for_body_block =
		form->CreateNewBlock(RISSE_WS("for_body"));

	// for 文の body を生成
	Body->GenerateReadSSA(form);

	// ジャンプ文を作成
	tSSAStatement * for_body_jump_stmt =
		form->AddStatement(GetPosition(), ocJump, NULL);

	// 新しい基本ブロックを作成(イテレータのため)
	tSSABlock * for_iter_block = NULL;
	tSSAStatement * for_iter_jump_stmt = NULL;
	if(Iterator)
	{
		for_iter_block =
			form->CreateNewBlock(RISSE_WS("for_iter"));

		// イテレータの内容を生成
		Iterator->GenerateReadSSA(form);

		// ジャンプ文を作成
		for_iter_jump_stmt =
			form->AddStatement(GetPosition(), ocJump, NULL);
	}

	// 新しい基本ブロックを作成(for文からの脱出)
	tSSABlock * for_exit_block =
		form->CreateNewBlock(RISSE_WS("for_exit"));

	// _ に void を代入 (値付き break で終了しないループの値は void )
	form->WriteLastEvalResult(GetPosition(),
		form->AddConstantValueStatement(GetPosition(), tVariant()));
	tSSAStatement * for_exit_jump_stmt = form->AddStatement(GetPosition(), ocJump, NULL);

	// 分岐/ジャンプ文のジャンプ先を設定
	entry_jump_stmt->SetJumpTarget(for_cond_block);
	for_body_jump_stmt->SetJumpTarget(for_iter_block ? for_iter_block : for_cond_block);
	if(for_iter_jump_stmt) for_iter_jump_stmt->SetJumpTarget(for_cond_block);
	branch_stmt->SetTrueBranch(for_body_block);
	branch_stmt->SetFalseBranch(for_exit_block);

	// 新しい基本ブロックを作成(break文のターゲット)
	tSSABlock * for_break_target_block =
		form->CreateNewBlock(RISSE_WS("for_break_target"));
	for_exit_jump_stmt->SetJumpTarget(for_break_target_block);

	// break の処理
	break_info->BindAll(for_break_target_block);

	// continue の処理
	continue_info->BindAll(for_iter_block ? for_iter_block : for_cond_block);

	// break に関する情報を元に戻す
	form->SetCurrentBreakInfo(old_break_info);

	// continue に関する情報を元に戻す
	form->SetCurrentContinueInfo(old_continue_info);

	// スコープを pop
	form->GetLocalNamespace()->Pop(); // スコープを pop

	// このノードは _ を返す。
	// _ への書き込みは上の form->WriteLastEvalResult ですでに行われているか、
	// あるいは break 文によって tSSAForm::AddBreakOrContinueStatement() 内で
	// _ への書き込みが行われている。
	tSSAVariable * res = form->GetLocalNamespace()->Read(
					GetPosition(), ss_lastEvalResultHiddenVarName);
	return res;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Return::DoReadSSA(tSSAForm *form, void * param) const
{
	tSSAVariable * var;
	if(Expression)
		var = Expression->GenerateReadSSA(form);// 戻りとなる値を作成する
	else
		var = NULL; // 戻りとなる値は void

	// return 文を作成
	form->AddReturnStatement(GetPosition(), var);

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Throw::DoReadSSA(tSSAForm *form, void * param) const
{
	tSSAVariable * var;
	if(Expression)
	{
		// 戻りとなる値を作成する
		var = Expression->GenerateReadSSA(form);
	}
	else
	{
		// 戻りとなる値は catch ブロックが受け取った値
		// TODO: catchブロックが受け取った値を投げる
		var = form->AddConstantValueStatement(GetPosition(), tVariant());
	}

	// return 文を作成
	form->AddStatement(GetPosition(), ocThrow, NULL, var);

	// 新しい基本ブロックを作成(ただしここには到達しない)
	form->CreateNewBlock("disconnected_by_throw");

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Break::DoReadSSA(
									tSSAForm *form, void * param) const
{
	tSSAVariable * var;
	if(Expression)
		var = Expression->GenerateReadSSA(form);// 値を作成する
	else
		var = NULL; // 値は void

	// break 文を追加する
	form->AddBreakStatement(GetPosition(), var);

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Continue::DoReadSSA(
									tSSAForm *form, void * param) const
{
	tSSAVariable * var;
	if(Expression)
		var = Expression->GenerateReadSSA(form);// 値を作成する
	else
		var = NULL; // 値は void

	// continue 文を追加する
	form->AddContinueStatement(GetPosition(), var);

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Synchronized::DoReadSSA(tSSAForm *form, void * param) const
{
	// ここの処理の中身は try ブロックによく似ているので注意

	// ここでは synchronized や using ブロックのことを critical block と呼ぶことにする

	// 式を評価
	tSSAVariable * object_var = GetObject()->GenerateReadSSA(form);

	// critical ブロックは、ブロックの中身を 遅延評価ブロックとして評価する
	tSSAVariable * lazyblock_var = NULL;
	tSSAForm * new_form = NULL;

	// スクリプトブロックより critical ブロックの通し番号を得る
	// 各 critical ブロックは try ブロックと同じく
	// 固有の ID をもち(というかtryブロックと共通)、例外で実装された大域脱出を行う
	// return や break, goto などの脱出先の ID となる
	risse_size try_id = form->GetScriptBlockInstance()->AddTryIdentifier();

	// アクセスマップを作成する
	tSSAVariableAccessMap * access_map = form->CreateAccessMap(GetPosition());

	// 遅延評価ブロックを作成する
	void * lazy_param = form->CreateLazyBlock(Body->GetPosition(),
		RISSE_WS("critical block"), false, access_map, new_form, lazyblock_var);

	new_form->SetTryIdentifierIndex(try_id); // try識別子を子SSA形式に対して設定

	// 内容を生成する
	new_form->GetLocalNamespace()->Push(); // スコープを push
	new_form->Generate(Body);
	new_form->GetLocalNamespace()->Pop(); // スコープを pop

	// 遅延評価ブロックで使用された変数の処理
	form->ListVariablesForLazyBlock(GetPosition(), access_map);

	// 遅延評価ブロックを実行するためのSync文を作成
	// 関数呼び出し文を生成する
	tSSAVariable * critical_block_ret_var = NULL;
	tSSAStatement * critical_block_call_stmt =
		form->AddStatement(GetPosition(), ocSync, &critical_block_ret_var, lazyblock_var);
	critical_block_call_stmt->SetFuncExpandFlags(0);

	// 関数呼び出し文の引数として object_var をわたす
	critical_block_call_stmt->SetFuncExpandFlags(0);
	critical_block_call_stmt->SetBlockCount(0);
	critical_block_call_stmt->AddUsed(object_var);

	// 遅延評価ブロックをクリーンアップ
	form->CleanupLazyBlock(lazy_param);

	// アクセスマップをクリーンアップ
	form->CleanupAccessMap(GetPosition(), access_map);

	// critical_block_ret_var は ocCatchBranch にわたる。
	// critical_block_ret_var の値と ocCatchBranch が具体的に
	// どの様に結びつけられるのかはここでは関知しないが、
	// 少なくとも critical_block_ret_var が ocCatchBranch を支配
	// していることはここで示しておかなければならない。
	tSSAStatement * catch_branch_stmt =
		form->AddStatement(GetPosition(), ocCatchBranch, NULL, critical_block_ret_var);
	catch_branch_stmt->SetTryIdentifierIndex(try_id); // try識別子を設定

	// あとで例外の分岐先を設定できるように
	form->AddCatchBranchAndExceptionValue(catch_branch_stmt, critical_block_ret_var);

	// 脱出用の新しい基本ブロックを作成
	tSSABlock * exit_critical_block =
		form->CreateNewBlock(RISSE_WS("exit_critical"));

	// critical ブロックの結果を _ に書き込むための文を生成
	form->WriteLastEvalResult(GetPosition(), critical_block_ret_var);
	tSSAStatement * exit_critical_block_jump_stmt =
		form->AddStatement(GetPosition(), ocJump, NULL);

	// catch用の新しい基本ブロックを作成
	tSSABlock * catch_encritical_block =
		form->CreateNewBlock(RISSE_WS("catch_entry"));

	// catch に相当する部分は、例外をそのまま投げる。
	form->AddStatement(GetPosition(), ocThrow, NULL, critical_block_ret_var);
	form->CreateNewBlock(RISSE_WS("disconnected_by_throw"));

	// critical ブロックからの脱出用ブロックを作成
	tSSABlock * exit_block =
		form->CreateNewBlock(RISSE_WS("catch_exit"));

	// 遅延評価ブロックを実行するためのTryFuncCall文 の分岐先を設定
	catch_branch_stmt->SetTryExitTarget(exit_critical_block);
	catch_branch_stmt->SetTryCatchTarget(catch_encritical_block);

	// critical ブロックからの脱出用ブロックへのジャンプを設定
	exit_critical_block_jump_stmt->SetJumpTarget(exit_block);

	// ss_lastEvalResultHiddenVarName の値を取り出す
	tSSAVariable * last_value =
		form->GetLocalNamespace()->Read(GetPosition(), ss_lastEvalResultHiddenVarName);

	// このノードは ss_lastEvalResultHiddenVarName を返す
	return last_value;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Label::DoReadSSA(tSSAForm *form, void * param) const
{
	// ジャンプ文を作成
	tSSAStatement * jump_stmt =
		form->AddStatement(GetPosition(), ocJump, NULL);

	// 新しい基本ブロックを作成
	tSSABlock * label_block =
		form->CreateNewBlock(Name);

	// ジャンプ文のジャンプ先を設定
	jump_stmt->SetJumpTarget(label_block);

	// form に登録
	form->GetFunction()->AddLabelMap(Name, label_block);

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Goto::DoReadSSA(tSSAForm *form, void * param) const
{
	// form に登録
	form->GetFunction()->AddPendingLabelJump(form->GetCurrentBlock(), Name);
	form->GetCurrentBlock()->SetLastStatementPosition(GetPosition());

	// 新しい基本ブロックを作成 (この基本ブロックには到達しない)
	form->CreateNewBlock(RISSE_WS("disconnected_by_goto"));

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Debugger::DoReadSSA(tSSAForm *form, void * param) const
{
	// 文を作成
	form->AddStatement(GetPosition(), ocDebugger, NULL);

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Switch::DoReadSSA(tSSAForm *form, void * param) const
{
	// 基準式を生成する
	tSSAVariable * ref = Object->GenerateReadSSA(form);

	// switch に関する情報を生成
	tSwitchInfo * switch_info = new tSwitchInfo(ref);

	// switch に関する情報を form に設定
	tSwitchInfo * old_switch_info = form->SetCurrentSwitchInfo(switch_info);

	// break に関する情報を生成
	tBreakInfo * break_info = new tBreakInfo(form);
	break_info->SetNonValueBreakShouldSetVoidToLastEvalValue(false);
		// swtich 文での値なし break で _ の値が void になってしまうと困るため

	// break に関する情報を form に設定
	tBreakInfo * old_break_info = form->SetCurrentBreakInfo(break_info);

	// ジャンプ文を作成
	switch_info->SetLastBlock(form->GetCurrentBlock());
	switch_info->SetLastStatement(form->AddStatement(GetPosition(), ocJump, NULL));

	// 新しい基本ブロックを作成
	form->CreateNewBlock("disconnected_by_switch");

	// ブロックの内容を生成
	Body->GenerateReadSSA(form);

	// ジャンプ文を作成
	tSSAStatement * jump_stmt =
		form->AddStatement(GetPosition(), ocJump, NULL);

	// 新しい基本ブロックを作成(switch文からの脱出)
	tSSABlock * exit_switch_block =
		form->CreateNewBlock(RISSE_WS("switch_exit"));
	jump_stmt->SetJumpTarget(exit_switch_block);

	// default / 最後のジャンプの処理
	tSSABlock * last_block =
		switch_info->GetDefaultBlock() ? switch_info->GetDefaultBlock() : exit_switch_block;
	if(switch_info->GetLastStatement()->GetCode() == ocJump)
		switch_info->GetLastStatement()->SetJumpTarget(last_block); // ジャンプ
	else
		switch_info->GetLastStatement()->SetFalseBranch(last_block); // 分岐

	// break の処理
	break_info->BindAll(exit_switch_block);

	// switch に関する情報を元に戻す
	form->SetCurrentSwitchInfo(old_switch_info);

	// break に関する情報を元に戻す
	form->SetCurrentBreakInfo(old_break_info);

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Case::DoReadSSA(tSSAForm *form, void * param) const
{
	// switch 文の中？
	tSwitchInfo * info = form->GetCurrentSwitchInfo();
	if(info == NULL)
		tCompileExceptionClass::Throw(
			form->GetFunction()->GetFunctionGroup()->
				GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
			tString(Expression ?
				RISSE_WS_TR("cannot place 'case' out of switch") :
				RISSE_WS_TR("cannot 'default' out of switch")),
				form->GetScriptBlockInstance(), GetPosition());

	// ジャンプ文を作成
	tSSAStatement * jump_stmt =
		form->AddStatement(GetPosition(), ocJump, NULL);

	// 新しい基本ブロックを作成(ここに条件判断式などが入る)
	tSSABlock * case_block =
		form->CreateNewBlock(Expression ?
			RISSE_WS("switch_case") :
			RISSE_WS("switch_default"));

	if(Expression)
	{
		// case

		// ジャンプ文のジャンプ先を設定
		if(info->GetLastStatement()->GetCode() == ocJump)
			info->GetLastStatement()->SetJumpTarget(case_block); // ジャンプ
		else
			info->GetLastStatement()->SetFalseBranch(case_block); // 分岐

		// 条件判断文を作成
		tSSAVariable * targ_var = Expression->GenerateReadSSA(form);
		tSSAVariable * comp_res = NULL;
		form->AddStatement(GetPosition(), ocDiscEqual, &comp_res,
			info->GetReference(), targ_var);

		// 分岐文を作成
		tSSAStatement * branch_stmt =
			form->AddStatement(GetPosition(), ocBranch, NULL, comp_res);
		info->SetLastBlock(form->GetCurrentBlock());
		info->SetLastStatement(branch_stmt);

		// 新しい基本ブロックを作成(ここに内容が入る)
		tSSABlock * case_body_block =
			form->CreateNewBlock(RISSE_WS("switch_case_body"));
		branch_stmt->SetTrueBranch(case_body_block);
		jump_stmt->SetJumpTarget(case_body_block);
	}
	else
	{
		// default

		// default 文のあるブロックを登録する
		if(info->GetDefaultBlock() != NULL)
			tCompileExceptionClass::Throw(
				form->GetFunction()->GetFunctionGroup()->
					GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
				tString(RISSE_WS_TR("cannot place multiple 'default' in a switch")),
				form->GetScriptBlockInstance(), GetPosition());
		info->SetDefaultBlock(case_block);

		jump_stmt->SetJumpTarget(case_block);
	}


	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Try::DoReadSSA(tSSAForm *form, void * param) const
{
	tSSAVariable * res;
	// try 文およびcatch文を作成する
	if(!Finally)
	{
		// finally 文が無い
		// 普通に try 文と catch 文を生成する
		res = GenerateTryCatchOrFinally(form, false);
	}
	else
	{
		// finally 文がある
		res = GenerateTryCatchOrFinally(form, true);
	}


	// このノードは最後に実行された try ブロック、あるいは catch / finally ブロックの
	// 値を返す。
	form->WriteLastEvalResult(GetPosition(), res);
	return res;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_Try::GenerateTryCatchOrFinally(tSSAForm *form,  bool is_finally) const
{
	// finallyがある？
	tSSABlock * finally_entry_block = NULL;
	tSSAStatement * finally_last_jump_stmt = NULL;
	if(is_finally)
	{
		// finally文がある
		// この場合は
		// try {
		//    try { // 本来の try-catch
		//    } catch {
		//    }
		// } finally {
		//    // finally の中身
		// }
		// のような構成で生成を行う。

		// finally ブロックは 本来のtry ブロックの中身が実行し終わる前には、また
		// 本来のtry ブロックの結果によって分岐を行う ocCatchBranch 文よりも前には
		// 存在していなければならない(ocCatchBranchがどうなっても実行される文なので)が、
		// その条件だといまここで遅延評価ブロックとして評価しなければならない
		// (なぜならばtSSAFormの現状の構造では 遅延評価ブロックの評価と
		// それに続く ocCatchBranch 文の間で他の遅延評価ブロックを評価できないので)
		// 名前空間については各finallyブロック内では名前空間のpush/popを行い、
		// 名前空間に影響を与えないので、finallyブロックの評価がtryのブロックの評価
		// よりも先に来てもかまわない。


		// finally ブロックの後にジャンプするための文を生成
		tSSAStatement * skip_finally_jump_stmt =
			form->AddStatement(GetPosition(), ocJump, NULL);

		// 新しい基本ブロックを生成
		finally_entry_block = form->CreateNewBlock(RISSE_WS("finally_entry"));

		// finally の中身を生成
		//- finally の結果がない場合に備えて void を _ に代入する
		form->WriteLastEvalResult(GetPosition(),
				form->AddConstantValueStatement(GetPosition(), tVariant()));
		form->GetLocalNamespace()->Push(); // スコープを push
		tSSAVariable * finally_res_var = Finally->GenerateReadSSA(form);
		//- finally の結果がない場合は void を _ に代入する(再度)
		if(!finally_res_var)
				form->WriteLastEvalResult(GetPosition(),
					form->AddConstantValueStatement(GetPosition(), tVariant()));
		form->GetLocalNamespace()->Pop(); // スコープを pop

		// finally ブロックの最後に jump 文を生成
		finally_last_jump_stmt =
			form->AddStatement(GetPosition(), ocJump, NULL);

		// 本来の try ブロック用の基本ブロックを作成
		tSSABlock * try_catch_entry_block =
			form->CreateNewBlock(RISSE_WS("try_catch_entry"));
		skip_finally_jump_stmt->SetJumpTarget(try_catch_entry_block);
	}

	// try ブロックの中身を 遅延評価ブロックとして評価する
	tSSAVariable * lazyblock_var = NULL;
	tSSAForm * new_form = NULL;

	// スクリプトブロックより try ブロックの通し番号を得る
	// 各 try ブロックは固有の ID をもち、例外で実装された大域脱出を行う
	// return や break, goto などの脱出先の ID となる
	risse_size try_id = form->GetScriptBlockInstance()->AddTryIdentifier();

	// アクセスマップを作成する
	tSSAVariableAccessMap * access_map = form->CreateAccessMap(GetPosition());

	// 遅延評価ブロックを作成する
	void * lazy_param = form->CreateLazyBlock(Body->GetPosition(),
		RISSE_WS("try block"), false, access_map, new_form, lazyblock_var);

	new_form->SetTryIdentifierIndex(try_id); // try識別子を子SSA形式に対して設定

	// 内容を生成する
	if(!is_finally)
	{
		new_form->GetLocalNamespace()->Push(); // スコープを push
		new_form->Generate(Body);
		new_form->GetLocalNamespace()->Pop(); // スコープを pop
	}
	else
	{
		GenerateTryCatchOrFinally(new_form, false); // try-catch を生成する
		tSSAVariable * void_ret_var =
			new_form->AddConstantValueStatement(GetPosition(), tVariant());
		new_form->AddStatement(GetPosition(), ocReturn, NULL, void_ret_var);
	}

	// 遅延評価ブロックで使用された変数の処理
	form->ListVariablesForLazyBlock(GetPosition(), access_map);

	// 遅延評価ブロックを実行するためのTryFuncCall文を作成
	// 関数呼び出し文を生成する
	tSSAVariable * try_block_ret_var = NULL;
	tSSAStatement * try_block_call_stmt =
		form->AddStatement(GetPosition(), ocTryFuncCall, &try_block_ret_var, lazyblock_var);
	try_block_call_stmt->SetFuncExpandFlags(0);

	// 遅延評価ブロックをクリーンアップ
	form->CleanupLazyBlock(lazy_param);

	// アクセスマップをクリーンアップ
	form->CleanupAccessMap(GetPosition(), access_map);

	if(is_finally)
	{
		// ここで finally の中身を呼び出す
		tSSAStatement * finally_jump_stmt = form->AddStatement(GetPosition(), ocJump, NULL);

		// ocCatchBranch用の基本ブロックを作成
		tSSABlock * catch_branch_block =
			form->CreateNewBlock(RISSE_WS("catch_branch"));
		finally_last_jump_stmt->SetJumpTarget(catch_branch_block);

		// ジャンプ先を設定
		finally_jump_stmt->SetJumpTarget(finally_entry_block);
	}

	// try_block_ret_var は ocCatchBranch にわたる。
	// try_block_ret_var の値と ocCatchBranch が具体的に
	// どの様に結びつけられるのかはここでは関知しないが、
	// 少なくとも try_block_ret_var が ocCatchBranch を支配
	// していることはここで示しておかなければならない。
	tSSAStatement * catch_branch_stmt =
		form->AddStatement(GetPosition(), ocCatchBranch, NULL, try_block_ret_var);
	catch_branch_stmt->SetTryIdentifierIndex(try_id); // try識別子を設定

	// あとで例外の分岐先を設定できるように
	form->AddCatchBranchAndExceptionValue(catch_branch_stmt, try_block_ret_var);

	if(!is_finally)
	{
		// 脱出用の新しい基本ブロックを作成
		tSSABlock * exit_try_block =
			form->CreateNewBlock(RISSE_WS("exit_try"));
		// try ブロックの結果を _ に書き込むための文を生成
		form->WriteLastEvalResult(GetPosition(), try_block_ret_var);
		tSSAStatement * exit_try_block_jump_stmt =
			form->AddStatement(GetPosition(), ocJump, NULL);

		// catch用の新しい基本ブロックを作成
		tSSABlock * catch_entry_block =
			form->CreateNewBlock(RISSE_WS("catch_entry"));

		// catch ブロックを生成
		GenerateCatchBlock(form, try_block_ret_var);

		// 遅延評価ブロックを実行するためのTryFuncCall文 の分岐先を設定
		catch_branch_stmt->SetTryExitTarget(exit_try_block);
		catch_branch_stmt->SetTryCatchTarget(catch_entry_block);

		// try ブロックからの脱出用ブロックからのジャンプを設定
		exit_try_block_jump_stmt->SetJumpTarget(form->GetCurrentBlock());
	}
	else
	{
		// catch用の新しい基本ブロックを作成
		// catch した例外はそのまま投げる
		tSSABlock * catch_entry_block =
			form->CreateNewBlock(RISSE_WS("try_catch_entry"));
		form->AddStatement(GetPosition(), ocThrow, NULL, try_block_ret_var);

		// 脱出用の新しい基本ブロックを作成
		tSSABlock * exit_try_block =
			form->CreateNewBlock(RISSE_WS("exit_try_entry"));

		// 遅延評価ブロックを実行するためのTryFuncCall文 の分岐先を設定
		catch_branch_stmt->SetTryExitTarget(exit_try_block);
		catch_branch_stmt->SetTryCatchTarget(catch_entry_block);
	}

	// ss_lastEvalResultHiddenVarName の値を取り出す
	tSSAVariable * last_value =
		form->GetLocalNamespace()->Read(GetPosition(), ss_lastEvalResultHiddenVarName);

	// このノードは ss_lastEvalResultHiddenVarName を返す
	return last_value;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tASTNode_Try::GenerateCatchBlock(tSSAForm * form,
	tSSAVariable * try_block_ret_var) const
{
	// catchブロックを順々に処理
	gc_vector<tSSAStatement *> catch_exit_jumps;
	tSSAStatement * branch_stmt = NULL;
	if(inherited::GetChildCount() > 0)
	{
		// catch文がある場合
		for(risse_size i = 0; i < inherited::GetChildCount(); i++)
		{
			RISSE_ASSERT(inherited::GetChildAt(i)->GetType() == antCatch);
			tASTNode_Catch * catch_node =
				static_cast<tASTNode_Catch*>(inherited::GetChildAt(i));

			tASTNode * condition = catch_node->GetCondition();

			// ローカル変数の名前空間を push
			form->GetLocalNamespace()->Push(); // スコープを push

			// 例外を受け取る変数がある場合は例外を受け取る変数に例外を代入する
			if(!catch_node->GetName().IsEmpty())
			{
				// 変数のローカル名前空間への登録
				form->GetLocalNamespace()->Add(catch_node->GetName(), NULL);

				// ローカル変数への書き込み
				form->GetLocalNamespace()->Write(GetPosition(),
												catch_node->GetName(), try_block_ret_var);
			}

			// 例外 catch の条件式の処理
			tSSAVariable * cond_var;
			if(condition)
			{
				// 条件判断文を作成
				cond_var = condition->GenerateReadSSA(form);
			}
			else
			{
				// 条件式がない
				// 条件式が無い場合は、例外が Exception クラスのサブクラスで
				// ある場合にのみ受け取るという条件文を作成する

				// global.Exception を表す AST ノードを作成
				tASTNode_Factor * global = new tASTNode_Factor(GetPosition(), aftGlobal);
				tASTNode_Factor * Exception =
					new tASTNode_Factor(GetPosition(), aftConstant,
						tVariant(tString(RISSE_WS("Exception"))));
				tASTNode_MemberSel * global_Exception_node =
					new tASTNode_MemberSel(GetPosition(), global, Exception, matDirect);

				// global.Exception を取得
				tSSAVariable * global_Exception = global_Exception_node->GenerateReadSSA(form);

				// instanceof
				form->AddStatement(GetPosition(), ocInstanceOf, &cond_var,
					try_block_ret_var, global_Exception);
			}

			// 分岐文を作成
			branch_stmt =
				form->AddStatement(GetPosition(), ocBranch, NULL, cond_var);

			// 新しい基本ブロックを作成
			tSSABlock * catch_body_block =
				form->CreateNewBlock(RISSE_WS("catch_block"));

			branch_stmt->SetTrueBranch(catch_body_block);

			// catch の内容がなかった場合に備えて void を _ に代入する
			form->WriteLastEvalResult(GetPosition(),
					form->AddConstantValueStatement(GetPosition(), tVariant()));

			// catch の内容を生成
			tSSAVariable * catch_result_var = catch_node->GetBody()->GenerateReadSSA(form);

			// catch の結果がない場合は void を _ に代入する(再度)
			if(!catch_result_var)
				form->WriteLastEvalResult(GetPosition(),
					form->AddConstantValueStatement(GetPosition(), tVariant()));

			// catch の終了用のジャンプ文を生成
			tSSAStatement * catch_exit_jump_stmt =
				form->AddStatement(GetPosition(), ocJump, NULL);
			catch_exit_jumps.push_back(catch_exit_jump_stmt);

			// ローカル変数の名前空間を pop
			form->GetLocalNamespace()->Pop(); // スコープを pop

			// 新しい基本ブロックを作成
			tSSABlock * catch_next_block =
				form->CreateNewBlock(RISSE_WS("catch_next"));
			if(branch_stmt) branch_stmt->SetFalseBranch(catch_next_block);
		}
		// どの条件にも合致しなかった場合には例外をそのまま投げる
		if(branch_stmt)
		{
			form->AddStatement(GetPosition(), ocThrow, NULL, try_block_ret_var);
			form->CreateNewBlock(RISSE_WS("disconnected_by_throw"));
		}
	}
	else
	{
		// catch 文が無い場合
		// catch 文が無い場合は例外をそのまま投げる
		form->AddStatement(GetPosition(), ocThrow, NULL, try_block_ret_var);
		form->CreateNewBlock(RISSE_WS("disconnected_by_throw"));
	}


	// 各catchブロックからの脱出を行うジャンプ先を設定
	tSSABlock * catch_exit_block =
		form->CreateNewBlock(RISSE_WS("catch_exit"));

	for(gc_vector<tSSAStatement *>::iterator i = catch_exit_jumps.begin();
			i != catch_exit_jumps.end(); i++)
		(*i)->SetJumpTarget(catch_exit_block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_FuncCall::DoReadSSA(
							tSSAForm *form, void * param) const
{
	// ブロック付きかどうかを得る
	bool with_block = Blocks.size() > 0;
	RISSE_ASSERT(!(CreateNew && with_block)); // new に対してブロックは現状指定できない

	// 関数を表す式を得る
	tSSAVariable * func_var = Expression->GenerateReadSSA(form);

	// 引数の列挙用配列
	gc_vector<tSSAVariable *> arg_vec;
	arg_vec.reserve(GetChildCount());

	// break に関する情報を生成
	tBreakInfo * break_info = new tBreakInfo(form, RISSE_WS("#block_break"));
		// tSSAForm::AddCatchBranchTargetsForOne は ラベル名の先頭の文字で種別を
		// 判断する。ラベル名の先頭が # の場合はすでにコード生成済みのラベルジャンプ先
		// として扱われる。
	break_info->SetIsBlock(true);

	// break に関する情報を form に設定
	tBreakInfo * old_break_info = form->SetCurrentBreakInfo(break_info);

	// continue に関する情報を生成
	tContinueInfo * continue_info = new tContinueInfo(form);
	continue_info->SetIsBlock(true);

	// continue に関する情報を form に設定
	tContinueInfo * old_continue_info = form->SetCurrentContinueInfo(continue_info);

	// ... が指定されていなければ引数を処理
	risse_uint32 exp_flag = 0; // 展開フラグ
	if(!Omit)
	{
		// 引数を処理する
		risse_uint32 exp_flag_bit = 1; // 展開フラグ用ビット
		for(risse_size i = 0; i < inherited::GetChildCount(); i++, exp_flag_bit <<= 1)
		{
			if(i >= MaxArgCount)
			{
				// 現状、関数の引数に列挙できる数はMaxArgCount個までとなっている
				// ので、それを超えるとエラーになる
				tCompileExceptionClass::Throw(
					form->GetFunction()->GetFunctionGroup()->
						GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
					tString(RISSE_WS_TR("too many function arguments")),
						form->GetScriptBlockInstance(), GetPosition());
			}

			RISSE_ASSERT(!(inherited::GetChildAt(i) &&
				inherited::GetChildAt(i)->GetType() != antFuncCallArg));
			tASTNode_FuncCallArg * arg =
				static_cast<tASTNode_FuncCallArg *>(
												inherited::GetChildAt(i));
			if(arg)
			{
				// 引数が省略されていない
				tSSAVariable * arg_var;

				if(arg->GetExpression())
				{
					// 式がある
					if(arg->GetExpand()) exp_flag |= exp_flag_bit; // 展開

					// 引数のコードを生成
					arg_var = arg->GetExpression()->GenerateReadSSA(form);
				}
				else
				{
					// 式がない
					// この場合は別に引数が省略されているのではなくて
					// * だけがそこに指定されている場合
					// (無名の引数配列を意味する)
					RISSE_ASSERT(arg->GetExpand() != false); // arg->GetExpand() は真のはず
					arg_var = form->GetFunctionCollapseArgumentVariable();
					if(!arg_var)
					{
						// 関数宣言の引数に無名の * が無い
						tCompileExceptionClass::Throw(
							form->GetFunction()->GetFunctionGroup()->
								GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
							tString(
							RISSE_WS_TR("no anonymous collapsed arguments defined in this method")),
								form->GetScriptBlockInstance(), GetPosition());
					}
				}

				// 関数呼び出し文の Used に追加するために配列に追加
				arg_vec.push_back(arg_var);
			}
			else
			{
				// 引数が省略されているので void を追加する
				tSSAVariable * void_var =
					form->AddConstantValueStatement(GetPosition(), tVariant());

				// 関数呼び出し文の Used に追加するために配列に追加
				arg_vec.push_back(void_var);
			}
		}
	}
	else
	{
		exp_flag = FuncCallFlag_Omitted; //  関数呼び出しは ... を伴っている
	}

	// ブロック引数のうち、関数宣言やブロックでないものはここで先に内容を行ってしまう。
	// これは、ブロック式中に入れ子のブロックがあった場合にアクセスマップが混乱するため。
	// また、引数の並び順とそれらが評価される順序は同一でない可能性があるということ。
	gc_vector<tSSAVariable *> non_func_block_arg_vec;
	for(tASTArray::const_reverse_iterator i = Blocks.rbegin();
		i != Blocks.rend(); i++) // あとで取り出しやすいようにここでは逆順に処理する
	{
		if((*i)->GetType() != antFuncDecl)
		{
			// 普通の式
			tSSAVariable * arg_var =
					(*i)->GenerateReadSSA(form);

			// 配列にpush
			non_func_block_arg_vec.push_back(arg_var);
		}
	}

	// ブロック引数を処理
	tSSAVariableAccessMap * access_map = NULL;
	risse_size break_try_idx = risse_size_max;
	if(with_block)
	{
		// アクセスマップを作成する
		// ブロックを調べ、ブロックの中に一つでもいわゆる「ブロック」
		// (匿名関数ではない物) があれば access_map を作成する
		bool block_found = false;
		for(tASTArray::const_iterator i = Blocks.begin(); i != Blocks.end(); i++)
		{
			if((*i)->GetType() == antFuncDecl)
			{
				tASTNode_FuncDecl * block_arg =
					static_cast<tASTNode_FuncDecl*>(*i);
				if(block_arg->GetIsBlock()) { block_found = true; break; }
			}
		}
		if(block_found)
		{
			// ブロックが見つかったのでアクセスマップを作成する
			access_map = form->CreateAccessMap(GetPosition());

			// form の ExitTryBranchTargetLabels に break の行き先を登録する
			// (ASSERTはしないが)これが form->ExitTryBranchTargetLabels の 最初の
			// エントリになるはず
			break_try_idx =
				form->AddExitTryBranchTargetLabel(break_info->GetJumpTargetLabel());
		}
	}

	if(Blocks.size() > MaxArgCount)
	{
		// 現状、関数のブロック引数に列挙できる数は
		// MaxArgCount 個までとなっている
		// (普通の引数と違って本来ブロック引数の数には制限が無いはずだが
		//  将来的にブロック引数にも普通の引数のように展開フラグなどを
		//  つけるかもしれないので、普通の引数と同じ制限を付ける)
		tCompileExceptionClass::Throw(
			form->GetFunction()->GetFunctionGroup()->
				GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
			tString(RISSE_WS_TR("too many function block arguments")),
				form->GetScriptBlockInstance(), GetPosition());
	}

	// try識別子を取得
	risse_size try_id = form->GetScriptBlockInstance()->AddTryIdentifier();

	// 各ブロックの内容を生成
	for(tASTArray::const_iterator i = Blocks.begin(); i != Blocks.end(); i++)
	{
		if((*i)->GetType() == antFuncDecl)
		{
			tASTNode_FuncDecl * block_arg =
				static_cast<tASTNode_FuncDecl*>(*i);

			// ブロックの中身を 遅延評価ブロックとして評価する
			tSSAVariable * lazyblock_var =
				block_arg->GenerateFuncDecl(form, access_map, try_id);

			// 配列にpush
			arg_vec.push_back(lazyblock_var);
		}
		else
		{
			// すでに先に作成しておいた物から引っ張ってくる
			RISSE_ASSERT(non_func_block_arg_vec.size() != 0);
			tSSAVariable * arg_var = non_func_block_arg_vec.back();
			non_func_block_arg_vec.pop_back();

			// 配列にpush
			arg_vec.push_back(arg_var);
		}
	}

	RISSE_ASSERT(non_func_block_arg_vec.size() == 0); // non_func_block_arg_vec はすでに使い切っているはず


	// 遅延評価ブロックで使用された変数の処理
	if(access_map) form->ListVariablesForLazyBlock(GetPosition(), access_map);

	// 関数呼び出しの文を生成する
	// ブロック付きの文の場合は ocTryFuncCall を用いる
	tSSAVariable * returned_var = NULL;
	tSSAStatement * call_stmt =
		form->AddStatement(GetPosition(),
			CreateNew ? ocNew : (access_map?ocTryFuncCall:ocFuncCall),
			&returned_var, func_var);

	call_stmt->SetFuncExpandFlags(exp_flag);
	call_stmt->SetBlockCount(Blocks.size());

	for(gc_vector<tSSAVariable *>::iterator i = arg_vec.begin();
		i != arg_vec.end(); i++)
	{
		call_stmt->AddUsed(*i);
	}

	// アクセスマップのクリーンアップを行う
	if(access_map) form->CleanupAccessMap(GetPosition(), access_map);

	// ブロック付き呼び出しの場合は、制御構文を実装するための分岐文を
	// 作成する
	if(access_map)
	{
		tSSAStatement * catch_branch_stmt =
			form->AddStatement(GetPosition(), ocCatchBranch, NULL, returned_var);
		catch_branch_stmt->SetTryIdentifierIndex(try_id); // try識別子を設定

		// catch用の新しい基本ブロックを作成
		tSSABlock * trycall_catch_block =
			form->CreateNewBlock(RISSE_WS("trycall_catch"));

		// catch した例外はそのまま投げる
		form->AddStatement(GetPosition(), ocThrow, NULL, returned_var);

		// break 用の新しい基本ブロックを作成
		tSSABlock * break_exit_block =
			form->CreateNewBlock(RISSE_WS("break_exit"));

		// break - 例外オブジェクトから値を取り出す
		tSSAVariable * break_ret_var = NULL;
		form->AddStatement(GetPosition(),
			ocGetExitTryValue, &break_ret_var, returned_var);

		// ジャンプ文を作成
		tSSAStatement * break_exit_jump_stmt =
			form->AddStatement(GetPosition(), ocJump, NULL);

		// exit用の新しい基本ブロックを作成
		tSSABlock * trycall_exit_block =
			form->CreateNewBlock(RISSE_WS("trycall_exit"));
		tSSAStatement * trycall_exit_jump_stmt =
			form->AddStatement(GetPosition(), ocJump, NULL);

		// TryFuncCall文 の分岐先を設定
		catch_branch_stmt->SetTryExitTarget(trycall_exit_block);
		catch_branch_stmt->SetTryCatchTarget(trycall_catch_block);
		catch_branch_stmt->AddTarget(break_exit_block);
			// catc_branch_stmt に登録した break_exit_block は
			// tSSAForm::AddCatchBranchTargetsForOne() の処理中では
			// 無視される(ラベル名の先頭が '#' なので)

		// あとで例外の分岐先を設定できるように
		form->AddCatchBranchAndExceptionValue(catch_branch_stmt, returned_var);

		// trycallの終了用の新しい基本ブロックを作成
		tSSABlock * trycall_fin_block =
			form->CreateNewBlock(RISSE_WS("trycall_fin"));
		trycall_exit_jump_stmt->SetJumpTarget(trycall_fin_block);

		// break から来るパスと exit から来るパス用にφ関数を作成する
		break_exit_jump_stmt->SetJumpTarget(trycall_fin_block);
		tSSAVariable * phi_ret_var = NULL;
		form->AddStatement(GetPosition(), ocPhi, &phi_ret_var,
								returned_var, break_ret_var);

		returned_var = phi_ret_var;
	}

	// break に関する情報を元に戻す
	form->SetCurrentBreakInfo(old_break_info);

	// continue に関する情報を元に戻す
	form->SetCurrentContinueInfo(old_continue_info);

	// 関数の戻り値を返す
	return returned_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_FuncDecl::DoReadSSA(tSSAForm *form, void * param) const
{
	return GenerateFuncDecl(form);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_FuncDecl::GenerateFuncDecl(tSSAForm *form,
		tSSAVariableAccessMap *access_map, risse_size try_id) const
{
	// 関数の中身を 遅延評価ブロックとして評価する
	RISSE_ASSERT(!(IsBlock && !access_map));

	tSSAVariable * lazyblock_var = NULL;
	tSSAForm * new_form = NULL;
	tString display_name = Name;
	if(display_name.IsEmpty()) display_name = RISSE_WS("<not determinate>");
	void * lazy_param = form->CreateLazyBlock(
							GetPosition(),
							IsBlock ? RISSE_WS("callback block") :
							Name.IsEmpty() ?
								RISSE_WS("anonymous function"):
								display_name,
							!IsBlock, access_map, new_form, lazyblock_var);
	if(try_id != risse_size_max) new_form->SetTryIdentifierIndex(try_id);

	// 引数を処理する
	for(risse_size i = 0; i < inherited::GetChildCount(); i++)
	{
		tASTNode_FuncDeclArg * child =
			static_cast<tASTNode_FuncDeclArg*>(inherited::GetChildAt(i));
		RISSE_ASSERT(child->GetType() == antFuncDeclArg);
		RISSE_ASSERT(child->GetCollapse() == false); // TODO: 配列圧縮

		tSSAVariable * param_var = NULL;

		// パラメータ内容の取得
		tSSAStatement * assignparam_stmt = 
			new_form->AddStatement(GetPosition(), ocAssignParam, &param_var);
		assignparam_stmt->SetIndex(i);

		// デフォルト引数の処理
		// if(param_var === void) { param_var = デフォルト引数の式; }
		// のようになる
		tASTNode * init_node = child->GetInitializer();
		if(init_node)
		{
			// param_var が void と同じかどうかを調べる
			tSSAVariable * void_var =
				new_form->AddConstantValueStatement(GetPosition(), tVariant());
			tSSAVariable * cond_var = NULL;
			new_form->AddStatement(GetPosition(), ocDiscEqual, &cond_var,
				param_var, void_var);

			// 分岐文を作成
			tSSAStatement *branch_stmt =
				new_form->AddStatement(GetPosition(), ocBranch, NULL, cond_var);

			// 新しい基本ブロックを作成(条件式が真の場合)
			tSSABlock * init_block =
				new_form->CreateNewBlock(RISSE_WS("param_init"));

			// 初期化式を作成
			tSSAVariable * init_var = init_node->GenerateReadSSA(new_form);

			// ジャンプ文を作成(初期化式を含むブロックからその次のブロックへ)
			tSSAStatement * init_exit_jump_stmt =
				new_form->AddStatement(GetPosition(), ocJump, NULL);

			// 新しい基本ブロックを作成(変数へ代入)
			tSSABlock * init_exit_block =
				new_form->CreateNewBlock(RISSE_WS("param_subst"));

			// 分岐/ジャンプ文のジャンプ先を設定
			branch_stmt->SetFalseBranch(init_exit_block);
			branch_stmt->SetTrueBranch(init_block);
			init_exit_jump_stmt->SetJumpTarget(init_exit_block);

			// φ関数を作成
			tSSAVariable * phi_ret_var = NULL;
			new_form->AddStatement(GetPosition(), ocPhi, &phi_ret_var,
									param_var, init_var);

			param_var = phi_ret_var;
		}

		// 変数のローカル名前空間への登録
		new_form->GetLocalNamespace()->Add(child->GetName(), NULL);

		// ローカル変数への書き込み
		new_form->GetLocalNamespace()->Write(GetPosition(),
										child->GetName(), param_var);
	}

	// ブロック引数を処理する
	for(risse_size i = 0; i < Blocks.size(); i++)
	{
		tASTNode_FuncDeclArg * child =
			static_cast<tASTNode_FuncDeclArg*>(Blocks[i]);
		RISSE_ASSERT(child->GetType() == antFuncDeclBlock);

		tSSAVariable * param_var = NULL;

		// パラメータ内容の取得
		tSSAStatement * assignparam_stmt = 
			new_form->AddStatement(GetPosition(), ocAssignBlockParam, &param_var);
		assignparam_stmt->SetIndex(i);

		// 変数のローカル名前空間への登録
		new_form->GetLocalNamespace()->Add(child->GetName(), NULL);

		// ローカル変数への書き込み
		new_form->GetLocalNamespace()->Write(GetPosition(),
										child->GetName(), param_var);
	}

	// ブロックの内容を生成する
	new_form->Generate(Body);

	// 遅延評価ブロックをクリーンアップ
	form->CleanupLazyBlock(lazy_param);

	// 関数インスタンスをFunctionクラスでラップするための命令を置く
	tSSAVariable * wrapped_lazyblock_var = NULL;
	form->AddStatement(GetPosition(), ocAssignNewFunction, &wrapped_lazyblock_var, lazyblock_var);

	// 属性を適用する
	tSSAVariable * final_var = wrapped_lazyblock_var;
	tASTNode_FuncDecl::ApplyMethodAttribute(form, GetPosition(), final_var, Attribute);

	// このノードはラップされた方の関数(メソッド)を返す
	return final_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tASTNode_FuncDecl::ApplyMethodAttribute(
	tSSAForm * form, risse_size position,
	tSSAVariable *& function, tDeclAttribute attrib)
{
	// static 指定がついていれば this にバインドするための命令を置く
	if(attrib.Has(tDeclAttribute::ccStatic))
	{
		tSSAVariable * this_var = NULL;
		form->AddStatement(position, ocAssignThis, &this_var);

		tSSAVariable * input_function = function;
		form->AddStatement(position,
			ocInContextOf, &function, input_function, this_var);
	}

	// synchronized 指定が付いていれば synchronized プロパティを真に設定する命令を置く
	if(attrib.Has(tDeclAttribute::scSynchronized))
	{
		tSSAVariable * true_var =
				form->AddConstantValueStatement(position, tVariant(true));

		tSSAVariable * synchronized_var =
				form->AddConstantValueStatement(position, tVariant(ss_synchronized));

		tSSAVariable * method_var = function;

		tSSAStatement * stmt =
			form->AddStatement(position, ocDSet, NULL,
								method_var, synchronized_var, true_var);
		stmt->SetAccessFlags(tOperateFlags());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_PropDecl::DoReadSSA(tSSAForm *form, void * param) const
{
	return GeneratePropertyDecl(form);
	// TODO: プロパティがローカル名前空間に作成されてもかなり意味がないのでどうするか
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_PropDecl::GeneratePropertyDecl(tSSAForm *form) const
{
	tString display_name = Name;

	// ゲッタノードを処理
	tSSAVariable * getter_var = NULL;
	if(Getter)
	{
		tSSAVariable * lazyblock_var = NULL;
		tSSAForm * new_form = NULL;
		if(display_name.IsEmpty()) display_name = RISSE_WS("<nondetermined>");
		void * lazy_param = form->CreateLazyBlock(
								GetPosition(),
								Name.IsEmpty() ?
									RISSE_WS("anonymous property getter"):
									RISSE_WS("property ") + display_name + RISSE_WS(" getter"),
								true, NULL, new_form, lazyblock_var);
		// ブロックの内容を生成する
		new_form->Generate(Getter);

		// 遅延評価ブロックをクリーンアップ
		form->CleanupLazyBlock(lazy_param);

		getter_var = lazyblock_var;
	}
	else
	{
		getter_var = 
			form->AddConstantValueStatement(GetPosition(), tVariant::GetNullObject());
	}

	// セッタノードを処理
	tSSAVariable * setter_var = NULL;
	if(Setter)
	{
		tSSAVariable * lazyblock_var = NULL;
		tSSAForm * new_form = NULL;
		void * lazy_param = form->CreateLazyBlock(
								GetPosition(),
								Name.IsEmpty() ?
									RISSE_WS("anonymous property setter"):
									RISSE_WS("property ") + display_name + RISSE_WS(" setter"),
								true, NULL, new_form, lazyblock_var);


		// パラメータ内容の取得
		tSSAVariable * param_var = NULL;
		tSSAStatement * assignparam_stmt = 
			new_form->AddStatement(GetPosition(), ocAssignParam, &param_var);
		assignparam_stmt->SetIndex(0);

		// 変数のローカル名前空間への登録
		new_form->GetLocalNamespace()->Add(SetterArgumentName, NULL);

		// ローカル変数への書き込み
		new_form->GetLocalNamespace()->Write(GetPosition(),
										SetterArgumentName, param_var);

		// ブロックの内容を生成する
		new_form->Generate(Setter);

		// 遅延評価ブロックをクリーンアップ
		form->CleanupLazyBlock(lazy_param);

		setter_var = lazyblock_var;
	}
	else
	{
		setter_var = 
			form->AddConstantValueStatement(GetPosition(), tVariant::GetNullObject());
	}

	// プロパティオブジェクトを作成する
	tSSAVariable * property_instance_var = NULL;
	form->AddStatement(GetPosition(),
		ocAssignNewProperty, &property_instance_var,
		getter_var, setter_var);


	// 属性を適用する
	tSSAVariable * final_var = property_instance_var;
	tASTNode_FuncDecl::ApplyMethodAttribute(form, GetPosition(), final_var, Attribute);

	// プロパティオブジェクトを返す
	return final_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_ClassDecl::DoReadSSA(tSSAForm *form, void * param) const
{
	return GenerateClassDecl(form);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tASTNode_ClassDecl::GenerateClassDecl(tSSAForm *form) const
{
	// Body は antContext かつ actTopLevel でなくてはならない
	RISSE_ASSERT(Body->GetType() == antContext);
	RISSE_ASSERT(static_cast<tASTNode_Context*>(Body)->GetContextType() == actTopLevel);

	// モジュールはスーパークラスを持つことはできない
	RISSE_ASSERT(!(IsModule && SuperClass));


	// クラス/モジュール名を決める
	tString display_name = Name;
	if(display_name.IsEmpty()) display_name = RISSE_WS("nondetermined");
	tString class_name = Name.IsEmpty() ? tString(RISSE_WS("anonymous")) : display_name;

	// クラス/モジュール名を定数として作る
	tSSAVariable * class_name_var =
		form->AddConstantValueStatement(GetPosition(), tVariant(class_name));


	// クラスの場合とモジュールの場合で違う処理
	tSSAVariable * class_instance_var = NULL;
	tSSAVariable * super_class_var = NULL;
	if(!IsModule)
	{
		// クラスの場合

		// 親クラスを得るための式を作る

		if(SuperClass)
		{
			// スーパークラスが指定されている
			super_class_var = SuperClass->GenerateReadSSA(form);
		}
		else
		{
			// スーパークラスが指定されていないので global.Object を
			// 使う
			tASTNode_Factor * global = new tASTNode_Factor(GetPosition(), aftGlobal);
			tASTNode_Factor * Object =
				new tASTNode_Factor(GetPosition(), aftConstant,
					tVariant(tString(RISSE_WS("Object"))));
			tASTNode_MemberSel * global_Object =
				new tASTNode_MemberSel(GetPosition(), global, Object, matDirect);

			super_class_var = global_Object->GenerateReadSSA(form);
		}

		// 新しいクラスインスタンスを作成する
		form->AddStatement(GetPosition(),
			ocAssignNewClass, &class_instance_var,
			super_class_var, class_name_var);
	}
	else
	{
		// モジュールの場合

		// 新しいモジュールインスタンスを作成する
		form->AddStatement(GetPosition(),
			ocAssignNewModule, &class_instance_var, class_name_var);
	}

	// クラス/モジュールの中身を作成する
	tCompiler * compiler = form->GetFunction()->GetFunctionGroup()->GetCompiler();
	tSSAVariable * classblock_var = NULL;
	tSSAForm * new_form = NULL;
	gc_vector<tASTNode *> roots;

	if(!IsModule)
	{
		roots.push_back(GenerateDefaultInitializeAST(Body->GetPosition()));
		// デフォルトの function initialize() { super::initialize(...); } を生成する。
		// これはあとからユーザスクリプトでオーバーライドしてもよいし、しなくてもよい。
		roots.push_back(GenerateDefaultConstructAST(Body->GetPosition()));
		// デフォルトの function construct() {} を生成する。
		// これはあとからユーザスクリプトでオーバーライドしてもよいし、しなくてもよい。
	}

	roots.push_back(Body);
	compiler->CompileClass(roots, class_name, form, new_form, classblock_var, !IsModule);

	// クラス/モジュールの中身のコンテキストを、新しく作成したインスタンスの物にする
	form->AddStatement(GetPosition(),
			ocInContextOf, &classblock_var, classblock_var, class_instance_var);

	// クラス/モジュールの中身を「実行」するためのSSA表現を生成する。
	// class  {    } の中身はそれが実行されることにより、クラス/モジュールインスタンスへの
	// メンバの登録などが行われる。
	// 関数呼び出しの文を生成する
	tSSAVariable * funccall_result = NULL;
	tSSAStatement * call_stmt;

	call_stmt = form->AddStatement(GetPosition(),
		ocFuncCall, &funccall_result, classblock_var, super_class_var);
		// 引数としてスーパークラスを指定する
		// (モジュールの場合は何も渡されない)

	call_stmt->SetFuncExpandFlags(0);
	call_stmt->SetBlockCount(0);

	// 新しく作成したクラス/モジュールインスタンスを返す
	return class_instance_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tASTNode * tASTNode_ClassDecl::GenerateDefaultInitializeAST(risse_size pos)
{
	// function initialize() { super::initialize(...); }
	// を表すASTノードを生成して返す

	// super::initialize() の部分を作成
	tASTNode_Factor * super = new tASTNode_Factor(pos, aftSuper);
	tASTNode_Factor * initialize =
		new tASTNode_Factor(pos, aftConstant,
			tVariant(tString(RISSE_WS("initialize"))));
	tASTNode_MemberSel * super_initialize =
		new tASTNode_MemberSel(pos, super, initialize, matDirectThis);

	tASTNode_FuncCall * funccall = new tASTNode_FuncCall(pos, true);
	funccall->SetExpression(super_initialize);

	tASTNode_ExprStmt * funccall_expr = new tASTNode_ExprStmt(pos, funccall);

	// function 定義のブロックを作成
	tASTNode_Context * body = new tASTNode_Context(pos, actBlock, RISSE_WS("Block"));
	body->AddChild(funccall_expr);

	// function 定義を作成
	tASTNode_FuncDecl * funcdecl = new tASTNode_FuncDecl(pos);
	funcdecl->SetBody(body);
	funcdecl->SetName(RISSE_WS("initialize"));
	tASTNode_VarDecl * funcvardecl = new tASTNode_VarDecl(pos);
	tASTNode_VarDeclPair * funcvardeclpair = new tASTNode_VarDeclPair(
			pos, new tASTNode_Id(pos, RISSE_WS("initialize"), false), funcdecl);
	funcvardecl->AddChild(funcvardeclpair);

	// var 定義を返す
	return funcvardecl;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tASTNode * tASTNode_ClassDecl::GenerateDefaultConstructAST(risse_size pos)
{
	// function construct() { }
	// を表すASTノードを生成して返す

	// function 定義のブロックを作成
	tASTNode_Context * body = new tASTNode_Context(pos, actBlock, RISSE_WS("Block"));

	// function 定義を作成
	tASTNode_FuncDecl * funcdecl = new tASTNode_FuncDecl(pos);
	funcdecl->SetBody(body);
	funcdecl->SetName(RISSE_WS("construct"));
	tASTNode_VarDecl * funcvardecl = new tASTNode_VarDecl(pos);
	tASTNode_VarDeclPair * funcvardeclpair = new tASTNode_VarDeclPair(
			pos, new tASTNode_Id(pos, RISSE_WS("construct"), false), funcdecl);
	funcvardecl->AddChild(funcvardeclpair);
	funcdecl->SetBody(body);

	// function 定義を返す
	return funcvardecl;
}
//---------------------------------------------------------------------------

} // namespace Risse

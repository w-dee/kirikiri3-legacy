//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
#include "../risseScriptBlockBase.h"
#include "../risseStaticStrings.h"

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
void tRisseASTNode::Dump(tRisseString & result, risse_int level)
{
	result += RisseASTNodeTypeNames[Type];
	tRisseString comment = GetDumpComment();
	if(!comment.IsEmpty())
		result += tRisseString(RISSE_WS(" (%1)"), comment);
#ifdef RISSE_TEXT_OUT_CRLF
	result += RISSE_WS("\r\n");
#else
	result += RISSE_WS("\n");
#endif

	level ++;

	risse_size child_count = GetChildCount();
	for(risse_size i = 0; i < child_count; i++)
	{
		result += tRisseString(RISSE_WS(" ")).Times(level) + GetChildNameAt(i);
		result += RISSE_WC(':');
		tRisseASTNode * child = GetChildAt(i);
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
tRisseString tRisseASTNode_List::GetChildNameAt(risse_size index) const
{
	if(index < Array.size())
	{
		risse_char buf[40];
		return tRisseString(RISSE_WS("node")) + Risse_int64_to_str(index, buf);
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_OneExpression::GetChildNameAt(risse_size index) const
{
	if(index == 0) return RISSE_WS("expression"); else return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Context::GetDumpComment() const
{
	return RisseASTContextTypeNames[ContextType];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Factor::GetDumpComment() const
{
	tRisseString ret = RisseASTFactorTypeNames[FactorType];
	if(FactorType == aftConstant)
	{
		ret += RISSE_WS(" ");
		ret += Value.AsHumanReadable();
	}
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Id::GetDumpComment() const
{
	if(IsPrivate)
		return RISSE_WS("@ ") + Name.AsHumanReadable();
	else
		return Name.AsHumanReadable();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Trinary::GetDumpComment() const
{
	return RisseASTTrinaryTypeNames[TrinaryType];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_VarDecl::GetChildNameAt(risse_size index) const
{
	if(index < inherited::GetChildCount())
	{
		risse_char buf[40];
		return tRisseString(RISSE_WS("item")) + Risse_int64_to_str(index, buf);
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_VarDeclPair::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("name");
	case 1: return RISSE_WS("initializer");
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_MemberSel::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("object");
	case 1: return RISSE_WS("membername");
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_MemberSel::GetDumpComment() const
{
	tRisseString str = RisseASTMemberAccessTypeNames[AccessType];
	tRisseString flags = Flags.AsString();
	if(!flags.IsEmpty())
	{
		str += RISSE_WS(" flags=<");
		str += flags;
		str += RISSE_WS(">");
	}
	tRisseString attribs = Attribute.AsString();
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
tRisseString tRisseASTNode_Unary::GetChildNameAt(risse_size index) const
{
	if(index == 0)
		return RISSE_WS("child");
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Unary::GetDumpComment() const
{
	return RisseASTUnaryTypeNames[UnaryType];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Binary::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("child0");
	case 1: return RISSE_WS("child1");
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Binary::GetDumpComment() const
{
	return RisseASTBinaryTypeNames[BinaryType];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Trinary::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("child0");
	case 1: return RISSE_WS("child1");
	case 2: return RISSE_WS("child2");
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_InContextOf::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("instance");
	case 1: return RISSE_WS("context");
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_InContextOf::GetDumpComment() const
{
	tRisseString ret;
	if(Context == NULL) ret = RISSE_WS("dynamic");
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseASTNode_Array::Strip()
{
	while(GetChildCount() > 0 && GetLastChild() == NULL) PopChild();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Array::GetChildNameAt(risse_size index) const
{
	if(index < inherited::GetChildCount())
	{
		risse_char buf[40];
		return tRisseString(RISSE_WS("item")) + Risse_int64_to_str(index, buf);
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Dict::GetChildNameAt(risse_size index) const
{
	if(index < inherited::GetChildCount())
	{
		risse_char buf[40];
		return tRisseString(RISSE_WS("item")) + Risse_int64_to_str(index, buf);
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_DictPair::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("name");
	case 1: return RISSE_WS("value");
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_RegExp::GetDumpComment() const
{
	return tRisseString(RISSE_WS("pattern=")) + Pattern.AsHumanReadable() +
		RISSE_WS(", flags=") + Flags.AsHumanReadable();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_If::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("condition");
	case 1: return RISSE_WS("true");
	case 2: return RISSE_WS("false");
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_While::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("condition");
	case 1: return RISSE_WS("body");
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_While::GetDumpComment() const
{
	if(SkipFirstCheck)
		return RISSE_WS("SkipFirstCheck");
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_For::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("initializer");
	case 1: return RISSE_WS("condition");
	case 2: return RISSE_WS("iterator");
	case 3: return RISSE_WS("body");
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_With_Switch::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("object");
	case 1: return RISSE_WS("body");
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Case::GetDumpComment() const
{
	if(GetExpression() == NULL) return RISSE_WS("default");
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Try::GetChildNameAt(risse_size index) const
{
	if(index == 0) return RISSE_WS("body");
	if(index == inherited::GetChildCount() + 1) return RISSE_WS("finally");
	index --;
	if(index < inherited::GetChildCount())
	{
		risse_char buf[40];
		return tRisseString(RISSE_WS("catch")) + Risse_int64_to_str(index, buf);
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Catch::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return RISSE_WS("condition");
	case 1: return RISSE_WS("body");
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_Catch::GetDumpComment() const
{
	return tRisseString(RISSE_WS("variable=")) + Name.AsHumanReadable();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_FuncCall::GetChildNameAt(risse_size index) const
{
	if(index == 0) return RISSE_WS("function");
	index --;
	if(index < inherited::GetChildCount())
	{
		risse_char buf[40];
		return tRisseString(RISSE_WS("argument")) + Risse_int64_to_str(index, buf);
	}
	index -= inherited::GetChildCount();
	if(index < Blocks.size())
	{
		risse_char buf[40];
		return tRisseString(RISSE_WS("block")) + Risse_int64_to_str(index, buf);
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_FuncCall::GetDumpComment() const
{
	tRisseString ret;
	if(CreateNew) ret += RISSE_WS("create_new");
	if(Omit) { if(!ret.IsEmpty()) ret += RISSE_WC(' '); ret += RISSE_WS("omit_arg"); }
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_FuncCallArg::GetChildNameAt(risse_size index) const
{
	if(index == 0)
		return RISSE_WS("expression");
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_FuncCallArg::GetDumpComment() const
{
	if(Expand)
		return RISSE_WS("expand");
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_FuncDecl::GetChildNameAt(risse_size index) const
{
	if(index == inherited::GetChildCount() + Blocks.size()) return RISSE_WS("body");
	if(index < inherited::GetChildCount())
	{
		risse_char buf[40];
		return tRisseString(RISSE_WS("argument")) + Risse_int64_to_str(index, buf);
	}
	index -= inherited::GetChildCount();
	if(index < Blocks.size())
	{
		risse_char buf[40];
		return tRisseString(RISSE_WS("block")) + Risse_int64_to_str(index, buf);
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_FuncDecl::GetDumpComment() const
{
	tRisseString attrib = Attribute.AsString();
	if(!attrib.IsEmpty()) attrib += RISSE_WC(' ');
	if(Name.IsEmpty()) attrib += RISSE_WS("anonymous"); else attrib += Name;
	if(IsBlock) attrib += RISSE_WS(" block");
	return attrib;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_FuncDeclArg::GetChildNameAt(risse_size index) const
{
	if(index == 0)
		return RISSE_WS("initializer");
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_FuncDeclArg::GetDumpComment() const
{
	tRisseString ret = Name.AsHumanReadable();
	if(Collapse)
		ret += RISSE_WS(", collapse");
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_FuncDeclBlock::GetDumpComment() const
{
	return Name.AsHumanReadable();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_PropDecl::GetChildNameAt(risse_size index) const
{
	switch(index)
	{
	case 0: return tRisseString(RISSE_WS("setter(argument=")) +
			SetterArgumentName.AsHumanReadable() + RISSE_WS(")");
	case 1: return RISSE_WS("getter");
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_PropDecl::GetDumpComment() const
{
	tRisseString attrib = Attribute.AsString();
	if(!attrib.IsEmpty()) attrib += RISSE_WC(' ');
	if(Name.IsEmpty()) return attrib + RISSE_WS("anonymous");
	return attrib + Name;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_ClassDecl::GetChildNameAt(risse_size index) const
{
	if(index == 0) return RISSE_WS("super");
	if(index == 1) return RISSE_WS("body");
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_ClassDecl::GetDumpComment() const
{
	tRisseString type = IsModule ?
					RISSE_WS("module ") :
					RISSE_WS("class ");
	tRisseString attrib = Attribute.AsString();
	if(!attrib.IsEmpty()) attrib += RISSE_WC(' ');
	if(Name.IsEmpty()) return type + attrib + RISSE_WS("anonymous");
	return type + attrib + Name;
}
//---------------------------------------------------------------------------














/*
	ここから SSA 形式の生成に関わる部分
*/














//---------------------------------------------------------------------------
tRisseString tRisseASTNode::GetAccessTargetId()
{
	if(!this) return tRisseString();
	if(GetType() == antId) return reinterpret_cast<tRisseASTNode_Id*>(this)->GetName();

	// obj.mem.mem.mem ... 等の . 演算子 / :: 演算子をどんどんと右にたどっていき、
	// id を見つける。見つかったらそれを返す。
	tRisseASTNode * target = this;
	while(target->GetType() == antMemberSel)
	{
		tRisseASTNode * membername = reinterpret_cast<tRisseASTNode_MemberSel*>(target)->GetMemberName();
		if(membername->GetType() == antId) return reinterpret_cast<tRisseASTNode_Id*>(membername)->GetName();
		target = membername;
	}

	// 特定できない
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisseASTNode::SearchEndPosition() const
{
	if(Type == antContext)
		return ((const tRisseASTNode_Context*)this)->GetEndPosition();

	risse_size pos = Position;
	risse_size child_count = GetChildCount();
	for(risse_size i = 0; i < child_count; i++)
	{
		tRisseASTNode * child = GetChildAt(i);
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
tRisseSSAVariable * tRisseASTNode_Context::DoReadSSA(
			tRisseSSAForm *form, void * param) const
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
	return form->GetLocalNamespace()->Read(form, GetEndPosition(), ss_lastEvalResultHiddenVarName);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_ExprStmt::DoReadSSA(
			tRisseSSAForm *form, void * param) const
{
	// このノードは式を保持しているだけなので子ノードに処理をさせるだけ
	// ただし、node は NULL の可能性がある(空文の場合)
	tRisseASTNode * node = GetChildAt(0);
	tRisseSSAVariable * res = NULL;
	if(node) res = node->GenerateReadSSA(form);

	// このノードは子の答えを返す
	form->WriteLastEvalResult(GetPosition(), res);
	return res;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_Factor::DoReadSSA(
			tRisseSSAForm *form, void * param) const
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
			return form->GetThisProxy();

	case aftSuper:		// "super"
		{
			// 文を作成して戻る
			// ローカル変数から "super" を探す
			tRisseSSAVariable * ret_var =
				form->GetLocalNamespace()->Read(form, GetPosition(), RISSE_WS("super"));
			if(!ret_var)
			{
				// "super" がみつからない、すなわちそこには super キーワードをおけない
				tRisseCompileExceptionClass::Throw(
					tRisseString(
					RISSE_WS_TR("cannot use 'super' keyword here")),
						form->GetScriptBlock(), GetPosition());
			}
			return ret_var;
		}

	case aftGlobal:		// "global"
		{
			// 文を作成して戻る
			tRisseSSAVariable * ret_var =
				form->AddVariableWithStatement(GetPosition(), ocAssignGlobal);
			ret_var->SetValueType(tRisseVariant::vtObject); // 結果は常に vtObject
			return ret_var;
		}

	case aftBinding:		// "(@)"
		{
			// 文を作成して戻る
			tRisseSSAVariable * ret_var =
				form->AddVariableWithStatement(GetPosition(), ocAssignNewBinding);
			ret_var->SetValueType(tRisseVariant::vtObject); // 結果は常に vtObject

			// bindingオブジェクトに変数とそのレジスタ番号の対応を追加する
			form->AddBindingMap(ret_var);

			return ret_var;
		}
	}
	// ありえん
	RISSE_ASSERT(!"at last at tRisseASTNode_Factor::DoReadSSA");
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseASTNode_VarDecl::SetAttribute(tRisseDeclAttribute attrib)
{
	// 子に再帰する
	for(risse_size i = 0; i < GetChildCount(); i++)
	{
		tRisseASTNode_VarDeclPair * pair = reinterpret_cast<tRisseASTNode_VarDeclPair*>(GetChildAt(i));
		RISSE_ASSERT(pair->GetType() == antVarDeclPair);
		pair->SetAttribute(attrib);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_VarDecl::DoReadSSA(
			tRisseSSAForm *form, void * param) const
{
	// 変数宣言
	// 子に再帰する
	tRisseSSAVariable * value;
	for(risse_size i = 0; i < GetChildCount(); i++)
		value = GetChildAt(i)->GenerateReadSSA(form);

	// このノードは最後に宣言された子の値を返す
	form->WriteLastEvalResult(GetPosition(), value);
	return value;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseASTNode_VarDeclPair::tRisseASTNode_VarDeclPair(risse_size position,
	tRisseASTNode * name, tRisseASTNode * initializer) :
	tRisseASTNode(position, antVarDeclPair),
		Name(name), Initializer(initializer)
{
	if(Name) Name->SetParent(this);
	if(Initializer) Initializer->SetParent(this);

	if(Name->GetType() == antMemberSel)
	{
		// Name がメンバ選択演算子だった場合
		tRisseASTNode_MemberSel * memsel =
			reinterpret_cast<tRisseASTNode_MemberSel*>(Name);
		// ofMemberEnsure を指定する
		memsel->SetFlags(memsel->GetFlags() | tRisseOperateFlags::ofMemberEnsure);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseASTNode_VarDeclPair::SetAttribute(tRisseDeclAttribute attrib)
{
	Attribute = attrib;
	if(Name->GetType() == antMemberSel)
	{
		// Name がメンバ選択演算子だった場合
		reinterpret_cast<tRisseASTNode_MemberSel*>(Name)->SetAttribute(attrib);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_VarDeclPair::DoReadSSA(
			tRisseSSAForm *form, void * param) const
{
	// 変数宣言の準備
	PrepareVarDecl(form, Name);

	// 初期化値の準備
	tRisseSSAVariable * init;
	if(Initializer)
	{
		// 初期化値がある
		init = Initializer->GenerateReadSSA(form);
	}
	else
	{
		// void の定数値の作成
		init = form->AddConstantValueStatement(GetPosition(), tRisseVariant());
	}

	// 変数宣言のSSA表現を生成する
	GenerateVarDecl(form, GetPosition(), Name, init, Attribute);

	// このノードは初期化値を返す
	return init;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseASTNode_VarDeclPair::PrepareVarDecl(tRisseSSAForm * form, const tRisseASTNode * name)
{
	if(name->GetType() == antId && !reinterpret_cast<const tRisseASTNode_Id*>(name)->GetIsPrivate())
	{
		// name ノードに id が直接来ている場合、かつプライベート (@付き) 変数ではない場合

		// グローバル変数として作成すべきかどうかをチェック
		tRisseString str_name = reinterpret_cast<const tRisseASTNode_Id*>(name)->GetName();
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
void tRisseASTNode_VarDeclPair::GenerateVarDecl(tRisseSSAForm * form,
	risse_size position,
	const tRisseASTNode * name, tRisseSSAVariable * init, tRisseDeclAttribute attrib)
{
	if(name->GetType() == antId && !reinterpret_cast<const tRisseASTNode_Id*>(name)->GetIsPrivate())
	{
		// name ノードに id が直接来ている場合、かつプライベート (@付き) 変数ではない場合
		tRisseString str_name = reinterpret_cast<const tRisseASTNode_Id*>(name)->GetName();

		// グローバル変数として作成すべきかどうかをチェック
		if(form->GetLocalNamespace()->GetHasScope())
		{
			// ローカル変数として作成する

			// ローカル変数への書き込み
			form->GetLocalNamespace()->Write(form, position, str_name, init);
		}
		else
		{
			// グローバル変数(あるいはクラス変数など)として作成する
			// this 上に変数を作成するノードを一時的に作成
			tRisseASTNode_MemberSel * write_node =
				new tRisseASTNode_MemberSel(position,
				new tRisseASTNode_Factor(position, aftThis),
				new tRisseASTNode_Factor(position, aftConstant, str_name), matDirect,

					tRisseOperateFlags(
						tRisseDeclAttribute(tRisseDeclAttribute::pcField)|
						tRisseOperateFlags::ofMemberEnsure)
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
void * tRisseASTNode_MemberSel::PrepareSSA(
		tRisseSSAForm *form, tPrepareMode mode) const
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
tRisseSSAVariable * tRisseASTNode_MemberSel::DoReadSSA(
			tRisseSSAForm *form, void * param) const
{
	// メンバ選択演算子
	tPrepareSSA * pws = reinterpret_cast<tPrepareSSA *>(param);

	// 文の作成
	tRisseSSAVariable * ret_var = NULL;
	tRisseOpCode code = ocNoOperation;
	tRisseOperateFlags new_flags = Flags;
	switch(AccessType)
	{
	case matDirect:		code = ocDGet;		break;
	case matDirectThis:	code = ocDGet;
			new_flags = new_flags | tRisseOperateFlags::ofUseThisAsContext;	break;
	case matIndirect:	code = ocIGet;		break;
	}
	RISSE_ASSERT(code != ocNoOperation);

	tRisseSSAStatement * stmt =
		form->AddStatement(GetPosition(), code, &ret_var,
							pws->ObjectVar, pws->MemberNameVar);
	stmt->SetAccessFlags(new_flags);

	// 戻りの変数を返す
	return ret_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseASTNode_MemberSel::DoWriteSSA(
		tRisseSSAForm *form, void * param, tRisseSSAVariable * value) const
{
	tPrepareSSA * pws = reinterpret_cast<tPrepareSSA *>(param);

	// 文の作成
	tRisseOperateFlags new_flags = Flags;
	tRisseOpCode code = ocNoOperation;
	switch(AccessType)
	{
	case matDirect:		code = ocDSet;		break;
	case matDirectThis:	code = ocDSet;
			new_flags = new_flags | tRisseOperateFlags::ofUseThisAsContext;	break;
	case matIndirect:	code = ocISet;		break;
	}
	RISSE_ASSERT(code != ocNoOperation);

	tRisseSSAStatement * stmt =
		form->AddStatement(GetPosition(), code, NULL,
							pws->ObjectVar, pws->MemberNameVar, value);
	stmt->SetAccessFlags(new_flags);

	if(Attribute.HasAny())
	{
		RISSE_ASSERT(AccessType == matDirect || AccessType == matDirectThis);
		// 属性を持っている場合はそれを設定する文を作成する
		tRisseSSAStatement * stmt =
			form->AddStatement(GetPosition(), ocDSetAttrib, NULL,
							pws->ObjectVar, pws->MemberNameVar);
		stmt->SetAccessFlags(Attribute);
	}

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tRisseASTNode_Id::PrepareSSA(
			tRisseSSAForm *form, tPrepareMode mode) const
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
tRisseSSAVariable * tRisseASTNode_Id::DoReadSSA(
			tRisseSSAForm *form, void * param) const
{
	// PrepareSSA と このメソッドの間でローカル変数の状態が変わると意図した
	// 動作をしない可能性があるので注意
	tPrepareSSA * pws = reinterpret_cast<tPrepareSSA *>(param);

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
		return form->GetLocalNamespace()->Read(form, GetPosition(), Name);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseASTNode_Id::DoWriteSSA(
		tRisseSSAForm *form, void * param,
		tRisseSSAVariable * value) const
{
	// PrepareSSA と このメソッドの間でローカル変数の状態が変わると意図した
	// 動作をしない可能性があるので注意
	tPrepareSSA * pws = reinterpret_cast<tPrepareSSA *>(param);

	if(pws->MemberSel)
	{
		// ローカル変数には見つからなかった
		// tRisseASTNode_MemberSel::DoWriteSSA を呼ぶ
		pws->MemberSel->DoWriteSSA(form, pws->MemberSelParam, value);
		return true;
	}
	else
	{
		// ローカル変数に見つかった;ローカル変数に上書きする
		bool result = form->GetLocalNamespace()->Write(form, GetPosition(), Name, value);
		RISSE_ASSERT(result == true);
		return result;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const tRisseASTNode_MemberSel * tRisseASTNode_Id::CreatePrivateAccessNodeOnThis(const tRisseString & prefix, bool write) const
{
	// private (@つき) 変数の場合は、クラス名 + '_' + 変数名を this 上において
	// アクセスこととなる。
	// 匿名クラス/モジュールの場合や、そもそも関数グループがクラスやモジュールでない
	// 場合は GetClassName は空文字列が帰るため、その場合は単に '_' がプリフィクスと
	// なる
	return
		new tRisseASTNode_MemberSel(GetPosition(),
		new tRisseASTNode_Factor(GetPosition(), aftThis),
		new tRisseASTNode_Factor(GetPosition(), aftConstant, prefix + Name), matDirect,
			write ?	tRisseOperateFlags(tRisseOperateFlags::ofMemberEnsure) :
					tRisseOperateFlags()
					);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const tRisseASTNode_MemberSel * tRisseASTNode_Id::CreateAccessNodeOnThisProxy() const
{
	return
		new tRisseASTNode_MemberSel(GetPosition(),
		new tRisseASTNode_Factor(GetPosition(), aftThisProxy),
		new tRisseASTNode_Factor(GetPosition(), aftConstant, Name), matDirect);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_Unary::DoReadSSA(
			tRisseSSAForm *form, void * param) const
{

	// 単項演算子
	switch(UnaryType)
	{
	case autLogNot:	// "!" logical not
	case autBitNot:	// "~" bit not
	case autPlus:	// "+"
	case autMinus:	// "-"
		{
			// 子の計算結果を得る
			tRisseSSAVariable * child_var = Child->GenerateReadSSA(form);
			if(!child_var)
			{
				// エラー
			}
			// オペコードを決定
			tRisseOpCode oc;
			switch(UnaryType)
			{
			case autLogNot:	oc = ocLogNot;	break;
			case autBitNot:	oc = ocBitNot;	break;
			case autPlus:	oc = ocPlus;	break;
			case autMinus:	oc = ocMinus;	break;
			default: oc = ocNoOperation;
			}
			// 文の作成
			tRisseSSAVariable * ret_var = NULL;
			form->AddStatement(GetPosition(), oc, &ret_var, child_var);

		//	ret_var->SetValueType(tRisseVariant::vtBoolean); // 結果は常に vtBoolean
			//////////////////////////////////////////////////////////////////////////////////////////
			// 演算子のオーバーロードによっては ! 演算子は bool を返さない可能性がある
			// この仕様は後に変更の可能性アリ (! 演算子をオーバーロードできないようにする可能性がある)
			// 他の ~ や + などの演算子についてもそうなる可能性がある
			//////////////////////////////////////////////////////////////////////////////////////////

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
			tRisseOpCode code; // オペレーションコード
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
			tRisseSSAVariable * one_var =
				form->AddConstantValueStatement(
						GetPosition(), tRisseVariant((risse_int64)1));

			// 子ノードを準備
			void * child_param = Child->PrepareSSA(form, pmReadWrite);

			// 子ノードの値を取得
			tRisseSSAVariable * child_var = Child->DoReadSSA(form, child_param);

			// 演算を行う文を生成
			tRisseSSAVariable * processed_var = NULL;
			form->AddStatement(GetPosition(), code, &processed_var, child_var, one_var);

			// その結果を識別子に書き込む文を生成
			if(!Child->DoWriteSSA(form, child_param, processed_var))
			{
				tRisseCompileExceptionClass::Throw(
					tRisseString(
					RISSE_WS_TR("writable expression expected as an increment/decrement operand")),
						form->GetScriptBlock(), GetPosition());
			}

			// 演算結果を返す
			return is_prepositioned ? processed_var : child_var;
		}

	case autDelete:		// "delete"
		;
	}
	// ありえない
	RISSE_ASSERT(!"at last at tRisseASTNode_Unary::DoReadSSA");
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_Binary::DoReadSSA(
			tRisseSSAForm *form, void * param) const
{
	// 演算子のタイプに従って分岐
	switch(BinaryType)
	{
	case abtAssign:				// =
		{
			// 単純代入
			// 右辺の値を得る
			tRisseSSAVariable * rhs_var = Child2->GenerateReadSSA(form);

			// 左辺に書き込む
			if(!Child1->GenerateWriteSSA(form, rhs_var))
			{
				tRisseCompileExceptionClass::Throw(
					tRisseString(RISSE_WS_TR("writable expression expected at left side of '='")),
						form->GetScriptBlock(), GetPosition());
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
			tRisseOpCode code; // オペレーションコード
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
			tRisseSSAVariable * rhs_var = Child2->GenerateReadSSA(form);

			// 左辺を準備
			void * lhs_param = Child1->PrepareSSA(form, pmReadWrite);

			// 左辺の値を取得
			tRisseSSAVariable * lhs_var = Child1->DoReadSSA(form, lhs_param);

			// 演算を行う文を生成
			tRisseSSAVariable * ret_var = NULL;
			form->AddStatement(GetPosition(), code, &ret_var, lhs_var, rhs_var);

			// その結果を識別子に書き込む文を生成
			if(!Child1->DoWriteSSA(form, lhs_param, ret_var))
			{
				tRisseCompileExceptionClass::Throw(
					tRisseString(
	RISSE_WS_TR("writable expression expected at left side of compound assignment operator")),
						form->GetScriptBlock(), GetPosition());

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
			tRisseSSAVariable * lhs_var = Child1->DoReadSSA(form, lhs_param);

			// 右辺の値を取得
			tRisseSSAVariable * rhs_var = Child2->DoReadSSA(form, rhs_param);

			// 右辺の値を左辺に代入
			if(!Child1->DoWriteSSA(form, lhs_param, rhs_var))
			{
				tRisseCompileExceptionClass::Throw(
					tRisseString(
					RISSE_WS_TR("writable expression expected at left side of '<->'")),
						form->GetScriptBlock(), GetPosition());
			}

			// 左辺の値を右辺に代入
			if(!Child2->DoWriteSSA(form, rhs_param, lhs_var))
			{
				tRisseCompileExceptionClass::Throw(
					tRisseString(
					RISSE_WS_TR("writable expression expected at right side of '<->'")),
						form->GetScriptBlock(), GetPosition());
			}

			// このノードは答えを返さない
			return NULL;
		}

	case abtComma:				// ,
		Child1->GenerateReadSSA(form); // 左辺値は捨てる
		return Child2->GenerateReadSSA(form); // 右辺値を返す

	case abtIf:					// if
	case abtLogOr:				// ||
	case abtLogAnd:				// &&
		return NULL;


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
			tRisseOpCode code; // オペレーションコード
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
			tRisseSSAVariable * lhs_var = Child1->GenerateReadSSA(form);
			// 右辺の値を得る
			tRisseSSAVariable * rhs_var = Child2->GenerateReadSSA(form);

			// 演算を行う文を生成
			tRisseSSAVariable * ret_var = NULL;
			form->AddStatement(GetPosition(), code, &ret_var, lhs_var, rhs_var);

			// 戻る
			return ret_var;
		}

	default:
		return NULL;
	}
	RISSE_ASSERT(!"at last at tRisseASTNode_Binary::DoReadSSA");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_Trinary::DoReadSSA(
			tRisseSSAForm *form, void * param) const
{
	RISSE_ASSERT(TrinaryType == attCondition);
	return tRisseASTNode_If::InternalDoReadSSA(form, GetPosition(),
		RISSE_WS("cond"), Child1, Child2, Child3, true);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_InContextOf::DoReadSSA(
			tRisseSSAForm *form, void * param) const
{

	// 左辺の値を得る
	tRisseSSAVariable * lhs_var = Instance->GenerateReadSSA(form);
	// 右辺の値を得る
	tRisseSSAVariable * rhs_var = Context ?  Context->GenerateReadSSA(form) : NULL;

	// 演算を行う文を生成
	tRisseSSAVariable * ret_var = NULL;
	form->AddStatement(GetPosition(), ocInContextOf, &ret_var, lhs_var, rhs_var);
		// Context が NULL の場合は rhs_var も NULL になるので注意

	// 戻る
	return ret_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tRisseASTNode_Array::PrepareSSA(tRisseSSAForm *form, tPrepareMode mode) const
{
	// この数値を超える量の インデックス用数値定数はここでは作成しない
	// (大量のインデックス用定数が変数領域を埋め尽くすのを避けるため)
	const risse_size max_prepare_index = 4; 

	// 配列の要素それぞれに対してprepareを行う
	tPrepareSSA * data = new tPrepareSSA();
	data->Elements.reserve(GetChildCount());
	data->Indices.reserve(std::max(max_prepare_index, GetChildCount()));

	for(risse_size i = 0; i < GetChildCount(); i++)
	{
		// 各要素の準備
		tRisseASTNode * child = GetChildAt(i);
		if(child)
			data->Elements.push_back(child->PrepareSSA(form, mode));
		else
			data->Elements.push_back(NULL);

		// インデックスの準備
		if(mode == pmWrite && !child)
		{
			// 読み込みを伴わない 、かつ childが NULL の場合はインデックスを準備する必要はない
			data->Indices.push_back(NULL);
		}
		else /* mode == pmRead || mode == pmReadWrite */
		{
			if(i < max_prepare_index)
			{
				tRisseSSAVariable * index_var =
					form->AddConstantValueStatement(GetPosition(), (risse_int64)i);

				data->Indices.push_back(index_var);
			}
		}
	}

	return data;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_Array::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	// インライン配列からの読み出し (配列オブジェクトを作成し、初期化して返す)
	tPrepareSSA * data = reinterpret_cast<tPrepareSSA*>(param);

	// 配列オブジェクトを作成
	tRisseSSAVariable * array_var = NULL;
	form->AddStatement(GetPosition(), ocAssignNewArray, &array_var);
	array_var->SetValueType(tRisseVariant::vtObject); // 結果は常に object

	// 配列の reserve メソッドを呼び出し、配列数を予約する
	if(GetChildCount() != 0)
	{
		tRisseSSAVariable * element_count_var =
			form->AddConstantValueStatement(GetPosition(), (risse_int64)GetChildCount());
		array_var->GenerateFuncCall(GetPosition(), RISSE_WS("reserve"), element_count_var);
	}

	// 各配列の要素となる変数を作成しつつ、配列に設定していく
	RISSE_ASSERT(data->Elements.size() == GetChildCount());
	for(risse_size i = 0; i < GetChildCount(); i++)
	{
		// 内容
		tRisseSSAVariable * element_var;
		tRisseASTNode * child = GetChildAt(i);
		if(child)
			element_var = child->DoReadSSA(form, data->Elements[i]);
		else
			element_var =
				form->AddConstantValueStatement(GetPosition(), tRisseVariant());

		// インデックス用数値定数
		tRisseSSAVariable * index_var;
		if(i < data->Indices.size())
			index_var = data->Indices[i]; // あらかじめインデックス用定数が用意されている
		else
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
bool tRisseASTNode_Array::DoWriteSSA(tRisseSSAForm *form, void * param,
		tRisseSSAVariable * value) const
{
	// インライン配列への書き込み
	tPrepareSSA * data = reinterpret_cast<tPrepareSSA*>(param);

	RISSE_ASSERT(data->Elements.size() == GetChildCount());
	for(risse_size i = 0; i < GetChildCount(); i++)
	{
		tRisseASTNode * child = GetChildAt(i);
		if(child)
		{
			// インデックス用数値定数
			tRisseSSAVariable * index_var;
			if(i < data->Indices.size())
				index_var = data->Indices[i]; // あらかじめインデックス用定数が用意されている
			else
				index_var =
					form->AddConstantValueStatement(GetPosition(), (risse_int64)i);

			// 要素の値を配列から得る
			tRisseSSAVariable * elm_var = NULL;
			form->AddStatement(GetPosition(), ocIGet, &elm_var, value, index_var);

			// 各要素に対する代入文を生成
			if(!child->DoWriteSSA(form, data->Elements[i], elm_var))
			{
				// 書き込みに失敗
				risse_char i_str[40];
				Risse_int_to_str(i, i_str);
				tRisseCompileExceptionClass::Throw(
					tRisseString(
					RISSE_WS_TR("writable expression expected at array index %1"), i_str),
						form->GetScriptBlock(), GetPosition());
			}
		}
	}
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tRisseASTNode_Dict::PrepareSSA(tRisseSSAForm *form, tPrepareMode mode) const
{
	// 辞書配列用の名前と値を準備
	tPrepareSSA * data = new tPrepareSSA();
	data->Names.reserve(GetChildCount());
	data->Values.reserve(GetChildCount());

	// 各要素に対して ...
	for(risse_size i = 0; i < GetChildCount(); i++)
	{
		RISSE_ASSERT(inherited::GetChildAt(i)->GetType() == antDictPair);
		tRisseASTNode_DictPair * pair_node =
			reinterpret_cast<tRisseASTNode_DictPair*>(GetChildAt(i));

		// 名前と値を準備
		void * name_prep_data = pair_node->GetName()->PrepareSSA(form, pmRead);
													// 名前は常に読み込み扱い
		void * value_prep_data = pair_node->GetValue()->PrepareSSA(form, mode);

		data->Names.push_back(name_prep_data);
		data->Values.push_back(value_prep_data);
	}
	return data;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_Dict::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	// インライン辞書配列からの読み出し (辞書配列オブジェクトを作成し、初期化して返す)
	tPrepareSSA * data = reinterpret_cast<tPrepareSSA*>(param);

	// 辞書配列オブジェクトを作成
	tRisseSSAVariable * dict_var = NULL;
	form->AddStatement(GetPosition(), ocAssignNewDict, &dict_var);
	dict_var->SetValueType(tRisseVariant::vtObject); // 結果は常に object

	RISSE_ASSERT(data->Names.size() == GetChildCount());
	RISSE_ASSERT(data->Values.size() == GetChildCount());

	// 各要素に対して ...
	for(risse_size i = 0; i < GetChildCount(); i++)
	{
		tRisseASTNode_DictPair * pair_node =
			reinterpret_cast<tRisseASTNode_DictPair*>(GetChildAt(i));

		// 名前の値を得る
		tRisseSSAVariable * name_var =
			pair_node->GetName()->DoReadSSA(form, data->Names[i]);

		// 値の値を得る
		tRisseSSAVariable * value_var =
			pair_node->GetValue()->DoReadSSA(form, data->Values[i]);

		// 代入文を生成
		form->AddStatement(GetPosition(), ocISet, NULL,
								dict_var, name_var, value_var);
	}
	return dict_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseASTNode_Dict::DoWriteSSA(tRisseSSAForm *form, void * param,
		tRisseSSAVariable * value) const
{
	// インライン辞書配列への書き込み (右辺値を辞書配列と見なし、値を設定する)
	tPrepareSSA * data = reinterpret_cast<tPrepareSSA*>(param);

	RISSE_ASSERT(data->Names.size() == GetChildCount());
	RISSE_ASSERT(data->Values.size() == GetChildCount());

	// 各要素に対して ...
	for(risse_size i = 0; i < GetChildCount(); i++)
	{
		tRisseASTNode_DictPair * pair_node =
			reinterpret_cast<tRisseASTNode_DictPair*>(GetChildAt(i));

		// 名前の値を得る
		tRisseSSAVariable * name_var =
			pair_node->GetName()->DoReadSSA(form, data->Names[i]);

		// その名前に対する値を得る
		tRisseSSAVariable * value_var = NULL;
		form->AddStatement(GetPosition(), ocIGet, &value_var,
								value, name_var);

		// 値を設定する
		if(!pair_node->GetValue()->DoWriteSSA(form, data->Values[i], value_var))
		{
			// 書き込みに失敗
			risse_char i_str[40];
			Risse_int_to_str(i, i_str);
			tRisseCompileExceptionClass::Throw(
				tRisseString(
				RISSE_WS_TR("writable expression expected at value of dictionary element index %1"), i_str),
					form->GetScriptBlock(), GetPosition());
		}
	}
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_RegExp::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	// 文字列定数を作成
	tRisseSSAVariable * pattern_var =
			form->AddConstantValueStatement(GetPosition(), Pattern);
	tRisseSSAVariable * flags_var =
			form->AddConstantValueStatement(GetPosition(), Flags);
	// 正規表現オブジェクトを作成する文を生成してその結果を返す
	tRisseSSAVariable * regexp_var = NULL;
	form->AddStatement(GetPosition(), ocAssignNewRegExp, &regexp_var,
		pattern_var, flags_var);
	regexp_var->SetValueType(tRisseVariant::vtObject); // 結果は常に object
	return regexp_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_If::InternalDoReadSSA(tRisseSSAForm *form,
		risse_size pos,
		const tRisseString &basename,
		tRisseASTNode * condition,
		tRisseASTNode * truenode,
		tRisseASTNode * falsenode,
		bool needresult
		)
{
	// 条件式の結果を得る
	tRisseSSAVariable * cond_var = condition->GenerateReadSSA(form);

	// 分岐文を作成
	tRisseSSAStatement * branch_stmt =
		form->AddStatement(pos, ocBranch, NULL, cond_var);

	// 新しい基本ブロックを作成(真の場合)
	form->GetCurrentBlock();
	tRisseSSABlock * true_block = form->CreateNewBlock(basename + RISSE_WS("_true"));

	// 真の場合に実行する内容を作成
	tRisseSSAVariable * true_var = truenode->GenerateReadSSA(form);

	// もし 値を得られないような物であれば void を代わりに使う
	if(!true_var) true_var = form->AddConstantValueStatement(pos, tRisseVariant());
	form->GetCurrentBlock();

	// ジャンプ文を作成
	tRisseSSAStatement * true_exit_jump_stmt =
		form->AddStatement(pos, ocJump, NULL);

	// 偽の場合に実行するノードがある場合のみ
	tRisseSSABlock * false_last_block = NULL;
	tRisseSSABlock * false_block = NULL;
	tRisseSSAStatement * false_exit_jump_stmt = NULL;
	tRisseSSAVariable * false_var = NULL;

	if(falsenode)
	{
		// 新しい基本ブロックを作成(偽の場合)
		false_block = form->CreateNewBlock(basename + RISSE_WS("_false"));

		// 偽の場合に実行する内容を作成
		false_var = falsenode->GenerateReadSSA(form);

		// もし 値を得られないような物であれば void を代わりに使う
		if(!false_var) false_var = form->AddConstantValueStatement(pos, tRisseVariant());

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
			false_var = form->AddConstantValueStatement(pos, tRisseVariant());
			false_last_block = form->GetCurrentBlock();

			// ジャンプ文を作成
			false_exit_jump_stmt =
				form->AddStatement(pos, ocJump, NULL);
		}
	}


	// 新しい基本ブロックを作成(if文からの脱出)
	tRisseSSABlock * if_exit_block;
	if_exit_block = form->CreateNewBlock(basename + RISSE_WS("_exit"));

	// 分岐/ジャンプ文のジャンプ先を設定
	branch_stmt->SetTrueBranch(true_block);
	branch_stmt->SetFalseBranch(false_block ? false_block : if_exit_block);
	true_exit_jump_stmt->SetJumpTarget(if_exit_block);
	if(false_exit_jump_stmt) false_exit_jump_stmt->SetJumpTarget(if_exit_block);

	// もし答えが必要ならば、答えを返すための φ関数を作成する
	tRisseSSAVariable * ret_var = NULL;
	if(needresult)
		form->AddStatement(pos, ocPhi, &ret_var, true_var, false_var);

	return ret_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_If::DoReadSSA(
			tRisseSSAForm *form, void * param) const
{
	// if文を作成
	tRisseSSAVariable * res = 
		InternalDoReadSSA(form, GetPosition(), RISSE_WS("if"), Condition,
			True, False, true);

	// 結果は _ に書き込まれるとともにこの関数の戻り値となる
	form->WriteLastEvalResult(GetPosition(), res);
	return res;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_While::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	// while ループ または do ～ while ループ

	// break に関する情報を生成
	tRisseBreakInfo * break_info = new tRisseBreakInfo(form);

	// break に関する情報を form に設定
	tRisseBreakInfo * old_break_info = form->SetCurrentBreakInfo(break_info);

	// continue に関する情報を生成
	tRisseContinueInfo * continue_info = new tRisseContinueInfo(form);

	// continue に関する情報を form に設定
	tRisseContinueInfo * old_continue_info = form->SetCurrentContinueInfo(continue_info);

	// 条件式または body にジャンプするための文を作成
	form->GetCurrentBlock();
	tRisseSSAStatement * entry_jump_stmt =
		form->AddStatement(SkipFirstCheck?Body->GetPosition():Condition->GetPosition(), ocJump, NULL);

	// 条件式を格納する基本ブロックを作成
	tRisseSSABlock * while_cond_block =
		form->CreateNewBlock(RISSE_WS("while_cond"));

	// 条件式の結果を得る
	tRisseSSAVariable * cond_var = Condition->GenerateReadSSA(form);

	// 分岐文を作成
	form->GetCurrentBlock();
	tRisseSSAStatement * branch_stmt =
		form->AddStatement(Condition->GetPosition(), ocBranch, NULL, cond_var);

	// 新しい基本ブロックを作成(条件式が真の場合)
	tRisseSSABlock * while_body_block =
		form->CreateNewBlock(RISSE_WS("while_body"));

	// while 文の body を生成
	Body->GenerateReadSSA(form);
	form->GetCurrentBlock();

	// ジャンプ文を作成
	tRisseSSAStatement * while_body_jump_stmt =
		form->AddStatement(GetPosition(), ocJump, NULL);

	// 新しい基本ブロックを作成(while文からの脱出)
	tRisseSSABlock * while_exit_block =
		form->CreateNewBlock(RISSE_WS("while_exit"));

	// _ に void を代入 (値付き break で終了しないループの値は void )
	form->WriteLastEvalResult(GetPosition(),
		form->AddConstantValueStatement(GetPosition(), tRisseVariant()));
	tRisseSSAStatement * while_exit_jump_stmt = form->AddStatement(GetPosition(), ocJump, NULL);

	// 分岐/ジャンプ文のジャンプ先を設定
	entry_jump_stmt->SetJumpTarget(SkipFirstCheck?while_body_block:while_cond_block);
	while_body_jump_stmt->SetJumpTarget(while_cond_block);
	branch_stmt->SetTrueBranch(while_body_block);
	branch_stmt->SetFalseBranch(while_exit_block);

	// 新しい基本ブロックを作成(break文のターゲット)
	tRisseSSABlock * while_break_target_block =
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
	// あるいは break 文によって tRisseSSAForm::AddBreakOrContinueStatement() 内で
	// _ への書き込みが行われている。
	tRisseSSAVariable * res = form->GetLocalNamespace()->Read(
					form, GetPosition(), ss_lastEvalResultHiddenVarName);
	return res;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_For::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	// for ループ
	// for ループの第１節ではローカル変数を宣言する可能性があるので
	// スコープを作っておく
	form->GetLocalNamespace()->Push(); // スコープを push

	// 初期化ノードを生成する
	if(Initializer) Initializer->GenerateReadSSA(form);


	// 条件式にジャンプするための文を作成
	form->GetCurrentBlock();
	tRisseSSAStatement * entry_jump_stmt =
		form->AddStatement(GetPosition(), ocJump, NULL);

	// 条件式を格納する基本ブロックを作成
	tRisseSSABlock * for_cond_block =
		form->CreateNewBlock(RISSE_WS("for_cond"));

	// break に関する情報を生成
	tRisseBreakInfo * break_info = new tRisseBreakInfo(form);

	// break に関する情報を form に設定
	tRisseBreakInfo * old_break_info = form->SetCurrentBreakInfo(break_info);

	// continue に関する情報を生成
	tRisseContinueInfo * continue_info = new tRisseContinueInfo(form);

	// continue に関する情報を form に設定
	tRisseContinueInfo * old_continue_info = form->SetCurrentContinueInfo(continue_info);

	// 条件式の結果を得る (条件式が省略されている場合は常に真であると見なす)
	tRisseSSAVariable * cond_var = NULL;
	if(Condition)
		cond_var = Condition->GenerateReadSSA(form);
	else
		cond_var = form->AddConstantValueStatement(GetPosition(), tRisseVariant(true));

	// 分岐文を作成
	form->GetCurrentBlock();
	tRisseSSAStatement * branch_stmt =
		form->AddStatement(GetPosition(), ocBranch, NULL, cond_var);

	// 新しい基本ブロックを作成(条件式が真の場合)
	tRisseSSABlock * for_body_block =
		form->CreateNewBlock(RISSE_WS("for_body"));

	// for 文の body を生成
	Body->GenerateReadSSA(form);
	form->GetCurrentBlock();

	// ジャンプ文を作成
	tRisseSSAStatement * for_body_jump_stmt =
		form->AddStatement(GetPosition(), ocJump, NULL);

	// 新しい基本ブロックを作成(イテレータのため)
	tRisseSSABlock * for_iter_block = NULL;
	tRisseSSAStatement * for_iter_jump_stmt = NULL;
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
	tRisseSSABlock * for_exit_block =
		form->CreateNewBlock(RISSE_WS("for_exit"));

	// _ に void を代入 (値付き break で終了しないループの値は void )
	form->WriteLastEvalResult(GetPosition(),
		form->AddConstantValueStatement(GetPosition(), tRisseVariant()));
	tRisseSSAStatement * for_exit_jump_stmt = form->AddStatement(GetPosition(), ocJump, NULL);

	// 分岐/ジャンプ文のジャンプ先を設定
	entry_jump_stmt->SetJumpTarget(for_cond_block);
	for_body_jump_stmt->SetJumpTarget(for_iter_block ? for_iter_block : for_cond_block);
	if(for_iter_jump_stmt) for_iter_jump_stmt->SetJumpTarget(for_cond_block);
	branch_stmt->SetTrueBranch(for_body_block);
	branch_stmt->SetFalseBranch(for_exit_block);

	// 新しい基本ブロックを作成(break文のターゲット)
	tRisseSSABlock * for_break_target_block =
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
	// あるいは break 文によって tRisseSSAForm::AddBreakOrContinueStatement() 内で
	// _ への書き込みが行われている。
	tRisseSSAVariable * res = form->GetLocalNamespace()->Read(
					form, GetPosition(), ss_lastEvalResultHiddenVarName);
	return res;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_Return::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	tRisseSSAVariable * var;
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
tRisseSSAVariable * tRisseASTNode_Throw::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	tRisseSSAVariable * var;
	if(Expression)
	{
		// 戻りとなる値を作成する
		var = Expression->GenerateReadSSA(form);
	}
	else
	{
		// 戻りとなる値は catch ブロックが受け取った値
		// TODO: catchブロックが受け取った値を投げる
		var = form->AddConstantValueStatement(GetPosition(), tRisseVariant());
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
tRisseSSAVariable * tRisseASTNode_Break::DoReadSSA(
									tRisseSSAForm *form, void * param) const
{
	tRisseSSAVariable * var;
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
tRisseSSAVariable * tRisseASTNode_Continue::DoReadSSA(
									tRisseSSAForm *form, void * param) const
{
	tRisseSSAVariable * var;
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
tRisseSSAVariable * tRisseASTNode_Label::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	// ジャンプ文を作成
	tRisseSSAStatement * jump_stmt =
		form->AddStatement(GetPosition(), ocJump, NULL);

	// 新しい基本ブロックを作成
	tRisseSSABlock * label_block =
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
tRisseSSAVariable * tRisseASTNode_Goto::DoReadSSA(tRisseSSAForm *form, void * param) const
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
tRisseSSAVariable * tRisseASTNode_Debugger::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	// 文を作成
	form->AddStatement(GetPosition(), ocDebugger, NULL);

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_Switch::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	// 基準式を生成する
	tRisseSSAVariable * ref = Object->GenerateReadSSA(form);

	// switch に関する情報を生成
	tRisseSwitchInfo * switch_info = new tRisseSwitchInfo(ref);

	// switch に関する情報を form に設定
	tRisseSwitchInfo * old_switch_info = form->SetCurrentSwitchInfo(switch_info);

	// break に関する情報を生成
	tRisseBreakInfo * break_info = new tRisseBreakInfo(form);
	break_info->SetNonValueBreakShouldSetVoidToLastEvalValue(false);
		// swtich 文での値なし break で _ の値が void になってしまうと困るため

	// break に関する情報を form に設定
	tRisseBreakInfo * old_break_info = form->SetCurrentBreakInfo(break_info);

	// ジャンプ文を作成
	switch_info->SetLastBlock(form->GetCurrentBlock());
	switch_info->SetLastStatement(form->AddStatement(GetPosition(), ocJump, NULL));

	// 新しい基本ブロックを作成
	form->CreateNewBlock("disconnected_by_switch");

	// ブロックの内容を生成
	Body->GenerateReadSSA(form);

	// ジャンプ文を作成
	tRisseSSAStatement * jump_stmt =
		form->AddStatement(GetPosition(), ocJump, NULL);

	// 新しい基本ブロックを作成(switch文からの脱出)
	tRisseSSABlock * exit_switch_block =
		form->CreateNewBlock(RISSE_WS("switch_exit"));
	jump_stmt->SetJumpTarget(exit_switch_block);

	// default / 最後のジャンプの処理
	tRisseSSABlock * last_block =
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
tRisseSSAVariable * tRisseASTNode_Case::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	// switch 文の中？
	tRisseSwitchInfo * info = form->GetCurrentSwitchInfo();
	if(info == NULL)
		tRisseCompileExceptionClass::Throw(
			tRisseString(Expression ?
				RISSE_WS_TR("cannot place 'case' out of switch") :
				RISSE_WS_TR("cannot 'default' out of switch")),
				form->GetScriptBlock(), GetPosition());

	// ジャンプ文を作成
	tRisseSSAStatement * jump_stmt =
		form->AddStatement(GetPosition(), ocJump, NULL);

	// 新しい基本ブロックを作成(ここに条件判断式などが入る)
	tRisseSSABlock * case_block =
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
		jump_stmt->SetJumpTarget(case_block);

		// 条件判断文を作成
		tRisseSSAVariable * targ_var = Expression->GenerateReadSSA(form);
		tRisseSSAVariable * comp_res = NULL;
		form->AddStatement(GetPosition(), ocDiscEqual, &comp_res,
			info->GetReference(), targ_var);

		// 分岐文を作成
		tRisseSSAStatement * branch_stmt =
			form->AddStatement(GetPosition(), ocBranch, NULL, comp_res);
		info->SetLastBlock(form->GetCurrentBlock());
		info->SetLastStatement(branch_stmt);

		// 新しい基本ブロックを作成(ここに内容が入る)
		tRisseSSABlock * case_body_block =
			form->CreateNewBlock(RISSE_WS("switch_case_body"));
		branch_stmt->SetTrueBranch(case_body_block);
	}
	else
	{
		// default

		// default 文のあるブロックを登録する
		if(info->GetDefaultBlock() != NULL)
			tRisseCompileExceptionClass::Throw(
				tRisseString(RISSE_WS_TR("cannot place multiple 'default' in a switch")),
				form->GetScriptBlock(), GetPosition());
		info->SetDefaultBlock(case_block);

		jump_stmt->SetJumpTarget(case_block);
	}


	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_Try::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	tRisseSSAVariable * res;
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
tRisseSSAVariable * tRisseASTNode_Try::GenerateTryCatchOrFinally(tRisseSSAForm *form,  bool is_finally) const
{
	// finallyがある？
	tRisseSSABlock * finally_entry_block = NULL;
	tRisseSSAStatement * finally_last_jump_stmt = NULL;
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
		// (なぜならばtRisseSSAFormの現状の構造では 遅延評価ブロックの評価と
		// それに続く ocCatchBranch 文の間で他の遅延評価ブロックを評価できないので)
		// 名前空間については各finallyブロック内では名前空間のpush/popを行い、
		// 名前空間に影響を与えないので、finallyブロックの評価がtryのブロックの評価
		// よりも先に来てもかまわない。


		// finally ブロックの後にジャンプするための文を生成
		tRisseSSAStatement * skip_finally_jump_stmt =
			form->AddStatement(GetPosition(), ocJump, NULL);

		// 新しい基本ブロックを生成
		finally_entry_block = form->CreateNewBlock(RISSE_WS("finally_entry"));

		// finally の中身を生成
		//- finally の結果がない場合に備えて void を _ に代入する
		form->WriteLastEvalResult(GetPosition(),
				form->AddConstantValueStatement(GetPosition(), tRisseVariant()));
		form->GetLocalNamespace()->Push(); // スコープを push
		tRisseSSAVariable * finally_res_var = Finally->GenerateReadSSA(form);
		//- finally の結果がない場合は void を _ に代入する(再度)
		if(!finally_res_var)
				form->WriteLastEvalResult(GetPosition(),
					form->AddConstantValueStatement(GetPosition(), tRisseVariant()));
		form->GetLocalNamespace()->Pop(); // スコープを pop

		// finally ブロックの最後に jump 文を生成
		finally_last_jump_stmt =
			form->AddStatement(GetPosition(), ocJump, NULL);

		// 本来の try ブロック用の基本ブロックを作成
		tRisseSSABlock * try_catch_entry_block =
			form->CreateNewBlock(RISSE_WS("try_catch_entry"));
		skip_finally_jump_stmt->SetJumpTarget(try_catch_entry_block);
	}

	// try ブロックの中身を 遅延評価ブロックとして評価する
	tRisseSSAVariable * lazyblock_var = NULL;
	tRisseSSAForm * new_form = NULL;

	// スクリプトブロックより try ブロックの通し番号を得る
	// 各 try ブロックは固有の ID をもち、例外で実装された大域脱出を行う
	// return や break, goto などの脱出先の ID となる
	risse_size try_id = form->GetScriptBlock()->AddTryIdentifier();

	// アクセスマップを作成する
	tRisseSSAVariableAccessMap * access_map = form->CreateAccessMap(GetPosition());

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
		tRisseSSAVariable * void_ret_var =
			new_form->AddConstantValueStatement(GetPosition(), tRisseVariant());
		new_form->AddStatement(GetPosition(), ocReturn, NULL, void_ret_var);
	}

	// 遅延評価ブロックで使用された変数の処理
	form->ListVariablesForLazyBlock(GetPosition(), access_map);

	// 遅延評価ブロックを実行するためのTryFuncCall文を作成
	// 関数呼び出し文を生成する
	tRisseSSAVariable * try_block_ret_var = NULL;
	tRisseSSAStatement * try_block_call_stmt =
		form->AddStatement(GetPosition(), ocTryFuncCall, &try_block_ret_var, lazyblock_var);
	try_block_call_stmt->SetFuncExpandFlags(0);

	// 遅延評価ブロックをクリーンアップ
	form->CleanupLazyBlock(lazy_param);

	// アクセスマップをクリーンアップ
	form->CleanupAccessMap(GetPosition(), access_map);

	if(is_finally)
	{
		// ここで finally の中身を呼び出す
		tRisseSSAStatement * finally_jump_stmt = form->AddStatement(GetPosition(), ocJump, NULL);

		// ocCatchBranch用の基本ブロックを作成
		tRisseSSABlock * catch_branch_block =
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
	tRisseSSAStatement * catch_branch_stmt =
		form->AddStatement(GetPosition(), ocCatchBranch, NULL, try_block_ret_var);
	catch_branch_stmt->SetTryIdentifierIndex(try_id); // try識別子を設定

	// あとで例外の分岐先を設定できるように
	form->AddCatchBranchAndExceptionValue(catch_branch_stmt, try_block_ret_var);

	if(!is_finally)
	{
		// 脱出用の新しい基本ブロックを作成
		tRisseSSABlock * exit_try_block =
			form->CreateNewBlock(RISSE_WS("exit_try"));
		// try ブロックの結果を _ に書き込むための文を生成
		form->WriteLastEvalResult(GetPosition(), try_block_ret_var);
		tRisseSSAStatement * exit_try_block_jump_stmt =
			form->AddStatement(GetPosition(), ocJump, NULL);

		// catch用の新しい基本ブロックを作成
		tRisseSSABlock * catch_entry_block =
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
		tRisseSSABlock * catch_entry_block =
			form->CreateNewBlock(RISSE_WS("try_catch_entry"));
		form->AddStatement(GetPosition(), ocThrow, NULL, try_block_ret_var);

		// 脱出用の新しい基本ブロックを作成
		tRisseSSABlock * exit_try_block =
			form->CreateNewBlock(RISSE_WS("exit_try_entry"));

		// 遅延評価ブロックを実行するためのTryFuncCall文 の分岐先を設定
		catch_branch_stmt->SetTryExitTarget(exit_try_block);
		catch_branch_stmt->SetTryCatchTarget(catch_entry_block);
	}

	// ss_lastEvalResultHiddenVarName の値を取り出す
	tRisseSSAVariable * last_value =
		form->GetLocalNamespace()->Read(form, GetPosition(), ss_lastEvalResultHiddenVarName);

	// このノードは ss_lastEvalResultHiddenVarName を返す
	return last_value;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseASTNode_Try::GenerateCatchBlock(tRisseSSAForm * form,
	tRisseSSAVariable * try_block_ret_var) const
{
	// catchブロックを順々に処理
	gc_vector<tRisseSSAStatement *> catch_exit_jumps;
	tRisseSSAStatement * branch_stmt = NULL;
	if(inherited::GetChildCount() > 0)
	{
		// catch文がある場合
		for(risse_size i = 0; i < inherited::GetChildCount(); i++)
		{
			RISSE_ASSERT(inherited::GetChildAt(i)->GetType() == antCatch);
			tRisseASTNode_Catch * catch_node =
				reinterpret_cast<tRisseASTNode_Catch*>(inherited::GetChildAt(i));

			tRisseASTNode * condition = catch_node->GetCondition();

			// ローカル変数の名前空間を push
			form->GetLocalNamespace()->Push(); // スコープを push

			// 例外を受け取る変数がある場合は例外を受け取る変数に例外を代入する
			if(!catch_node->GetName().IsEmpty())
			{
				// 変数のローカル名前空間への登録
				form->GetLocalNamespace()->Add(catch_node->GetName(), NULL);

				// ローカル変数への書き込み
				form->GetLocalNamespace()->Write(form, GetPosition(),
												catch_node->GetName(), try_block_ret_var);
			}

			// 例外 catch の条件式の処理
			tRisseSSAVariable * cond_var;
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
				tRisseASTNode_Factor * global = new tRisseASTNode_Factor(GetPosition(), aftGlobal);
				tRisseASTNode_Factor * Exception =
					new tRisseASTNode_Factor(GetPosition(), aftConstant,
						tRisseVariant(tRisseString(RISSE_WS("Exception"))));
				tRisseASTNode_MemberSel * global_Exception_node =
					new tRisseASTNode_MemberSel(GetPosition(), global, Exception, matDirect);

				// global.Exception を取得
				tRisseSSAVariable * global_Exception = global_Exception_node->GenerateReadSSA(form);

				// instanceof
				form->AddStatement(GetPosition(), ocInstanceOf, &cond_var,
					try_block_ret_var, global_Exception);
			}

			// 分岐文を作成
			branch_stmt =
				form->AddStatement(GetPosition(), ocBranch, NULL, cond_var);

			// 新しい基本ブロックを作成
			tRisseSSABlock * catch_body_block =
				form->CreateNewBlock(RISSE_WS("catch_block"));

			branch_stmt->SetTrueBranch(catch_body_block);

			// catch の内容がなかった場合に備えて void を _ に代入する
			form->WriteLastEvalResult(GetPosition(),
					form->AddConstantValueStatement(GetPosition(), tRisseVariant()));

			// catch の内容を生成
			tRisseSSAVariable * catch_result_var = catch_node->GetBody()->GenerateReadSSA(form);

			// catch の結果がない場合は void を _ に代入する(再度)
			if(!catch_result_var)
				form->WriteLastEvalResult(GetPosition(),
					form->AddConstantValueStatement(GetPosition(), tRisseVariant()));

			// catch の終了用のジャンプ文を生成
			tRisseSSAStatement * catch_exit_jump_stmt =
				form->AddStatement(GetPosition(), ocJump, NULL);
			catch_exit_jumps.push_back(catch_exit_jump_stmt);

			// ローカル変数の名前空間を pop
			form->GetLocalNamespace()->Pop(); // スコープを pop

			// 新しい基本ブロックを作成
			tRisseSSABlock * catch_next_block =
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
	tRisseSSABlock * catch_exit_block =
		form->CreateNewBlock(RISSE_WS("catch_exit"));

	for(gc_vector<tRisseSSAStatement *>::iterator i = catch_exit_jumps.begin();
			i != catch_exit_jumps.end(); i++)
		(*i)->SetJumpTarget(catch_exit_block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_FuncCall::DoReadSSA(
							tRisseSSAForm *form, void * param) const
{
	// ブロック付きかどうかを得る
	bool with_block = Blocks.size() > 0;
	RISSE_ASSERT(!(CreateNew && with_block)); // new に対してブロックは現状指定できない

	// 関数を表す式を得る
	tRisseSSAVariable * func_var = Expression->GenerateReadSSA(form);

	// 引数の列挙用配列
	gc_vector<tRisseSSAVariable *> arg_vec;
	arg_vec.reserve(GetChildCount());

	// break に関する情報を生成
	tRisseBreakInfo * break_info = new tRisseBreakInfo(form, RISSE_WS("#block_break"));
		// tRisseSSAForm::AddCatchBranchTargetsForOne は ラベル名の先頭の文字で種別を
		// 判断する。ラベル名の先頭が # の場合はすでにコード生成済みのラベルジャンプ先
		// として扱われる。
	break_info->SetIsBlock(true);

	// break に関する情報を form に設定
	tRisseBreakInfo * old_break_info = form->SetCurrentBreakInfo(break_info);

	// continue に関する情報を生成
	tRisseContinueInfo * continue_info = new tRisseContinueInfo(form);
	continue_info->SetIsBlock(true);

	// continue に関する情報を form に設定
	tRisseContinueInfo * old_continue_info = form->SetCurrentContinueInfo(continue_info);

	// ... が指定されていなければ引数を処理
	risse_uint32 exp_flag = 0; // 展開フラグ
	if(!Omit)
	{
		// 引数を処理する
		risse_uint32 exp_flag_bit = 1; // 展開フラグ用ビット
		for(risse_size i = 0; i < inherited::GetChildCount(); i++, exp_flag_bit <<= 1)
		{
			if(i >= RisseMaxArgCount)
			{
				// 現状、関数の引数に列挙できる数はRisseMaxArgCount個までとなっている
				// ので、それを超えるとエラーになる
				tRisseCompileExceptionClass::Throw(
					tRisseString(RISSE_WS_TR("too many function arguments")),
						form->GetScriptBlock(), GetPosition());
			}

			RISSE_ASSERT(!(inherited::GetChildAt(i) &&
				inherited::GetChildAt(i)->GetType() != antFuncCallArg));
			tRisseASTNode_FuncCallArg * arg =
				reinterpret_cast<tRisseASTNode_FuncCallArg *>(
												inherited::GetChildAt(i));
			if(arg)
			{
				// 引数が省略されていない
				tRisseSSAVariable * arg_var;

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
						tRisseCompileExceptionClass::Throw(
							tRisseString(
							RISSE_WS_TR("no anonymous collapsed arguments defined in this method")),
								form->GetScriptBlock(), GetPosition());
					}
				}

				// 関数呼び出し文の Used に追加するために配列に追加
				arg_vec.push_back(arg_var);
			}
			else
			{
				// 引数が省略されているので void を追加する
				tRisseSSAVariable * void_var =
					form->AddConstantValueStatement(GetPosition(), tRisseVariant());

				// 関数呼び出し文の Used に追加するために配列に追加
				arg_vec.push_back(void_var);
			}
		}
	}
	else
	{
		exp_flag = RisseFuncCallFlag_Omitted; //  関数呼び出しは ... を伴っている
	}

	// ブロック引数を処理
	tRisseSSAVariableAccessMap * access_map = NULL;
	risse_size break_try_idx = risse_size_max;
	if(with_block)
	{
		// アクセスマップを作成する
		// ブロックを調べ、ブロックの中に一つでもいわゆる「ブロック」
		// (匿名関すではない物) があれば access_map を作成する
		bool block_found = false;
		for(tRisseASTArray::const_iterator i = Blocks.begin(); i != Blocks.end(); i++)
		{
			RISSE_ASSERT((*i)->GetType() == antFuncDecl);
			tRisseASTNode_FuncDecl * block_arg =
				reinterpret_cast<tRisseASTNode_FuncDecl*>(*i);
			if(block_arg->GetIsBlock()) { block_found = true; break; }
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

	if(Blocks.size() > RisseMaxArgCount)
	{
		// 現状、関数のブロック引数に列挙できる数は
		// RisseMaxArgCount 個までとなっている
		// (普通の引数と違って本来ブロック引数の数には制限が無いはずだが
		//  将来的にブロック引数にも普通の引数のように展開フラグなどを
		//  つけるかもしれないので、普通の引数と同じ制限を付ける)
		tRisseCompileExceptionClass::Throw(
			tRisseString(RISSE_WS_TR("too many function block arguments")),
				form->GetScriptBlock(), GetPosition());
	}

	// try識別子を取得
	risse_size try_id = form->GetScriptBlock()->AddTryIdentifier();

	// 各ブロックの内容を生成
	for(tRisseASTArray::const_iterator i = Blocks.begin(); i != Blocks.end(); i++)
	{
		RISSE_ASSERT((*i)->GetType() == antFuncDecl);
		tRisseASTNode_FuncDecl * block_arg =
			reinterpret_cast<tRisseASTNode_FuncDecl*>(*i);

		// ブロックの中身を 遅延評価ブロックとして評価する
		tRisseSSAVariable * lazyblock_var =
			block_arg->GenerateFuncDecl(form, access_map, try_id);

		// 配列にpush
		arg_vec.push_back(lazyblock_var);
	}

	// 遅延評価ブロックで使用された変数の処理
	if(access_map) form->ListVariablesForLazyBlock(GetPosition(), access_map);

	// 関数呼び出しの文を生成する
	// ブロック付きの文の場合は ocTryFuncCall を用いる
	tRisseSSAVariable * returned_var = NULL;
	tRisseSSAStatement * call_stmt =
		form->AddStatement(GetPosition(),
			CreateNew ? ocNew : (access_map?ocTryFuncCall:ocFuncCall),
			&returned_var, func_var);

	call_stmt->SetFuncExpandFlags(exp_flag);
	call_stmt->SetBlockCount(Blocks.size());

	for(gc_vector<tRisseSSAVariable *>::iterator i = arg_vec.begin();
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
		tRisseSSAStatement * catch_branch_stmt =
			form->AddStatement(GetPosition(), ocCatchBranch, NULL, returned_var);
		catch_branch_stmt->SetTryIdentifierIndex(try_id); // try識別子を設定

		// catch用の新しい基本ブロックを作成
		tRisseSSABlock * trycall_catch_block =
			form->CreateNewBlock(RISSE_WS("trycall_catch"));

		// catch した例外はそのまま投げる
		form->AddStatement(GetPosition(), ocThrow, NULL, returned_var);

		// break 用の新しい基本ブロックを作成
		tRisseSSABlock * break_exit_block =
			form->CreateNewBlock(RISSE_WS("break_exit"));

		// break - 例外オブジェクトから値を取り出す
		tRisseSSAVariable * break_ret_var = NULL;
		form->AddStatement(GetPosition(),
			ocGetExitTryValue, &break_ret_var, returned_var);

		// ジャンプ文を作成
		tRisseSSAStatement * break_exit_jump_stmt =
			form->AddStatement(GetPosition(), ocJump, NULL);

		// exit用の新しい基本ブロックを作成
		tRisseSSABlock * trycall_exit_block =
			form->CreateNewBlock(RISSE_WS("trycall_exit"));
		tRisseSSAStatement * trycall_exit_jump_stmt =
			form->AddStatement(GetPosition(), ocJump, NULL);

		// TryFuncCall文 の分岐先を設定
		catch_branch_stmt->SetTryExitTarget(trycall_exit_block);
		catch_branch_stmt->SetTryCatchTarget(trycall_catch_block);
		catch_branch_stmt->AddTarget(break_exit_block);
			// catc_branch_stmt に登録した break_exit_block は
			// tRisseSSAForm::AddCatchBranchTargetsForOne() の処理中では
			// 無視される(ラベル名の先頭が '#' なので)

		// あとで例外の分岐先を設定できるように
		form->AddCatchBranchAndExceptionValue(catch_branch_stmt, returned_var);

		// trycallの終了用の新しい基本ブロックを作成
		tRisseSSABlock * trycall_fin_block =
			form->CreateNewBlock(RISSE_WS("trycall_fin"));
		trycall_exit_jump_stmt->SetJumpTarget(trycall_fin_block);

		// break から来るパスと exit から来るパス用にφ関数を作成する
		break_exit_jump_stmt->SetJumpTarget(trycall_fin_block);
		tRisseSSAVariable * phi_ret_var = NULL;
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
tRisseSSAVariable * tRisseASTNode_FuncDecl::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	return GenerateFuncDecl(form);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_FuncDecl::GenerateFuncDecl(tRisseSSAForm *form,
		tRisseSSAVariableAccessMap *access_map, risse_size try_id) const
{
	// 関数の中身を 遅延評価ブロックとして評価する
	RISSE_ASSERT(!(IsBlock && !access_map));

	tRisseSSAVariable * lazyblock_var = NULL;
	tRisseSSAForm * new_form = NULL;
	tRisseString display_name = Name;
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
		tRisseASTNode_FuncDeclArg * child =
			reinterpret_cast<tRisseASTNode_FuncDeclArg*>(inherited::GetChildAt(i));
		RISSE_ASSERT(child->GetType() == antFuncDeclArg);
		RISSE_ASSERT(child->GetCollapse() == false); // TODO: 配列圧縮

		tRisseSSAVariable * param_var = NULL;

		// パラメータ内容の取得
		tRisseSSAStatement * assignparam_stmt = 
			new_form->AddStatement(GetPosition(), ocAssignParam, &param_var);
		assignparam_stmt->SetIndex(i);

		// デフォルト引数の処理
		// if(param_var === void) { param_var = デフォルト引数の式; }
		// のようになる
		tRisseASTNode * init_node = child->GetInitializer();
		if(init_node)
		{
			// param_var が void と同じかどうかを調べる
			tRisseSSAVariable * void_var =
				new_form->AddConstantValueStatement(GetPosition(), tRisseVariant());
			tRisseSSAVariable * cond_var = NULL;
			new_form->AddStatement(GetPosition(), ocDiscEqual, &cond_var,
				param_var, void_var);

			// 分岐文を作成
			tRisseSSAStatement *branch_stmt =
				new_form->AddStatement(GetPosition(), ocBranch, NULL, cond_var);

			// 新しい基本ブロックを作成(条件式が真の場合)
			tRisseSSABlock * init_block =
				new_form->CreateNewBlock(RISSE_WS("param_init"));

			// 初期化式を作成
			tRisseSSAVariable * init_var = init_node->GenerateReadSSA(new_form);

			// ジャンプ文を作成(初期化式を含むブロックからその次のブロックへ)
			tRisseSSAStatement * init_exit_jump_stmt =
				new_form->AddStatement(GetPosition(), ocJump, NULL);

			// 新しい基本ブロックを作成(変数へ代入)
			tRisseSSABlock * init_exit_block =
				new_form->CreateNewBlock(RISSE_WS("param_subst"));

			// 分岐/ジャンプ文のジャンプ先を設定
			branch_stmt->SetFalseBranch(init_exit_block);
			branch_stmt->SetTrueBranch(init_block);
			init_exit_jump_stmt->SetJumpTarget(init_exit_block);

			// φ関数を作成
			tRisseSSAVariable * phi_ret_var = NULL;
			new_form->AddStatement(GetPosition(), ocPhi, &phi_ret_var,
									param_var, init_var);

			param_var = phi_ret_var;
		}

		// 変数のローカル名前空間への登録
		new_form->GetLocalNamespace()->Add(child->GetName(), NULL);

		// ローカル変数への書き込み
		new_form->GetLocalNamespace()->Write(new_form, GetPosition(),
										child->GetName(), param_var);
	}

	// ブロック引数を処理する
	for(risse_size i = 0; i < Blocks.size(); i++)
	{
		tRisseASTNode_FuncDeclArg * child =
			reinterpret_cast<tRisseASTNode_FuncDeclArg*>(Blocks[i]);
		RISSE_ASSERT(child->GetType() == antFuncDeclBlock);

		tRisseSSAVariable * param_var = NULL;

		// パラメータ内容の取得
		tRisseSSAStatement * assignparam_stmt = 
			new_form->AddStatement(GetPosition(), ocAssignBlockParam, &param_var);
		assignparam_stmt->SetIndex(i);

		// 変数のローカル名前空間への登録
		new_form->GetLocalNamespace()->Add(child->GetName(), NULL);

		// ローカル変数への書き込み
		new_form->GetLocalNamespace()->Write(new_form, GetPosition(),
										child->GetName(), param_var);
	}

	// ブロックの内容を生成する
	new_form->Generate(Body);

	// 遅延評価ブロックをクリーンアップ
	form->CleanupLazyBlock(lazy_param);

	// 関数インスタンスをFunctionクラスでラップするための命令を置く
	tRisseSSAVariable * wrapped_lazyblock_var = NULL;
	form->AddStatement(GetPosition(), ocAssignNewFunction, &wrapped_lazyblock_var, lazyblock_var);

	// static 指定がついていれば this にバインドするための命令を置く
	// TODO: MemberAttribute は static 以外用をなしていない。本当に必要なのか？
	tRisseSSAVariable * final_var = wrapped_lazyblock_var;
	if(Attribute.Has(tRisseDeclAttribute::ccStatic))
	{
		tRisseSSAVariable * this_var = NULL;
		form->AddStatement(GetPosition(), ocAssignThis, &this_var);

		form->AddStatement(GetPosition(),
			ocInContextOf, &final_var, wrapped_lazyblock_var, this_var);
	}

	// このノードはラップされた方の関数(メソッド)を返す
	return final_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_PropDecl::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	return GeneratePropertyDecl(form);
	// TODO: プロパティがローカル名前空間に作成されてもかなり意味がないのでどうするか
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_PropDecl::GeneratePropertyDecl(tRisseSSAForm *form) const
{
	tRisseString display_name = Name;

	// ゲッタノードを処理
	tRisseSSAVariable * getter_var = NULL;
	if(Getter)
	{
		tRisseSSAVariable * lazyblock_var = NULL;
		tRisseSSAForm * new_form = NULL;
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
			form->AddConstantValueStatement(GetPosition(), tRisseVariant::GetNullObject());
	}

	// セッタノードを処理
	tRisseSSAVariable * setter_var = NULL;
	if(Setter)
	{
		tRisseSSAVariable * lazyblock_var = NULL;
		tRisseSSAForm * new_form = NULL;
		void * lazy_param = form->CreateLazyBlock(
								GetPosition(),
								Name.IsEmpty() ?
									RISSE_WS("anonymous property setter"):
									RISSE_WS("property ") + display_name + RISSE_WS(" setter"),
								true, NULL, new_form, lazyblock_var);


		// パラメータ内容の取得
		tRisseSSAVariable * param_var = NULL;
		tRisseSSAStatement * assignparam_stmt = 
			new_form->AddStatement(GetPosition(), ocAssignParam, &param_var);
		assignparam_stmt->SetIndex(0);

		// 変数のローカル名前空間への登録
		new_form->GetLocalNamespace()->Add(SetterArgumentName, NULL);

		// ローカル変数への書き込み
		new_form->GetLocalNamespace()->Write(new_form, GetPosition(),
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
			form->AddConstantValueStatement(GetPosition(), tRisseVariant::GetNullObject());
	}

	// プロパティオブジェクトを作成する
	tRisseSSAVariable * property_instance_var = NULL;
	form->AddStatement(GetPosition(),
		ocAssignNewProperty, &property_instance_var,
		getter_var, setter_var);


	// static 指定がついていれば this にバインドするための命令を置く
	// TODO: MemberAttribute は static 以外用をなしていない。本当に必要なのか？
	tRisseSSAVariable * final_var = property_instance_var;
	if(Attribute.Has(tRisseDeclAttribute::ccStatic))
	{
		tRisseSSAVariable * this_var = NULL;
		form->AddStatement(GetPosition(), ocAssignThis, &this_var);

		form->AddStatement(GetPosition(),
			ocInContextOf, &final_var, property_instance_var, this_var);
	}

	// プロパティオブジェクトを返す
	return final_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_ClassDecl::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	return GenerateClassDecl(form);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_ClassDecl::GenerateClassDecl(tRisseSSAForm *form) const
{
	// Body は antContext かつ actTopLevel でなくてはならない
	RISSE_ASSERT(Body->GetType() == antContext);
	RISSE_ASSERT(reinterpret_cast<tRisseASTNode_Context*>(Body)->GetContextType() == actTopLevel);

	// モジュールはスーパークラスを持つことはできない
	RISSE_ASSERT(!(IsModule && SuperClass));


	// クラス/モジュール名を決める
	tRisseString display_name = Name;
	if(display_name.IsEmpty()) display_name = RISSE_WS("nondetermined");
	tRisseString class_name = Name.IsEmpty() ? tRisseString(RISSE_WS("anonymous")) : display_name;

	// クラス/モジュール名を定数として作る
	tRisseSSAVariable * class_name_var =
		form->AddConstantValueStatement(GetPosition(), tRisseVariant(class_name));


	// クラスの場合とモジュールの場合で違う処理
	tRisseSSAVariable * class_instance_var = NULL;
	tRisseSSAVariable * super_class_var = NULL;
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
			tRisseASTNode_Factor * global = new tRisseASTNode_Factor(GetPosition(), aftGlobal);
			tRisseASTNode_Factor * Object =
				new tRisseASTNode_Factor(GetPosition(), aftConstant,
					tRisseVariant(tRisseString(RISSE_WS("Object"))));
			tRisseASTNode_MemberSel * global_Object =
				new tRisseASTNode_MemberSel(GetPosition(), global, Object, matDirect);

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
	tRisseCompiler * compiler = form->GetFunction()->GetFunctionGroup()->GetCompiler();
	tRisseSSAVariable * classblock_var = NULL;
	tRisseSSAForm * new_form = NULL;
	gc_vector<tRisseASTNode *> roots;

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
	tRisseSSAVariable * funccall_result = NULL;
	tRisseSSAStatement * call_stmt;

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
tRisseASTNode * tRisseASTNode_ClassDecl::GenerateDefaultInitializeAST(risse_size pos)
{
	// function initialize() { super::initialize(...); }
	// を表すASTノードを生成して返す

	// super::initialize() の部分を作成
	tRisseASTNode_Factor * super = new tRisseASTNode_Factor(pos, aftSuper);
	tRisseASTNode_Factor * initialize =
		new tRisseASTNode_Factor(pos, aftConstant,
			tRisseVariant(tRisseString(RISSE_WS("initialize"))));
	tRisseASTNode_MemberSel * super_initialize =
		new tRisseASTNode_MemberSel(pos, super, initialize, matDirectThis);

	tRisseASTNode_FuncCall * funccall = new tRisseASTNode_FuncCall(pos, true);
	funccall->SetExpression(super_initialize);

	tRisseASTNode_ExprStmt * funccall_expr = new tRisseASTNode_ExprStmt(pos, funccall);

	// function 定義のブロックを作成
	tRisseASTNode_Context * body = new tRisseASTNode_Context(pos, actBlock, RISSE_WS("Block"));
	body->AddChild(funccall_expr);

	// function 定義を作成
	tRisseASTNode_FuncDecl * funcdecl = new tRisseASTNode_FuncDecl(pos);
	funcdecl->SetBody(body);
	funcdecl->SetName(RISSE_WS("initialize"));
	tRisseASTNode_VarDecl * funcvardecl = new tRisseASTNode_VarDecl(pos);
	tRisseASTNode_VarDeclPair * funcvardeclpair = new tRisseASTNode_VarDeclPair(
			pos, new tRisseASTNode_Id(pos, RISSE_WS("initialize"), false), funcdecl);
	funcvardecl->AddChild(funcvardeclpair);

	// var 定義を返す
	return funcvardecl;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseASTNode * tRisseASTNode_ClassDecl::GenerateDefaultConstructAST(risse_size pos)
{
	// function construct() { }
	// を表すASTノードを生成して返す

	// function 定義のブロックを作成
	tRisseASTNode_Context * body = new tRisseASTNode_Context(pos, actBlock, RISSE_WS("Block"));

	// function 定義を作成
	tRisseASTNode_FuncDecl * funcdecl = new tRisseASTNode_FuncDecl(pos);
	funcdecl->SetBody(body);
	funcdecl->SetName(RISSE_WS("construct"));
	tRisseASTNode_VarDecl * funcvardecl = new tRisseASTNode_VarDecl(pos);
	tRisseASTNode_VarDeclPair * funcvardeclpair = new tRisseASTNode_VarDeclPair(
			pos, new tRisseASTNode_Id(pos, RISSE_WS("construct"), false), funcdecl);
	funcvardecl->AddChild(funcvardeclpair);
	funcdecl->SetBody(body);

	// function 定義を返す
	return funcvardecl;
}
//---------------------------------------------------------------------------

} // namespace Risse

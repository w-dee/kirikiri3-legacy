//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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
#include "../risseException.h"

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
tRisseString tRisseASTNode_FuncCall::GetChildNameAt(risse_size index) const
{
	if(index == 0) return RISSE_WS("function");
	index --;
	if(index < inherited::GetChildCount())
	{
		risse_char buf[40];
		return tRisseString(RISSE_WS("argument")) + Risse_int64_to_str(index, buf);
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
	case 0: return RISSE_WS("initializer");
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
	return IsDirect ?
		RISSE_WS("direct"):
		RISSE_WS("indirect");
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
tRisseString tRisseASTNode_FuncDecl::GetChildNameAt(risse_size index) const
{
	if(index == inherited::GetChildCount()) return RISSE_WS("body");
	if(index < inherited::GetChildCount())
	{
		risse_char buf[40];
		return tRisseString(RISSE_WS("argument")) + Risse_int64_to_str(index, buf);
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_FuncDecl::GetDumpComment() const
{
	tRisseString attrib = Attribute.AsString();
	if(!attrib.IsEmpty()) attrib += RISSE_WC(' ');
	if(Name.IsEmpty()) return attrib + RISSE_WS("anonymous");
	return attrib + Name.AsHumanReadable();
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
	return attrib + Name.AsHumanReadable();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_ClassDecl::GetChildNameAt(risse_size index) const
{
	if(index == inherited::GetChildCount()) return RISSE_WS("body");
	if(index < inherited::GetChildCount())
	{
		risse_char buf[40];
		return tRisseString(RISSE_WS("super")) + Risse_int64_to_str(index, buf);
	}
	return tRisseString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseASTNode_ClassDecl::GetDumpComment() const
{
	tRisseString attrib = Attribute.AsString();
	if(!attrib.IsEmpty()) attrib += RISSE_WC(' ');
	if(Name.IsEmpty()) return attrib + RISSE_WS("anonymous");
	return attrib + Name.AsHumanReadable();
}
//---------------------------------------------------------------------------














/*
	ここから SSA 形式の生成に関わる部分
*/














//---------------------------------------------------------------------------
risse_size tRisseASTNode::SearchEndPosition() const
{
	if(Type == antContext)
		return ((const tRisseASTNode_Context*)this)->GetEndPosition();

	risse_size pos = Position;
	risse_size child_count = GetChildCount();
	for(risse_size i = 0; i < child_count; i++)
	{
		risse_size child_end_pos = GetChildAt(i)->SearchEndPosition();
		if(pos < child_end_pos) pos = child_end_pos;
	}

	return pos;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_Context::DoReadSSA(
			tRisseSSAForm *form, void * param) const
{
	// ContextType がトップレベルの場合は、SSA 生成においては、おおかたこのノードが
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

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_FuncCall::DoReadSSA(
							tRisseSSAForm *form, void * param) const
{
	// 関数を表す式を得る
	tRisseSSAVariable * func_var = Expression->GenerateReadSSA(form);

	// 引数の列挙用配列
	gc_vector<tRisseSSAVariable *> arg_vec;
	arg_vec.reserve(inherited::GetChildCount());

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
				// 現状、関数の引数に列挙できる数は30個までとなっている
				// ので、それを超えるとエラーになる
				eRisseCompileError::Throw(
					tRisseString(RISSE_WS_TR("Too many function arguments")),
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
						eRisseCompileError::Throw(
							tRisseString(
							RISSE_WS_TR("No anonymous collapsed arguments defined in this method")),
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

	// 関数呼び出しの文を生成する
	tRisseSSAVariable * returned_var = NULL;
	tRisseSSAStatement * call_stmt =
		form->AddStatement(GetPosition(), CreateNew ? ocNew : ocFuncCall,
					&returned_var, func_var);

	call_stmt->SetFuncArgOmitted(Omit);
	call_stmt->SetFuncExpandFlags(exp_flag);

	for(gc_vector<tRisseSSAVariable *>::iterator i = arg_vec.begin();
		i != arg_vec.end(); i++)
	{
		call_stmt->AddUsed(*i);
	}

	// 関数の戻り値を返す
	return returned_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_ExprStmt::DoReadSSA(
			tRisseSSAForm *form, void * param) const
{
	// このノードは式を保持しているだけなので子ノードに処理をさせるだけ
	// ただし、node は NULL の可能性がある(空文の場合)
	tRisseASTNode * node = GetChildAt(0);
	if(node) node->GenerateReadSSA(form);

	// このノードは答えを返さない
	return NULL;
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

	case aftSuper:		// "super"
			// 文を作成して戻る
			return form->AddVariableWithStatement(GetPosition(), ocAssignSuper);

	case aftGlobal:		// "global"
			// 文を作成して戻る
			tRisseSSAVariable * ret_var =
				form->AddVariableWithStatement(GetPosition(), ocAssignGlobal);
			ret_var->SetValueType(tRisseVariant::vtObject); // 結果は常に vtObject
			return ret_var;
	}
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_VarDecl::DoReadSSA(
			tRisseSSAForm *form, void * param) const
{
	// 変数宣言
	// 子に再帰する
	for(risse_size i = 0; i < GetChildCount(); i++)
		GetChildAt(i)->GenerateReadSSA(form);
	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_VarDeclPair::DoReadSSA(
			tRisseSSAForm *form, void * param) const
{
	tRisseSSAVariable * init_var;
	if(Initializer)
	{
		// 初期化値がある
		init_var = Initializer->GenerateReadSSA(form);
	}
	else
	{
		// void の定数値の作成
		init_var = form->AddConstantValueStatement(GetPosition(), tRisseVariant());
	}

	// 変数のローカル名前空間への登録
	form->GetLocalNamespace()->Add(Name, NULL);

	// ローカル変数への書き込み
	form->GetLocalNamespace()->Write(form, GetPosition(), Name, init_var);

	// このノードは答えを返さない
	return NULL;
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
	form->AddStatement(GetPosition(), IsDirect?ocDGet:ocIGet, &ret_var,
							pws->ObjectVar, pws->MemberNameVar);

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
	form->AddStatement(GetPosition(), IsDirect?ocDSet:ocISet, NULL,
							pws->ObjectVar, pws->MemberNameVar, value);

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tRisseASTNode_Id::PrepareSSA(
			tRisseSSAForm *form, tPrepareMode mode) const
{
	tPrepareSSA * pws = new tPrepareSSA;

	bool need_access_this; // this 上のメンバにアクセスする必要があるかどうか
	need_access_this = !form->GetLocalNamespace()->IsAvailable(Name);

	if(need_access_this)
	{
		// this 上のメンバにアクセスする必要がある
		pws->MemberSel = CreateAccessNodeOnThis();
		pws->MemberSelParam = pws->MemberSel->PrepareSSA(form, mode);
	}
	else
	{
		// ローカル変数に見つかった
		pws->MemberSel = NULL;
		pws->MemberSelParam = NULL;
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
		// ローカル変数に見つからない場合は、this へのアクセスを行う物として
		// 置き換えて処理を行う

		// this 上のメンバをアクセスするためだけに新規に作成した AST ノードに処理をさせる
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
const tRisseASTNode_MemberSel * tRisseASTNode_Id::CreateAccessNodeOnThis() const
{
	return
		new tRisseASTNode_MemberSel(GetPosition(),
		new tRisseASTNode_Factor(GetPosition(), aftThis),
		new tRisseASTNode_Factor(GetPosition(), aftConstant, Name), true);
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
				eRisseCompileError::Throw(
					tRisseString(
					RISSE_WS_TR("Writable expression required against increment/decrement oprator")),
						form->GetScriptBlock(), GetPosition());
			}

			// 演算結果を返す
			return is_prepositioned ? processed_var : child_var;
		}

	case autDelete:		// "delete"
		;
	}
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
				eRisseCompileError::Throw(
					tRisseString(RISSE_WS_TR("Writable expression required at left side of '='")),
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
				eRisseCompileError::Throw(
					tRisseString(
	RISSE_WS_TR("Writable expression required at left side of compound assignment operator")),
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
				eRisseCompileError::Throw(
					tRisseString(
					RISSE_WS_TR("Writable expression required at left side of '<->'")),
						form->GetScriptBlock(), GetPosition());
			}

			// 左辺の値を右辺に代入
			if(!Child2->DoWriteSSA(form, rhs_param, lhs_var))
			{
				eRisseCompileError::Throw(
					tRisseString(
					RISSE_WS_TR("Writable expression required at right side of '<->'")),
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
	case abtRBitShift:			// >>>
	case abtLShift:				// <<
	case abtRShift:				// >>
	case abtMod:				// %
	case abtDiv:				// /
	case abtIdiv:				// \ (integer div)
	case abtMul:				// *
	case abtAdd:				// +
	case abtSub:				// -
	case abtIncontextOf:		// incontextof
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
			case abtRBitShift:			code = ocRBitShift;			break; // >>>
			case abtLShift:				code = ocLShift;			break; // <<
			case abtRShift:				code = ocRShift;			break; // >>
			case abtMod:				code = ocMod;				break; // %
			case abtDiv:				code = ocDiv;				break; // /
			case abtIdiv:				code = ocIdiv;				break; // \ (integer div)
			case abtMul:				code = ocMul;				break; // *
			case abtAdd:				code = ocAdd;				break; // +
			case abtSub:				code = ocSub;				break; // -
			case abtIncontextOf:		code = ocIncontextOf;		break; // incontextof
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
			// 書き込みモードかつchildが NULL の場合はインデックスを準備する必要はない
			data->Indices.push_back(NULL);
		}
		else
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
				eRisseCompileError::Throw(
					tRisseString(
					RISSE_WS_TR("Writable expression required at array index %1"), i_str),
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
			eRisseCompileError::Throw(
				tRisseString(
				RISSE_WS_TR("Writable expression required at value of dictionary element index %1"), i_str),
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
		false_last_block = form->GetCurrentBlock();

		// ジャンプ文を作成
		false_exit_jump_stmt =
			form->AddStatement(pos, ocJump, NULL);
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
	{
		// false_var は false ノードが無い場合は NULL になるので、
		// そのまま渡す (その場合は true_var だけがφ関数の引数になる)
		form->AddStatement(pos, ocPhi, &ret_var, true_var, false_var);
	}

	return ret_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_If::DoReadSSA(
			tRisseSSAForm *form, void * param) const
{
	return InternalDoReadSSA(form, GetPosition(), RISSE_WS("if"), Condition,
		True, False, false);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_While::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	// while ループ または do ～ while ループ

	// break に関する情報を生成
	tRisseBreakInfo * break_info = new tRisseBreakInfo();

	// break に関する情報を form に設定
	tRisseBreakInfo * old_break_info = form->SetCurrentBreakInfo(break_info);

	// continue に関する情報を生成
	tRisseContinueInfo * continue_info = new tRisseContinueInfo();

	// continue に関する情報を form に設定
	tRisseContinueInfo * old_continue_info = form->SetCurrentContinueInfo(continue_info);

	// 条件式または body にジャンプするための文を作成
	form->GetCurrentBlock();
	tRisseSSAStatement * entry_jump_stmt =
		form->AddStatement(GetPosition(), ocJump, NULL);

	// 条件式を格納する基本ブロックを作成
	tRisseSSABlock * while_cond_block =
		form->CreateNewBlock(RISSE_WS("while_cond"));

	// 条件式の結果を得る
	tRisseSSAVariable * cond_var = Condition->GenerateReadSSA(form);

	// 分岐文を作成
	form->GetCurrentBlock();
	tRisseSSAStatement * branch_stmt =
		form->AddStatement(GetPosition(), ocBranch, NULL, cond_var);

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

	// 分岐/ジャンプ文のジャンプ先を設定
	entry_jump_stmt->SetJumpTarget(SkipFirstCheck?while_body_block:while_cond_block);
	while_body_jump_stmt->SetJumpTarget(while_cond_block);
	branch_stmt->SetTrueBranch(while_body_block);
	branch_stmt->SetFalseBranch(while_exit_block);

	// break の処理
	break_info->BindAll(while_exit_block);

	// continue の処理
	continue_info->BindAll(while_cond_block);

	// break に関する情報を元に戻す
	form->SetCurrentBreakInfo(old_break_info);

	// continue に関する情報を元に戻す
	form->SetCurrentContinueInfo(old_continue_info);

	// このノードは答えを返さない
	return NULL;
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
	tRisseBreakInfo * break_info = new tRisseBreakInfo();

	// break に関する情報を form に設定
	tRisseBreakInfo * old_break_info = form->SetCurrentBreakInfo(break_info);

	// continue に関する情報を生成
	tRisseContinueInfo * continue_info = new tRisseContinueInfo();

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

	// 分岐/ジャンプ文のジャンプ先を設定
	entry_jump_stmt->SetJumpTarget(for_cond_block);
	for_body_jump_stmt->SetJumpTarget(for_iter_block ? for_iter_block : for_cond_block);
	if(for_iter_jump_stmt) for_iter_jump_stmt->SetJumpTarget(for_cond_block);
	branch_stmt->SetTrueBranch(for_body_block);
	branch_stmt->SetFalseBranch(for_exit_block);

	// break の処理
	break_info->BindAll(for_exit_block);

	// continue の処理
	continue_info->BindAll(for_iter_block ? for_iter_block : for_cond_block);

	// break に関する情報を元に戻す
	form->SetCurrentBreakInfo(old_break_info);

	// continue に関する情報を元に戻す
	form->SetCurrentContinueInfo(old_continue_info);

	// スコープを pop
	form->GetLocalNamespace()->Pop(); // スコープを pop

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_Return::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	tRisseSSAVariable * var;
	if(Expression)
	{
		// 戻りとなる値を作成する
		var = Expression->GenerateReadSSA(form);
	}
	else
	{
		// 戻りとなる値は void
		var = form->AddConstantValueStatement(GetPosition(), tRisseVariant());
	}

	// return 文を作成
	form->AddStatement(GetPosition(), ocReturn, NULL, var);

	// 新しい基本ブロックを作成(ただしここには到達しない)
	form->CreateNewBlock("disconnected_by_return");

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
	// break 文をサポートする構文の中？
	tRisseBreakInfo * info = form->GetCurrentBreakInfo();
	if(info == NULL)
		eRisseCompileError::Throw(
			tRisseString(RISSE_WS_TR("cannot place 'break' here")),
				form->GetScriptBlock(), GetPosition());

	// ジャンプ文を作成
	tRisseSSAStatement *jump_stmt = form->AddStatement(GetPosition(), ocJump, NULL);

	// 新しい基本ブロックを作成
	form->CreateNewBlock("disconnected_by_break");

	// ジャンプ文を info に登録
	info->AddJump(jump_stmt);

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_Continue::DoReadSSA(
									tRisseSSAForm *form, void * param) const
{
	// continue 文をサポートする構文の中？
	tRisseContinueInfo * info = form->GetCurrentContinueInfo();
	if(info == NULL)
		eRisseCompileError::Throw(
			tRisseString(RISSE_WS_TR("cannot place 'continue' here")),
				form->GetScriptBlock(), GetPosition());

	// ジャンプ文を作成
	tRisseSSAStatement *jump_stmt = form->AddStatement(GetPosition(), ocJump, NULL);

	// 新しい基本ブロックを作成
	form->CreateNewBlock("disconnected_by_continue");

	// ジャンプ文を info に登録
	info->AddJump(jump_stmt);

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
	form->GetLabelMap()->AddMap(Name, label_block, GetPosition());

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_Goto::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	// 現在の基本ブロックを取得
	tRisseSSABlock * cur_block = form->GetCurrentBlock();

	// 新しい基本ブロックを作成 (この基本ブロックには到達しない)
	form->CreateNewBlock(RISSE_WS("disconnected_by_goto"));

	// form に登録
	form->GetLabelMap()->AddPendingLabelJump(cur_block, GetPosition(), Name);

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
	tRisseBreakInfo * break_info = new tRisseBreakInfo();

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
	break_info->BindAll(last_block);

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
		eRisseCompileError::Throw(
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
			eRisseCompileError::Throw(
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
	// try ブロックの中身を 遅延評価ブロックとして評価する
	tRisseSSAVariable * lazyblock_var = NULL;
	tRisseSSAForm * new_form = NULL;
	void * lazy_param = form->CreateLazyBlock(GetPosition(),
		RISSE_WS("try block"), false, new_form, lazyblock_var);

	new_form->Generate(Body);

	// 遅延評価ブロックを実行するためのfunccall文を作成
	lazyblock_var->GenerateFuncCall(GetPosition(), tRisseString::GetEmptyString());

	// 遅延評価ブロックをクリーンアップ
	form->CleanupLazyBlock(lazy_param);

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_FuncDecl::DoReadSSA(tRisseSSAForm *form, void * param) const
{
	// 関数の中身を 遅延評価ブロックとして評価する
	tRisseSSAVariable * lazyblock_var = NULL;
	tRisseSSAForm * new_form = NULL;
	void * lazy_param = form->CreateLazyBlock(GetPosition(),
												Name.IsEmpty() ?
													RISSE_WS("anonymous function"):
													RISSE_WS("function ") + Name,
												true, new_form, lazyblock_var);

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

	// ブロックの内容を生成する
	new_form->Generate(Body);

	// 遅延評価ブロックをクリーンアップ
	form->CleanupLazyBlock(lazy_param);

	// このノードは作成された関数(メソッド)を返す
	return lazyblock_var;
}
//---------------------------------------------------------------------------


} // namespace Risse
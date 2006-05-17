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
#include "prec.h"
#include "risseAST.h"
#include "risseCodeGen.h"

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
tRisseString tRisseASTNode_OneExpression::GetChildNameAt(risse_size index) const
{
	if(index == 0) return RISSE_WS("expression"); else return tRisseString();
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
tRisseString tRisseASTNode_RegExp::GetDumpComment() const
{
	return tRisseString(RISSE_WS("pattern=")) + Pattern.AsHumanReadable() +
		RISSE_WS(", flags=") + Flags.AsHumanReadable();
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
tRisseString tRisseASTNode_Trinary::GetDumpComment() const
{
	return RisseASTTrinaryTypeNames[TrinaryType];
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
tRisseSSAVariable * tRisseASTNode_Context::DoReadSSA(
			tRisseScriptBlockBase * sb, tRisseSSAForm *form, void * param) const
{
	// ContextType がトップレベルの場合は、SSA 生成においては、おおかたこのノードが
	// 一番最初に呼ばれることになる。

	// ローカル変数スコープの生成  - コンテキストの種類に従って分岐
	switch(ContextType)
	{
	case actTopLevel: // トップレベル
		break; // 何もしない
	case actBlock: // ブロック
		form->GetLocalNamespace()->Push(); // スコープを push
		break;
	}

	// すべての子ノードに再帰する
	for(risse_size i = 0; i < GetChildCount(); i++)
		GetChildAt(i)->GenerateSSA(sb, form);

	// ローカル変数スコープの消滅  - コンテキストの種類に従って分岐
	switch(ContextType)
	{
	case actTopLevel: // トップレベル
		break; // 何もしない
	case actBlock: // ブロック
		form->GetLocalNamespace()->Pop(); // スコープを pop
		break;
	}

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_ExprStmt::DoReadSSA(
			tRisseScriptBlockBase * sb, tRisseSSAForm *form, void * param) const
{
	// このノードは式を保持しているだけなので子ノードに処理をさせるだけ
	GetChildAt(0)->GenerateSSA(sb, form);

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_Factor::DoReadSSA(
			tRisseScriptBlockBase * sb, tRisseSSAForm *form, void * param) const
{
	// "項"
	switch(FactorType)
	{
	case aftConstant:	// 定数
			return form->GetCurrentBlock()->AddConstantValueStatement(GetPosition(), Value);

	case aftThis:		// "this"
		{
			// 文の作成
			tRisseSSAStatement * stmt =
				new tRisseSSAStatement(form, GetPosition(), ocAssignThis);

			// 戻りの変数の作成
			tRisseSSAVariable * ret_var = new tRisseSSAVariable(form, stmt);

			// 文のSSAブロックへの追加
			form->GetCurrentBlock()->AddStatement(stmt);

			// 戻る
			return ret_var;
		}
	case aftSuper:		// "super"
		{
			// 文の作成
			tRisseSSAStatement * stmt =
				new tRisseSSAStatement(form, GetPosition(), ocAssignSuper);

			// 戻りの変数の作成
			tRisseSSAVariable * ret_var = new tRisseSSAVariable(form, stmt);

			// 文のSSAブロックへの追加
			form->GetCurrentBlock()->AddStatement(stmt);

			// 戻る
			return ret_var;
		}
	case aftGlobal:		// "global"
		{
			// 文の作成
			tRisseSSAStatement * stmt =
				new tRisseSSAStatement(form, GetPosition(), ocAssignGlobal);

			// 戻りの変数の作成
			tRisseSSAVariable * ret_var = new tRisseSSAVariable(form, stmt);
			ret_var->SetValueType(tRisseVariant::vtObject); // global オブジェクトは常に vtObject

			// 文のSSAブロックへの追加
			form->GetCurrentBlock()->AddStatement(stmt);

			// 戻る
			return ret_var;
		}
	}
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tRisseASTNode_Id::PrepareSSA(
					tRisseScriptBlockBase * sb, tRisseSSAForm *form) const
{
	tPrepareSSA * pws = new tPrepareSSA;

	tRisseSSAVariable * dest_var =
		form->GetLocalNamespace()->MakePhiFunction(form, GetPosition(), Name);
	if(!dest_var)
	{
		// ローカル変数に見つからない
		pws->IsLocal = false;
		pws->MemberSel = CreateAccessNodeOnThis();
		pws->MemberSelParam = pws->MemberSel->PrepareSSA(sb, form);
	}
	else
	{
		// ローカル変数に見つかった
		pws->IsLocal = true;
		pws->Var = dest_var;
	}

	return pws;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_Id::DoReadSSA(
			tRisseScriptBlockBase * sb, tRisseSSAForm *form, void * param) const
{
	tPrepareSSA * pws = reinterpret_cast<tPrepareSSA *>(param);

	// 識別子
	if(!pws->IsLocal)
	{
		// ローカル変数に見つからない
		// ローカル変数に見つからない場合は、this へのアクセスを行う物として
		// 置き換えて処理を行う

		// this 上のメンバをアクセスするためだけに新規に作成した AST ノードに処理をさせる
		return pws->MemberSel->DoReadSSA(sb, form, pws->MemberSelParam);
	}
	else
	{
		// ローカル変数に見つかった
		return pws->Var;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseASTNode_Id::DoWriteSSA(
		tRisseScriptBlockBase * sb, tRisseSSAForm *form, void * param,
		tRisseSSAVariable * value) const
{
	tPrepareSSA * pws = reinterpret_cast<tPrepareSSA *>(param);

	if(!pws->IsLocal)
	{
		// ローカル変数には見つからなかった
		// tRisseASTNode_MemberSel::DoWriteSSA を呼ぶ
		pws->MemberSel->DoWriteSSA(sb, form, pws->MemberSelParam, value);
	}
	else
	{
		// ローカル変数に見つかった;ローカル変数に上書きする
		// 文の作成
		tRisseSSAStatement * stmt =
			new tRisseSSAStatement(form, GetPosition(), ocAssign);
		stmt->AddUsed(value); // この文の使用リストに変数を加える

		// 変数の作成
		tRisseSSAVariable * var = new tRisseSSAVariable(form, stmt, Name);

		// 文のSSAブロックへの追加
		form->GetCurrentBlock()->AddStatement(stmt);

		// 変数のローカル名前空間への登録(上書き)
		form->GetLocalNamespace()->Add(Name, var);
	}
	return true;
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
const tRisseASTNode * tRisseASTNode_Id::GetAccessNode(tRisseSSAForm * form) const
{
	if(form->GetLocalNamespace()->MakePhiFunction(form, GetPosition(), Name))
		return this; // ローカル名前空間に存在する
	const tRisseASTNode * node = CreateAccessNodeOnThis();
	return node; // 存在しないので this 上へのアクセスを行う
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_Unary::DoReadSSA(
			tRisseScriptBlockBase * sb, tRisseSSAForm *form, void * param) const
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
			tRisseSSAVariable * child_var = Child->GenerateSSA(sb, form);
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
			tRisseSSAStatement * stmt =
				new tRisseSSAStatement(form, GetPosition(), oc);
			stmt->AddUsed(child_var); // この文の使用リストに変数を加える

			// 戻りの変数の作成
			tRisseSSAVariable * ret_var = new tRisseSSAVariable(form, stmt);
		//	ret_var->SetValueType(tRisseVariant::vtBoolean); // 結果は常に vtBoolean

			//////////////////////////////////////////////////////////////////////////////////////////
			// 演算子のオーバーロードによっては ! 演算子は boolean を返さない可能性がある
			// この仕様は後に変更の可能性アリ (! 演算子をオーバーロードできないようにする可能性がある)
			// 他の ~ や + などの演算子についてもそうなる可能性がある
			//////////////////////////////////////////////////////////////////////////////////////////

			// 文のSSAブロックへの追加
			form->GetCurrentBlock()->AddStatement(stmt);

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

			// Id の存在をチェック
			const tRisseASTNode * child = Child;
			if(child->GetType() == antId)
				child = reinterpret_cast<const tRisseASTNode_Id *>(child)->GetAccessNode(form);

			// 子のタイプをチェック
			if(child->GetType() == antId)
			{
				// 識別子
				// 子の計算結果を得る
				tRisseSSAVariable * child_var = child->GenerateSSA(sb, form);

				// インクリメント、デクリメント演算子は、整数値 1 を加算あるいは
				// 減算するものとして扱われる
				// (実際にそれを inc や dec の VM 命令にするのは CG や optimizer の仕事)
				// 定数 1 を生成
				tRisseSSAVariable * one_var =
					form->GetCurrentBlock()->AddConstantValueStatement(
							GetPosition(), tRisseVariant((risse_int64)1));

				// 定数 1 を加算/減算する文を生成
				tRisseSSAStatement * stmt =
					new tRisseSSAStatement(form, GetPosition(), code);
				stmt->AddUsed(child_var); // この文の使用リストに変数を加える
				stmt->AddUsed(one_var); // この文の使用リストに変数を加える

				// 戻りの変数の作成
				tRisseSSAVariable * processed_var = new tRisseSSAVariable(form, stmt);

				// 文のSSAブロックへの追加
				form->GetCurrentBlock()->AddStatement(stmt);

				// その結果を識別子に書き込む文を生成
				reinterpret_cast<const tRisseASTNode_Id*>(child)->
							GenerateWriteSSA(sb, form, processed_var);

				// 戻る
				// 前置の場合は計算後の値を、後置の場合は計算前の値を返す
				return is_prepositioned ? processed_var : child_var;
			}
			else if(child->GetType() == antMemberSel)
			{
				// メンバ選択演算子
				// メンバ選択演算子を介しての操作は、ocIncAssign あるいは
				// ocDecAssign のオペレーションを呼び出すものとして実装する
				tRisseSSAVariable * child_var = child->GenerateSSA(sb, form);

				// inc あるいは dec のコードを生成
				tRisseSSAStatement * stmt =
					new tRisseSSAStatement(form, GetPosition(),
								code == ocSub ? ocDecAssign : ocIncAssign);
				stmt->AddUsed(child_var); // この文の使用リストに変数を加える

				// 戻りの変数の作成
				tRisseSSAVariable * processed_var = new tRisseSSAVariable(form, stmt);

				// 文のSSAブロックへの追加
				form->GetCurrentBlock()->AddStatement(stmt);

				// 戻る
				// 前置の場合は計算後の値を、後置の場合は計算前の値を返す
				return is_prepositioned ? processed_var : child_var;
			}
			else
			{
				// エラー
			}
		}

	case autDelete:		// "delete"
		;
	}
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_Binary::DoReadSSA(
			tRisseScriptBlockBase * sb, tRisseSSAForm *form, void * param) const
{
	// 演算子のタイプに従って分岐
	switch(BinaryType)
	{
	case abtAssign:				// =
		{
			// 単純代入
			// 右辺の値を得る
			tRisseSSAVariable * rhs_var = Child2->GenerateSSA(sb, form);

			// 左辺に書き込む
			Child1->GenerateWriteSSA(sb, form, rhs_var);

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

			// 右辺の値を得る
			tRisseSSAVariable * rhs_var = Child2->GenerateSSA(sb, form);

			// Id の存在をチェック
			const tRisseASTNode * child = Child1;
			if(child->GetType() == antId)
				child = reinterpret_cast<const tRisseASTNode_Id *>(child)->GetAccessNode(form);

			// 左辺のタイプをチェック
			if(child->GetType() == antId)
			{
				// 識別子

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

				// 識別子の場合は、たとえば a &= b ならば a.1 = a.0 & b のような
				// SSA 形式に展開を行う。

				// 識別子の内容を得る
				tRisseSSAVariable * lhs_var = child->GenerateSSA(sb, form);

				// 演算を行う文を生成
				tRisseSSAStatement * stmt =
					new tRisseSSAStatement(form, GetPosition(), code);
				stmt->AddUsed(lhs_var); // この文の使用リストに変数を加える
				stmt->AddUsed(rhs_var); // この文の使用リストに変数を加える

				// 戻りの変数の作成
				tRisseSSAVariable * ret_var = new tRisseSSAVariable(form, stmt);

				// 文のSSAブロックへの追加
				form->GetCurrentBlock()->AddStatement(stmt);

				// その結果を識別子に書き込む文を生成
				reinterpret_cast<const tRisseASTNode_Id*>(child)->
							GenerateWriteSSA(sb, form, ret_var);

				// 戻る
				return ret_var;
			}
			else if(child->GetType() == antMemberSel)
			{
				// メンバ選択演算子
				// メンバ選択演算子を介しての操作は、ocXXXXAssign のオペーレーションを
				// 呼び出すものとして実装する

				// オペレーションコードを得る
				tRisseOpCode code; // オペレーションコード
				switch(BinaryType)
				{
				case abtBitAndAssign:		code = ocBitAndAssign;		break; // &=
				case abtBitOrAssign:		code = ocBitOrAssign;		break; // |=
				case abtBitXorAssign:		code = ocBitXorAssign;		break; // ^=
				case abtSubAssign:			code = ocSubAssign;			break; // -=
				case abtAddAssign:			code = ocAddAssign;			break; // +=
				case abtModAssign:			code = ocModAssign;			break; // %=
				case abtDivAssign:			code = ocDivAssign;			break; // /=
				case abtIdivAssign:			code = ocIdivAssign;		break; // \=
				case abtMulAssign:			code = ocMulAssign;			break; // *=
				case abtLogOrAssign:		code = ocLogOrAssign;		break; // ||=
				case abtLogAndAssign:		code = ocLogAndAssign;		break; // &&=
				case abtRBitShiftAssign:	code = ocRBitShiftAssign;	break; // >>>=
				case abtLShiftAssign:		code = ocLShiftAssign;		break; // <<=
				case abtRShiftAssign:		code = ocRShiftAssign;		break; // >>=
				default:
					// ここには来ないがこれを書いておかないと
					// コンパイラが文句をたれるので
					code = ocNoOperation;
					break;
				}

				// 左辺の値を得る
				tRisseSSAVariable * lhs_var = child->GenerateSSA(sb, form);

				// inc あるいは dec のコードを生成
				tRisseSSAStatement * stmt =
					new tRisseSSAStatement(form, GetPosition(), code);
				stmt->AddUsed(lhs_var); // この文の使用リストに変数を加える
				stmt->AddUsed(rhs_var); // この文の使用リストに変数を加える

				// 戻りの変数の作成
				tRisseSSAVariable * ret_var = new tRisseSSAVariable(form, stmt);

				// 文のSSAブロックへの追加
				form->GetCurrentBlock()->AddStatement(stmt);

				// 戻る
				// 前置の場合は計算後の値を、後置の場合は計算前の値を返す
				return ret_var;
			}
			else
			{
				// エラー
			}
		}

	case abtSwap:				// <->
		{
			// 交換演算子

			// これはちょっと厄介。
			// a.b <-> c.d と記述された場合、a と c 、 b と d
			// は１回のみの評価としたいので、先に評価を行わなければならない。

			// 評価の準備を行う
			void * lhs_param = Child1->PrepareSSA(sb, form);
			void * rhs_param = Child2->PrepareSSA(sb, form);

			// 左辺の値を取得
			tRisseSSAVariable * lhs_var = Child1->DoReadSSA(sb, form, lhs_param);

			// 右辺の値を取得
			tRisseSSAVariable * rhs_var = Child2->DoReadSSA(sb, form, rhs_param);

			// 右辺の値を左辺に代入
			Child1->DoWriteSSA(sb, form, lhs_param, rhs_var);

			// 左辺の値を右辺に代入
			Child2->DoWriteSSA(sb, form, rhs_param, lhs_var);

			// このノードは答えを返さない
			return NULL;
		}

	case abtComma:				// ,
		Child1->GenerateSSA(sb, form); // 左辺値は捨てる
		return Child2->GenerateSSA(sb, form); // 右辺値を返す

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
			tRisseSSAVariable * lhs_var = Child1->GenerateSSA(sb, form);
			// 右辺の値を得る
			tRisseSSAVariable * rhs_var = Child2->GenerateSSA(sb, form);

			// 演算を行う文を生成
			tRisseSSAStatement * stmt =
				new tRisseSSAStatement(form, GetPosition(), code);
			stmt->AddUsed(lhs_var); // この文の使用リストに変数を加える
			stmt->AddUsed(rhs_var); // この文の使用リストに変数を加える

			// 戻りの変数の作成
			tRisseSSAVariable * ret_var = new tRisseSSAVariable(form, stmt);

			// 文のSSAブロックへの追加
			form->GetCurrentBlock()->AddStatement(stmt);

			// 戻る
			return ret_var;
		}

	default:
		return NULL;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tRisseASTNode_MemberSel::PrepareSSA(tRisseScriptBlockBase * sb, tRisseSSAForm *form) const
{
	tPrepareSSA * pws = new tPrepareSSA;
	// オブジェクトの式の値を得る
	pws->ObjectVar = Object->GenerateSSA(sb, form);
	// メンバ名の式の値を得る
	pws->MemberNameVar = MemberName->GenerateSSA(sb, form);

	return pws;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_MemberSel::DoReadSSA(
			tRisseScriptBlockBase * sb, tRisseSSAForm *form, void * param) const
{
	// メンバ選択演算子
	tPrepareSSA * pws = reinterpret_cast<tPrepareSSA *>(param);

	// 文の作成
	tRisseSSAStatement * stmt =
		new tRisseSSAStatement(form, GetPosition(), IsDirect?ocDGet:ocIGet);
	stmt->AddUsed(pws->ObjectVar); // 使用リストに追加
	stmt->AddUsed(pws->MemberNameVar); // 使用リストに追加

	// 戻りの変数の作成
	tRisseSSAVariable * ret_var = new tRisseSSAVariable(form, stmt);

	// 文のSSAブロックへの追加
	form->GetCurrentBlock()->AddStatement(stmt);

	// 戻りの変数を返す
	return ret_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseASTNode_MemberSel::DoWriteSSA(
		tRisseScriptBlockBase * sb, tRisseSSAForm *form, void * param,
		tRisseSSAVariable * value) const
{
	tPrepareSSA * pws = reinterpret_cast<tPrepareSSA *>(param);

	// 文の作成
	tRisseSSAStatement * stmt =
		new tRisseSSAStatement(form, GetPosition(), IsDirect?ocDSet:ocISet);
	stmt->AddUsed(pws->ObjectVar); // 使用リストに追加
	stmt->AddUsed(pws->MemberNameVar); // 使用リストに追加
	stmt->AddUsed(value); // 使用リストに追加

	// 文のSSAブロックへの追加
	form->GetCurrentBlock()->AddStatement(stmt);

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_If::DoReadSSA(
			tRisseScriptBlockBase * sb, tRisseSSAForm *form, void * param) const
{
	// 条件式の結果を得る
	tRisseSSAVariable * cond_var = Condition->GenerateSSA(sb, form);

	// 分岐文を作成
	tRisseSSAStatement * branch_stmt =
		new tRisseSSAStatement(form, GetPosition(), ocBranch);
	branch_stmt->AddUsed(cond_var); // 使用リストに追加
	form->GetCurrentBlock()->AddStatement(branch_stmt); // ブロックに追加

	// 新しいブロックを作成(真の場合)
	tRisseSSABlock * block_before_if_stmt = form->GetCurrentBlock();
	tRisseSSABlock * true_block = form->CreateNewBlock(RISSE_WS("if_true"));

	// 真の場合に実行する内容を作成
	True->GenerateSSA(sb, form);

	// ジャンプ文を作成
	tRisseSSAStatement * true_exit_jump_stmt =
		new tRisseSSAStatement(form, GetPosition(), ocJump);
	form->GetCurrentBlock()->AddStatement(true_exit_jump_stmt); // ブロックに追加

	// 新しいブロックを作成
	tRisseSSABlock * block_after_if_stmt =
		form->CreateNewBlock(RISSE_WS("if_exit"), block_before_if_stmt);

	block_after_if_stmt->AddPred(true_block);

	// 分岐/ジャンプ文のジャンプ先を設定
	branch_stmt->SetTrueBranch(true_block);
	branch_stmt->SetFalseBranch(block_after_if_stmt);
	true_exit_jump_stmt->SetJumpTarget(block_after_if_stmt);

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_VarDecl::DoReadSSA(
			tRisseScriptBlockBase * sb, tRisseSSAForm *form, void * param) const
{
	// 変数宣言
	// 子に再帰する
	for(risse_size i = 0; i < GetChildCount(); i++)
		GetChildAt(i)->GenerateSSA(sb, form);
	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseASTNode_VarDeclPair::DoReadSSA(
			tRisseScriptBlockBase * sb, tRisseSSAForm *form, void * param) const
{
	tRisseSSAVariable * init_var;
	if(Initializer)
	{
		// 初期化値がある
		init_var = Initializer->GenerateSSA(sb, form);
	}
	else
	{
		// 定数値の作成
		init_var = form->GetCurrentBlock()->AddConstantValueStatement(
					GetPosition(), tRisseVariant());
	}

	// 文の作成
	tRisseSSAStatement * stmt =
		new tRisseSSAStatement(form, GetPosition(), ocAssign);
	stmt->AddUsed(init_var); // この文の使用リストに変数を加える

	// 変数の作成
	tRisseSSAVariable * var = new tRisseSSAVariable(form, stmt, Name);

	// 文のSSAブロックへの追加
	form->GetCurrentBlock()->AddStatement(stmt);

	// 変数のローカル名前空間への登録
	form->GetLocalNamespace()->Add(Name, var);

	// このノードは答えを返さない
	return NULL;
}
//---------------------------------------------------------------------------

} // namespace Risse

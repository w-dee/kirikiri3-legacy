%{
/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
/*---------------------------------------------------------------------------*/
/*! @brief   Risse bison 入力ファイル (文法定義) */
/* risse.y */
/* Risse bison input file */

#include <stdio.h>
#include <string.h>
#include "../risseTypes.h"
#include "../risseGC.h"
#include "../risseException.h"
#include "risseParser.h"

/* エラーの詳細出力 */
#define YYERROR_VERBOSE 1

/* yyparse に渡すパラメータ */
#define YYPARSE_PARAM	pr

/* yylex に渡すパラメータ */
#define YYLEX_PARAM pr

/* メモリ確保は Risse のインターフェースを使うように */
#define YYMALLOC	RisseMallocCollectee
#define YYFREE		RisseFreeCollectee

/*! パーサへのアクセス */
#define PR (reinterpret_cast<tRisseParser*>(pr))

/*! 字句解析器の現在の解析位置 */
#define LP (PR->GetCurrentLexerPosition())

/* tRisseASTNode_XXXX の省略形 */
#ifdef N
 #undef N
#endif
#define N(XXXX) tRisseASTNode_##XXXX

/* tRisseASTNode_XXXX へのキャスト */
#ifdef C
 #undef C
#endif
#define C(XXXX, EXP) (reinterpret_cast<tRisseASTNode_##XXXX*>(EXP))

/* 名前空間を Risse に */
namespace Risse
{
RISSE_DEFINE_SOURCE_ID(26374,32704,8215,19346,5601,19578,20566,1441);

/* yylex のプロトタイプ */
int yylex(YYSTYPE * value, void *pr);

/* yyerror のプロトタイプ */
int raise_yyerror(char * msg, void *pr);

/* yyerror のリダイレクト */
#define risseerror(X) raise_yyerror(X, pr);

%}

/*###########################################################################*/

/* 再入可能なパーサを出力 */
%pure_parser

/* シフト・還元競合は２つある */
%expect 2

/* union 定義 */
%union{
	tRisseVariant * value;
	tRisseASTNode * np;
}

/* トークン定義 */
%token
	T_NONE
	T_DEFINE				"=>"
	T_COMMA					","
	T_EQUAL					"="
	T_AMPERSANDEQUAL		"&="
	T_VERTLINEEQUAL			"|="
	T_CHEVRONEQUAL			"^="
	T_MINUSEQUAL			"-="
	T_PLUSEQUAL				"+="
	T_PERCENTEQUAL			"%="
	T_SLASHEQUAL			"/="
	T_BACKSLASHEQUAL		"\\="
	T_ASTERISKEQUAL			"*="
	T_LOGICALOREQUAL		"||="
	T_LOGICALANDEQUAL		"&&="
	T_RARITHSHIFTEQUAL		">>>="
	T_LARITHSHIFTEQUAL		"<<="
	T_RBITSHIFTEQUAL		">>="
	T_QUESTION				"?"
	T_LOGICALOR				"||"
	T_LOGICALAND			"&&"
	T_VERTLINE				"|"
	T_CHEVRON				"^"
	T_AMPERSAND				"&"
	T_NOTEQUAL				"!="
	T_EQUALEQUAL			"=="
	T_DISCNOTEQUAL			"!=="
	T_DISCEQUAL				"==="
	T_SWAP					"<->"
	T_LT					"<"
	T_GT					">"
	T_LTOREQUAL				"<="
	T_GTOREQUAL				">="
	T_RARITHSHIFT			">>"
	T_LARITHSHIFT			"<<"
	T_RBITSHIFT				">>>"
	T_PERCENT				"%"
	T_SLASH					"/"
	T_BACKSLASH				"\\"
	T_ASTERISK				"*"
	T_EXCRAMATION			"!"
	T_TILDE					"~"
	T_DECREMENT				"--"
	T_INCREMENT				"++"
	T_NEW					"new"
	T_DELETE				"delete"
	T_TYPEOF				"typeof"
	T_PLUS					"+"
	T_MINUS					"-"
	T_SHARP					"#"
	T_DOLLAR				"$"
	T_ISVALID				"isvalid"
	T_INVALIDATE			"invalidate"
	T_INSTANCEOF			"instanceof"
	T_LPARENTHESIS			"("
	T_DOT					"."
	T_LBRACKET				"["
	T_THIS					"this"
	T_SUPER					"super"
	T_GLOBAL				"global"
	T_RBRACKET				"]"
	T_CLASS					"class"
	T_RPARENTHESIS			")"
	T_COLON					":"
	T_SEMICOLON				";"
	T_LBRACE				"{"
	T_RBRACE				"}"
	T_CONTINUE				"continue"
	T_FUNCTION				"function"
	T_DEBUGGER				"debugger"
	T_DEFAULT				"default"
	T_CASE					"case"
	T_EXTENDS				"extends"
	T_FINALLY				"finally"
	T_PROPERTY				"property"
	T_PRIVATE				"private"
	T_PUBLIC				"public"
	T_PROTECTED				"protected"
	T_FINAL					"final"
	T_STATIC				"static"
	T_RETURN				"return"
	T_BREAK					"break"
	T_EXPORT				"export"
	T_IMPORT				"import"
	T_SWITCH				"switch"
	T_IN					"in"
	T_INCONTEXTOF			"incontextof"
	T_FOR					"for"
	T_WHILE					"while"
	T_DO					"do"
	T_IF					"if"
	T_VAR					"var"
	T_CONST					"const"
	T_ENUM					"enum"
	T_GOTO					"goto"
	T_THROW					"throw"
	T_TRY					"try"
	T_SETTER				"setter"
	T_GETTER				"getter"
	T_ELSE					"else"
	T_CATCH					"catch"
	T_OMIT					"..."
	T_SYNCHRONIZED			"synchronized"
	T_WITH					"with"
	T_INT					"int"
	T_REAL					"real"
	T_STRING				"string"
	T_OCTET					"octet"

	T_FALSE					"false"
	T_NULL					"null"
	T_TRUE					"true"
	T_VOID					"void"
	T_NAN					"NaN"
	T_INFINITY				"Infinity"

	T_BEGIN_NUMBER
	T_BEGIN_STRING_LITERAL
	T_BEGIN_MACRO
	T_BEGIN_EMSTRING_LITERAL
	T_BEGIN_OCTET_LITERAL
	T_BEGIN_COMMENT
	T_EMSTRING_AMPERSAND_D
	T_EMSTRING_AMPERSAND_S
	T_EMSTRING_DOLLAR_D
	T_EMSTRING_DOLLAR_S



%token <value>		T_CONSTVAL
%token <value>		T_SYMBOL
%token <value>		T_REGEXP


%type <np>
	expr expr_with_comma factor_expr call_arg call_arg_list
	func_expr_def func_call_expr inline_array array_elm_list
	inline_dic dic_elm dic_elm_list
	if if_else
	block block_or_statement statement
	while do_while for variable_def variable_def_inner func_def property_def
	class_def return switch with case try throw


/* 演算子の優先順位 */
%right		<np>	"if"
%left		<np>	","
%right		<np>	"=" "&=" "|=" "^=" "-=" "+=" "%=" "/=" "\\=" "*="
					"||=" "&&=" ">>=" "<<=" ">>>="
%nonassoc	<np>	"<->"
%right		<np>	"?" ":"
%left		<np>	"||"
%left		<np>	"&&"
%left		<np>	"|"
%left		<np>	"^"
%left		<np>	"&"
%left		<np>	"!=" "==" "!==" "==="
%left		<np>	"<" ">" "<=" ">="
%left		<np>	">>" "<<" ">>>"
%left		<np>	"+" "-"
%left		<np>	"%" "/" "\\" "*"
%right		<np>	"!" "~" "--" "++" "new" "delete" "typeof" "#" "$" T_UNARY
%nonassoc	<np>	"incontextof" T_POSTUNARY
%left		<np>	"(" ")" "[" "]" "."


%%
/*###########################################################################*/


/* the program */
program
	: global_list
;

/* global definitions */
global_list
	:										{ PR->PushContext(actTopLevel, RISSE_WS("top level")); }
	  def_list								{ PR->PopContext(); }
;

/* definition list */
def_list
	:
	| def_list block_or_statement			{ if($2) PR->AddNode($2); }
	| def_list error ";"					{ if(yynerrs > 20)
												YYABORT;
											  else yyerrok; }
;

/* a block or a single statement */
block_or_statement
	: statement
	| block
;

/* a statement */
statement
	: ";"									{ $$ = NULL; }
	| expr_with_comma ";"					{ $$ = new N(ExprStmt)(LP, $1); }
	| if
	| if_else
	| while
	| do_while
	| for
	| "break" ";"							{ /*cc->DoBreak();*/ }
	| "continue" ";"						{ /*cc->DoContinue();*/ }
	| "debugger" ";"						{ /*cc->DoDebugger();*/ }
	| variable_def
	| func_def
	| property_def
	| class_def
	| return
	| switch
	| with
	| case
	| try
	| throw
;

/* a block */
block
	: "{"									{ /*cc->EnterBlock();*/ }
	  def_list
	  "}"									{ /*cc->ExitBlock();*/ }
;

/* a while loop */
while
	: "while"								{ /*cc->EnterWhileCode(false);*/ }
	  "(" expr_with_comma ")"							{ /*cc->CreateWhileExprCode($4, false);*/ }
	  block_or_statement					{ /*cc->ExitWhileCode(false);*/ }
;

/* a do-while loop */
do_while
	: "do"									{ /*cc->EnterWhileCode(true); */ }
	  block_or_statement
	  "while"
	  "(" expr_with_comma ")"							{ /*cc->CreateWhileExprCode($6, true); */ }
	  ";"									{ /*cc->ExitWhileCode(true); */ }
;

/* an if statement */
if
	: "if" "("
	  expr_with_comma
	  ")" block_or_statement				{ $$ = new N(If)(LP, $3, $5); }
;

/* an if-else statement */
/* この規則は上記の if とシフト・還元競合を起こす */
if_else
	: if "else"
	  block_or_statement					{ $$ = $1; C(If, $$)->SetFalse($3); }
;

/* a for loop */
for
	: "for" "("
	  for_first_clause ";"
	  for_second_clause ";"
	  for_third_clause ")"
	  block_or_statement					{ /*cc->ExitForCode(); */ }
;


/* the first clause of a for statement */
for_first_clause
	: /* empty */							{ /*cc->EnterForCode(false); */ }
	|										{ /*cc->EnterForCode(true); */ }
	  variable_def_inner
	| expr_with_comma									{ /*cc->EnterForCode(false);
											  cc->CreateExprCode($1); */ }
;

/* the second clause of a for statement */
for_second_clause
	: /* empty */							{ /*cc->CreateForExprCode(NULL); */ }
	| expr_with_comma									{ /*cc->CreateForExprCode($1); */ }
;

/* the third clause of a for statement */
for_third_clause
	: /* empty */							{ /*cc->SetForThirdExprCode(NULL); */ }
	| expr_with_comma									{ /*cc->SetForThirdExprCode($1); */ }
;

/* variable definition */
variable_def
	: variable_def_inner ";"
;

variable_def_inner
	: "var" variable_id_list				{ ; }
	| "const" variable_id_list				{ ; }
		/* const: note that current version does not
		   actually disallow re-assigning new value */
;

/* list for the variable definition */
variable_id_list
	: variable_id
	| variable_id_list "," variable_id
;

/* a variable id and an optional initializer expression */
variable_id
	: T_SYMBOL								{ /*cc->AddLocalVariable(
												lx->GetString($1)); */ }
	| T_SYMBOL "=" expr						{ /*cc->InitLocalVariable(
											  lx->GetString($1), $3); */ }
;

/* a function definition */
func_def
	: "function" T_SYMBOL					{ /*sb->PushContextStack(
												lx->GetString($2),
											  ctFunction);
											  cc->EnterBlock();*/ }
	  func_decl_arg_opt
	  block									{ /*cc->ExitBlock(); sb->PopContextStack(); */ }
;

/* a function expression definition */
func_expr_def
	: "function"							{ /*sb->PushContextStack(
												RISSE_WS("(anonymous)"),
											  ctExprFunction);
											  cc->EnterBlock(); */ }
	  func_decl_arg_opt
	  block									{ /*cc->ExitBlock();
											  tRisseVariant v(cc);
											  sb->PopContextStack();
											  $$ = cc->MakeNP0(T_CONSTVAL);
											  $$->SetValue(v); */ }
;

/* the argument definition of a function definition */
func_decl_arg_opt
	: /* empty */
	| "(" func_decl_arg_collapse ")"
	| "(" func_decl_arg_list ")"
	| "(" func_decl_arg_at_least_one "," func_decl_arg_collapse ")"
;

/* the argument list */
func_decl_arg_list
	: /* empty */
	| func_decl_arg_at_least_one
;

func_decl_arg_at_least_one
	: func_decl_arg
	| func_decl_arg_at_least_one "," func_decl_arg
;

func_decl_arg
	: T_SYMBOL								{ /*cc->AddFunctionDeclArg(
												lx->GetString($1), NULL); */ }
	| T_SYMBOL "=" expr						{ /*cc->AddFunctionDeclArg(
												lx->GetString($1), $3); */ }
;

func_decl_arg_collapse
	: "*"									{ /*cc->AddFunctionDeclArgCollapse(
												NULL); */ }
	| T_SYMBOL "*"							{ /*cc->AddFunctionDeclArgCollapse(
												lx->GetString($1)); */ }
/*
	These are currently not supported
	| T_SYMBOL "*" "=" inline_array			{ ; }
	| T_SYMBOL "*=" inline_array			{ ; }
*/
;

/* a property handler definition */
property_def
	: "property" T_SYMBOL
	  "{"									{ /*sb->PushContextStack(
												lx->GetString($2),
												ctProperty);*/ }
	  property_handler_def_list
	  "}"									{ /*sb->PopContextStack();*/ }
;

property_handler_def_list
	: property_handler_setter
	| property_handler_getter
	| property_handler_setter property_handler_getter
	| property_handler_getter property_handler_setter
;

property_handler_setter
	: "setter" "(" T_SYMBOL ")"				{ /*sb->PushContextStack(
												RISSE_WS("(setter)"),
												ctPropertySetter);
											  cc->EnterBlock();
											  cc->SetPropertyDeclArg(
												lx->GetString($3));*/ }
	  block									{ /*cc->ExitBlock();
											  sb->PopContextStack();*/ }
;

property_handler_getter
	: property_getter_handler_head			{ /*sb->PushContextStack(
												RISSE_WS("(getter)"),
												ctPropertyGetter);
											  cc->EnterBlock();*/ }
	  block									{ /*cc->ExitBlock();
											  sb->PopContextStack();*/ }
;

property_getter_handler_head
	: "getter" "(" ")"
	| "getter"
;


/* a class definition */
class_def
	: "class" T_SYMBOL						{ /*sb->PushContextStack(
												lx->GetString($2),
												ctClass);*/ }
	  class_extender
	  block									{ /*sb->PopContextStack();*/ }
;

class_extender
	:
	| "extends" expr						{ /*cc->CreateExtendsExprCode($2, true);*/ }
	| "extends" expr ","					{ /*cc->CreateExtendsExprCode($2, false);*/ }
	  extends_list
;

extends_list
	: extends_name
	| extends_list "," extends_name
;

extends_name
	: expr									{ /*cc->CreateExtendsExprCode($1, false);*/ }
;

/* a return statement */
return
	: "return" ";"							{ /*cc->ReturnFromFunc(NULL);*/ }
	| "return" expr_with_comma ";"						{ /*cc->ReturnFromFunc($2);*/ }
;


/* a switch statement */
switch
	: "switch" "("
	  expr_with_comma ")"								{ /*cc->EnterSwitchCode($3);*/ }
	  block									{ /*cc->ExitSwitchCode();*/ }
;

/* a with statement */
with
	: "with" "("
	  expr_with_comma ")"								{ /*cc->EnterWithCode($3);*/ }
	  block_or_statement					{ /*cc->ExitWithCode();*/ }
;

/* case: */
case
	: "case" expr ":"						{ /*cc->ProcessCaseCode($2);*/ }
	| "default" ":"							{ /*cc->ProcessCaseCode(NULL);*/ }
;

/* a structured exception handling */
try
	: "try"									{ /*cc->EnterTryCode();*/ }
	  block_or_statement
	  catch
	  block_or_statement					{ /*cc->ExitTryCode();*/ }
;

catch
	: "catch" "(" ")"						{ /*cc->EnterCatchCode(NULL);*/ }
	| "catch" "(" T_SYMBOL ")"				{ /*cc->EnterCatchCode(
												lx->GetString($3));*/ }
;

/* a throw statement */
throw
	: "throw" expr_with_comma ";"						{ /*cc->ProcessThrowCode($2);*/ }
;

/* 式 */
/* カンマとそれ以下の優先順位の式を含む場合はこちらを使う */
expr_with_comma
	: expr_with_comma "if" expr_with_comma	{ $$ = new N(Binary)(LP, abtIf			,$3, $1);/*順番に注意*/ }
	| expr_with_comma ","  expr_with_comma	{ $$ = new N(Binary)(LP, abtComma		,$1, $3); }
	| expr
;

/*
	expr_no_comma は、関数への引数リストなど、式中にカンマがあると、リストの
	区切り記号と区別が付かない場合はこちらを使う
*/
expr
	: expr "=" expr					{ $$ = new N(Binary)(LP, abtAssign			,$1, $3); }
	| expr "&=" expr				{ $$ = new N(Binary)(LP, abtBitAndAssign	,$1, $3); }
	| expr "|=" expr				{ $$ = new N(Binary)(LP, abtBitOrAssign		,$1, $3); }
	| expr "^=" expr				{ $$ = new N(Binary)(LP, abtBitXorAssign	,$1, $3); }
	| expr "-=" expr				{ $$ = new N(Binary)(LP, abtSubAssign		,$1, $3); }
	| expr "+=" expr				{ $$ = new N(Binary)(LP, abtAddAssign		,$1, $3); }
	| expr "%=" expr				{ $$ = new N(Binary)(LP, abtModAssign		,$1, $3); }
	| expr "/=" expr				{ $$ = new N(Binary)(LP, abtDivAssign		,$1, $3); }
	| expr "\\=" expr				{ $$ = new N(Binary)(LP, abtIdivAssign		,$1, $3); }
	| expr "*=" expr				{ $$ = new N(Binary)(LP, abtMulAssign		,$1, $3); }
	| expr "||=" expr				{ $$ = new N(Binary)(LP, abtLogOrAssign		,$1, $3); }
	| expr "&&=" expr				{ $$ = new N(Binary)(LP, abtLogAndAssign	,$1, $3); }
	| expr ">>=" expr				{ $$ = new N(Binary)(LP, abtRShiftAssign	,$1, $3); }
	| expr "<<=" expr				{ $$ = new N(Binary)(LP, abtLShiftAssign	,$1, $3); }
	| expr ">>>=" expr				{ $$ = new N(Binary)(LP, abtRBitShiftAssign	,$1, $3); }
	| expr "<->" expr				{ $$ = new N(Binary)(LP, abtSwap			,$1, $3); }
	| expr "?" expr ":" expr		{ $$ = new N(Trinary)(LP, attCondition 		,$1, $3, $5); }
	| expr "||" expr				{ $$ = new N(Binary)(LP, abtLogOr			,$1, $3); }
	| expr "&&" expr				{ $$ = new N(Binary)(LP, abtLogAnd			,$1, $3); }
	| expr "|" expr					{ $$ = new N(Binary)(LP, abtBitOr			,$1, $3); }
	| expr "^" expr					{ $$ = new N(Binary)(LP, abtBitXor			,$1, $3); }
	| expr "&" expr					{ $$ = new N(Binary)(LP, abtBitAnd			,$1, $3); }
	| expr "!=" expr				{ $$ = new N(Binary)(LP, abtNotEqual		,$1, $3); }
	| expr "==" expr				{ $$ = new N(Binary)(LP, abtEqual			,$1, $3); }
	| expr "!==" expr				{ $$ = new N(Binary)(LP, abtDiscNotEqual	,$1, $3); }
	| expr "===" expr				{ $$ = new N(Binary)(LP, abtDiscEqual		,$1, $3); }
	| expr "<" expr					{ $$ = new N(Binary)(LP, abtLesser			,$1, $3); }
	| expr ">" expr					{ $$ = new N(Binary)(LP, abtGreater			,$1, $3); }
	| expr "<=" expr				{ $$ = new N(Binary)(LP, abtLesserOrEqual	,$1, $3); }
	| expr ">=" expr				{ $$ = new N(Binary)(LP, abtGreaterOrEqual	,$1, $3); }
	| expr ">>" expr				{ $$ = new N(Binary)(LP, abtLShift			,$1, $3); }
	| expr "<<" expr				{ $$ = new N(Binary)(LP, abtRShift			,$1, $3); }
	| expr ">>>" expr				{ $$ = new N(Binary)(LP, abtRBitShift		,$1, $3); }
	| expr "+" expr					{ $$ = new N(Binary)(LP, abtAdd				,$1, $3); }
	| expr "-" expr					{ $$ = new N(Binary)(LP, abtSub				,$1, $3); }
	| expr "%" expr					{ $$ = new N(Binary)(LP, abtMod				,$1, $3); }
	| expr "/" expr					{ $$ = new N(Binary)(LP, abtDiv				,$1, $3); }
	| expr "\\" expr				{ $$ = new N(Binary)(LP, abtIdiv			,$1, $3); }
	| expr "*" expr					{ $$ = new N(Binary)(LP, abtMul				,$1, $3); }
	| "!" expr						{ $$ = new N(Unary)(LP, autLogNot			,$2); }
	| "~" expr						{ $$ = new N(Unary)(LP, autBitNot			,$2); }
	| "--" expr						{ $$ = new N(Unary)(LP, autPreDec			,$2); }
	| "++" expr						{ $$ = new N(Unary)(LP, autPreInc			,$2); }
	| "new" expr					{ $$ = new N(Unary)(LP, autNew				,$2); }
	| "delete" expr					{ $$ = new N(Unary)(LP, autDelete			,$2); }
	| "typeof" expr					{ ; }
	| "+" expr %prec T_UNARY		{ $$ = new N(Unary)(LP, autPlus				,$2); }
	| "-" expr %prec T_UNARY		{ $$ = new N(Unary)(LP, autMinus			,$2); }
	| expr "incontextof" expr		{ ; }
	| expr "--" %prec T_POSTUNARY	{ $$ = new N(Unary)(LP, autPostDec			,$1); }
	| expr "++" %prec T_POSTUNARY	{ $$ = new N(Unary)(LP, autPostInc			,$1); }
	| func_call_expr				{ ; }
	| "(" expr_with_comma ")"		{ $$ = $2; }
	| expr "[" expr "]"				{ $$ = new N(Binary)(LP, abtIndirectSel		,$1, $3); }
	| expr "." expr					{ $$ = new N(Binary)(LP, abtDirectSel		,$1, $3); }
	| factor_expr
;



factor_expr
	: T_CONSTVAL					{ $$ = new N(Factor)(LP, aftConstant, *$1); }
	| T_SYMBOL						{ $$ = new N(Factor)(LP, aftSymbol, *$1); }
	| "this"						{ $$ = new N(Factor)(LP, aftThis);  }
	| "super"						{ $$ = new N(Factor)(LP, aftSuper);  }
	| func_expr_def					{ /*$$ = $1;*/ }
	| "global"						{ $$ = new N(Factor)(LP, aftGlobal); }
	| inline_array
	| inline_dic
	| "/="							{ /*lx->SetStartOfRegExp();*/ }
	  T_REGEXP						{ /*$$ = cc->MakeNP0(T_REGEXP);
									  $$->SetValue(lx->GetValue($3));*/ }
	| "/"							{ /*lx->SetStartOfRegExp();*/ }
	  T_REGEXP						{ /*$$ = cc->MakeNP0(T_REGEXP);
									  $$->SetValue(lx->GetValue($3));*/ }
;


/* an expression for function call */
func_call_expr
	: expr "(" call_arg_list ")"		{ $$ = $3; C(FuncCall, $$)->SetExpression($1); }
	| expr "(" "..." ")"				{ $$ = new N(FuncCall)(LP, true);  C(FuncCall, $$)->SetExpression($1); }
	| expr "(" ")"						{ $$ = new N(FuncCall)(LP, false); C(FuncCall, $$)->SetExpression($1); }
		/* このルールは "(" と ")" の間で下記の 「call_arg が empty」 のルールと
		  シフト・還元競合を起こす(こちらが優先される) */
;

/* argument(s) for function call */
call_arg_list
	: call_arg							{ $$ = new N(FuncCall)(LP, false); C(FuncCall, $$)->AddChild($1); }
	| call_arg_list "," call_arg		{ C(FuncCall, $1)->AddChild($3); }
;

call_arg
	: /* empty */						{ $$ = NULL; }
	| "*"								{ $$ = new N(FuncArg)(LP, NULL, true); }
	| expr "*" 							{ $$ = new N(FuncArg)(LP, $1, true); }
	| expr								{ $$ = new N(FuncArg)(LP, $1, false); }
;


/* an inline array object */
inline_array
	: "["
	  array_elm_list
	  dummy_elm_opt
	  "]"								{ $$ = $2; }
;

/* an inline array's element list */
array_elm_list
	: /* empty */						{ $$ = new N(Array)(LP); }
	| expr								{ $$ = new N(Array)(LP); C(Array, $$)->AddChild($1); }
	| array_elm_list "," expr			{ $$ = $1; C(Array, $$)->AddChild($3); }
;

/* an inline dictionary */
inline_dic
	: "%" "["
	  dic_elm_list
	  dummy_elm_opt
	  "]"								{ $$ = $3; }
;


/* an inline dictionary's element list */
dic_elm_list
    : /* empty */						{ $$ = new N(Dict)(LP); }
	| dic_elm							{ $$ = new N(Dict)(LP); C(Dict, $$)->AddChild($1); }
	| dic_elm_list "," dic_elm			{ $$ = $1; C(Dict, $$)->AddChild($3); }
;

/* an inline dictionary's element */
dic_elm
	: expr "=>" expr					{ $$ = new N(DictPair)(LP, $1, $3); }
	| T_SYMBOL ":" expr					{ $$ = new N(DictPair)(LP,
										  new N(Factor)(LP, aftConstant, *$1), $3); }
;

/* a dummy element at the tail of inline dictionary elements */
dummy_elm_opt
	: /* empty */
	| ","
;

/*###########################################################################*/

%%

//---------------------------------------------------------------------------
int yylex(YYSTYPE * value, void *pr)
{
	value->value = new tRisseVariant();
	return PR->GetToken(*(value->value));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int raise_yyerror(char * msg, void *pr)
{
	eRisseError::Throw(msg);
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseParser::tRisseParser(tRisseLexer * lexer)
{
	Root = NULL;
	CurrentContext = NULL;
	Lexer = lexer;

	yyparse(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseParser::PushContext(tRisseASTContextType ctype, const tRisseString & name)
{
	if(!Root)
	{
		// 最初のコンテキスト
		RISSE_ASSERT(CurrentContext == NULL && ctype == actTopLevel);
	}

	tRisseASTNode_Context * newcontext =
		new tRisseASTNode_Context(GetCurrentLexerPosition(), ctype, name);

	if(!Root)
	{
		// 最初のコンテキスト
		CurrentContext = newcontext;
		Root = CurrentContext;
	}
	else
	{
		// 入れ子のコンテキスト
		CurrentContext->AddChild(newcontext);
		CurrentContext = newcontext;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseParser::PopContext()
{
	CurrentContext = reinterpret_cast<tRisseASTNode_Context*>(CurrentContext->GetParent());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseParser::AddNode(tRisseASTNode * node)
{
	RISSE_ASSERT(CurrentContext != NULL);

	CurrentContext->AddChild(node);
}
//---------------------------------------------------------------------------




} // namespace Risse





%{
/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
/*---------------------------------------------------------------------------*/
/*! @brief   Risse bison 入力ファイル (文法定義) */
/* risse.y */
/* Risse bison input file */

#include "../prec.h"

#include <stdio.h>
#include <string.h>
#include "../risseTypes.h"
#include "../risseGC.h"
#include "../risseObject.h"
#include "../risseExceptionClass.h"
#include "../risseOpCodes.h"
#include "../risseStaticStrings.h"
#include "risseParser.h"
#include "risseScriptBlock.h"

/* メモリ確保は Risse のインターフェースを使うように */
#define YYMALLOC	RisseMallocCollectee
#define YYREALLOC	RisseReallocCollectee
#define YYFREE		RisseFreeCollectee

/* 最大深さ */
#define YYMAXDEPTH 20000

/*! パーサへのアクセス */
#define PR (reinterpret_cast<tRisseParser*>(pr))

/*! 字句解析器 */
#define LX (PR->GetLexer())

/*! 字句解析器の現在の解析位置 */
#define LP (LX->GetLastTokenStart())

/* new tRisseASTNode_XXXX の省略形 */
#ifdef N
 #undef N
#endif
#define N(XXXX) new tRisseASTNode_##XXXX

/* tRisseASTNode_XXXX へのキャスト */
#ifdef C
 #undef C
#endif
#define C(XXXX, EXP) (reinterpret_cast<tRisseASTNode_##XXXX*>(EXP))

/* 名前空間を Risse に */
namespace Risse
{
RISSE_DEFINE_SOURCE_ID(26374,32704,8215,19346,5601,19578,20566,1441);

union YYSTYPE;

/* yylex のプロトタイプ */
int yylex(YYSTYPE * value, void *pr);

/* yyerror のプロトタイプ */
int raise_yyerror(const char * msg, void *pr);

/* yyerror のリダイレクト */
#define risseerror(pr, X) raise_yyerror(X, pr);


/*!
	@brief	文字列リテラルと式を連結するノードを返す
	@param	lp			lexer 解析位置
	@param	conststr	文字列リテラル
	@param	node		式ノード
	@return	連結を表すノード
*/
static tRisseASTNode * RisseAddExprConstStr(risse_size lp,
		const tRisseVariant & conststr, tRisseASTNode * node)
{
	if(((tRisseString)(conststr)).IsEmpty())
		return node; // 文字列リテラルが空文字列
	return N(Binary)(lp, abtAdd, N(Factor)(lp, aftConstant, conststr), node);
}


/*!
	@brief	文字列リテラルと式を連結するノードを返す
	@param	lp			lexer 解析位置
	@param	conststr	文字列リテラル
	@param	node		式ノード
	@return	連結を表すノード
*/
static tRisseASTNode * RisseAddExprConstStr(risse_size lp,
		tRisseASTNode * node, const tRisseVariant & conststr)
{
	if(((tRisseString)(conststr)).IsEmpty())
		return node; // 文字列リテラルが空文字列
	return N(Binary)(lp, abtAdd, node, N(Factor)(lp, aftConstant, conststr));
}


/*!
	@brief	属性を上書きする
	@param	l			上書きされる属性
	@param	r			上書きする属性
	@return		結果
	@note	矛盾した属性を上書きしようとすると例外を発する
*/
static tRisseDeclAttribute * RisseOverwriteDeclAttribute(
	tRisseParser * pr,
	tRisseDeclAttribute * l, const tRisseDeclAttribute * r)
{
	if(l->Overwrite(*r))
		tRisseCompileExceptionClass::Throw(RISSE_WS_TR("duplicated attribute specifier"),
			PR->GetScriptBlock(), LP);
	return l;
}

%}

/*###########################################################################*/

/* デバッグ */
/* TODO: リリース前にはずしておこう！ */
%debug

/* 再入可能なパーサを出力 */
%pure-parser

%parse-param {void * pr}
%lex-param   {void * pr}

/* 詳細なエラー出力 */
%error-verbose

/* GLR parser を出力 */
%glr-parser

/* シフト・還元競合の数 */
%expect    37
%expect-rr 3

/* union 定義 */
%union{
	tRisseVariant * value;
	tRisseASTNode * np;
	tRisseDeclAttribute * attr;
	tRisseASTArray * array;
}

/* トークン定義 */
%token
	T_NONE
	T_AT					"@"
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
	T_MODULE				"module"
	T_RPARENTHESIS			")"
	T_COLON					":"
	T_DOUBLECOLON			"::"
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
	T_INTERNAL				"internal"
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
	T_AS					"as"
	T_USE					"use"
	T_DYNAMIC				"dynamic"
	T_ABSTRACT				"abstract"
	T_IMPLEMENTS			"implements"
	T_INTERFACE				"interface"
	T_NATIVE				"native"
	T_THROWS				"throws"
	T_TRANSIENT				"transient"
	T_VOLATILE				"volatile"
	T_ENUMERABLE			"enumerable"
	T_HIDDEN				"hidden"

	T_FALSE					"false"
	T_NULL					"null"
	T_TRUE					"true"
	T_VOID					"void"
	T_NAN					"NaN"
	T_INFINITY				"Infinity"

	T_BEGIN_NUMBER
	T_BEGIN_STRING_LITERAL
	T_BEGIN_EMSTRING_LITERAL
	T_BEGIN_OCTET_LITERAL
	T_BEGIN_COMMENT



%token <value>		T_CONSTVAL
%token <value>		T_EMSTRING_AMPERSAND_D T_EMSTRING_AMPERSAND_S
%token <value>		T_EMSTRING_DOLLAR_D T_EMSTRING_DOLLAR_S
%token <value>		T_ID
%token <value>		T_REGEXP T_REGEXP_FLAGS

%type <attr>		decl_attr_list
					decl_attr
					decl_attr_access
					decl_attr_visibility
					decl_attr_override

%type <value>		member_name

%type <np>
	toplevel_def_list def_list
	expr expr_with_comma access_expr call_arg call_arg_list
	decl_name_expr factor
	func_expr_def func_expr_def_inner
	func_call_expr
	func_call_expr_body
	inline_array array_elm_list array_elm
	inline_dic dic_elm dic_elm_list
	if
	block block_or_statement statement
	while do_while
	for for_first_clause for_second_clause for_third_clause
	definition
	variable_def variable_def_no_var variable_def_no_semicolon
	variable_id variable_id_list
	func_def func_def_inner
	func_decl_arg func_decl_arg_list func_decl_arg_at_least_one
	func_decl_arg_elm func_decl_arg_elm_collapse call_block call_block_arg_opt
	property_def property_def_inner
	property_expr_def property_expr_def_inner
	property_handler_def_list
	property_handler_getter property_handler_setter
	class_module_def class_module_expr_def class_extender
	break continue
	return switch with label goto try throw catch catch_list
	regexp
	embeddable_string
	embeddable_string_d embeddable_string_d_unit
	embeddable_string_s embeddable_string_s_unit


%type <array>
	call_block_list_opt call_block_list
	func_decl_block_list func_decl_block_at_least_one


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
%left		<np>	"<" ">" "<=" ">=" "instanceof"
%left		<np>	">>" "<<" ">>>"
%left		<np>	"+" "-"
%left		<np>	"%" "/" "\\" "*"
%right		<np>	"!" "~" "--" "++" "new" "delete" "typeof" "#" "$" T_UNARY
%nonassoc	<np>	"incontextof" T_POSTUNARY
%left		<np>	"(" ")" "[" "]" "." "::" T_FUNCCALL


%%
/*###########################################################################*/


/* the program */
program
	: toplevel_list
;

/* toplevel definitions */
toplevel_list
	: toplevel_def_list						{ PR->SetRootNode(C(Context, $1));
											  C(Context, $1)->SetEndPosition(LP); }
;

/* toplevel definition list */
toplevel_def_list
	:										{ $$ = N(Context)(LP, actTopLevel, RISSE_WS("TopLevel")); }
	| toplevel_def_list block_or_statement  { $$ = $1; if($2) C(Context, $$)->AddChild($2); }
	| toplevel_def_list error ";"			{ if(yynerrs > 20)
												YYABORT;
											  else yyerrok; }
;

/* definition list of a block */
def_list
	: 										{ $$ = N(Context)(LP, actBlock, RISSE_WS("Block"));  }
	| def_list block_or_statement			{ $$ = $1; if($2) C(Context, $$)->AddChild($2); }
	| def_list error ";"					{ if(yynerrs > 20)
												YYABORT;
											  else yyerrok; }
;

/* a block */
/*

	TODO:
		{};
	という入力文が、 {} が 辞書配列なのか ブロックなのか分からないという理由で文法エラーになる。
*/
block
	: "{"
	  def_list
	  "}"									{ $$ = $2; C(Context, $2)->SetEndPosition(LP); }
;


/* a block or a single statement */
block_or_statement
	: block
	| statement
;

/* a statement */
statement
	: ";"									{ $$ = N(ExprStmt)(LP, NULL); }
	| expr_with_comma ";"					{ $$ = N(ExprStmt)(LP, $1); }
	| if
	| while
	| do_while
	| for
	| break
	| continue
	| "debugger" ";"						{ $$ = N(Debugger)(LP); }
	| return
	| switch
	| with
	| try
	| throw
	| label
	| goto
	| definition
;

/*---------------------------------------------------------------------------
  iteration
  ---------------------------------------------------------------------------*/

/* a while loop */
while
	: "while"
	  "(" expr ")"
	  block_or_statement					{ $$ = N(While)(LP, $3, $5, false); }
;

/* a do-while loop */
do_while
	: "do"
	  block_or_statement
	  "while"
	  "(" expr ")"
	  ";"									{ $$ = N(While)(LP, $5, $2, true); }
;

/* a for loop */
for
	: "for" "("
	  for_first_clause ";"
	  for_second_clause ";"
	  for_third_clause ")"
	  block_or_statement					{ $$ = N(For)(LP, $3, $5, $7, $9); }
;


/* the first clause of a for statement */
for_first_clause
	: /* empty */							{ $$ = NULL; }
	| variable_def_no_semicolon
	| expr_with_comma
;

/* the second clause of a for statement */
for_second_clause
	: /* empty */							{ $$ = NULL; }
	| expr_with_comma
;

/* the third clause of a for statement */
for_third_clause
	: /* empty */							{ $$ = NULL; }
	| expr_with_comma
;

/*---------------------------------------------------------------------------
  flow control
  ---------------------------------------------------------------------------*/

/* a switch statement */
switch
	: "switch" "("
	  expr_with_comma ")"
	  block									{ $$ = N(Switch)(LP, $3, $5); }
;

/* an if statement */
if
	: "if" "("
	  expr_with_comma
	  ")" block_or_statement	%dprec 2	{ $$ = N(If)(LP, $3, $5); }
	| "if" "("
	  expr_with_comma
	  ")" block_or_statement
	   "else"
	  block_or_statement		%dprec 1	{ $$ = N(If)(LP, $3, $5); C(If, $$)->SetFalse($7); }
/* この規則は とシフト・還元競合を起こす */

/* a break statement */
break
	: "break" ";"							{ $$ = N(Break)(LP, NULL); }
	| "break" expr_with_comma ";"			{ $$ = N(Break)(LP, $2); }
;

/* a continue statment */
continue
	: "continue" ";"						{ $$ = N(Continue)(LP, NULL); }
	| "continue" expr_with_comma ";"		{ $$ = N(Continue)(LP, $2); }
;

/* a return statement */
return
	: "return" ";"							{ $$ = N(Return)(LP, NULL); }
	| "return" expr_with_comma ";"			{ $$ = N(Return)(LP, $2); }
;

/* label or case: */
label
	: "case" expr ":"						{ $$ = N(Case)(LP, $2); }
	| "default" ":"							{ $$ = N(Case)(LP, NULL); }
	| T_ID ":"								{ $$ = N(Label)(LP, *$1); }
;

/* goto */
goto
	: "goto" T_ID ";"						{ $$ = N(Goto)(LP, *$2); }
;

/*---------------------------------------------------------------------------
  syntax sugar
  ---------------------------------------------------------------------------*/

/* a with statement */
with
	: "with" "("
	  expr_with_comma ")"
	  block_or_statement					{ $$ = N(With)(LP, $3, $5); }
;


/*---------------------------------------------------------------------------
  variable definition
  ---------------------------------------------------------------------------*/

/* variable definition */

variable_def
	: "var"
	  variable_id_list ";"					{ $$ = $2; }
;

variable_def_no_var
	:  variable_id_list ";"					{ $$ = $1; }
;

variable_def_no_semicolon
	: "var" variable_id_list				{ $$ = $2; }
	| "const" variable_id_list				{ $$ = $2;
											  C(VarDecl, $$)->SetAttribute(
											  	tRisseDeclAttribute(
											  	tRisseDeclAttribute::ocConst));; }
;

/* list for the variable definition */
variable_id_list
	: variable_id							{ $$ = N(VarDecl)(LP); C(VarDecl, $$)->AddChild($1); }
	| variable_id_list "," variable_id		{ $$ = $1;             C(VarDecl, $$)->AddChild($3); }
;

/* a variable id and an optional initializer expression */
variable_id
	: access_expr							{ $$ = N(VarDeclPair)(LP, $1, NULL); }
	| access_expr "=" expr					{ $$ = N(VarDeclPair)(LP, $1, $3); }
;

/*---------------------------------------------------------------------------
  structured exception handling
  ---------------------------------------------------------------------------*/


/* a structured exception handling */

try
	: "try" block_or_statement
	  catch_list								{ $$ = $3; C(Try, $$)->SetBody($2); }
	| "try" block_or_statement
	  catch_list "finally" block_or_statement
												{ $$ = $3; C(Try, $$)->SetBody($2);
												  C(Try, $$)->SetFinally($5); }
	| "try" block_or_statement
	  "finally" block_or_statement				{ $$ = N(Try)(LP); C(Try, $$)->SetBody($2);
												  C(Try, $$)->SetFinally($4);}
	/* この構文はシフト・還元競合を２つ起こす */
	/* あえて %dprec は指定しない。解決が難しいような形で入力文を記述すると
	   parser が自動的に文法エラーにする */

catch_list
	: catch										{ $$ = N(Try)(LP); C(Try, $$)->AddChild($1); }
	| catch_list catch							{ $$ = $1; C(Try, $1)->AddChild($2); }
;

catch
	: "catch" "(" ")" block_or_statement		{ $$ = N(Catch)(
												  		LP, tRisseString::GetEmptyString(), NULL, $4); }
	| "catch" "(" T_ID ")"
		block_or_statement						{ $$ = N(Catch)(LP, *$3, NULL, $5); }
	| "catch" "(" T_ID "if" expr ")"
		block_or_statement						{ $$ = N(Catch)(LP, *$3, $5, $7); }
	| "catch" "(" "if" expr ")"
		block_or_statement						{ $$ = N(Catch)(
												  		LP, tRisseString::GetEmptyString(), $4, $6); }
;


/* a throw statement */
throw
	: "throw" expr_with_comma ";"			{ $$ = N(Throw)(LP, $2); }
	| "throw" ";"							{ $$ = N(Throw)(LP, NULL); }
;

/*---------------------------------------------------------------------------
  function definition
  ---------------------------------------------------------------------------*/


/* a function definition */
func_def
	: "static" func_def_inner				{ $$ = $2;
											  C(FuncDecl, $$)->SetAttribute(
											  	tRisseDeclAttribute(
											  	tRisseDeclAttribute::ccStatic)); }
	| func_def_inner						{ $$ = $1; }
;

func_def_inner
	: "function" decl_name_expr func_decl_arg
	  block						%dprec 2	{ $$ = $3; C(FuncDecl, $$)->SetName($2);
	  										  C(FuncDecl, $$)->SetBody($4); }
	| "function" decl_name_expr
	  block						%dprec 1	{ $$ = N(FuncDecl)(LP); C(FuncDecl, $$)->SetName($2);
	  										  C(FuncDecl, $$)->SetBody($3); }
;

/* a function expression definition */
func_expr_def
	: "static" func_expr_def_inner			{ $$ = $2;
											  C(FuncDecl, $$)->SetAttribute(
											  	tRisseDeclAttribute(
											  	tRisseDeclAttribute::ccStatic)); }
	| func_expr_def_inner					{ $$ = $1; }
;

func_expr_def_inner
	: "function"
	  block									{ $$ = N(FuncDecl)(LP); C(FuncDecl, $$)->SetBody($2); }
	| "function"
	  func_decl_arg
	  block									{ $$ = $2; C(FuncDecl, $$)->SetBody($3); }
;

/* the argument definition of a function definition */
func_decl_arg
	: "(" func_decl_arg_elm_collapse ")"
	  func_decl_block_list					{ $$ = $2; C(FuncDecl, $$)->AssignBlocks($4); }
	| "(" func_decl_arg_list ")"
	  func_decl_block_list					{ $$ = $2; C(FuncDecl, $$)->AssignBlocks($4); }
	| "(" func_decl_arg_at_least_one ","
	  func_decl_arg_elm_collapse ")"
	  func_decl_block_list					{ $$ = $2; C(FuncDecl, $$)->AddChild($4);
	  										  C(FuncDecl, $$)->AssignBlocks($6); }
;

/* the argument list */
func_decl_arg_list
	: /* empty */							{ $$ = N(FuncDecl)(LP); }
	| func_decl_arg_at_least_one
;

func_decl_arg_at_least_one
	: func_decl_arg_elm						{ $$ = N(FuncDecl)(LP); C(FuncDecl, $$)->AddChild($1); }
	| func_decl_arg_at_least_one ","
	  func_decl_arg_elm						{ $$ =$1; C(FuncDecl, $$)->AddChild($3); }
;

func_decl_arg_elm
	: T_ID									{ $$ = N(FuncDeclArg)(LP, *$1, NULL, false); }
	| T_ID "=" expr							{ $$ = N(FuncDeclArg)(LP, *$1, $3, false); }
;

func_decl_arg_elm_collapse
	: "*"									{ $$ = N(FuncDeclArg)(LP, tRisseString::GetEmptyString(), NULL, true); }
	| T_ID "*"								{ $$ = N(FuncDeclArg)(LP, *$1, NULL, true); }
/*
	These are currently not supported
	| T_ID "*" "=" inline_array			{ ; }
	| T_ID "*=" inline_array			{ ; }
*/
;

func_decl_block_list
	: /* empty */							{ $$ = NULL; }
	| func_decl_block_at_least_one
;

func_decl_block_at_least_one
	: T_ID									{ $$ = new tRisseASTArray();
											  $$->push_back(N(FuncDeclBlock)(LP, *$1)); }
	| func_decl_block_at_least_one T_ID		{ $$ = $1;
											  $$->push_back(N(FuncDeclBlock)(LP, *$2)); }
;


/*---------------------------------------------------------------------------
  property definition
  ---------------------------------------------------------------------------*/

/* a property handler definition */
property_def
	: "static" property_def_inner			{ $$ = $2;
											  C(PropDecl, $$)->SetAttribute(
											  	tRisseDeclAttribute(
											  	tRisseDeclAttribute::ccStatic)); }
	| property_def_inner					{ $$ = $1; }
;

property_def_inner
	: 
	  "property" decl_name_expr
	  "{"
	  property_handler_def_list
	  "}"									{ $$ = $4; C(PropDecl, $$)->SetName($2); }
;

/* a property expression definition */
property_expr_def
	: "static" property_expr_def_inner		{ $$ = $2;
											  C(PropDecl, $$)->SetAttribute(
											  	tRisseDeclAttribute(
											  	tRisseDeclAttribute::ccStatic)); }
	| property_expr_def_inner				{ $$ = $1; }
;

property_expr_def_inner
	: 
	  "property"
	  "{"
	  property_handler_def_list
	  "}"									{ $$ = $3; }
;

property_handler_def_list
	: property_handler_setter
	| property_handler_getter
	| property_handler_setter
	  property_handler_getter				{ $$ = $1; C(PropDecl, $$)->
											  SetGetter(C(PropDecl, $2)->GetGetter()); }
	| property_handler_getter
	  property_handler_setter				{ $$ = $1;
											  C(PropDecl, $$)->
												SetSetter(C(PropDecl, $2)->GetSetter());
											  C(PropDecl, $$)->
												SetSetterArgumentName(C(PropDecl, $2)->
															GetSetterArgumentName()); }
;

property_handler_setter
	: "setter" "(" T_ID ")"
	  block									{ $$ = N(PropDecl)(LP);
											  C(PropDecl, $$)->SetSetter($5);
											  C(PropDecl, $$)->SetSetterArgumentName(*$3); }
;

property_handler_getter
	: property_getter_handler_head
	  block									{ $$ = N(PropDecl)(LP);
											  C(PropDecl, $$)->SetGetter($2); }
;

property_getter_handler_head
	: "getter" "(" ")"
	| "getter"
;

/*---------------------------------------------------------------------------
  class/module definition
  ---------------------------------------------------------------------------*/

/* a class or module definition */
class_module_def
	: "class" decl_name_expr
	  class_extender
	  "{" toplevel_def_list "}"				{ $$ = $3;
											  C(ClassDecl, $$)->SetBody($5);
											  C(ClassDecl, $$)->SetName($2); }
	| "module" decl_name_expr
	  "{" toplevel_def_list "}"				{ $$ = N(ClassDecl)(LP, true, NULL);
											  C(ClassDecl, $$)->SetBody($4);
											  C(ClassDecl, $$)->SetName($2); }
;

class_module_expr_def
	: "class"
	  class_extender
	  "{" toplevel_def_list "}"				{ $$ = $2;
											  C(ClassDecl, $$)->SetBody($4); }
	| "module"
	  "{" toplevel_def_list "}"				{ $$ = N(ClassDecl)(LP, true, NULL);
											  C(ClassDecl, $$)->SetBody($3); }
;

class_extender
	:										{ $$ = N(ClassDecl)(LP, false, NULL); }
	| "extends" expr						{ $$ = N(ClassDecl)(LP, false, $2); }
	| "<" expr/* syntax sugar, Ruby like */	{ $$ = N(ClassDecl)(LP, false, $2); }
	| ":" expr/* syntax sugar, C++ like */	{ $$ = N(ClassDecl)(LP, false, $2); }
;


/*---------------------------------------------------------------------------
  declaration attributes
  ---------------------------------------------------------------------------*/

/* definitions */
definition
	: decl_attr_list variable_def			{ $$ = $2; C(VarDecl, $$)->SetAttribute(*$1); }
	| decl_attr_list variable_def_no_var	{ $$ = $2; C(VarDecl, $$)->SetAttribute(*$1); }
	| variable_def
	| decl_attr_list func_def				{ $$ = $2; C(FuncDecl, $$)->SetAttribute(*$1); }
	| func_def
	| decl_attr_list property_def			{ $$ = $2; C(PropDecl, $$)->SetAttribute(*$1); }
	| property_def
	| decl_attr_list class_module_def		{ $$ = $2; C(ClassDecl, $$)->SetAttribute(*$1); }
	| class_module_def
;

/* attribute lists */

decl_attr_list
	: decl_attr								{ $$ = new tRisseDeclAttribute(*$1); }
	| decl_attr_list decl_attr				{ $$ = RisseOverwriteDeclAttribute(PR, $1, $2); }
;

decl_attr
	: decl_attr_access | decl_attr_visibility | decl_attr_override
;

/* attribute specifiers */

decl_attr_access
	: "public"								{ $$ = new tRisseDeclAttribute(tRisseDeclAttribute::acPublic); }
	| "internal"							{ $$ = new tRisseDeclAttribute(tRisseDeclAttribute::acInternal); }
	| "private"								{ $$ = new tRisseDeclAttribute(tRisseDeclAttribute::acPrivate); }
;

decl_attr_visibility
	: "enumerable"							{ $$ = new tRisseDeclAttribute(tRisseDeclAttribute::vcEnumerable); }
	| "hidden"								{ $$ = new tRisseDeclAttribute(tRisseDeclAttribute::vcHidden); }
;

decl_attr_override
	: "const"								{ $$ = new tRisseDeclAttribute(tRisseMemberAttribute::ocConst); }
	| "virtual"								{ $$ = new tRisseDeclAttribute(tRisseMemberAttribute::ocVirtual); }
;


/*---------------------------------------------------------------------------
  member_name
  ---------------------------------------------------------------------------*/
/* メンバ名として使える名前 */
/*
 通常の ID の他、new などの演算子もメンバ名として使える。
 たとえば演算子の定義の時に function == (rhs) { ... } のように。
*/

member_name
	: T_ID									{ $$ = $1; }
	| "~"									{ $$ = new tRisseVariant(mnBitNot); }
	| "--"									{ $$ = new tRisseVariant(mnDecAssign); }
	| "++"									{ $$ = new tRisseVariant(mnIncAssign); }
	| "+" "@"								{ $$ = new tRisseVariant(mnPlus); }
	| "-" "@"								{ $$ = new tRisseVariant(mnMinus); }
	| "||"									{ $$ = new tRisseVariant(mnLogOr); }
	| "&&"									{ $$ = new tRisseVariant(mnLogAnd); }
	| "|"									{ $$ = new tRisseVariant(mnBitOr); }
	| "^"									{ $$ = new tRisseVariant(mnBitXor); }
	| "&"									{ $$ = new tRisseVariant(mnBitAnd); }
	| "!="									{ $$ = new tRisseVariant(mnNotEqual); }
	| "=="									{ $$ = new tRisseVariant(mnEqual); }
	| "!=="									{ $$ = new tRisseVariant(mnDiscNotEqual); }
	| "<"									{ $$ = new tRisseVariant(mnLesser); }
	| ">"									{ $$ = new tRisseVariant(mnGreater); }
	| "<="									{ $$ = new tRisseVariant(mnLesserOrEqual); }
	| ">="									{ $$ = new tRisseVariant(mnGreaterOrEqual); }
	| ">>>"									{ $$ = new tRisseVariant(mnRBitShift); }
	| "<<"									{ $$ = new tRisseVariant(mnLShift); }
	| ">>"									{ $$ = new tRisseVariant(mnRShift); }
	| "%"									{ $$ = new tRisseVariant(mnMod); }
	| "/"									{ $$ = new tRisseVariant(mnDiv); }
	| "\\"									{ $$ = new tRisseVariant(mnIdiv); }
	| "*"									{ $$ = new tRisseVariant(mnMul); }
	| "+"									{ $$ = new tRisseVariant(mnAdd); }
	| "-"									{ $$ = new tRisseVariant(mnSub); }
	| "[" "]"								{ $$ = new tRisseVariant(mnIGet); }
	| "delete" "[" "]"						{ $$ = new tRisseVariant(mnIDelete); }
	| "[" "]" "="							{ $$ = new tRisseVariant(mnISet); }

/* 以下は words.txt と同期させること */
	| "break"								{ $$ = new tRisseVariant(ss_break          ); }
	| "continue"							{ $$ = new tRisseVariant(ss_continue       ); }
	| "const"								{ $$ = new tRisseVariant(ss_const          ); }
	| "catch"								{ $$ = new tRisseVariant(ss_catch          ); }
	| "class"								{ $$ = new tRisseVariant(ss_class          ); }
	| "case"								{ $$ = new tRisseVariant(ss_case           ); }
	| "debugger"							{ $$ = new tRisseVariant(ss_debugger       ); }
	| "default"								{ $$ = new tRisseVariant(ss_default        ); }
	| "delete"								{ $$ = new tRisseVariant(ss_delete         ); }
	| "do"									{ $$ = new tRisseVariant(ss_do             ); }
	| "dynamic"								{ $$ = new tRisseVariant(ss_dynamic        ); }
	| "extends"								{ $$ = new tRisseVariant(ss_extends        ); }
	| "export"								{ $$ = new tRisseVariant(ss_export         ); }
	| "enum"								{ $$ = new tRisseVariant(ss_enum           ); }
	| "else"								{ $$ = new tRisseVariant(ss_else           ); }
	| "function"							{ $$ = new tRisseVariant(ss_function       ); }
	| "finally"								{ $$ = new tRisseVariant(ss_finally        ); }
	| "final"								{ $$ = new tRisseVariant(ss_final          ); }
	| "for"									{ $$ = new tRisseVariant(ss_for            ); }
	| "global"								{ $$ = new tRisseVariant(ss_global         ); }
	| "getter"								{ $$ = new tRisseVariant(ss_getter         ); }
	| "goto"								{ $$ = new tRisseVariant(ss_goto           ); }
	| "incontextof"							{ $$ = new tRisseVariant(ss_incontextof    ); }
	| "invalidate"							{ $$ = new tRisseVariant(ss_invalidate     ); }
	| "instanceof"							{ $$ = new tRisseVariant(ss_instanceof     ); }
	| "isvalid"								{ $$ = new tRisseVariant(ss_isvalid        ); }
	| "import"								{ $$ = new tRisseVariant(ss_import         ); }
	| "int"									{ $$ = new tRisseVariant(ss_int            ); }
	| "internal"							{ $$ = new tRisseVariant(ss_internal       ); }
	| "in"									{ $$ = new tRisseVariant(ss_in             ); }
	| "if"									{ $$ = new tRisseVariant(ss_if             ); }
	| "module"								{ $$ = new tRisseVariant(ss_module         ); }
	| "new"									{ $$ = new tRisseVariant(ss_new            ); }
	| "octet"								{ $$ = new tRisseVariant(ss_octet          ); }
	| "protected"							{ $$ = new tRisseVariant(ss_protected      ); }
	| "property"							{ $$ = new tRisseVariant(ss_property       ); }
	| "private"								{ $$ = new tRisseVariant(ss_private        ); }
	| "public"								{ $$ = new tRisseVariant(ss_public         ); }
	| "return"								{ $$ = new tRisseVariant(ss_return         ); }
	| "real"								{ $$ = new tRisseVariant(ss_real           ); }
	| "synchronized"						{ $$ = new tRisseVariant(ss_synchronized   ); }
	| "switch"								{ $$ = new tRisseVariant(ss_switch         ); }
	| "static"								{ $$ = new tRisseVariant(ss_static         ); }
	| "setter"								{ $$ = new tRisseVariant(ss_setter         ); }
	| "string"								{ $$ = new tRisseVariant(ss_string         ); }
	| "super"								{ $$ = new tRisseVariant(ss_super          ); }
	| "typeof"								{ $$ = new tRisseVariant(ss_typeof         ); }
	| "throw"								{ $$ = new tRisseVariant(ss_throw          ); }
	| "this"								{ $$ = new tRisseVariant(ss_this           ); }
	| "try"									{ $$ = new tRisseVariant(ss_try            ); }
	| "var"									{ $$ = new tRisseVariant(ss_var            ); }
	| "while"								{ $$ = new tRisseVariant(ss_while          ); }
	| "with"								{ $$ = new tRisseVariant(ss_with           ); }
	| "as"									{ $$ = new tRisseVariant(ss_as             ); }
	| "use"									{ $$ = new tRisseVariant(ss_use            ); }
	| "abstract"							{ $$ = new tRisseVariant(ss_abstract       ); }
	| "implements"							{ $$ = new tRisseVariant(ss_implements     ); }
	| "interface"							{ $$ = new tRisseVariant(ss_interface      ); }
	| "native"								{ $$ = new tRisseVariant(ss_native         ); }
	| "throws"								{ $$ = new tRisseVariant(ss_throws         ); }
	| "transient"							{ $$ = new tRisseVariant(ss_transient      ); }
	| "volatile"							{ $$ = new tRisseVariant(ss_volatile       ); }
	| "enumerable"							{ $$ = new tRisseVariant(ss_enumerable     ); }
	| "hidden"								{ $$ = new tRisseVariant(ss_hidden         ); }

	| "void"								{ $$ = new tRisseVariant(ss_void           ); }
	| "null"								{ $$ = new tRisseVariant(ss_null           ); }
	| "true"								{ $$ = new tRisseVariant(ss_true           ); }
	| "false"								{ $$ = new tRisseVariant(ss_false          ); }
	| "NaN"									{ $$ = new tRisseVariant(ss_NaN            ); }
	| "Infinity"							{ $$ = new tRisseVariant(ss_Infinity       ); }
;


/*---------------------------------------------------------------------------
  expression
  ---------------------------------------------------------------------------*/


/* 式 */
/* カンマとそれ以下の優先順位の式を含む場合はこちらを使う */
expr_with_comma
	: expr_with_comma "if" expr_with_comma	{ $$ = N(Binary)(LP, abtIf			,$3, $1);/*順番に注意*/ }
	| expr_with_comma ","  expr_with_comma	{ $$ = N(Binary)(LP, abtComma		,$1, $3); }
	| expr
;

/*
	expr は、関数への引数リストなど、式中にカンマがあると、リストの
	区切り記号と区別が付かない場合はこちらを使う
*/
expr
	: expr "=" expr					{ $$ = N(Binary)(LP, abtAssign			,$1, $3); }
	| expr "&=" expr				{ $$ = N(Binary)(LP, abtBitAndAssign	,$1, $3); }
	| expr "|=" expr				{ $$ = N(Binary)(LP, abtBitOrAssign		,$1, $3); }
	| expr "^=" expr				{ $$ = N(Binary)(LP, abtBitXorAssign	,$1, $3); }
	| expr "-=" expr				{ $$ = N(Binary)(LP, abtSubAssign		,$1, $3); }
	| expr "+=" expr				{ $$ = N(Binary)(LP, abtAddAssign		,$1, $3); }
	| expr "%=" expr				{ $$ = N(Binary)(LP, abtModAssign		,$1, $3); }
	| expr "/=" expr				{ $$ = N(Binary)(LP, abtDivAssign		,$1, $3); }
	| expr "\\=" expr				{ $$ = N(Binary)(LP, abtIdivAssign		,$1, $3); }
	| expr "*=" expr				{ $$ = N(Binary)(LP, abtMulAssign		,$1, $3); }
	| expr "||=" expr				{ $$ = N(Binary)(LP, abtLogOrAssign		,$1, $3); }
	| expr "&&=" expr				{ $$ = N(Binary)(LP, abtLogAndAssign	,$1, $3); }
	| expr ">>=" expr				{ $$ = N(Binary)(LP, abtRShiftAssign	,$1, $3); }
	| expr "<<=" expr				{ $$ = N(Binary)(LP, abtLShiftAssign	,$1, $3); }
	| expr ">>>=" expr				{ $$ = N(Binary)(LP, abtRBitShiftAssign	,$1, $3); }
	| expr "<->" expr				{ $$ = N(Binary)(LP, abtSwap			,$1, $3); }
	| expr "?" expr ":" expr		{ $$ = N(Trinary)(LP, attCondition 		,$1, $3, $5); }
	| expr "||" expr				{ $$ = N(Binary)(LP, abtLogOr			,$1, $3); }
	| expr "&&" expr				{ $$ = N(Binary)(LP, abtLogAnd			,$1, $3); }
	| expr "|" expr					{ $$ = N(Binary)(LP, abtBitOr			,$1, $3); }
	| expr "^" expr					{ $$ = N(Binary)(LP, abtBitXor			,$1, $3); }
	| expr "&" expr					{ $$ = N(Binary)(LP, abtBitAnd			,$1, $3); }
	| expr "!=" expr				{ $$ = N(Binary)(LP, abtNotEqual		,$1, $3); }
	| expr "==" expr				{ $$ = N(Binary)(LP, abtEqual			,$1, $3); }
	| expr "!==" expr				{ $$ = N(Binary)(LP, abtDiscNotEqual	,$1, $3); }
	| expr "===" expr				{ $$ = N(Binary)(LP, abtDiscEqual		,$1, $3); }
	| expr "<" expr					{ $$ = N(Binary)(LP, abtLesser			,$1, $3); }
	| expr ">" expr					{ $$ = N(Binary)(LP, abtGreater			,$1, $3); }
	| expr "<=" expr				{ $$ = N(Binary)(LP, abtLesserOrEqual	,$1, $3); }
	| expr ">=" expr				{ $$ = N(Binary)(LP, abtGreaterOrEqual	,$1, $3); }
	| expr "instanceof" expr		{ $$ = N(Binary)(LP, abtInstanceOf		,$1, $3); }
	| expr ">>" expr				{ $$ = N(Binary)(LP, abtLShift			,$1, $3); }
	| expr "<<" expr				{ $$ = N(Binary)(LP, abtRShift			,$1, $3); }
	| expr ">>>" expr				{ $$ = N(Binary)(LP, abtRBitShift		,$1, $3); }
	| expr "+" expr					{ $$ = N(Binary)(LP, abtAdd				,$1, $3); }
	| expr "-" expr					{ $$ = N(Binary)(LP, abtSub				,$1, $3); }
	| expr "%" expr					{ $$ = N(Binary)(LP, abtMod				,$1, $3); }
	| expr "/" expr					{ $$ = N(Binary)(LP, abtDiv				,$1, $3); }
	| expr "\\" expr				{ $$ = N(Binary)(LP, abtIdiv			,$1, $3); }
	| expr "*" expr					{ $$ = N(Binary)(LP, abtMul				,$1, $3); }
	| "!" expr						{ $$ = N(Unary)(LP, autLogNot			,$2); }
	| "~" expr						{ $$ = N(Unary)(LP, autBitNot			,$2); }
	| "--" expr						{ $$ = N(Unary)(LP, autPreDec			,$2); }
	| "++" expr						{ $$ = N(Unary)(LP, autPreInc			,$2); }
	| "new" expr					{ $$ = $2;
									  /* new の子ノードは必ず関数呼び出し式である必要がある */
									  if($$->GetType() != antFuncCall)
									  { yyerror(PR, "expected func_call_expr after new");
											    YYERROR; }
									  C(FuncCall, $$)->SetCreateNew(); }
	| "delete" expr					{ $$ = N(Unary)(LP, autDelete			,$2); }
	| "typeof" expr					{ ; }
	| "+" expr %prec T_UNARY		{ $$ = N(Unary)(LP, autPlus				,$2); }
	| "-" expr %prec T_UNARY		{ $$ = N(Unary)(LP, autMinus			,$2); }
	| expr "incontextof" expr		{ $$ = N(InContextOf)(LP,  $1, $3  ); }
	| expr "incontextof" "dynamic"	{ $$ = N(InContextOf)(LP,  $1, NULL); }
	| expr "--" %prec T_POSTUNARY	{ $$ = N(Unary)(LP, autPostDec			,$1); }
	| expr "++" %prec T_POSTUNARY	{ $$ = N(Unary)(LP, autPostInc			,$1); }
	| access_expr
;

access_expr
	: access_expr "[" expr "]"		{ $$ = N(MemberSel)(LP, $1, $3, matIndirect); }
	| access_expr "." member_name	{ $$ = N(MemberSel)(LP, $1, N(Factor)(LP, aftConstant, *$3), matDirect); }
	| access_expr "." "(" expr ")"	{ $$ = N(MemberSel)(LP, $1, $4, matDirect); }
	| access_expr "::" member_name	{ $$ = N(MemberSel)(LP, $1, N(Factor)(LP, aftConstant, *$3), matDirectThis); }
	| access_expr "::" "(" expr ")"	{ $$ = N(MemberSel)(LP, $1, $4, matDirectThis); }
	| "(" expr_with_comma ")"		{ $$ = $2; }
	| func_call_expr
	| func_expr_def
	| property_expr_def
	| class_module_expr_def
	| factor
;


factor
	: T_ID							{ $$ = N(Id)(LP, *$1, false); }
	| "@" T_ID						{ $$ = N(Id)(LP, *$2, true);  }
	| inline_array
	| inline_dic
	| "/="							{ LX->SetNextIsRegularExpression(); }
	  regexp						{ $$ = $3; }
	| "/"							{ LX->SetNextIsRegularExpression(); }
	  regexp						{ $$ = $3; }
	| "this"						{ $$ = N(Factor)(LP, aftThis);  }
	| "super"						{ $$ = N(Factor)(LP, aftSuper);  }
	| T_CONSTVAL					{ $$ = N(Factor)(LP, aftConstant, *$1); }
	| "global"						{ $$ = N(Factor)(LP, aftGlobal); }
	| embeddable_string
;


/*
  function XXX () { } や class XXXX {  } の XXXX の部分に相当する場所に
  記述する式。とくに function においては、ここに普通の式を記述できるようにしてしまうと
  reduce/reduce 競合による parser stack の使い果たしで簡単に memory exhausted に
  なってしまうため、そのような記述はできなくする必要がある。
  ここの記述はある程度上記の記述と重複する。
*/
decl_name_expr
	: decl_name_expr "[" expr "]"		{ $$ = N(MemberSel)(LP, $1, $3, matIndirect); }
	| decl_name_expr "." member_name	{ $$ = N(MemberSel)(LP, $1, N(Factor)(LP, aftConstant, *$3), matDirect); }
	| decl_name_expr "." "(" expr ")"	{ $$ = N(MemberSel)(LP, $1, $4, matDirect); }
	| decl_name_expr "::" member_name	{ $$ = N(MemberSel)(LP, $1, N(Factor)(LP, aftConstant, *$3), matDirectThis); }
	| decl_name_expr "::" "(" expr ")"	{ $$ = N(MemberSel)(LP, $1, $4, matDirectThis); }
	| "(" expr_with_comma ")"			{ $$ = $2; }
	| factor							{ $$ = $1; }
;


/* an expression for function call */
func_call_expr
	: func_call_expr_body
	  call_block_list_opt				{ $$ = $1; C(FuncCall, $$)->AssignBlocks($2); }
;

func_call_expr_body
	: access_expr "(" call_arg_list ")"
							%dprec 1	{ $$ = $3; C(FuncCall, $$)->SetExpression($1); }
	| access_expr "(" "..." ")"			{ $$ = N(FuncCall)(LP, true);  C(FuncCall, $$)->SetExpression($1); }
	| access_expr "(" ")"	%dprec 2	{ $$ = N(FuncCall)(LP, false); C(FuncCall, $$)->SetExpression($1); }
		/* このルールは "(" と ")" の間で下記の 「call_arg が empty」 のルールと
		  シフト・還元競合を起こす(こちらが優先される) */
;

/* argument(s) for function call */
call_arg_list
	: call_arg							{ $$ = N(FuncCall)(LP, false); C(FuncCall, $$)->AddChild($1); }
	| call_arg_list "," call_arg		{ C(FuncCall, $1)->AddChild($3); }
;

call_arg
	: /* empty */						{ $$ = NULL; }
	| "*"								{ $$ = N(FuncCallArg)(LP, NULL, true); }
	| expr "*" 							{ $$ = N(FuncCallArg)(LP, $1, true); }
	| expr								{ $$ = N(FuncCallArg)(LP, $1, false); }
;

/* block argument(s) for function call */
/* このルールは 2つのシフト・還元競合を起こす */
call_block_list_opt
	: /* empty */						{ $$ = NULL; }
	| call_block_list
;

call_block_list
	: call_block						{ $$ = new tRisseASTArray(); $$->push_back($1); }
	| call_block_list call_block		{ $$ = $1; $$->push_back($2); }
;

call_block
	: "{" call_block_arg_opt
	  def_list "}"						{ $$ = $2;
										  C(FuncDecl, $$)->SetBody($3);
										  C(FuncDecl, $$)->SetIsBlock(true);
										  C(Context, $3)->SetEndPosition(LP); }
	| func_expr_def
;

call_block_arg_opt
	: /* empty */						{ $$ = N(FuncDecl)(LP); }
	| "|" func_decl_arg_at_least_one
	  "|"								{ $$ = $2; }
;

/* regular expression */
regexp
	: T_REGEXP T_REGEXP_FLAGS			{ $$ = N(RegExp)(LP, *$1, *$2); }
;

/*---------------------------------------------------------------------------
  dictionary / array expression
  ---------------------------------------------------------------------------*/

/* an inline array object */
inline_array
	: "["
	  array_elm_list
	  "]"								{ $$ = $2; C(Array, $$)->Strip(); }
;

/* an inline array's element list */
array_elm_list
	: array_elm							{ $$ = N(Array)(LP); if($1) C(Array, $$)->AddChild($1); }
	| array_elm_list "," array_elm		{ $$ = $1; C(Array, $$)->AddChild($3); }
;

/* an inline array's element */
array_elm
	: /* empty */						{ $$ = NULL; }
	| expr
;

/* an inline dictionary */
inline_dic
	: "{"
	  dic_elm_list
	  dummy_elm_opt
	  "}"								{ $$ = $2; }
;


/* an inline dictionary's element list */
dic_elm_list
    : /* empty */						{ $$ = N(Dict)(LP); }
	| dic_elm							{ $$ = N(Dict)(LP); C(Dict, $$)->AddChild($1); }
	| dic_elm_list "," dic_elm			{ $$ = $1; C(Dict, $$)->AddChild($3); }
;

/* an inline dictionary's element */
dic_elm
	: expr "=>" expr					{ $$ = N(DictPair)(LP, $1, $3); }
	| T_ID ":" expr						{ $$ = N(DictPair)(LP,
										  N(Factor)(LP, aftConstant, *$1), $3); }
;

/* a dummy element at the tail of inline dictionary elements */
dummy_elm_opt
	: /* empty */
	| ","
;

/*---------------------------------------------------------------------------
  embeddable string literal
  ---------------------------------------------------------------------------*/




/* 埋め込み可能な文字列リテラル */
/*
   埋め込み可能な文字列リテラルは @ に続くシングルクオーテーションまたは
   ダブルクオーテーションで囲まれた文字列リテラルであり、中に ${式} あるいは
   &式; の形式での式の埋め込みができる (実行時に展開される) */
/*
   ダブルクオーテーションで始まった文字列リテラルはダブルクオーテーションで
   終了する必要があり、シングルクオーテーションにおいても同様だが、
   クオーテーションの種類についてはここでは文脈を分けることで対処する。
*/
/*
   ${ で始まる埋め込み式は }  で終わるため、parser は式と '}' が来たその直後に
   Lexer に対して埋め込み可能な文字列リテラルの再開を告げる
   (&式; 形式についても同様)。Lexer はこれを受け、再び文字列リテラルの解析モードに入る。
*/
embeddable_string
	: embeddable_string_d T_CONSTVAL { $$ = RisseAddExprConstStr(LP, $1, *$2); }
	| embeddable_string_s T_CONSTVAL { $$ = RisseAddExprConstStr(LP, $1, *$2); }
;

/* 埋め込み可能な文字列リテラル(ダブルクオーテーション) */
embeddable_string_d
	: embeddable_string_d_unit
	| embeddable_string_d embeddable_string_d_unit	{ $$ = N(Binary)(LP, abtAdd, $1, $2); }
;

embeddable_string_d_unit
	: T_EMSTRING_AMPERSAND_D expr_with_comma ";" { $$ = RisseAddExprConstStr(LP, *$1, $2);
													LX->SetContinueEmbeddableString(RISSE_WC('"')); }
	| T_EMSTRING_DOLLAR_D    expr_with_comma "}" { $$ = RisseAddExprConstStr(LP, *$1, $2);
													LX->SetContinueEmbeddableString(RISSE_WC('"')); }
;

/* 埋め込み可能な文字列リテラル(シングルクオーテーション) */
embeddable_string_s
	: embeddable_string_s_unit
	| embeddable_string_s embeddable_string_s_unit	{ $$ = N(Binary)(LP, abtAdd, $1, $2); }
;

embeddable_string_s_unit
	: T_EMSTRING_AMPERSAND_S expr_with_comma ";" { $$ = RisseAddExprConstStr(LP, *$1, $2);
													LX->SetContinueEmbeddableString(RISSE_WC('\'')); }
	| T_EMSTRING_DOLLAR_S    expr_with_comma "}" { $$ = RisseAddExprConstStr(LP, *$1, $2);
													LX->SetContinueEmbeddableString(RISSE_WC('\'')); }
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
int raise_yyerror(const char * msg, void *pr)
{
	tRisseCompileExceptionClass::Throw(tRisseString(msg), PR->GetScriptBlock(), LP);
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseParser::tRisseParser(tRisseScriptBlock * sb, tRisseLexer * lexer)
{
	ScriptBlock = sb;
	Root = NULL;
	Lexer = lexer;

	yyparse(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tRisseParser::GetToken(tRisseVariant & value)
{
	// lexer 中に発生するかもしれない例外をキャッチする
	try
	{
		return Lexer->GetToken(value);
	}
	catch(const tRisseVariant * e)
	{
		e->AddTrace(ScriptBlock, Lexer->GetPosition());
		throw e;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseParser::SetRootNode(tRisseASTNode * root)
{
	Root = root;
}
//---------------------------------------------------------------------------


} // namespace Risse





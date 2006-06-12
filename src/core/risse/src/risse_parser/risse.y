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
#include "../risseCxxObject.h"
#include "risseParser.h"
#include "risseScriptBlock.h"

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

/*! 字句解析器 */
#define LX (PR->GetLexer())

/*! 字句解析器の現在の解析位置 */
#define LP (LX->GetPosition())

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

/* yylex のプロトタイプ */
int yylex(YYSTYPE * value, void *pr);

/* yyerror のプロトタイプ */
int raise_yyerror(char * msg, void *pr);

/* yyerror のリダイレクト */
#define risseerror(X) raise_yyerror(X, pr);


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


%}

/*###########################################################################*/

/* 再入可能なパーサを出力 */
%pure_parser

/* シフト・還元競合は4つある */
%expect 4

/* union 定義 */
%union{
	tRisseVariant * value;
	tRisseASTNode * np;
	tRisseMemberAttribute * attr;
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
	T_BEGIN_MACRO
	T_BEGIN_EMSTRING_LITERAL
	T_BEGIN_OCTET_LITERAL
	T_BEGIN_COMMENT
	T_EMSTRING_AMPERSAND_D
	T_EMSTRING_AMPERSAND_S
	T_EMSTRING_DOLLAR_D
	T_EMSTRING_DOLLAR_S



%token <value>		T_CONSTVAL
%token <value>		T_EMSTRING_AMPERSAND_D T_EMSTRING_AMPERSAND_S
%token <value>		T_EMSTRING_DOLLAR_D T_EMSTRING_DOLLAR_S
%token <value>		T_ID
%token <value>		T_REGEXP T_REGEXP_FLAGS

%type <attr>		member_attr_list member_attr member_attr_list_non_prop
					member_attr_non_prop member_attr_prop


%type <np>
	toplevel_def_list def_list
	expr expr_with_comma factor_expr call_arg call_arg_list
	func_expr_def func_call_expr func_call_expr_body
	inline_array array_elm_list array_elm
	inline_dic dic_elm dic_elm_list
	if if_else
	block block_or_statement statement
	while do_while
	for for_first_clause for_second_clause for_third_clause
	definition
	variable_def variable_def_inner variable_id variable_id_list
	func_def func_decl_arg_opt func_decl_arg_list func_decl_arg_at_least_one
	func_decl_arg func_decl_arg_collapse
	property_def property_expr_def property_handler_def_list
	property_handler_getter property_handler_setter
	class_def class_expr_def class_extender class_extends_list
	return switch with label goto try throw catch catch_list catch_or_finally
	regexp
	embeddable_string
	embeddable_string_d embeddable_string_d_unit
	embeddable_string_s embeddable_string_s_unit


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
%left		<np>	"(" ")" "[" "]" "." T_FUNCCALL


%%
/*###########################################################################*/


/* the program */
program
	: toplevel_list
;

/* toplevel definitions */
toplevel_list
	: toplevel_def_list						{ PR->SetRootNode(C(Context, $1)); }
;

/* toplevel definition list */
toplevel_def_list
	:										{ $$ = N(Context)(LP, actTopLevel, RISSE_WS("TopLevel")); }
	| toplevel_def_list block_or_statement	{ $$ = $1; if($2) C(Context, $$)->AddChild($2); }
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
block
	: "{"
	  def_list
	  "}"									{ $$ = $2; }
;


/* a block or a single statement */
block_or_statement
	: statement
	| block
;

/* a statement */
statement
	: ";"									{ $$ = N(ExprStmt)(LP, NULL); }
	| expr_with_comma ";"					{ $$ = N(ExprStmt)(LP, $1); }
	| if
	| if_else
	| while
	| do_while
	| for
	| "break" ";"							{ $$ = N(Break)(LP); }
	| "continue" ";"						{ $$ = N(Continue)(LP); }
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
	| variable_def_inner
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
	  ")" block_or_statement				{ $$ = N(If)(LP, $3, $5); }
;

/* an if-else statement */
/* この規則は上記の if とシフト・還元競合を起こす */
if_else
	: if "else"
	  block_or_statement					{ $$ = $1; C(If, $$)->SetFalse($3); }
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
	: variable_def_inner ";"				{ $$ = $1; }
;

variable_def_inner
	: "var" variable_id_list				{ $$ = $2; C(VarDecl, $$)->SetIsConstant(false); }
	| "const" variable_id_list				{ $$ = $2; C(VarDecl, $$)->SetIsConstant(true); }
;

/* list for the variable definition */
variable_id_list
	: variable_id							{ $$ = N(VarDecl)(LP); C(VarDecl, $$)->AddChild($1); }
	| variable_id_list "," variable_id		{ $$ = $1;             C(VarDecl, $$)->AddChild($3); }
;

/* a variable id and an optional initializer expression */
variable_id
	: T_ID									{ $$ = N(VarDeclPair)(LP, *$1, NULL); }
	| T_ID "=" expr							{ $$ = N(VarDeclPair)(LP, *$1, $3); }
;

/*---------------------------------------------------------------------------
  structured exception handling
  ---------------------------------------------------------------------------*/


/* a structured exception handling */

try
	: "try"
	  block_or_statement
	  catch_or_finally							{ $$ = $3; C(Try, $$)->SetBody($2); }
;

catch_or_finally
	: catch_list
	| catch_list "finally" block_or_statement	{ $$ = $1; C(Try, $$)->SetFinally($3); }
	| "finally" block_or_statement				{ $$ = N(Try)(LP); C(Try, $$)->SetFinally($2); }
	/* この構文はシフト・還元競合を 2 つ起こすが、問題ない */
;

catch_list
	: catch										{ $$ = N(Try)(LP); C(Try, $$)->AddChild($1); }
	| catch_list catch							{ $$ = $1; C(Try, $1)->AddChild($2); }
;

catch
	: "catch" "(" ")" block_or_statement		{ $$ = N(Catch)(
												  		LP, tRisseString(), NULL, $4); }
	| "catch" "(" T_ID ")"
		block_or_statement						{ $$ = N(Catch)(LP, *$3, NULL, $5); }
	| "catch" "(" T_ID "if" expr ")"
		block_or_statement						{ $$ = N(Catch)(LP, *$3, $5, $7); }
	| "catch" "(" "if" expr ")"
		block_or_statement						{ $$ = N(Catch)(
												  		LP, tRisseString(), $4, $6); }
;


/* a throw statement */
throw
	: "throw" expr_with_comma ";"			{ $$ = N(Throw)(LP, $2); }
;

/*---------------------------------------------------------------------------
  function definition
  ---------------------------------------------------------------------------*/


/* a function definition */
func_def
	: "function" T_ID
	  func_decl_arg_opt
	  block									{ $$ = $3; C(FuncDecl, $$)->SetName(*$2);
	  										  C(FuncDecl, $$)->SetBody($4); }
;

/* a function expression definition */
func_expr_def
	: "function"
	  func_decl_arg_opt
	  block									{ $$ = $2; C(FuncDecl, $$)->SetBody($3); }
;

/* the argument definition of a function definition */
func_decl_arg_opt
	: /* empty */							{ $$ = N(FuncDecl)(LP); }
	| "(" func_decl_arg_collapse ")"		{ $$ = $2; }
	| "(" func_decl_arg_list ")"			{ $$ = $2; }
	| "(" func_decl_arg_at_least_one ","
	  func_decl_arg_collapse ")"			{ $$ = $2; C(FuncDecl, $$)->AddChild($4); }
;

/* the argument list */
func_decl_arg_list
	: /* empty */							{ $$ = N(FuncDecl)(LP); }
	| func_decl_arg_at_least_one
;

func_decl_arg_at_least_one
	: func_decl_arg							{ $$ = N(FuncDecl)(LP); C(FuncDecl, $$)->AddChild($1); }
	| func_decl_arg_at_least_one ","
	  func_decl_arg							{ $$ =$1; C(FuncDecl, $$)->AddChild($3); }
;

func_decl_arg
	: T_ID									{ $$ = N(FuncDeclArg)(LP, *$1, NULL, false); }
	| T_ID "=" expr							{ $$ = N(FuncDeclArg)(LP, *$1, $3, false); }
;

func_decl_arg_collapse
	: "*"									{ $$ = N(FuncDeclArg)(LP, tRisseString(), NULL, true); }
	| T_ID "*"								{ $$ = N(FuncDeclArg)(LP, *$1, NULL, true); }
/*
	These are currently not supported
	| T_ID "*" "=" inline_array			{ ; }
	| T_ID "*=" inline_array			{ ; }
*/
;

/*---------------------------------------------------------------------------
  property definition
  ---------------------------------------------------------------------------*/

/* a property handler definition */
property_def
	: "property" T_ID
	  "{"
	  property_handler_def_list
	  "}"									{ $$ = $4; C(PropDecl, $$)->SetName(*$2); }
;

/* a property expression definition */
property_expr_def
	: "property"
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
  class definition
  ---------------------------------------------------------------------------*/

/* a class definition */
class_def
	: "class" T_ID
	  class_extender
	  block									{ $$ = $3;
											  C(ClassDecl, $$)->SetBody($4);
											  C(ClassDecl, $$)->SetName(*$2); }
;

class_expr_def
	: "class"
	  class_extender
	  block									{ $$ = $2;
											  C(ClassDecl, $$)->SetBody($3); }
;

class_extender
	:										{ $$ = N(ClassDecl)(LP); }
	| "extends" class_extends_list			{ $$ = $2; }
;

class_extends_list
	: expr									{ $$ = N(ClassDecl)(LP);
											  C(ClassDecl, $$)->AddChild($1); }
	| class_extends_list "," expr			{ $$ = $1; C(ClassDecl, $$)->AddChild($3); }
;


/*---------------------------------------------------------------------------
  member attribute
  ---------------------------------------------------------------------------*/

/* definitions */
definition
	: member_attr_list_non_prop
	  variable_def							{ $$ = $2; C(VarDecl, $$)->SetAttribute(*$1); }
	| variable_def
	| member_attr_list_non_prop
	  func_def								{ $$ = $2; C(FuncDecl, $$)->SetAttribute(*$1); }
	| func_def
	| member_attr_list_non_prop
	  property_def							{ $$ = $2; C(PropDecl, $$)->SetAttribute(*$1); }
	| property_def
	| member_attr_list_non_prop
	  class_def								{ $$ = $2; C(ClassDecl, $$)->SetAttribute(*$1); }
	| class_def
;


/* member attributes */
member_attr_list
	: member_attr							{ $$ = new tRisseMemberAttribute(*$1); }
	| member_attr_list member_attr			{ $$ = $1;
											  if($1->Overwrite(*$2))
											  { yyerror("duplicated attribute specifier");
											    YYERROR; }
											}
;

member_attr_list_non_prop
	: member_attr_non_prop					{ $$ = new tRisseMemberAttribute(*$1); }
	| member_attr_list_non_prop
	  member_attr_non_prop					{ $$ = $1;
											  if($1->Overwrite(*$2))
											  { yyerror("duplicated attribute specifier");
											    YYERROR; }
											}
;

member_attr
	: member_attr_non_prop
	| member_attr_prop
;

member_attr_non_prop
	: "public"								{ $$ = new tRisseMemberAttribute(tRisseMemberAttribute::acPublic); }
	| "internal"							{ $$ = new tRisseMemberAttribute(tRisseMemberAttribute::acInternal); }
	| "private"								{ $$ = new tRisseMemberAttribute(tRisseMemberAttribute::acPrivate); }
	| "enumerable"							{ $$ = new tRisseMemberAttribute(tRisseMemberAttribute::vcEnumerable); }
	| "hidden"								{ $$ = new tRisseMemberAttribute(tRisseMemberAttribute::vcHidden); }
	| "static"								{ $$ = new tRisseMemberAttribute(tRisseMemberAttribute::ocStatic); }
	| "final"								{ $$ = new tRisseMemberAttribute(tRisseMemberAttribute::ocFinal); }
	| "virtual"								{ $$ = new tRisseMemberAttribute(tRisseMemberAttribute::ocVirtual); }
;

member_attr_prop
	: "property"							{ $$ = new tRisseMemberAttribute(tRisseMemberAttribute::pcProperty); }
	| "const"								{ $$ = new tRisseMemberAttribute(tRisseMemberAttribute::pcConst); }
	| "var"									{ $$ = new tRisseMemberAttribute(tRisseMemberAttribute::pcVar); }
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
	expr_no_comma は、関数への引数リストなど、式中にカンマがあると、リストの
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
									  { yyerror("expected func_call_expr after new");
											    YYERROR; }
									  C(FuncCall, $$)->SetCreateNew(); }
	| "delete" expr					{ $$ = N(Unary)(LP, autDelete			,$2); }
	| "typeof" expr					{ ; }
	| "+" expr %prec T_UNARY		{ $$ = N(Unary)(LP, autPlus				,$2); }
	| "-" expr %prec T_UNARY		{ $$ = N(Unary)(LP, autMinus			,$2); }
	| expr "incontextof" expr		{ ; }
	| expr "--" %prec T_POSTUNARY	{ $$ = N(Unary)(LP, autPostDec			,$1); }
	| expr "++" %prec T_POSTUNARY	{ $$ = N(Unary)(LP, autPostInc			,$1); }
	| func_call_expr				{ ; }
	| "(" expr_with_comma ")"		{ $$ = $2; }
	| expr "[" expr "]"				{ $$ = N(MemberSel)(LP, $1, $3, false); }
	| expr "." T_ID					{ $$ = N(MemberSel)(LP, $1, N(Factor)(LP, aftConstant, *$3), true); }
	| expr "." "(" expr ")"			{ $$ = N(MemberSel)(LP, $1, $4, true); }
	| expr "<" member_attr_list ">"
	  %prec T_FUNCCALL				{ $$ = N(CastAttr)(LP, *$3, $1); }
	| factor_expr
;

factor_expr
	: T_ID							{ $$ = N(Id)(LP, *$1); }
	| T_CONSTVAL					{ $$ = N(Factor)(LP, aftConstant, *$1); }
	| "this"						{ $$ = N(Factor)(LP, aftThis);  }
	| "super"						{ $$ = N(Factor)(LP, aftSuper);  }
	| func_expr_def
	| property_expr_def
	| class_expr_def
	| "global"						{ $$ = N(Factor)(LP, aftGlobal); }
	| inline_array
	| inline_dic
	| "/="							{ LX->SetNextIsRegularExpression(); }
	  regexp						{ $$ = $3; }
	| "/"							{ LX->SetNextIsRegularExpression(); }
	  regexp						{ $$ = $3; }
	| embeddable_string
;


/* an expression for function call */
func_call_expr
	: func_call_expr_body
	| func_call_expr_body func_expr_def	{ $$ = $1;
										  C(FuncCall, $$)->AddChild(N(FuncCallArg(LP, $2, false))); }
;

func_call_expr_body
	: expr "(" call_arg_list ")"		{ $$ = $3; C(FuncCall, $$)->SetExpression($1); }
	| expr "(" "..." ")"				{ $$ = N(FuncCall)(LP, true);  C(FuncCall, $$)->SetExpression($1); }
	| expr "(" ")"						{ $$ = N(FuncCall)(LP, false); C(FuncCall, $$)->SetExpression($1); }
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
	: "%" "["
	  dic_elm_list
	  dummy_elm_opt
	  "]"								{ $$ = $3; }
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
int raise_yyerror(char * msg, void *pr)
{
	eRisseCompileError::Throw(tRisseString(msg), PR->GetScriptBlock(), LP);
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
void tRisseParser::SetRootNode(tRisseASTNode * root)
{
	Root = root;
}
//---------------------------------------------------------------------------


} // namespace Risse





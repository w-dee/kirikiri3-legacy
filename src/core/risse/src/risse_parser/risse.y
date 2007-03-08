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

/* 位置情報を表す定義 */
struct RisseYYLTYPE
{
	risse_size first;
	risse_size last;
};
#define YYLTYPE RisseYYLTYPE

/* 位置情報のマージ */

#define YYLLOC_DEFAULT(Current, Rhs, N)                    \
 do                                                        \
   if (N)                                                  \
     {                                                     \
       (Current).first   = YYRHSLOC(Rhs, 1).first;         \
       (Current).last    = YYRHSLOC(Rhs, N).last;          \
     }                                                     \
   else                                                    \
     {                                                     \
       (Current).first   = (Current).last   =              \
         YYRHSLOC(Rhs, 0).last;                            \
     }                                                     \
 while (0)

/* YYSTYPE の前方参照用定義 */
union YYSTYPE;

/* yylex のプロトタイプ */
int yylex(YYSTYPE * value, YYLTYPE *llocp, void *pr);

/* yyerror のプロトタイプ */
int raise_yyerror(const char * msg, void *pr, YYLTYPE *loc);

/* yyerror のリダイレクト */
#define risseerror(loc, pr, X) raise_yyerror(X, pr, loc);


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
	tRisseParser * pr, risse_size pos,
	tRisseDeclAttribute * l, const tRisseDeclAttribute * r)
{
	if(l->Overwrite(*r))
		tRisseCompileExceptionClass::Throw(RISSE_WS_TR("duplicated attribute specifier"),
			PR->GetScriptBlock(), pos);
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

/* 位置情報を使う */
%locations

/* シフト・還元競合の数 */
%expect    6

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

	T_NL					"\n"

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
											  C(Context, $1)->SetEndPosition(@$.last); }
;

/* toplevel definition list */
toplevel_def_list
	: 										{ $$ = N(Context)(@$.first, actTopLevel, RISSE_WS("TopLevel")); }
	| toplevel_def_list block_or_statement  { $$ = $1; if($2) C(Context, $$)->AddChild($2); }
	| toplevel_def_list error snl			{ if(yynerrs > 20)
												YYABORT;
											  else yyerrok; }
;

/* definition list of a block */
def_list
	: 										{ $$ = N(Context)(@$.first, actBlock, RISSE_WS("Block"));  }
	| def_list block_or_statement			{ $$ = $1; if($2) C(Context, $$)->AddChild($2); }
	| def_list error snl					{ if(yynerrs > 20)
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
	  onl
	  def_list
	  "}"									{ $$ = $3; C(Context, $$)->SetEndPosition(@$.last); }
;


/* a block or a single statement */
block_or_statement
	: block	onl			/*%dprec 2*/		{ $$ = $1; }
	| statement			/*%dprec 1*/		{ $$ = $1; }
;

/* a statement */
statement
	: ";" nl								{ $$ = N(ExprStmt)(@1.first, NULL); }
	| ";"									{ $$ = N(ExprStmt)(@1.first, NULL); }
	| expr_with_comma snl					{ $$ = N(ExprStmt)(@1.first, $1); }
	| if
	| while
	| do_while
	| for
	| break
	| continue
	| "debugger" snl						{ $$ = N(Debugger)(@1.first); }
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
	: "while" onl
	  "(" onl expr onl ")" onl
	  block_or_statement					{ $$ = N(While)(@1.first, $5, $9, false); }
;

/* a do-while loop */
do_while
	: "do" onl
	  block_or_statement
	  "while" onl
	  "(" onl expr onl ")"
	  snl									{ $$ = N(While)(@1.first, $8, $3, true); }
;

/* a for loop */
for
	: "for" onl "(" 
	  for_first_clause  ";"
	  for_second_clause ";" 
	  for_third_clause  ")" onl
	  block_or_statement					{ $$ = N(For)(@1.first, $4, $6, $8, $11); }
;


/* the first clause of a for statement */
for_first_clause
	: onl									{ $$ = NULL; }
	| onl variable_def_no_semicolon onl		{ $$ = $2; }
	| onl expr_with_comma onl				{ $$ = $2; }
;

/* the second clause of a for statement */
for_second_clause
	: onl									{ $$ = NULL; }
	| onl expr_with_comma onl				{ $$ = $2; }
;

/* the third clause of a for statement */
for_third_clause
	: onl									{ $$ = NULL; }
	| onl expr_with_comma onl				{ $$ = $2; }
;

/*---------------------------------------------------------------------------
  flow control
  ---------------------------------------------------------------------------*/

/* a switch statement */
switch
	: "switch" onl "(" onl
	  expr_with_comma onl ")" onl
	  block	onl								{ $$ = N(Switch)(@1.first, $5, $9); }
;

/* an if statement */
if
	: "if" onl "(" 
	  expr_with_comma onl
	  ")" onl block_or_statement
	  							/*%dprec 2*/	{ $$ = N(If)(@1.first, $4, $8); }
	| "if" onl "(" 
	  expr_with_comma onl
	  ")" onl block_or_statement
	   "else" onl
	  block_or_statement		/*%dprec 1*/	{ $$ = N(If)(@1.first, $4, $8); C(If, $$)->SetFalse($11); }
/* この規則は とシフト・還元競合を起こす */

/* a break statement */
break
	: "break" snl							{ $$ = N(Break)(@1.first, NULL); }
	| "break" expr_with_comma snl			{ $$ = N(Break)(@1.first, $2); }
;

/* a continue statment */
continue
	: "continue" snl						{ $$ = N(Continue)(@1.first, NULL); }
	| "continue" expr_with_comma snl		{ $$ = N(Continue)(@1.first, $2); }
;

/* a return statement */
return
	: "return" snl							{ $$ = N(Return)(@1.first, NULL); }
	| "return" expr_with_comma snl			{ $$ = N(Return)(@1.first, $2); }
;

/* label or case: */
label
	: "case" onl expr ":" onl	 			{ $$ = N(Case)(@1.first, $3); }
	| "default" onl  ":" onl	 			{ $$ = N(Case)(@1.first, NULL); }
	| T_ID ":" onl	 						{ $$ = N(Label)(@1.first, *$1); }
;

/* goto */
goto
	: "goto" onl T_ID snl					{ $$ = N(Goto)(@1.first, *$3); }
;

/*---------------------------------------------------------------------------
  syntax sugar
  ---------------------------------------------------------------------------*/

/* a with statement */
with
	: "with" onl "(" onl
	  expr_with_comma onl ")" onl
	  block_or_statement					{ $$ = N(With)(@1.first, $5, $9); }
;


/*---------------------------------------------------------------------------
  variable definition
  ---------------------------------------------------------------------------*/

/* variable definition */

variable_def
	: "var" onl 
	  variable_id_list snl					{ $$ = $3; }
;

variable_def_no_var
	:  variable_id_list snl				{ $$ = $1; }
;

variable_def_no_semicolon
	: "var" onl variable_id_list			{ $$ = $3; }
	| "const" onl variable_id_list			{ $$ = $3;
											  C(VarDecl, $$)->SetAttribute(
											  	tRisseDeclAttribute(
											  	tRisseDeclAttribute::ocConst));; }
;

/* list for the variable definition */
variable_id_list
	: variable_id							{ $$ = N(VarDecl)(@1.first); C(VarDecl, $$)->AddChild($1); }
	| variable_id_list "," onl variable_id	{ $$ = $1;             C(VarDecl, $$)->AddChild($4); }
;

/* a variable id and an optional initializer expression */
variable_id
	: access_expr							{ $$ = N(VarDeclPair)(@1.first, $1, NULL); }
	| access_expr "=" onl expr				{ $$ = N(VarDeclPair)(@1.first, $1, $4); }
;

/*---------------------------------------------------------------------------
  structured exception handling
  ---------------------------------------------------------------------------*/


/* a structured exception handling */

try
	: "try" onl block_or_statement
	  catch_list								{ $$ = $4; C(Try, $$)->SetBody($3); $$->SetPosition(@1.first); }
	| "try" onl block_or_statement
	  catch_list "finally" onl block_or_statement
												{ $$ = $4; C(Try, $$)->SetBody($3);
												  C(Try, $$)->SetFinally($7); $$->SetPosition(@1.first); }
	| "try" onl block_or_statement
	  "finally" onl block_or_statement			{ $$ = N(Try)(@1.first); C(Try, $$)->SetBody($3);
												  C(Try, $$)->SetFinally($6);  $$->SetPosition(@1.first); }
	/* この構文はシフト・還元競合を２つ起こす */

catch_list
	: catch 									{ $$ = N(Try)(@1.first); C(Try, $$)->AddChild($1); }
	| catch_list catch							{ $$ = $1; C(Try, $1)->AddChild($2); }
;

catch
	: "catch" onl "(" onl ")" onl
	  block_or_statement 						{ $$ = N(Catch)(
												  		@1.first, tRisseString::GetEmptyString(), NULL, $7); }
	| "catch" onl "(" onl T_ID onl ")" onl
	  block_or_statement 						{ $$ = N(Catch)(@1.first, *$5, NULL, $9); }
	| "catch" onl "(" onl T_ID onl "if" onl expr onl ")" onl
	  block_or_statement						{ $$ = N(Catch)(@1.first, *$5, $9, $13); }
	| "catch" onl "(" onl "if" onl expr onl ")" onl
	  block_or_statement 						{ $$ = N(Catch)(
												  		@1.first, tRisseString::GetEmptyString(), $7, $11); }
;


/* a throw statement */
throw
	: "throw" expr_with_comma snl				{ $$ = N(Throw)(@1.first, $2); }
	| "throw" snl								{ $$ = N(Throw)(@1.first, NULL); }
;

/*---------------------------------------------------------------------------
  function definition
  ---------------------------------------------------------------------------*/


/* a function definition */
func_def
	: "static" onl func_def_inner			{ $$ = $3;
											  C(FuncDecl, $$)->SetAttribute(
											  	tRisseDeclAttribute(
											  	tRisseDeclAttribute::ccStatic)); }
	| func_def_inner						{ $$ = $1; }
;

func_def_inner
	: "function" onl decl_name_expr onl func_decl_arg
	  block	onl					/*%dprec 2*/{ $$ = $5; C(FuncDecl, $$)->SetName($3);
	  										  C(FuncDecl, $$)->SetBody($6);  $$->SetPosition(@1.first); }
	| "function" onl decl_name_expr onl
	  block	onl					/*%dprec 1*/{ $$ = N(FuncDecl)(@1.first); C(FuncDecl, $$)->SetName($3);
	  										  C(FuncDecl, $$)->SetBody($5); }
;

/* a function expression definition */
func_expr_def
	: "static" onl func_expr_def_inner		{ $$ = $3;
											  C(FuncDecl, $$)->SetAttribute(
											  	tRisseDeclAttribute(
											  	tRisseDeclAttribute::ccStatic)); }
	| func_expr_def_inner					{ $$ = $1; }
;

func_expr_def_inner
	: "function"
	  onl block								{ $$ = N(FuncDecl)(@1.first); C(FuncDecl, $$)->SetBody($3); }
	| "function"
	  onl func_decl_arg
	  block									{ $$ = $3; C(FuncDecl, $$)->SetBody($4);  $$->SetPosition(@1.first); }
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
	: onl 									{ $$ = N(FuncDecl)(@1.first); }
	| func_decl_arg_at_least_one
;

func_decl_arg_at_least_one
	: func_decl_arg_elm						{ $$ = N(FuncDecl)(@1.first); C(FuncDecl, $$)->AddChild($1); }
	| func_decl_arg_at_least_one ","
	  func_decl_arg_elm						{ $$ =$1; C(FuncDecl, $$)->AddChild($3); }
;

func_decl_arg_elm
	: onl T_ID onl							{ $$ = N(FuncDeclArg)(@2.first, *$2, NULL, false); }
	| onl T_ID onl "=" onl expr				{ $$ = N(FuncDeclArg)(@2.first, *$2, $6, false); }
;

func_decl_arg_elm_collapse
	: onl "*" onl							{ $$ = N(FuncDeclArg)(@2.first, tRisseString::GetEmptyString(), NULL, true); }
	| onl T_ID "*"	onl						{ $$ = N(FuncDeclArg)(@2.first, *$2, NULL, true); }
/*
	These are currently not supported
	| T_ID "*" "=" inline_array			{ ; }
	| T_ID "*=" inline_array			{ ; }
*/
;

func_decl_block_list
	: onl									{ $$ = NULL; }
	| func_decl_block_at_least_one			{ $$ = $1; }
;

func_decl_block_at_least_one
	: T_ID onl								{ $$ = new tRisseASTArray();
											  $$->push_back(N(FuncDeclBlock)(@1.first, *$1)); }
	| func_decl_block_at_least_one T_ID onl	{ $$ = $1;
											  $$->push_back(N(FuncDeclBlock)(@2.first, *$2)); }
;


/*---------------------------------------------------------------------------
  property definition
  ---------------------------------------------------------------------------*/

/* a property handler definition */
property_def
	: "static" onl property_def_inner	onl	{ $$ = $3;
											  C(PropDecl, $$)->SetAttribute(
											  	tRisseDeclAttribute(
											  	tRisseDeclAttribute::ccStatic)); }
	| property_def_inner	onl				{ $$ = $1; }
;

property_def_inner
	: "property" onl decl_name_expr onl
	  "{"
	  property_handler_def_list
	  "}"									{ $$ = $6; C(PropDecl, $$)->SetName($3);  $$->SetPosition(@1.first); }
;

/* a property expression definition */
property_expr_def
	: "static" onl property_expr_def_inner	{ $$ = $3;
											  C(PropDecl, $$)->SetAttribute(
											  	tRisseDeclAttribute(
											  	tRisseDeclAttribute::ccStatic)); }
	| property_expr_def_inner				{ $$ = $1; }
;

property_expr_def_inner
	: "property" onl
	  "{"
	  property_handler_def_list
	  "}"									{ $$ = $4;  $$->SetPosition(@1.first); }
;

property_handler_def_list
	: onl property_handler_setter onl		{ $$ = $2; }
	| onl property_handler_getter onl		{ $$ = $2; }
	| onl property_handler_setter onl
	  property_handler_getter onl			{ $$ = $2; C(PropDecl, $$)->
											  SetGetter(C(PropDecl, $4)->GetGetter()); }
	| onl property_handler_getter onl 
	  property_handler_setter onl			{ $$ = $2;
											  C(PropDecl, $$)->
												SetSetter(C(PropDecl, $4)->GetSetter());
											  C(PropDecl, $$)->
												SetSetterArgumentName(C(PropDecl, $4)->
															GetSetterArgumentName()); }
;

property_handler_setter
	: "setter" onl "(" onl T_ID onl ")"
	  block									{ $$ = N(PropDecl)(@1.first);
											  C(PropDecl, $$)->SetSetter($8);
											  C(PropDecl, $$)->SetSetterArgumentName(*$5); }
;

property_handler_getter
	: property_getter_handler_head
	  block									{ $$ = N(PropDecl)(@1.first);
											  C(PropDecl, $$)->SetGetter($2); }
;

property_getter_handler_head
	: "getter" "(" onl ")"
	| "getter"
;

/*---------------------------------------------------------------------------
  class/module definition
  ---------------------------------------------------------------------------*/

/* a class or module definition */
class_module_def
	: "class" onl decl_name_expr
	  class_extender
	  "{" onl toplevel_def_list "}"	onl	 	{ $$ = $4;  $$->SetPosition(@1.first);
											  C(ClassDecl, $$)->SetBody($7);
											  C(ClassDecl, $$)->SetName($3); }
	| "module" onl decl_name_expr onl
	  "{" onl toplevel_def_list "}"	onl		{ $$ = N(ClassDecl)(@1.first, true, NULL);
											  C(ClassDecl, $$)->SetBody($7);
											  C(ClassDecl, $$)->SetName($3); }
;

class_module_expr_def
	: "class"
	  class_extender
	  "{" onl toplevel_def_list "}"			{ $$ = $2;  $$->SetPosition(@1.first);
											  C(ClassDecl, $$)->SetBody($5); }
	| "module" onl
	  "{" onl toplevel_def_list "}"			{ $$ = N(ClassDecl)(@1.first, true, NULL);
											  C(ClassDecl, $$)->SetBody($5); }
;

class_extender
	: onl 									{ $$ = N(ClassDecl)(@1.first, false, NULL); }
	| onl "extends" onl expr onl 			{ $$ = N(ClassDecl)(@2.first, false, $4); }
	| onl "<" onl expr onl /* syntax sugar, Ruby like */	{ $$ = N(ClassDecl)(@2.first, false, $4); }
	| onl ":" onl expr onl /* syntax sugar, C++ like */	{ $$ = N(ClassDecl)(@2.first, false, $4); }
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
	| decl_attr_list decl_attr				{ $$ = RisseOverwriteDeclAttribute(PR, @1.first, $1, $2); }
;

decl_attr
	: decl_attr_access | decl_attr_visibility | decl_attr_override
;

/* attribute specifiers */

decl_attr_access
	: "public"	onl 						{ $$ = new tRisseDeclAttribute(tRisseDeclAttribute::acPublic); }
	| "internal"onl 						{ $$ = new tRisseDeclAttribute(tRisseDeclAttribute::acInternal); }
	| "private"	onl 						{ $$ = new tRisseDeclAttribute(tRisseDeclAttribute::acPrivate); }
;

decl_attr_visibility
	: "enumerable"	onl 					{ $$ = new tRisseDeclAttribute(tRisseDeclAttribute::vcEnumerable); }
	| "hidden"		onl 					{ $$ = new tRisseDeclAttribute(tRisseDeclAttribute::vcHidden); }
;

decl_attr_override
	: "const"		onl 					{ $$ = new tRisseDeclAttribute(tRisseMemberAttribute::ocConst); }
	| "virtual"		onl 					{ $$ = new tRisseDeclAttribute(tRisseMemberAttribute::ocVirtual); }
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
	: expr_with_comma "if" onl expr_with_comma	{ $$ = N(Binary)(@2.first, abtIf			,$4, $1);/*順番に注意*/ }
	| expr_with_comma ","  onl expr_with_comma	{ $$ = N(Binary)(@2.first, abtComma		,$1, $4); }
	| expr
;

/*
	expr は、関数への引数リストなど、式中にカンマがあると、リストの
	区切り記号と区別が付かない場合はこちらを使う
*/
expr
	: expr "=" onl expr				{ $$ = N(Binary)(@2.first, abtAssign			,$1, $4); }
	| expr "&=" onl expr			{ $$ = N(Binary)(@2.first, abtBitAndAssign	,$1, $4); }
	| expr "|=" onl expr			{ $$ = N(Binary)(@2.first, abtBitOrAssign		,$1, $4); }
	| expr "^=" onl expr			{ $$ = N(Binary)(@2.first, abtBitXorAssign	,$1, $4); }
	| expr "-=" onl expr			{ $$ = N(Binary)(@2.first, abtSubAssign		,$1, $4); }
	| expr "+=" onl expr			{ $$ = N(Binary)(@2.first, abtAddAssign		,$1, $4); }
	| expr "%=" onl expr			{ $$ = N(Binary)(@2.first, abtModAssign		,$1, $4); }
	| expr "/=" onl expr			{ $$ = N(Binary)(@2.first, abtDivAssign		,$1, $4); }
	| expr "\\=" onl expr			{ $$ = N(Binary)(@2.first, abtIdivAssign		,$1, $4); }
	| expr "*=" onl expr			{ $$ = N(Binary)(@2.first, abtMulAssign		,$1, $4); }
	| expr "||=" onl expr			{ $$ = N(Binary)(@2.first, abtLogOrAssign		,$1, $4); }
	| expr "&&=" onl expr			{ $$ = N(Binary)(@2.first, abtLogAndAssign	,$1, $4); }
	| expr ">>=" onl expr			{ $$ = N(Binary)(@2.first, abtRShiftAssign	,$1, $4); }
	| expr "<<=" onl expr			{ $$ = N(Binary)(@2.first, abtLShiftAssign	,$1, $4); }
	| expr ">>>=" onl expr			{ $$ = N(Binary)(@2.first, abtRBitShiftAssign	,$1, $4); }
	| expr "<->" onl expr			{ $$ = N(Binary)(@2.first, abtSwap			,$1, $4); }
	| expr "?" onl expr onl ":" onl expr		{ $$ = N(Trinary)(@2.first, attCondition 		,$1, $4, $8); }
	| expr "||" onl expr			{ $$ = N(Binary)(@2.first, abtLogOr			,$1, $4); }
	| expr "&&" onl expr			{ $$ = N(Binary)(@2.first, abtLogAnd			,$1, $4); }
	| expr "|" onl expr				{ $$ = N(Binary)(@2.first, abtBitOr			,$1, $4); }
	| expr "^" onl expr				{ $$ = N(Binary)(@2.first, abtBitXor			,$1, $4); }
	| expr "&" onl expr				{ $$ = N(Binary)(@2.first, abtBitAnd			,$1, $4); }
	| expr "!=" onl expr			{ $$ = N(Binary)(@2.first, abtNotEqual		,$1, $4); }
	| expr "==" onl expr			{ $$ = N(Binary)(@2.first, abtEqual			,$1, $4); }
	| expr "!==" onl expr			{ $$ = N(Binary)(@2.first, abtDiscNotEqual	,$1, $4); }
	| expr "===" onl expr			{ $$ = N(Binary)(@2.first, abtDiscEqual		,$1, $4); }
	| expr "<" onl expr				{ $$ = N(Binary)(@2.first, abtLesser			,$1, $4); }
	| expr ">" onl expr				{ $$ = N(Binary)(@2.first, abtGreater			,$1, $4); }
	| expr "<=" onl expr			{ $$ = N(Binary)(@2.first, abtLesserOrEqual	,$1, $4); }
	| expr ">=" onl expr			{ $$ = N(Binary)(@2.first, abtGreaterOrEqual	,$1, $4); }
	| expr "instanceof" onl expr	{ $$ = N(Binary)(@2.first, abtInstanceOf		,$1, $4); }
	| expr ">>" onl expr			{ $$ = N(Binary)(@2.first, abtLShift			,$1, $4); }
	| expr "<<" onl expr			{ $$ = N(Binary)(@2.first, abtRShift			,$1, $4); }
	| expr ">>>" onl expr			{ $$ = N(Binary)(@2.first, abtRBitShift		,$1, $4); }
	| expr "+" onl expr				{ $$ = N(Binary)(@2.first, abtAdd				,$1, $4); }
	| expr "-" onl expr				{ $$ = N(Binary)(@2.first, abtSub				,$1, $4); }
	| expr "%" onl expr				{ $$ = N(Binary)(@2.first, abtMod				,$1, $4); }
	| expr "/" onl expr				{ $$ = N(Binary)(@2.first, abtDiv				,$1, $4); }
	| expr "\\" onl expr			{ $$ = N(Binary)(@2.first, abtIdiv			,$1, $4); }
	| expr "*" onl expr				{ $$ = N(Binary)(@2.first, abtMul				,$1, $4); }
	| "!" onl expr					{ $$ = N(Unary)(@2.first, autLogNot			,$3); }
	| "~" onl expr					{ $$ = N(Unary)(@2.first, autBitNot			,$3); }
	| "--" onl expr					{ $$ = N(Unary)(@2.first, autPreDec			,$3); }
	| "++" onl expr					{ $$ = N(Unary)(@2.first, autPreInc			,$3); }
	| "new" onl expr				{ $$ = $3;
									  /* new の子ノードは必ず関数呼び出し式である必要がある */
									  if($$->GetType() != antFuncCall)
									  { yyerror(&@$, PR, "expected func_call_expr after new");
											    YYERROR; }
									  C(FuncCall, $$)->SetCreateNew(); }
	| "delete" onl expr				{ $$ = N(Unary)(@1.first, autDelete			,$3); }
	| "typeof" onl expr				{ ; }
	| "+" onl expr %prec T_UNARY	{ $$ = N(Unary)(@1.first, autPlus				,$3); }
	| "-" onl expr %prec T_UNARY	{ $$ = N(Unary)(@1.first, autMinus			,$3); }
	| expr "incontextof" onl expr		{ $$ = N(InContextOf)(@2.first,  $1, $4  ); }
	| expr "incontextof" onl "dynamic"	{ $$ = N(InContextOf)(@2.first,  $1, NULL); }
	| expr "--" %prec T_POSTUNARY	{ $$ = N(Unary)(@2.first, autPostDec			,$1); }
	| expr "++" %prec T_POSTUNARY	{ $$ = N(Unary)(@2.first, autPostInc			,$1); }
	| access_expr
;

access_expr
	: access_expr "[" onl expr onl "]"
									{ $$ = N(MemberSel)(@2.first, $1, $4, matIndirect); }
	| access_expr "." onl member_name
									{ $$ = N(MemberSel)(@2.first, $1, N(Factor)(@4.first, aftConstant, *$4), matDirect); }
	| access_expr "." onl "(" onl expr onl ")"
									{ $$ = N(MemberSel)(@2.first, $1, $6, matDirect); }
	| access_expr "::" onl member_name
									{ $$ = N(MemberSel)(@2.first, $1, N(Factor)(@4.first, aftConstant, *$4), matDirectThis); }
	| access_expr "::" onl "(" onl expr onl ")"
									{ $$ = N(MemberSel)(@2.first, $1, $6, matDirectThis); }
	| "(" onl expr_with_comma onl ")"
									{ $$ = $3; }
	| func_call_expr
	| func_expr_def
	| property_expr_def
	| class_module_expr_def
	| factor
;


factor
	: T_ID							{ $$ = N(Id)(@1.first, *$1, false); }
	| "@" T_ID						{ $$ = N(Id)(@2.first, *$2, true);  }
	| inline_array
	| inline_dic
	| "/="							{ LX->SetNextIsRegularExpression(); }
	  regexp						{ $$ = $3; }
	| "/"							{ LX->SetNextIsRegularExpression(); }
	  regexp						{ $$ = $3; }
	| "this"						{ $$ = N(Factor)(@1.first, aftThis);  }
	| "super"						{ $$ = N(Factor)(@1.first, aftSuper);  }
	| T_CONSTVAL					{ $$ = N(Factor)(@1.first, aftConstant, *$1); }
	| "global"						{ $$ = N(Factor)(@1.first, aftGlobal); }
	| embeddable_string
;


/*
  function XXX () { } や class XXXX {  } の XXXX の部分に相当する場所に
  記述する式。  ここの記述はある程度上記の記述と重複する。
*/
decl_name_expr
	: decl_name_expr "[" onl expr onl "]"
									{ $$ = N(MemberSel)(@2.first, $1, $4, matIndirect); }
	| decl_name_expr "." onl member_name
									{ $$ = N(MemberSel)(@2.first, $1, N(Factor)(@4.first, aftConstant, *$4), matDirect); }
	| decl_name_expr "." onl "(" onl expr onl ")"
									{ $$ = N(MemberSel)(@2.first, $1, $6, matDirect); }
	| decl_name_expr "::" onl member_name
									{ $$ = N(MemberSel)(@2.first, $1, N(Factor)(@4.first, aftConstant, *$4), matDirectThis); }
	| decl_name_expr "::" onl "(" onl expr onl ")"
									{ $$ = N(MemberSel)(@2.first, $1, $6, matDirectThis); }
	| factor						{ $$ = $1; }
;


/* an expression for function call */
func_call_expr
	: func_call_expr_body				{ LX->SetFuncCallReduced(); }
	  call_block_list_opt				{ $$ = $1; C(FuncCall, $$)->AssignBlocks($3); }
;

func_call_expr_body
	: access_expr "(" call_arg_list ")"
						/*%dprec 1*/	{ $$ = $3; C(FuncCall, $$)->SetExpression($1); }
	| access_expr "(" onl "..." onl ")"	{ $$ = N(FuncCall)(@2.first, true);  C(FuncCall, $$)->SetExpression($1); }
	| access_expr "(" onl ")"
						/*%dprec 2*/	{ $$ = N(FuncCall)(@2.first, false); C(FuncCall, $$)->SetExpression($1); }
		/* このルールは "(" と ")" の間で下記の 「call_arg が empty」 のルールと
		  シフト・還元競合を起こす(こちらが優先される) */
;

/* argument(s) for function call */
call_arg_list
	: call_arg 							{ $$ = N(FuncCall)(@1.first, false); C(FuncCall, $$)->AddChild($1); }
	| call_arg_list "," call_arg		{ C(FuncCall, $1)->AddChild($3); }
;

call_arg
	: onl								{ $$ = NULL; }
	| onl "*" onl						{ $$ = N(FuncCallArg)(@2.first, NULL, true); }
	| onl expr "*" onl					{ $$ = N(FuncCallArg)(@2.first, $2, true); }
	| onl expr onl						{ $$ = N(FuncCallArg)(@2.first, $2, false); }
;

/* block argument(s) for function call */
/* このルールは 2つのシフト・還元競合を起こす */
call_block_list_opt
	: 									{ $$ = NULL; }
	| call_block_list					{ $$ = $1; }
;

call_block_list
	: call_block						{ $$ = new tRisseASTArray(); $$->push_back($1); }
	| call_block_list  call_block		{ $$ = $1; $$->push_back($2); }
;

call_block
	: "{" call_block_arg_opt
	  def_list "}"						{ $$ = $2;
										  C(FuncDecl, $$)->SetBody($3);
										  C(FuncDecl, $$)->SetIsBlock(true);
										  C(Context, $3)->SetEndPosition(@$.last); }
	| func_expr_def						{ $$ = $1; }
;

call_block_arg_opt
	: onl								{ $$ = N(FuncDecl)(@1.first); }
	| onl "|" func_decl_arg_at_least_one
	  "|" onl							{ $$ = $3; }
;

/* regular expression */
regexp
	: T_REGEXP T_REGEXP_FLAGS			{ $$ = N(RegExp)(@1.first, *$1, *$2); }
;

/*---------------------------------------------------------------------------
  dictionary / array expression
  ---------------------------------------------------------------------------*/

/* an inline array object */
inline_array
	: "["
	  array_elm_list
	  "]"								{ $$ = $3; C(Array, $$)->Strip(); }
;

/* an inline array's element list */
array_elm_list
	: array_elm							{ $$ = N(Array)(@1.first); if($1) C(Array, $$)->AddChild($1); }
	| array_elm_list "," array_elm		{ $$ = $1; C(Array, $$)->AddChild($3); }
;

/* an inline array's element */
array_elm
	: onl								{ $$ = NULL; }
	| onl expr onl						{ $$ = $2; }
;

/* an inline dictionary */
inline_dic
	: "%" "["
	  dic_elm_list
	  dummy_elm_opt
	  "]"								{ $$ = $2; }
;

/* an inline dictionary's element list */
dic_elm_list
    : onl								{ $$ = N(Dict)(@1.first); }
	| dic_elm 							{ $$ = N(Dict)(@1.first); C(Dict, $$)->AddChild($1); }
	| dic_elm_list "," dic_elm			{ $$ = $1; C(Dict, $$)->AddChild($3); }
;

/* an inline dictionary's element */
dic_elm
	: onl expr onl "=>" onl expr onl	{ $$ = N(DictPair)(@2.first, $2, $6); }
	| onl T_ID  ":"  onl expr onl		{ $$ = N(DictPair)(@2.first,
										  N(Factor)(@2.first, aftConstant, *$2), $5); }
	/* 注意   T_ID の後の改行は許されない */
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
	: embeddable_string_d T_CONSTVAL { $$ = RisseAddExprConstStr(@1.first, $1, *$2); }
	| embeddable_string_s T_CONSTVAL { $$ = RisseAddExprConstStr(@1.first, $1, *$2); }
;

/* 埋め込み可能な文字列リテラル(ダブルクオーテーション) */
embeddable_string_d
	: embeddable_string_d_unit
	| embeddable_string_d embeddable_string_d_unit	{ $$ = N(Binary)(@1.first, abtAdd, $1, $2); }
;

embeddable_string_d_unit
	: T_EMSTRING_AMPERSAND_D expr_with_comma ";" { $$ = RisseAddExprConstStr(@1.first, *$1, $2);
													LX->SetContinueEmbeddableString(RISSE_WC('"')); }
	| T_EMSTRING_DOLLAR_D    expr_with_comma "}" { $$ = RisseAddExprConstStr(@1.first, *$1, $2);
													LX->SetContinueEmbeddableString(RISSE_WC('"')); }
;

/* 埋め込み可能な文字列リテラル(シングルクオーテーション) */
embeddable_string_s
	: embeddable_string_s_unit
	| embeddable_string_s embeddable_string_s_unit	{ $$ = N(Binary)(@1.first, abtAdd, $1, $2); }
;

embeddable_string_s_unit
	: T_EMSTRING_AMPERSAND_S expr_with_comma ";" { $$ = RisseAddExprConstStr(@1.first, *$1, $2);
													LX->SetContinueEmbeddableString(RISSE_WC('\'')); }
	| T_EMSTRING_DOLLAR_S    expr_with_comma "}" { $$ = RisseAddExprConstStr(@1.first, *$1, $2);
													LX->SetContinueEmbeddableString(RISSE_WC('\'')); }
;

/* new line */
nl		: "\n" ;

/* optional new line */
onl		: /* empty */ | nl ;

/* semicolon or new line */
snl		: ";" |  nl | ";" nl ;


/*###########################################################################*/

%%

//---------------------------------------------------------------------------
int yylex(YYSTYPE * value, YYLTYPE * llocp, void *pr)
{
	value->value = new tRisseVariant();
	int token = PR->GetToken(*(value->value));
	llocp->first = LX->GetLastTokenStart();
	llocp->last = LX->GetPosition();
//	fprintf(stderr, "token : %d\n", token);
	return token;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int raise_yyerror(const char * msg, void *pr, YYLTYPE *loc)
{
	tRisseCompileExceptionClass::Throw(tRisseString(msg), PR->GetScriptBlock(), loc->first);
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





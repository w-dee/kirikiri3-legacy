//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief VM コードの逆アセンブラ
//---------------------------------------------------------------------------
#include "risseCommHead.h"

#include "risseInterCodeGen.h"
#include "risseScriptBlock.h"
#include "risseUtils.h"

//---------------------------------------------------------------------------
namespace Risse  // following is in the namespace
{
RISSE_DEFINE_SOURCE_ID(9417,63301,50714,18296,47522,26784,35105,31076);
//---------------------------------------------------------------------------
tRisseString tRisseInterCodeContext::GetValueComment(const tRisseVariant &val)
{
	// make val a human readable string and return it
	return RisseVariantToReadableString(val);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::Disassemble(
	void (*output_func)(const risse_char *msg, const risse_char *comment, risse_int addr,
	const risse_int32 *codestart, risse_int size, void *data),
	void (*output_func_src)(const risse_char *msg, const risse_char *name, risse_int line,
	void *data), void *data, risse_int start, risse_int end)
{
#if 0
	// dis-assemble the intermediate code.
	// "output_func" points a line output function.

	tRisseVariantString * s;

	tRisseString msg;
	tRisseString com;

	risse_int prevline = -1;
	risse_int curline = -1;

	if(end <= 0) end = CodeAreaSize;
	if(end > CodeAreaSize) end = CodeAreaSize;

	for(risse_int i = start; i < end; )
	{
		msg.Clear();
		com.Clear();
		risse_int size;
		risse_int srcpos = CodePosToSrcPos(i);
		risse_int line = Block->SrcPosToLine(srcpos);

		// output source lines as comment
		if(curline == -1 || curline <= line)
		{
			if(curline == -1) curline = line;
			risse_int nl = line - curline;
			while(curline <= line)
			{
				if(nl<3 || nl >= 3 && line-curline <= 2)
				{
					risse_int len;
					risse_char *src = Block->GetLine(curline, &len);
					risse_char * buf = new risse_char[len + 1];
					Risse_strcpy_maxlen(buf, src, len);
					try
					{
						output_func_src(buf, RISSE_WS(""), curline, data);
					}
					catch(...)
					{
						delete [] buf;
						throw;
					}
					delete [] buf;
					curline++;
				}
				else
				{
					curline = line - 2;
				}
			}
		}
		else if(prevline != line)
		{
			risse_int len;
			risse_char *src = Block->GetLine(line, &len);
			risse_char * buf = new risse_char[len + 1];
			Risse_strcpy_maxlen(buf, src, len);
			try
			{
				output_func_src((const risse_char*)buf, RISSE_WS(""), line, data);
			}
			catch(...)
			{
				delete [] buf;
				throw;
			}
			delete [] buf;
		}

		prevline = line;

		// decode each instructions
		switch(CodeArea[i])
		{
		case VM_NOP:
			msg.printf(RISSE_WS("nop"));
			size = 1;
			break;

		case VM_NF:
			msg.printf(RISSE_WS("nf"));
			size = 1;
			break;

		case VM_CONST:
			msg.printf(RISSE_WS("const %%%d, *%d"),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]));
			if(DataArea)
			{
				com.printf(RISSE_WS("*%d = %ls"), RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]),
					GetValueComment(RISSE_GET_VM_REG(DataArea, CodeArea[i+2])).c_str());
			}
			size = 3;
			break;


#define OP2_DISASM(c, x) \
	case c: \
		msg.printf(RISSE_W(x " %%%d, %%%d"), RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]), \
										RISSE_FROM_VM_REG_ADDR(CodeArea[i+2])); \
		size = 3; \
		break
		// instructions that
		// 1. have two operands that represent registers.
		// 2. do not have property access variants.
		OP2_DISASM(VM_CP,		"cp");
		OP2_DISASM(VM_CEQ,		"ceq");
		OP2_DISASM(VM_CDEQ,		"cdeq");
		OP2_DISASM(VM_CLT,		"clt");
		OP2_DISASM(VM_CGT,		"cgt");
		OP2_DISASM(VM_CHKINS,	"chkins");
#undef OP2_DISASM


#define OP2_DISASM(c, x) \
	case c: \
		msg.printf(RISSE_W(x " %%%d, %%%d"), RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]), \
									RISSE_FROM_VM_REG_ADDR(CodeArea[i+2])); \
		size = 3; \
		break; \
	case c+1: \
		msg.printf(RISSE_W(x "pd" " %%%d, %%%d.*%d, %%%d"), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+3]), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+4])); \
		if(DataArea) \
		{ \
			com.printf(RISSE_W("*%d = %ls"), RISSE_FROM_VM_REG_ADDR(CodeArea[i+3]), \
				GetValueComment(RISSE_GET_VM_REG(DataArea, CodeArea[i+3])).c_str()); \
		} \
		size = 5; \
		break; \
	case c+2: \
		msg.printf(RISSE_W(x "pi" " %%%d, %%%d.%%%d, %%%d"), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+3]), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+4])); \
		size = 5; \
		break; \
	case c+3: \
		msg.printf(RISSE_W(x "p" " %%%d, %%%d, %%%d"), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+3])); \
		size = 4; \
		break
		// instructions that
		// 1. have two operands that represent registers.
		// 2. have property access variants
		OP2_DISASM(VM_LOR,		"lor");
		OP2_DISASM(VM_LAND,		"land");
		OP2_DISASM(VM_BOR,		"bor");
		OP2_DISASM(VM_BXOR,		"bxor");
		OP2_DISASM(VM_BAND,		"band");
		OP2_DISASM(VM_SAR,		"sar");
		OP2_DISASM(VM_SAL,		"sal");
		OP2_DISASM(VM_SR,		"sr");
		OP2_DISASM(VM_ADD,		"add");
		OP2_DISASM(VM_SUB,		"sub");
		OP2_DISASM(VM_MOD,		"mod");
		OP2_DISASM(VM_DIV,		"div");
		OP2_DISASM(VM_IDIV,		"idiv");
		OP2_DISASM(VM_MUL,		"mul");
#undef OP2_DISASM

#define OP1_DISASM(x) \
	msg.printf(RISSE_W(x " %%%d"), RISSE_FROM_VM_REG_ADDR(CodeArea[i+1])); \
	size = 2
		// instructions that have one operand which represent a register,
		// except for inc, dec
		case VM_TT:			OP1_DISASM("tt");		break;
		case VM_TF:			OP1_DISASM("tf");		break;
		case VM_SETF:		OP1_DISASM("setf");		break;
		case VM_SETNF:		OP1_DISASM("setnf");	break;
		case VM_LNOT:		OP1_DISASM("lnot");		break;
		case VM_BNOT:		OP1_DISASM("bnot");		break;
		case VM_ASC:		OP1_DISASM("asc");		break;
		case VM_CHR:		OP1_DISASM("chr");		break;
		case VM_NUM:		OP1_DISASM("num");		break;
		case VM_CHS:		OP1_DISASM("chs");		break;
		case VM_CL:			OP1_DISASM("cl");		break;
		case VM_INV:		OP1_DISASM("inv");		break;
		case VM_CHKINV:		OP1_DISASM("chkinv");	break;
		case VM_TYPEOF:		OP1_DISASM("typeof");	break;
		case VM_EVAL:		OP1_DISASM("eval");		break;
		case VM_EEXP:		OP1_DISASM("eexp");		break;
		case VM_INT:		OP1_DISASM("int");		break;
		case VM_REAL:		OP1_DISASM("real");		break;
		case VM_STR:		OP1_DISASM("str");		break;
		case VM_OCTET:		OP1_DISASM("octet");	break;
#undef OP1_DISASM

		case VM_CCL:
			msg.printf(RISSE_W("ccl %%%d-%%%d"), RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]) + CodeArea[i+2] -1);
			size = 3;
			break;

#define OP1_DISASM(c, x) \
	case c: \
		msg.printf(RISSE_W(x " %%%d"), RISSE_FROM_VM_REG_ADDR(CodeArea[i+1])); \
		size = 2; \
		break; \
	case c+1: \
		msg.printf(RISSE_W(x "pd" " %%%d, %%%d.*%d"), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+3])); \
		if(DataArea) \
		{ \
			com.printf(RISSE_W("*%d = %ls"), \
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+3]), \
				GetValueComment(RISSE_GET_VM_REG(DataArea, CodeArea[i+3])).c_str()); \
		} \
		size = 4; \
		break; \
	case c+2: \
		msg.printf(RISSE_W(x "pi" " %%%d, %%%d.%%%d"), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+3])); \
		size = 4; \
		break; \
	case c+3: \
		msg.printf(RISSE_W(x "p" " %%%d, %%%d"), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]), \
			RISSE_FROM_VM_REG_ADDR(CodeArea[i+2])); \
		size = 3; \
		break

		// inc and dec
		OP1_DISASM(VM_INC,	"inc");
		OP1_DISASM(VM_DEC,	"dec");
#undef OP1_DISASM



#define OP1A_DISASM(x) \
	msg.printf(RISSE_W(x " %09d"), RISSE_FROM_VM_CODE_ADDR(CodeArea[i+1]) + i); \
	size = 2
		// instructions that have one operand which represents code area
		case VM_JF:		OP1A_DISASM("jf");		break;
		case VM_JNF:	OP1A_DISASM("jnf");		break;
		case VM_JMP:	OP1A_DISASM("jmp");		break;
#undef OP1A_DISASM

		case VM_CALL:
		case VM_CALLD:
		case VM_CALLI:
		case VM_NEW:
		  {
			// function call variants

			msg.printf(
				CodeArea[i] == VM_CALL  ?RISSE_W("call %%%d, %%%d("):
				CodeArea[i] == VM_CALLD ?RISSE_W("calld %%%d, %%%d.*%d("):
				CodeArea[i] == VM_CALLI ?RISSE_W("calli %%%d, %%%d.%%%d("):
										 RISSE_W("new %%%d, %%%d("),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+3]));
			risse_int st; // start of arguments
			if(CodeArea[i] == VM_CALLD || CodeArea[i] == VM_CALLI)
				st = 5;
			else
				st = 4;
			risse_int num = CodeArea[i+st-1];     // st-1 = argument count
			bool first = true;
			risse_char buf[256];
			risse_int c = 0;
			if(num == -1)
			{
				// omit arg
				size = st;
				msg += RISSE_W("...");
			}
			else if(num == -2)
			{
				// expand arg
				st ++;
				num = CodeArea[i+st-1];
				size = st + num * 2;
				for(risse_int j = 0; j < num; j++)
				{
					if(!first) msg += RISSE_W(", ");
					first = false;
					switch(CodeArea[i+st+j*2])
					{
					case fatNormal:
						Risse_sprintf(buf, RISSE_W("%%%d"),
							RISSE_FROM_VM_REG_ADDR(CodeArea[i+st+j*2+1]));
						break;
					case fatExpand:
						Risse_sprintf(buf, RISSE_W("%%%d*"),
							RISSE_FROM_VM_REG_ADDR(CodeArea[i+st+j*2+1]));
						break;
					case fatUnnamedExpand:
						Risse_strcpy(buf, RISSE_W("*"));
						break;
					}
					msg += buf;
				}
			}
			else
			{
				// normal operation
				size = st + num;
				while(num--)
				{
					if(!first) msg += RISSE_W(", ");
					first = false;
					Risse_sprintf(buf, RISSE_W("%%%d"),
						RISSE_FROM_VM_REG_ADDR(CodeArea[i+c+st]));
					c++;
					msg += buf;
				}
			}

			msg += RISSE_W(")");
			if(DataArea && CodeArea[i] == VM_CALLD)
			{
				com.printf(RISSE_W("*%d = %ls"), RISSE_FROM_VM_REG_ADDR(CodeArea[i+3]),
					GetValueComment(RISSE_GET_VM_REG(DataArea, CodeArea[i+3])).c_str());
			}

			break;
		  }

		case VM_GPD:
		case VM_GPDS:
			// property get direct
			msg.printf(
				CodeArea[i] == VM_GPD?RISSE_W("gpd %%%d, %%%d.*%d"):
									  RISSE_W("gpds %%%d, %%%d.*%d"),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+3]));
			if(DataArea)
			{
				com.printf(RISSE_W("*%d = %ls"), RISSE_FROM_VM_REG_ADDR(CodeArea[i+3]),
					GetValueComment(RISSE_GET_VM_REG(DataArea, CodeArea[i+3])).c_str());
			}
			size = 4;
			break;


		case VM_SPD:
		case VM_SPDE:
		case VM_SPDEH:
		case VM_SPDS:
			// property set direct
			msg.printf(
				CodeArea[i] == VM_SPD ? RISSE_W("spd %%%d.*%d, %%%d"):
				CodeArea[i] == VM_SPDE? RISSE_W("spde %%%d.*%d, %%%d"):
				CodeArea[i] == VM_SPDEH?RISSE_W("spdeh %%%d.*%d, %%%d"):
										RISSE_W("spds %%%d.*%d, %%%d"),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+3]));
			if(DataArea)
			{
				com.printf(RISSE_W("*%d = %ls"), RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]),
					GetValueComment(RISSE_GET_VM_REG(DataArea, CodeArea[i+2])).c_str());
			}

			size = 4;
			break;


		case VM_GPI:
		case VM_GPIS:
			// property get indirect
			msg.printf(
				CodeArea[i] == VM_GPI ?  RISSE_W("gpi %%%d, %%%d.%%%d"):
										 RISSE_W("gpis %%%d, %%%d.%%%d"),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+3]));
			size = 4;
			break;


		case VM_SPI:
		case VM_SPIE:
		case VM_SPIS:
			// property set indirect
			msg.printf(
				CodeArea[i] == VM_SPI  ?RISSE_W("spi %%%d.%%%d, %%%d"):
				CodeArea[i] == VM_SPIE ?RISSE_W("spie %%%d.%%%d, %%%d"):
										RISSE_W("spis %%%d.%%%d, %%%d"),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+3]));
			size = 4;
			break;


		case VM_SETP:
			// property set
			msg.printf(
				RISSE_W("setp %%%d, %%%d"),
					RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]),
					RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]));
			size = 3;
			break;

		case VM_GETP:
			// property get
			msg.printf(
				RISSE_W("getp %%%d, %%%d"),
					RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]),
					RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]));
			size = 3;
			break;


		case VM_DELD:
		case VM_TYPEOFD:
			// member delete direct / typeof direct
			msg.printf(
				CodeArea[i] == VM_DELD   ?RISSE_W("deld %%%d, %%%d.*%d"):
										  RISSE_W("typeofd %%%d, %%%d.*%d"),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+3]));
			if(DataArea)
			{
				com.printf(RISSE_W("*%d = %ls"), RISSE_FROM_VM_REG_ADDR(CodeArea[i+3]),
					GetValueComment(RISSE_GET_VM_REG(DataArea, CodeArea[i+3])).c_str());
			}
			size = 4;
			break;

		case VM_DELI:
		case VM_TYPEOFI:
			// member delete indirect / typeof indirect
			msg.printf(
				CodeArea[i] == VM_DELI   ?RISSE_W("deli %%%d, %%%d.%%%d"):
										  RISSE_W("typeofi %%%d, %%%d.%%%d"),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+3]));
			size = 4;
			break;

		case VM_SRV:
			// set return value
			msg.printf(RISSE_W("srv %%%d"), RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]));
			size = 2;
			break;

		case VM_RET:
			// return
			msg.printf(RISSE_W("ret"));
			size = 1;
			break;

		case VM_ENTRY:
			// enter try-protected block
			msg.printf(RISSE_W("entry %09d, %%%d"),
				RISSE_FROM_VM_CODE_ADDR(CodeArea[i+1]) + i,
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]));
			size = 3;
			break;


		case VM_EXTRY:
			// exit from try-protected block
			msg.printf(RISSE_W("extry"));
			size = 1;
			break;

		case VM_THROW:
			msg.printf(RISSE_W("throw %%%d"), RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]));
			size = 2;
			break;

		case VM_CHGTHIS:
			msg.printf(RISSE_W("chgthis %%%d, %%%d"),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]));
			size = 3;
			break;

		case VM_GLOBAL:
			msg.printf(RISSE_W("global %%%d"),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]));
			size = 2;
			break;

		case VM_ADDCI:
			msg.printf(RISSE_W("addci %%%d, %%%d"),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+1]),
				RISSE_FROM_VM_REG_ADDR(CodeArea[i+2]));
			size = 3;
			break;

		case VM_REGMEMBER:
			msg.printf(RISSE_W("regmember"));
			size = 1;
			break;

		case VM_DEBUGGER:
			msg.printf(RISSE_W("debugger"));
			size = 1;
			break;

		default:
			msg.printf(RISSE_W("unknown instruction %d"), CodeArea[i]);
			size = 1;
			break;
		} /* switch */

		output_func(msg.c_str(), com.c_str(), i,
			CodeArea + i, size, data);  // call the callback

		i+=size;
	}
#endif
}
//---------------------------------------------------------------------------
struct of_data
{
	void (*func)(const risse_char*, void *);
	void *funcdata;
};

void tRisseInterCodeContext::_output_func(const risse_char *msg,
	const risse_char *comment, risse_int addr, const risse_int32 *codestart,
		risse_int size, void *data)

{
#if 0
	risse_char *buf = new risse_char[Risse_strlen(msg) + Risse_strlen(comment) + 20];

	Risse_sprintf(buf, RISSE_W("%08d %ls"), addr, msg);
	if(comment[0])
	{
		Risse_strcat(buf, RISSE_W("\t// "));
		Risse_strcat(buf, comment);
	}

	try
	{
		of_data *dat = (of_data *)(data);
		dat->func(buf, dat->funcdata);
	}
	catch(...)
	{
		delete [] buf;
		throw;
	}

	delete [] buf;
#endif
}
void tRisseInterCodeContext::_output_func_src(const risse_char *msg,
	const risse_char *name, risse_int line, void *data)
{
#if 0
	risse_char *buf = new risse_char[Risse_strlen(msg) + Risse_strlen(name) + 20];
	if(line >= 0)
		Risse_sprintf(buf, RISSE_W("#%ls(%d) %ls"), name, line+1, msg);
	else
		Risse_sprintf(buf, RISSE_W("#%ls %ls"), name, msg);
	try
	{
		of_data *dat = (of_data *)(data);
		dat->func(buf, dat->funcdata);
	}
	catch(...)
	{
		delete [] buf;
		throw;
	}

	delete [] buf;
#endif
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::Disassemble(
	void (*output_func)(const risse_char *msg,void *data), void *data, risse_int start,
		risse_int end)

{
	// dis-assemble
	of_data dat;
	dat.func =  output_func;
	dat.funcdata = data;
	Disassemble(_output_func, _output_func_src, (void*)&dat, start, end);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::Disassemble(risse_int start, risse_int end)
{
	Disassemble(tRisseScriptBlock::GetConsoleOutput(), Block, start, end);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::DisassenbleSrcLine(risse_int codepos)
{
	risse_int start = FindSrcLineStartCodePos(codepos);
	Disassemble(start, codepos + 1);
}
//---------------------------------------------------------------------------
} // namespace Risse



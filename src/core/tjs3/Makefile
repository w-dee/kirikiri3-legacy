LIB=tjs3.a

MINGWROOT = C:/MinGW
CXX = g++
AR = ar
WINDRES = windres
RM = rm -f
MV = mv
XGETTEXT = xgettext
MSGMERGE = msgmerge
PERL = perl
SED = sed
FIND = find
SH = sh

DEBUG=yes

ifeq ($(DEBUG),yes)
DEBUG_LIB_SUFFIX=d
DEBUG_CFLAGS=-O0 -D__WXDEBUG__ -g
WX_CONFIG_INCLUDE_NAME=debug
else
DEBUG_LIB_SUFFIX=
DEBUG_CFLAGS=-O2 -g
WX_CONFIG_INCLUDE_NAME=release
endif

CPPFLAGS = $(CPPINCLUDE) $(DEBUG_CFLAGS) \
	-Winvalid-pch \
	-mthreads -fno-pcc-struct-return -fstrict-aliasing -Wall \
	-D__WXMSW__ -D__GNUWIN32__ -D_UNICODE -DUNICODE -DWXUSINGDLL \
	-DTJS_NO_REGEXP

CPPINCLUDE = \
	-I"$(MINGWROOT)/include" \
	-I"$(MINGWROOT)/include/boost-1_33_1" \
	-I"$(MINGWROOT)/lib/wx/include/msw-unicode-$(WX_CONFIG_INCLUDE_NAME)-2.6" \
	-I"$(MINGWROOT)/include/wx-2.6" \
	-I"."

PREC_H    = tjsCommHead.h
PREC_GCH  = tjsCommHead.h.gch


CPPFILES = 	tjs.cpp                     \
			tjs.tab.cpp                 \
			tjsArray.cpp                \
			tjsCharUtils.cpp            \
			tjsCompileControl.cpp       \
			tjsConfig.cpp               \
			tjsDate.cpp                 \
			tjsDateParser.cpp           \
			tjsDebug.cpp                \
			tjsDictionary.cpp           \
			tjsDisassemble.cpp          \
			tjsError.cpp                \
			tjsException.cpp            \
			tjsGlobalStringMap.cpp      \
			tjsInterCodeExec.cpp        \
			tjsInterCodeGen.cpp         \
			tjsInterface.cpp            \
			tjsLex.cpp                  \
			tjsMT19937ar-cok.cpp        \
			tjsMath.cpp                 \
			tjsMessage.cpp              \
			tjsNamespace.cpp            \
			tjsNative.cpp               \
			tjsObject.cpp               \
			tjsRandomGenerator.cpp      \
			tjsScriptBlock.cpp          \
			tjsScriptCache.cpp          \
			tjsString.cpp               \
			tjsUtils.cpp                \
			tjsVariant.cpp              \
			tjsVariantString.cpp        \
			tjsdate.tab.cpp             \
			tjspp.tab.cpp               

OBJS = $(CPPFILES:.cpp=.o)

.SUFFIXES:
.SUFFIXES: .o .c .cpp .cc .f .p

default: all

%.d: %.cpp
	$(SHELL) -ec '$(CC) -MM $(CPPFLAGS) $< \
					  | sed "s/\($*\)\.o[ :]*/\1.o $@ : /g" > $@; \
					  [ -s $@ ] || rm -f $@'

clean:
	${RM} $(OBJS) $(LIB) $(PREC_GCH) *.d


$(PREC_GCH) : $(PREC_H)
	$(CXX) $(CPPFLAGS) $(PREC_H) -o $(PREC_GCH)

$(LIB) : $(OBJS)
	$(AR) rs $(LIB) $(OBJS)

all: $(PREC_GCH) $(LIB)

-include $(CPPFILES:.cpp=.d)

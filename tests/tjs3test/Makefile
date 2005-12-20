BIN = tjs3test.exe

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

TJS3ROOT=../../src/core/tjs3

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
	-DTJS_NO_REGEXP \
	-DTJS_SUPPORT_WX

LDFLAGS = -g -mconsole

CPPINCLUDE = \
	-I"$(MINGWROOT)/include" \
	-I"$(TJS3ROOT)" \
	-I"$(MINGWROOT)/include/boost-1_33_1" \
	-I"$(MINGWROOT)/lib/wx/include/msw-unicode-$(WX_CONFIG_INCLUDE_NAME)-2.6" \
	-I"$(MINGWROOT)/include/wx-2.6" \
	-I"."

TJS3LIB=$(TJS3ROOT)/tjs3.a

LIBS = -L"$(MINGWROOT)/lib"  \
	-Wl,--enable-auto-import \
	$(TJS3LIB) \
	-lwx_baseu$(DEBUG_LIB_SUFFIX)-2.6.dll \
	-lwx_mswu$(DEBUG_LIB_SUFFIX)_core-2.6.dll \
	-lwx_mswu$(DEBUG_LIB_SUFFIX)_adv-2.6.dll \
	-lwx_baseu$(DEBUG_LIB_SUFFIX)_xml-2.6.dll \
	-lwxzlib-2.6 \
	-ltomcrypt \
	-lunicows \
	-lkernel32 \
	-luser32 \
	-lgdi32 \
	-lcomdlg32 \
	-lwinspool \
	-lwinmm \
	-lshell32 \
	-lcomctl32 \
	-lole32 \
	-loleaut32 \
	-luuid \
	-lrpcrt4 \
	-ladvapi32 \
	-lwsock32 \
	-lodbc32 \
	-lopengl32 


PREC_H    = prec.h
PREC_GCH  = prec.h.gch


CPPFILES = 	main.cpp

OBJS = $(CPPFILES:.cpp=.o)
LINKOBJS = $(OBJS)

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

$(BIN): $(OBJS) $(TJS3LIB)
	(cd $(TJS3ROOT) && make)
	$(CXX) $(LDFLAGS) -o $(BIN) $(LINKOBJS) $(LIBS)
	echo "Link completed"

all: $(PREC_GCH) $(BIN)

-include $(CPPFILES:.cpp=.d)

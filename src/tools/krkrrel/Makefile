BIN=krkrrel.exe

DEBUG=yes

MINGWROOT = C:/MinGW
CXX = g++
WINDRES = windres
RM = rm -f
MV = mv
XGETTEXT = xgettext
MSGMERGE = msgmerge
PERL = perl
SED = sed
FIND = find
SH = sh

ifeq ($(DEBUG),yes)
DEBUG_LIB_SUFFIX=d
DEBUG_CFLAGS=-O0 -D__WXDEBUG__ -g
WX_CONFIG_INCLUDE_NAME=debug
else
DEBUG_LIB_SUFFIX=
DEBUG_CFLAGS=-O2 -g
WX_CONFIG_INCLUDE_NAME=release
endif

LIBS = -L"$(MINGWROOT)/lib"  \
	-Wl,--enable-auto-import \
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
POT  = krkrrel.pot
LANG_JA_PO = messages/ja.po
LANG_JA_MO = locales/ja/krkrrel.mo
MO_FILES = $(LANG_JA_MO)


CPPFILES =  ConsoleMain.cpp \
			FileList.cpp  \
			WriteXP4.cpp \
			ReadXP4Meta.cpp \
			wxFileWrapper.cpp \
			XP4Hash.cpp

HFILES =	$(CPPFILES:.cpp=.h) \
			ProgressCallback.h

XGETTEXTINPUT = $(CPPFILES)

OBJS = $(CPPFILES:.cpp=.o)
LINKOBJS = $(OBJS)


CPPINCLUDE = \
	-I"$(MINGWROOT)/include" \
	-I"$(MINGWROOT)/include/tomcrypt" \
	-I"$(MINGWROOT)/lib/wx/include/msw-unicode-$(WX_CONFIG_INCLUDE_NAME)-2.6" \
	-I"$(MINGWROOT)/include/wx-2.6" \
	-I"."

CPPFLAGS = $(CPPINCLUDE) $(DEBUG_CFLAGS) \
	-Winvalid-pch \
	-mconsole \
	-mthreads -fno-pcc-struct-return -fstrict-aliasing -Wall \
	-D__WXMSW__ -D__GNUWIN32__ -D_UNICODE -DUNICODE -DWXUSINGDLL


LDFLAGS = -g -mconsole

default: all

clean:
	${RM} $(OBJS) $(BIN) $(PREC_GCH) .depend
	${RM} $(POT) $(MO_FILES)
	${RM} messages/*~

$(BIN): $(PREC_GCH) $(LINKOBJS) $(MO_FILES)
	$(CXX) $(LDFLAGS) $(LINKOBJS) -o $(BIN) $(LIBS)
	echo "Link completed"

.SUFFIXES:
.SUFFIXES: .o .c .cpp .cc .f .p


$(POT): $(CPPFILES)
	$(XGETTEXT) -c -C -k_ $(CPPFILES) -o - | $(SED) -f remove_pot_creation_date.sed > $(POT)

$(LANG_JA_PO): $(POT)
	msgmerge --update $(LANG_JA_PO) $(POT)
	touch $(LANG_JA_PO)

$(LANG_JA_MO): $(LANG_JA_PO)
	msgfmt -o $(LANG_JA_MO) $(LANG_JA_PO)

$(PREC_GCH) : $(PREC_H)
	$(CXX) $(CPPFLAGS) $(PREC_H) -o $(PREC_GCH)

depend: $(CPPFILES) .depend
	$(RM)   .depend
	$(MAKE) .depend

.depend:
	$(CXX) -E -MM $(CPPFLAGS) $(CPPFILES)  > .depend

ifeq (.depend,$(wildcard .depend))

all: $(BIN)
-include .depend

else

all:  $(CPPFILES) .depend $(BIN)
-include .depend

endif

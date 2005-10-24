BIN=krkrfont.exe

MINGWROOT = C:/MinGW
CXX = g++
WINDRES = windres
WXRC = wxrc
RM = rm -f
MV = mv
XGETTEXT = xgettext
MSGMERGE = msgmerge
PERL = perl
SED = sed
FIND = find
SH = sh

LIBS = -L"$(MINGWROOT)/lib"  \
	-mwindows -Wl,--enable-auto-import \
	-lwx_baseu-2.6.dll \
	-lwx_mswu_core-2.6.dll \
	-lwx_mswu_adv-2.6.dll \
	-lwx_mswu_xrc-2.6.dll \
	-lwx_baseu_xml-2.6.dll \
	-lfreetype \
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




RC   = winres.rc
RES  = winres.res
XRC  = krkrfont.xrc
PREC_H    = prec.h
PREC_GCH  = prec.h.gch
XRCOUT_CPP = resource.cpp
XRCOUT_H   = resource.h
XRCMSG = krkrfont.xrcmsg
POT  = krkrfont.pot
LANG_JA_PO = messages/ja.po
LANG_JA_MO = locales/ja/krkrfont.mo
MO_FILES = $(LANG_JA_MO)
CP932_OUT_CPP = uni_cp932.cpp cp932_uni.cpp
CP932_IN      = cp932map/conv.pl cp932map/conv_r.pl cp932map/CP932.TXT


CPPFILES =  $(XRCOUT_CPP) \
			KrkrFontApp.cpp \
			msw/NativeFreeTypeFace.cpp msw/NativeFreeTypeDriver.cpp \
			MainDialog.cpp FreeType.cpp PreviewWindow.cpp \
			FontDriver.cpp $(CP932_OUT_CPP)

HFILES =	$(CPPFILES:.cpp=.h) \
			tjsString.h tjsTypes.h tvpfontstruc.h prec.h

XGETTEXTINPUT = $(CPPFILES) $(XRCMSG)

OBJS = $(CPPFILES:.cpp=.o) $(RES)
LINKOBJS = $(OBJS)


CPPINCLUDE = \
	-I"$(MINGWROOT)/include" \
	-I"$(MINGWROOT)/include/wx-2.6" \
	-I"$(MINGWROOT)/lib/wx/include/msw-unicode-release-2.6" \
	-I"$(MINGWROOT)/include/freetype2" \
	-I"msw" \
	-I"."

CPPFLAGS = $(CPPINCLUDE) \
	-g \
	-O0 \
	-mwindows -mconsole \
	-mthreads -fno-pcc-struct-return -fstrict-aliasing -Wall \
	-D__WXMSW__ -D__GNUWIN32__ -D_UNICODE -DUNICODE -DWXUSINGDLL


LDFLAGS = -g -mwindows -mconsole

default: all

clean:
	${RM} $(OBJS) $(BIN) $(PREC_GCH) .depend \
	$(XRCOUT_CPP).bak $(XRCMSG) $(XRCOUT_CPP) $(XRCOUT_H) $(POT) $(MO_FILES)
	${RM} messages/*~

$(BIN): $(PREC_GCH) $(LINKOBJS) $(RES) $(MO_FILES)
	$(CXX) $(LDFLAGS) $(LINKOBJS) -o $(BIN) $(LIBS)
	echo "Link completed"

.SUFFIXES:
.SUFFIXES: .o .c .cpp .cc .f .p


$(RES): $(RC)
	$(WINDRES) $(CPPINCLUDE) -i $(RC) -o $(RES) -O coff $(INCLUDE)

$(XRCOUT_CPP): $(XRC)
	$(WXRC) --cpp-code --extra-cpp-code $(XRC)
	$(MV) $(XRCOUT_CPP) $(XRCOUT_CPP).bak
	$(SED) "s:<wx/wxprec.h>:\x22prec.h\x22:g" $(XRCOUT_CPP).bak > $(XRCOUT_CPP)
	$(RM) $(XRCOUT_CPP).bak

$(XRCMSG): $(XRC)
	$(WXRC) --gettext $(XRC) > $(XRCMSG)


$(POT): $(XRCMSG) $(CPPFILES)
	$(XGETTEXT) -c -C -k_ $(XRCMSG) $(CPPFILES) -o - | $(SED) "/^.POT-Creation-Date:/d" > $(POT)

$(LANG_JA_PO): $(POT)
	msgmerge --update $(LANG_JA_PO) $(POT)
	touch $(LANG_JA_PO)

$(LANG_JA_MO): $(LANG_JA_PO)
	msgfmt -o $(LANG_JA_MO) $(LANG_JA_PO)

$(PREC_GCH) : $(PREC_H)
	$(CXX) $(CPPFLAGS) $(PREC_H) -o $(PREC_GCH)

$(CP932_OUT_CPP) $(CP932_OUT_H) : $(CP932_IN)
	(cd cp932map && $(PERL) conv.pl && $(PERL) conv_r.pl)

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

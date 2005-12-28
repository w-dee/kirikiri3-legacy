TOP_DIR=../../..

DEBUG=yes

#----------------------------#

BIN=krkrrel.exe
default: $(BIN)

#----------------------------#


CPPFILES =  ConsoleMain.cpp \
			FileList.cpp  \
			WriteXP4.cpp \
			ReadXP4Meta.cpp \
			wxFileWrapper.cpp \
			XP4Hash.cpp

HFILES =	$(CPPFILES:.cpp=.h) \
			ProgressCallback.h

OBJS = $(CPPFILES:.cpp=.o)


#----------------------------#

CFLAGS = $(WX_CFLAGS) $(ZLIB_CFLAGS) $(LIBTOMCRYPT_CFLAGS) $(COMMON_CFLAGS)
CXXFLAGS = $(WX_CXXFLAGS) $(ZLIB_CXXFLAGS) $(LIBTOMCRYPT_CXXFLAGS) $(COMMON_CFLAGS)
CPPFLAGS = $(WX_CPPFLAGS) $(ZLIB_CPPFLAGS) $(LIBTOMCRYPT_CPPFLAGS) $(COMMON_CFLAGS)
LIBS = $(WX_LIBS) $(ZLIB_LIBS) $(LIBTOMCRYPT_LIBS) $(COMMON_LIBS)
LDFLAGS = -g -mconsole

#----------------------------#

prec.h.gch: prec.h

locales/ja/krkrrel.mo: messages/ja.po

clean:
	rm -f $(OBJS) $(BIN) prec.h.gch *.d
	rm -f messages.pot $(MO_FILES)
	rm -f messages/*~

messages.pot: $(CPPFILES)

$(LANG_JA_PO): $(POT)

$(BIN): prec.h.gch $(OBJS) $(MO_FILES)
	$(LD) $(OBJS) -o $(BIN) $(LIBS)

#----------------------------#


include $(TOP_DIR)/Makefile_config
include $(TOP_DIR)/Makefile_common
-include $(CPPFILES:.cpp=.d)


# サブディレクトリの定義

SUBLIBS= zlib libpng libjpeg expat wxWidgets icu libtomcrypt freetype boost


# 各サブディレクトリに対して再帰

default:
	for dir in $(SUBLIBS); do ( cd $$dir && $(MAKE)  default  ); done

all:
	for dir in $(SUBLIBS); do ( cd $$dir && $(MAKE)  all      ); done

clean:
	for dir in $(SUBLIBS); do ( cd $$dir && $(MAKE)  clean    ); done

download:
	for dir in $(SUBLIBS); do ( cd $$dir && $(MAKE)  download ); done



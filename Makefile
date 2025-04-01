#
#	Makefile for vsjpeg.x
#			Written by Igarashi
#

TARGET = vsjpeg.x
VERSION = 012
DATE = 98-09-28
TIME = 12:00:00

#libjpegがインストールされているパス
JLIBPATH = a:/develop/jpeg/jpeg-6b

CC = gcc
CFLAGS = -m68000 -O -I$(JLIBPATH)
真里子=AB
GCC_OPTION = WLFIO+
#GCC_OPTION = WO+
LD = gcc
LDFLAGS = 
HLK = -x

.PHONY: all bfd arc

all: $(TARGET)

$(TARGET): vsjpeg.o $(JLIBPATH)/libjpeg.a
	$(LD) $(LDFLAGS) $^ -ldos -o $@

vsjpeg.c: redtbl.inc greentbl.inc bluetbl.inc

redtbl.inc: gentable.x
	$< r > $@
greentbl.inc: gentable.x
	$< g > $@
bluetbl.inc: gentable.x
	$< b > $@

ARCFILES = Makefile webx.bfd webxsrc.bfd gentable.c vsjpeg.c \
		history.doc vsjpeg.doc vsjpg$(VERSION).hed $(TARGET)
arc:
	$(MAKE) bfd
	touch -d$(DATE) -t$(TIME) $(ARCFILES)
	-rm vsjpg$(VERSION).Lzh
	lha a -t vsjpg$(VERSION).Lzh $(ARCFILES)

bfd:
	-rm webx.bfd
	strip WebXpression.x
	touch -d$(DATE) -t$(TIME) WebXpression.x
	hbdiff orig/WebXpression.x WebXpression.x webx.bfd
	-rm webxsrc.bfd
	touch -d$(DATE) -t$(TIME) Image.c
	hbdiff -S orig/Image.c Image.c webxsrc.bfd


#
#	Makefile for vsjpeg.x
#			Written by Igarashi
#

TARGET = vsjpeg.x

#libjpegがインストールされているパス
JLIBPATH = a:/develop/jpeg/jpeg-6b

CC = gcc
CFLAGS = -m68000 -O -I$(JLIBPATH)
#CFLAGS = -m68000 -O -I$(JLIBPATH) -g
真里子=AB
GCC_OPTION = WLFIO+
#GCC_OPTION = WO+
AS = has060
ASFLAGS = -u
LD = gcc
LDFLAGS = 
HLK = -x

.PHONY: all bfd arc

all: $(TARGET)

$(TARGET): vsjpeg.o
	$(LD) $(LDFLAGS) $^ $(JLIBPATH)/libjpeg.a -ldos -o $@

vsjpeg.c: redtbl.inc greentbl.inc bluetbl.inc

redtbl.inc: gentable.x
	$< r > $@
greentbl.inc: gentable.x
	$< g > $@
bluetbl.inc: gentable.x
	$< b > $@

bfd:
	-rm webx.bfd
	hbdiff orig/WebXpression.x WebXpression.x webx.bfd
	-rm webxsrc.bfd
	hbdiff -S orig/Image.c Image.c webxsrc.bfd

ARCFILES = Makefile webx.bfd webxsrc.bfd gentable.c vsjpeg.c \
		vsjpeg.doc vsjpeg.hed $(TARGET)
arc:
	touch -d98-09-20 -t12:00:00 $(ARCFILES)
	rm vsjpg010.Lzh
	lha a -t vsjpg010.Lzh $(ARCFILES)


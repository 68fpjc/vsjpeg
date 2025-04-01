#========================================================================
#	Makefile for vsjpeg.x
#			Copyright 1998, 2001 Igarashi
#========================================================================

#	libjpeg.a およびヘッダファイルがインストールされているパス
#	各自の環境に合わせて変更すること
JLIBPATH = $(HOME)/bin/iga/libjpeg/jpeg-6b-010303.h01

CC = egcs
CFLAGS = -O3 -Wall -I$(JLIBPATH)
LD = egcs
LDLIBS = -ldos $(JLIBPATH)/libjpeg.a
export G2LK = -x

TARGET		= vsjpeg.x
VERSION		= 015
ARCHIVE		= vsjpg$(VERSION)
ARCFILES	= \
		Makefile \
		gentable.c \
		vsjpeg.c \
		history.doc \
		vsjpeg.doc \
		vsjpg015.hed \
		$(TARGET)

.PHONY: all clean arc

all: $(TARGET)

vsjpeg.o: redtbl.inc greentbl.inc bluetbl.inc

redtbl.inc: gentable.x
	$< r > $@
greentbl.inc: gentable.x
	$< g > $@
bluetbl.inc: gentable.x
	$< b > $@

clean:
#	-rm *.o *.x *.inc
	-rm *.o gentable.x *.inc

arc:
	-rm $(ARCHIVE).zip
	strip $(TARGET)
	zip -9 $(ARCHIVE).zip $(ARCFILES)

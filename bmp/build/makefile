#
# makefile for bmp library
#
# Copyright (C) 2002 Hiroaki Inaba
#

GUILIBS = user32.lib gdi32.lib kernel32.lib
CFLAGS = -nologo -EHsc -I../src
CC = cl

all: bmp_info.exe bmp_dump.exe bmp_copy.exe bmp_copy2.exe bmp_draw.exe bmp_viewer.exe

bmp_info.exe: ../examples/bmp_info.c ../src/bmp.c
	$(CC) $(CFLAGS) /Fe$@ $**

bmp_dump.exe: ../examples/bmp_dump.c ../src/bmp.c
	$(CC) $(CFLAGS) /Fe$@ $**

bmp_copy.exe: ../examples/bmp_copy.c ../src/bmp.c
	$(CC) $(CFLAGS) /Fe$@ $**

bmp_copy2.exe : ../examples/bmp_copy2.c ../src/bmp.c
	$(CC) $(CFLAGS) $**

bmp_draw.exe : ../examples/bmp_draw.c ../src/bmp.c
	$(CC) $(CFLAGS) $**

bmp_viewer.exe : ../examples/bmp_viewer.cpp ../src/bmp.c
	$(CC) $(CFLAGS) $** $(GUILIBS)

clean:
	del *.obj
	del *.exe

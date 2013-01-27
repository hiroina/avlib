#
# makefile for bmp library
#
# Copyright (C) 2002 Hiroaki Inaba
#

CFLAGS = -nologo -EHsc -I../src
CC = cl

all: wav_copy.exe wav_dump.exe wav_player.exe

#wav_info.exe: ../examples/wav_info.c ../src/wav.c
#	$(CC) $(CFLAGS) /Fe$@ $**

wav_dump.exe: ../examples/wav_dump.c ../src/wav.c
	$(CC) $(CFLAGS) /Fe$@ $**

wav_copy.exe: ../examples/wav_copy.c ../src/wav.c
	$(CC) $(CFLAGS) /Fe$@ $**

wav_player.exe : ../examples/wav_player.cpp ../src/wav.c
	$(CC) $(CFLAGS) $** winmm.lib

clean:
	del *.obj
	del *.exe
	del *.wav

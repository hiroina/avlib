/*
 * Copyright (C) 2003 Hiroaki Inaba
 *
 * Wav file player on Windows
 */

#include <windows.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

VOID CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

int toInt(char* p, int size)
{
  int retval;

  switch (size) {
  case 1:	retval = (int)*((char*)p);		break;
  case 2:	retval = (int)*((short*)p);		break;
  case 4:	retval = (int)*((int*)p);		break;
  default:	retval = 0;
  }

  return retval;
}

//------------------------------------------------------------

struct WAVEFORM_EX {
  short wFormatTag;
  short nChannels;
  int nSamplesPerSec;
  int nAvgBytesPerSec;
  short nBlockAlign;
  short wBitsPerSample;
  WAVEFORM_EX() : wFormatTag(0), nChannels(0), nSamplesPerSec(0),
				  nAvgBytesPerSec(0), nBlockAlign(0), wBitsPerSample(0) { }
};

WAVEFORM_EX Fmt;

ostream& operator<<(ostream& o, const WAVEFORM_EX& f)
{
  o << "wFormatTag      = " << setw(4) << setfill('0') << hex << f.wFormatTag << dec << endl;
  o << "nChannles       = " << f.nChannels << endl;
  o << "nSamplesPerSec  = " << f.nSamplesPerSec << endl;
  o << "nAvgBytesPerSec = " << f.nAvgBytesPerSec << endl;
  o << "nBlockAlign     = " << f.nBlockAlign << endl;
  o << "wBitsPerSample  = " << f.wBitsPerSample << endl;
  return o;
}

//------------------------------------------------------------

char* bptr = 0;	// Pointer to the first byte of data portion
int blen;		// Number of bytes of data portion
bool play_finished = false;

//------------------------------------------------------------

int main(int argc, char* argv[])
{
  char id[4];
  char* buf = 0;
  int buf_size;
  ifstream is;
  bool v_flag = false;

  if (argc == 2) {
	is.open(argv[1], ios::binary);
  }
  else if ( (argc == 3) && (strcmp(argv[1], "-v") == 0) ) {
	v_flag = true;
	is.open(argv[2], ios::binary);
  }
  else {
	cerr << "Usage: wave [-v] filename" << endl;
	return -1;
  }

  // RIFF
  is.read(id, 4);
  is.read((char*)&buf_size, 4);
  cout << id[0] << id[1] << id[2] << id[3] << " : " << buf_size << endl;

  // Read Data
  buf = new char[buf_size];
  is.read(buf, buf_size);

  //------------------------------------------------------------

  // WAVE
  cout << buf[0] << buf[1] << buf[2] << buf[3] << endl;
  if (!(buf[0] == 'W' && buf[1] == 'A' && buf[2] == 'V' && buf[3] == 'E'))
	cerr << "Warning: This should be \"WAVE\"" << endl;

  int index = 4;
  while (index < buf_size) {
	cout << "------------------------------------------------------------" << endl;

	// ID
	string str_id(buf + index, 4);
	index += 4;

	// Size
	int size = *((int*)&buf[index]);
	index += 4;

  //------------------------------------------------------------
	// Data Processing
	if (str_id == "fmt ") {
	  cout << "fmt : " << size << endl;
	  memcpy(&Fmt, &buf[index], sizeof(WAVEFORM_EX));

	  cout << Fmt;
	}
	else if (str_id == "data") {
	  cout << "data : " << size << endl;
	  blen = size;
	  bptr = &buf[index];

	  if (v_flag && Fmt.wFormatTag == 0x0001) {		// PCM
		int bi = 0;
		int bytes_per_sample = Fmt.wBitsPerSample / 8;
		
		while (bi < size) {
		  cout << toInt(bptr + bi, bytes_per_sample);	bi += bytes_per_sample;
		  
		  if ((bi % (bytes_per_sample * Fmt.nChannels)) == 0)
			cout << endl;
		  else
			cout << "\t";
		}
	  }
	}
	else {
	  cout << str_id << " : " << size << endl;
	}
	//------------------------------------------------------------
	// Data
	index += size;
  }

  //------------------------------------------------------------
  // Play Sound
  MMRESULT		mmRes;
  HWAVEOUT		hwo;
  WAVEFORMATEX	wfx;

  wfx.wFormatTag = Fmt.wFormatTag;
  wfx.nChannels = Fmt.nChannels;
  wfx.nSamplesPerSec = Fmt.nSamplesPerSec;
  wfx.wBitsPerSample = Fmt.wBitsPerSample;
  wfx.nBlockAlign = Fmt.nBlockAlign;
  wfx.nAvgBytesPerSec = Fmt.nAvgBytesPerSec;
  wfx.cbSize = 0;
  
  mmRes = waveOutOpen(&hwo,
					  WAVE_MAPPER,
					  &wfx,
					  (DWORD)(LPVOID)waveOutProc,
					  (DWORD)NULL,
					  CALLBACK_FUNCTION
					  );
  
  if(mmRes != MMSYSERR_NOERROR){
	cerr << "Error at waveOutOpen" << endl;
  }
  else {
	WAVEHDR whdr;
	memset(&whdr, 0, sizeof(WAVEHDR));
	whdr.lpData = bptr;
	whdr.dwBufferLength = blen;

	mmRes = waveOutPrepareHeader(hwo, &whdr, sizeof(WAVEHDR));
	if(mmRes != MMSYSERR_NOERROR) {
	  cerr << "Error at waveOutPrepareHeader" << endl;
	}
	else {
	  mmRes = waveOutWrite(hwo, &whdr, sizeof(WAVEHDR));
	  if(mmRes != MMSYSERR_NOERROR)
		cerr << "Error at waveOutWrite" << endl;
	}

	while (play_finished == false)
	  Sleep(100);

	waveOutUnprepareHeader(hwo, &whdr, sizeof(WAVEHDR));
	waveOutClose(hwo);
  }

  //------------------------------------------------------------

  // Clean
  is.close();
  delete [] buf;

  return 0;
}

VOID CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
  switch(uMsg){
        case WOM_OPEN:
		  break;
        case WOM_CLOSE:
		  break;
        case WOM_DONE:
		  play_finished = true;
		  break;
    }
}

/*
 * Copyright (C) 2002-2012 Hiroaki Inaba
 *
 * BMP file viewer program for Windows.
 * It automatically refresh BMP when its window is clicked.
 */

#include <windows.h>
#include <iostream>
extern "C" {
#include "bmp.h"
}
#if CNET_DEBUG
#include "cnetbuf.h"
#endif
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
using namespace std;

// Globals
bmp_handle bmp_h;
char *filename;
time_t gmtime;

#if CNET_DEBUG
cnetbuf cbuf("localhost");
ostream cnet(&cbuf);
#endif

time_t get_mtime(void)
{
  // get the last update time of the BMP file
  struct _stat buf;
  int fh = _open(filename, _O_RDONLY);
  int result = _fstat( fh, &buf );
  _close( fh );

  return buf.st_mtime;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
						 PSTR szCmdLine, int iCmdShow)
{
  // Process Command Line Option
  if (szCmdLine)
	filename = szCmdLine;
  else
	filename = "a.bmp";

  bmp_open(&bmp_h, filename);
  gmtime = get_mtime();

#if CNET_DEBUG
  cnet << "mtime = " << gmtime << endl;
#endif
  //------------------------------------------------------------

  static char szAppName[] = "bmpview";
  HWND			hwnd;
  MSG			msg;
  WNDCLASSEX	wndclass;

  wndclass.cbSize			= sizeof(wndclass);
  wndclass.style			= CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc		= WndProc;
  wndclass.cbClsExtra		= 0;
  wndclass.cbWndExtra		= 0;
  wndclass.hInstance		= hInstance;
  wndclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
  wndclass.hCursor			= LoadCursor(NULL, IDC_ARROW);
  wndclass.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
  wndclass.lpszMenuName		= NULL;
  wndclass.lpszClassName	= szAppName;
  wndclass.hIconSm			= LoadIcon(NULL, IDI_APPLICATION);

  RegisterClassEx(&wndclass);

  bmp_config config;
  bmp_get_config(bmp_h, &config);

  hwnd = CreateWindow(szAppName,			// window class name
					  filename,
					  //					  "The Hello Program",	// window caption
					  //					  WS_OVERLAPPEDWINDOW,	// window style
					  WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
					  CW_USEDEFAULT,		// initial x position
					  CW_USEDEFAULT,		// initial y position
					  config.width,		// initial x size
					  config.height,		// initial y size
					  NULL,					// parent window handle
					  NULL,					// window menu handle
					  hInstance,			// program instance handle
					  NULL					// create parameters
					  );

  ShowWindow(hwnd, iCmdShow);
  UpdateWindow(hwnd);

  while (GetMessage(&msg, NULL, 0, 0)) {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
  }

  return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
  HDC			hdc;
  PAINTSTRUCT	ps;
  RECT			rect;

  switch (iMsg) {
  case WM_ACTIVATE: {
	time_t mtime = get_mtime();
#if CNET_DEBUG
	cnet << "new mtime = " << mtime << endl;
#endif
	if (mtime != gmtime) {
//	  bmpfile.read(filename);
        bmp_load(bmp_h, filename);
	  InvalidateRgn(hwnd, NULL, TRUE);
	  gmtime = mtime;
	}
	return 0;
  }
  case WM_PAINT: {
	hdc = BeginPaint(hwnd, &ps);
	GetClientRect(hwnd, &rect);
	//	DrawText(hdc, "Hello, Windows95!", -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    bmp_config config;
    bmp_get_config(bmp_h, &config);

	for (int y = 0; y < config.height; y++) {
        for (int x = 0; x < config.width; x++) {
            uint32_t color;
            bmp_get_color(bmp_h, x, y, &color);
            SetPixel(hdc, x, y, RGB(RGB_R(color), RGB_G(color), RGB_B(color)));
        }
    }

	EndPaint(hwnd, &ps);

	return 0;
  }
  case WM_DESTROY:
	PostQuitMessage(0);
	return 0;
  }

  return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

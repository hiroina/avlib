bmp library
===========

A simple C library to access each pixel of bmp file.


How to build
------------

```
cd build
nmake
```


Examples
--------

Some example files are included in `examples` directory.

* `bmp_copy.c` - copy a bmp file by copying each pixel.
* `bmp_copy2.c` - similar to bmp_copy.c, but degrade each color.
* `bmp_draw.c` - draw simple graphics.
* `bmp_info.c` - print bmp file info.
* `bmp_viewer.cpp` - win32 bmp viewer app.


Notes
-----

* Currently it only supports 24 bit per pixel.
* Tested on Windows using Visual Studio.  But it should be easy to port on Linux.

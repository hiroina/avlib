wav library
===========

A simple C library to access each audio sample in wav file.


How to build
------------

```
cd build
nmake
```


Examples
--------

Some example files are included in `examples` directory.

* `wav_copy.c` - copy a wav file by copying each audio sample.
* `wav_dump.c` - print each samples.
* `wav_player.cpp` - win32 wav player app.  It does not use this wav library.  This is for test purpose.


Notes
-----

* Tested on Windows using Visual Studio.

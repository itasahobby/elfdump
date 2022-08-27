# elfdump

Inspired by [elfcat](https://github.com/randomstuff/elfcat) to dump program sections or program entries in hexdump like format.

PoC:
```
./elfdump /bin/bash .interp
2F 6C 69 62 36 34 2F 6C  64 2D 6C 69 6E 75 78 2D  |  /lib64/ld-linux- 
78 38 36 2D 36 34 2E 73  6F 2E 32 00              |  x86-64.so.2.
```

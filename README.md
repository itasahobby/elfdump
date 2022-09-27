# elfdump

Inspired by [elfcat](https://github.com/randomstuff/elfcat) to dump program sections or program entries in hexdump like format.

PoC:
```
./elfdump /bin/bash .interp | xxd
00000000: 2f6c 6962 3634 2f6c 642d 6c69 6e75 782d  /lib64/ld-linux-
00000010: 7838 362d 3634 2e73 6f2e 3200            x86-64.so.2.
```
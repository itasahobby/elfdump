SOURCE	= elfdump.c
HEADER	= elfdump.h
OUT		= elfdump
CC	 	= gcc
LFLAGS	= -l elf

all: $(SOURCE)
	$(CC) $(SOURCE) -o $(OUT) $(LFLAGS)

clean:
	rm -f $(OUT)
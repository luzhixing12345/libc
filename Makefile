
CC = gcc
CFLAGS = -Wall -Wunused -Werror -Wformat-security

SRC = $(wildcard examples/*.c)
OBJ = $(SRC:.c=.o)
EXE = $(SRC:.c=)

all: $(EXE)

$(EXE): %: %.o
	$(CC) $< -o $@

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm $(EXE) $(OBJ)

## for develop
XBOX_HEADER_FILES = $(wildcard src/*.h)
TARGET_DIR = xbox

link: $(XBOX_HEADER_FILES)
	@rm -r ~/coreutils/src/xbox
	@rm -r ~/binutils/src/xbox
	@mkdir -p ~/coreutils/src/xbox
	@mkdir -p ~/binutils/src/xbox
	@ln -s ~/libc/src/*.h ~/coreutils/src/xbox
	@ln -s ~/libc/src/*.h ~/binutils/src/xbox
	@echo finished link
	
ulink:
	@-rm -r ~/coreutils/src/xbox
	@-rm -r ~/binutils/src/xbox
	@cp -r ~/libc/src/ ~/coreutils/src/xbox
	@cp -r ~/libc/src/ ~/binutils/src/xbox
	@echo remove link


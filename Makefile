
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
	@rm -r ~/git/src/xbox
	@mkdir -p ~/coreutils/src/xbox
	@mkdir -p ~/binutils/src/xbox
	@mkdir -p ~/git/src/xbox
	@ln -s ~/libc/src/*.h ~/coreutils/src/xbox
	@ln -s ~/libc/src/*.h ~/binutils/src/xbox
	@ln -s ~/libc/src/*.h ~/git/src/xbox
	@echo finished link
	
ulink:
	@-rm -r ~/coreutils/src/xbox
	@-rm -r ~/binutils/src/xbox
	@-rm -r ~/git/src/xbox
	@cp -r ~/libc/src/ ~/coreutils/src/xbox
	@cp -r ~/libc/src/ ~/binutils/src/xbox
	@cp -r ~/libc/src/ ~/git/src/xbox
	@echo remove link


test:
	-./examples/main --help
	-./examples/main -v
	-./examples/main -i
	-./examples/main -i 100
	-./examples/main -i 100 -s 200
	-./examples/main -i 100 -s 200 300
	-./examples/main -i 100 -s 200 300 400
	-./examples/main -i 100 -s 200 300 "400"
	-./examples/main -i 100 -s 200 "300" "400"
	-./examples/main -i 100 -s 200 "300" "400" "500"
	-./examples/main -i 100 -s 200 "300" "400" abc
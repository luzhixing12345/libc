
CC = gcc
CFLAGS =  -Wall -Wunused -Werror -Wformat-security -Wshadow -Wpedantic -Wstrict-aliasing -Wuninitialized -Wnull-dereference -Wformat=2

SRC = $(wildcard examples/*.c)
OBJ = $(SRC:.c=.o)
EXE = $(SRC:.c=)

XBOX_SRC = $(wildcard src/*.c)
XBOX_OBJ = $(XBOX_SRC:.c=.o)

ifeq ($(MAKECMDGOALS),debug)
CFLAGS+=-g
endif

debug: all 

all: $(EXE)

$(EXE): %: %.o $(XBOX_OBJ)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -Isrc/ -c $< -o $@

.PHONY: clean

clean:
	rm $(EXE) $(OBJ) $(XBOX_OBJ)


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
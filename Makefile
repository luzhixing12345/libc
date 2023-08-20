
CC = gcc
CFLAGS = -Wall -Wunused -Werror -Wformat-security

SRC = $(wildcard utils/*.c)
OBJ = $(SRC:.c=.o)
EXE = $(SRC:.c=)

all: $(EXE)

$(EXE): %: %.o
	$(CC) $< -o $@
	mv -i $@ ./

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm $(EXE) $(OBJ)
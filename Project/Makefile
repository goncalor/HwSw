CC = gcc
INCLUDES = includes/
FLAGS = -Wall -I$(INCLUDES) -g -std=gnu99
ODIR = obj
SRCDIR = src

_DEPS = read_char.h misc.h huffman_code.h
DEPS = $(patsubst %,$(INCLUDES)/%,$(_DEPS))

_OBJ = huffman.o read_char.o misc.o huffman_code.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(FLAGS)

huffman: $(OBJ)
	$(CC) $(FLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f obj/*.o huffman

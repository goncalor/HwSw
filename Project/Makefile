CC = gcc
INCLUDES = includes/
FLAGS = -Wall -I$(INCLUDES)
ODIR = obj
SRCDIR = src

_DEPS = read_char.h
DEPS = $(patsubst %,$(INCLUDES)/%,$(_DEPS))

_OBJ = huffman.o read_char.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(FLAGS)

huffman: $(OBJ)
	$(CC) $(FLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f obj/*.o huffman

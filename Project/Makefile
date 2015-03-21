CC = gcc
FLAGS = -Wall

huffman: huffman.o read_char.o
	$(CC) $(FLAGS) -o huffman huffman.o read_char.o

.PHONY: clean
clean:
	rm -f *.o huffman

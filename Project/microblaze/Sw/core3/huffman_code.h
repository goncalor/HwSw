#ifndef HUFF
#define HUFF

// This constant can be avoided by explicitly calculating height of Huffman Tree
#define MAX_TREE_HT 256

unsigned short tree_size;	// init to 0
int bits;

unsigned encode_file(char *orig, char *dest, char *codewords);

#endif //HUFF

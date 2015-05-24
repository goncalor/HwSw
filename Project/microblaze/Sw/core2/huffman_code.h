#ifndef HUFF
#define HUFF

unsigned short tree_size;	// init to 0
int bits;

unsigned encode_file(char *orig, char *dest, char *codewords);

#endif //HUFF

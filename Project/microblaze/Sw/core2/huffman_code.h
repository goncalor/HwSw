#ifndef HUFF
#define HUFF

// A Huffman tree node
typedef struct MinHeapNode
{
    char data;  // One of the input characters
    unsigned freq;  // Frequency of the character
    struct MinHeapNode *left, *right; // Left and right child of this node
} MinHeapNode;

unsigned short tree_size;	// init to 0
int bits;

unsigned encode_file(char *orig, char *dest, char *codewords);

#endif //HUFF

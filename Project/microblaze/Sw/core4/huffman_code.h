#ifndef HUFF
#define HUFF

// This constant can be avoided by explicitly calculating height of Huffman Tree
#define MAX_TREE_HT 256

// A Huffman tree node
typedef struct MinHeapNode
{
    char data;  // One of the input characters
    unsigned freq;  // Frequency of the character
    struct MinHeapNode *left, *right; // Left and right child of this node
} MinHeapNode;

unsigned short tree_size;	// init to 0
MinHeap min_heap;
int bits;

unsigned encode_file(char *orig, char *dest, char *codewords);

#endif //HUFF

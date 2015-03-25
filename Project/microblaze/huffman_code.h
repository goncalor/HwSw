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

// A Min Heap:  Collection of min heap (or Hufmman tree) nodes
typedef struct MinHeap
{
    unsigned size;    // Current size of min heap
    unsigned capacity;   // capacity of min heap
    struct MinHeapNode *array[256];  // Attay of minheap node pointers
} MinHeap;

MinHeapNode heap_nodes[512];
unsigned short tree_size;	// init to 0
MinHeap min_heap;
int bits;

void HuffmanCodes(char data[], unsigned freq[], int size);
void encode_text(char *file);

#endif //HUFF

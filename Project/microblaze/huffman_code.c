#include <stdio.h>
#include <stdlib.h>

#include "huffman_code.h"
#include "misc.h"
#include "define.h"

// A utility function allocate a new min heap node with given character
// and frequency of the character
MinHeapNode* newNode(char data, unsigned freq)
{
	MinHeapNode* new = &heap_nodes[tree_size];

	tree_size++;
	new->left = new->right = NULL;
	new->data = data;
	new->freq = freq;
	return new;
}

// A utility function to create a min heap of given capacity
MinHeap* createMinHeap(unsigned capacity)
{
	MinHeap* minHeap = &min_heap;

	minHeap->size = 0;  // current size is 0
	minHeap->capacity = capacity;
	return minHeap;
}

// A utility function to swap two min heap nodes
void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b)
{
	MinHeapNode* t = *a;
	*a = *b;
	*b = t;
}

// The standard minHeapify function.
void minHeapify(MinHeap* minHeap, int idx)
{
	int smallest = idx;
	int left = 2 * idx + 1;
	int right = 2 * idx + 2;

	if (left < minHeap->size &&
			minHeap->array[left]->freq < minHeap->array[smallest]->freq)
		smallest = left;

	if (right < minHeap->size &&
			minHeap->array[right]->freq < minHeap->array[smallest]->freq)
		smallest = right;

	if (smallest != idx)
	{
		swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
		minHeapify(minHeap, smallest);
	}
}

// A utility function to check if size of heap is 1 or not
int isSizeOne(MinHeap* minHeap)
{
	return (minHeap->size == 1);
}

// A standard function to extract minimum value node from heap
MinHeapNode* extractMin(MinHeap* minHeap)
{
	MinHeapNode* temp = minHeap->array[0];
	minHeap->array[0] = minHeap->array[minHeap->size - 1];
	--minHeap->size;
	minHeapify(minHeap, 0);
	return temp;
}

// A utility function to insert a new node to Min Heap
void insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode)
{
	++minHeap->size;
	int i = minHeap->size - 1;
	while (i && minHeapNode->freq < minHeap->array[(i - 1)/2]->freq)
	{
		minHeap->array[i] = minHeap->array[(i - 1)/2];
		i = (i - 1)/2;
	}
	minHeap->array[i] = minHeapNode;
}

// A standard funvtion to build min heap
void buildMinHeap(MinHeap* minHeap)
{
	int n = minHeap->size - 1;
	int i;
	for (i = (n - 1) / 2; i >= 0; --i)
		minHeapify(minHeap, i);
}

// A utility function to print an array of size n
void printArr(int arr[], int n)
{
	int i;
	for (i = 0; i < n; ++i){
#ifndef MB
		printf("%d", arr[i]);
#else
		xil_printf("%d", arr[i]);
#endif
		bits++;
	}
	putchar(' ');
}

// Utility function to check if this node is leaf
int isLeaf(MinHeapNode* root)
{
	return !(root->left) && !(root->right) ;
}

// Creates a min heap of capacity equal to size and inserts all character of
// data[] in min heap. Initially size of min heap is equal to capacity
MinHeap* createAndBuildMinHeap(char data[], unsigned freq[], int size)
{
	MinHeap* minHeap = createMinHeap(size);
	int i;

	for (i = 0; i < size; ++i)
		minHeap->array[i] = newNode(data[i], freq[i]);
	minHeap->size = size;
	buildMinHeap(minHeap);
	return minHeap;
}

// The main function that builds Huffman tree
MinHeapNode* buildHuffmanTree(char data[], unsigned freq[], int size)
{
	MinHeapNode *left, *right, *top;

	// Step 1: Create a min heap of capacity equal to size.  Initially, there are
	// modes equal to size.

	bits = 0;
	tree_size = 0;
	MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);

	// Iterate while size of heap doesn't become 1
	while (!isSizeOne(minHeap))
	{
		// Step 2: Extract the two minimum freq items from min heap
		left = extractMin(minHeap);
		right = extractMin(minHeap);

		// Step 3:  Create a new internal node with frequency equal to the
		// sum of the two nodes frequencies. Make the two extracted node as
		// left and right children of this new node. Add this node to the min heap
		// '$' is a special value for internal nodes, not used
		top = newNode('$', left->freq + right->freq);
		top->left = left;
		top->right = right;
		insertMinHeap(minHeap, top);
	}

	// Step 4: The remaining node is the root node and the tree is complete.
	return extractMin(minHeap);
}

// Prints huffman codes from the root of Huffman Tree.  It uses arr[] to
// store codes
void printCodes(MinHeapNode* root, int arr[], int top, char a)
{
	// Assign 0 to left edge and recur
	if (root->left)
	{
		arr[top] = 0;
		printCodes(root->left, arr, top + 1, a);
	}

	// Assign 1 to right edge and recur
	if (root->right)
	{
		arr[top] = 1;
		printCodes(root->right, arr, top + 1, a);
	}

	// If this is a leaf node, then it contains one of the input
	// characters, print the character and its code from arr[]
	if (isLeaf(root) && root->data == a)
	{
		//printf("%c: ", root->data);
		printArr(arr, top);
	}
}

/**
 * Encodes tree in header of FILE
 * @param root root of tree
 */
void encode_tree(MinHeapNode* root){
	int * aux, i;

	if(!isLeaf(root)){
		putchar('0');
		bits++;
		encode_tree(root->left);
		encode_tree(root->right);
	}else{
		putchar('1');
		bits++;
		putchar(' ');
		aux = (int *) malloc(8 * sizeof(int));
		aux = int2bin(aux, root->data);
		for(i = 7; i >= 0; i--){
#ifndef MB
			printf("%d", aux[i]);
#else
			xil_printf("%d", aux[i]);
#endif
			bits++;
		}
		free(aux);
		putchar(' ');
	}
}

void encode_text(char *file){
	int arr[MAX_TREE_HT], top = 0, i;
	MinHeapNode* root = min_heap.array[0];
	char code;

	i = 0;
	do
	{
		code = file[i];
		printCodes(root, arr, top, code);
		i++;
	}
	while(code != FILE_END_CODE);

	puts("< EOF");
}

// Traverses the Huffman tree pointed to by root and prints the file header and body
void HuffmanPrint(MinHeapNode* root, char *file)
{
#ifndef MB
	puts("\n--------------FILE OUT--------------\n");
	encode_tree(root);
	//printf("\ntree_size = %u\n", tree_size);
	puts("\n");
	encode_text(file);
	puts("");
#else
	// Print Huffman codes using the Huffman tree built above
	xil_printf("\n--------------FILE OUT--------------\n\n");
	encode_tree(root);
	xil_printf("\n\n");
	encode_text(file);
	xil_printf("\n");
#endif
}


// Each table record has two chars:
// - the first char is the codeword
// - the second char is the length of the codeword (in bits).
// The first call should be with pos = 1 in normal usage.
// The first call should be with code = 0 in normal usage.
// The number 8 is used because that's the number of bits in a char.
void tree_to_table(MinHeapNode* root, char *table, char code, short pos)
{

	if(root->left != NULL)
		tree_to_table(root->left, table, code, pos+1);
		//tree_to_table(root->left, table, code&~(1 << 8-pos), pos+1);

	if(root->right != NULL)
		tree_to_table(root->right, table, code|(1 << (8-pos)), pos+1);

	if(isLeaf(root))
	{
#ifdef debug
		printf("%d\t%d\t%d\n", root->data, code, pos-1);
#endif
		table[root->data << 2] = code;
		table[(root->data << 2)+1] = pos-1;
	}
}

// encodes orig into dest. returns the number of bytes written
// to dest.
unsigned encode_file(char *orig, char *dest, char *codewords)
{
	unsigned orig_ptr, dest_ptr;
	unsigned short word_len = 8, code_len, bits_used = 0;
	unsigned char ascii, outword = 0, code;

	orig_ptr = 0;
	dest_ptr = 0;
	do
	{
		ascii = orig[orig_ptr];
		code = codewords[ascii<<2];	// retrieve the code for this ascii char
		code_len = codewords[(ascii<<2)+1];	// and the length of the code

		if(bits_used + code_len > word_len)
		{
			outword |= code >> bits_used;
			dest[dest_ptr] = outword;
			dest_ptr++;
			outword = code << (word_len - bits_used);
			bits_used = code_len + bits_used - word_len;
		}
		else
		{
			outword |= code >> bits_used;
			bits_used += code_len;
		}

		orig_ptr++;
	}
	while(ascii != FILE_END_CODE);

	dest[dest_ptr] = outword;	// the last codeword has not been written yet. write it!
	return dest_ptr+1;
}

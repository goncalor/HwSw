#include <stdio.h>
#include <stdlib.h>

#include "huffman_code.h"
#include "misc.h"
#include "microblaze.h"

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
		printf("%d", arr[i]);
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
void encode_tree(struct MinHeapNode* root){
  int * aux;

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
    for(int i = 7; i >= 0; i--){
      printf("%d", aux[i]);
      bits++;
    }
    free(aux);
    putchar(' ');
  }
}

void encode_text(char *file){
	int arr[MAX_TREE_HT], top = 0, i;
	MinHeapNode* root = min_heap.array[0];
	i = 0;

	do
	{
		printCodes(root, arr, top, file[i]);
		i++;
	}
	while(file[i] != 0); // this should be FILE_END_CODE

	puts("EOF");
}

// The main function that builds a Huffman Tree and print codes by traversing
// the built Huffman Tree
void HuffmanCodes(char data[], unsigned freq[], int size)
{
	//  Construct Huffman Tree
	MinHeapNode* root = buildHuffmanTree(data, freq, size);
	bits = 0;

#ifndef MB
	// Print Huffman codes using the Huffman tree built above
	putchar('\n');
	puts("--------------FILE OUT--------------");
	putchar('\n');
	encode_tree(root);
	//printf("\ntree_size = %u\n", tree_size);
	puts("\r\n");
#endif

}

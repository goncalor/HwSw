#include <stdio.h>
#include <stdlib.h>

#include "huffman_code.h"
#include "define.h"

//#define debug

#define MAX_FILE_SIZE 7*1024*1024	// max 128 MB, external memory size

#ifndef MB
char file[MAX_FILE_SIZE];
#else
volatile char *file = (char *) (0xa8000000);
#endif

unsigned stats[256];	// for 128 MB we might have 2^27 occurrences of a single character, hence uint32

#ifndef MB
void read_file(char *filepath, char *file)
{
	FILE *fp;
	int i = 0;
	char c;

	fp = fopen(filepath, "r");
	if(fp == NULL)
	{
		puts("Failed to open input file.");
		exit(EXIT_FAILURE);
	}

	file[MAX_FILE_SIZE-1] = 0;	// make sure there is a \0
	while((c = fgetc(fp)) != EOF)
	{
		file[i] = c;
		i++;
	}

	file[i] = FILE_END_CODE;	// place end of file code, in case the file does not have it
}

void write_file(char *filepath, char *file, unsigned len)
{
	FILE *fp;
	int i;

	fp = fopen(filepath, "w");
	if(fp == NULL)
	{
		puts("Failed to open output file.");
		exit(EXIT_FAILURE);
	}

	for(i=0; i<len; i++)
		fputc(file[i], fp);
}
#endif

void compute_stats()
{
	int i;

	/* initialise count of each character */
	for(i=0; i<256; i++)
		stats[i] = 0;

	i = 0;
	do
	{
		stats[(int) file[i]]++;
		i++;
	}
	while(file[i] != FILE_END_CODE);

	stats[FILE_END_CODE] = 1;	// end of file code appears once. possibly not needed
}


int main(int argc, char **argv)
{
	int i, j;
	char ascii[256];

#ifndef MB
	if(argc != 2)
	{
		printf("Usage: %s file\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	read_file(argv[1], file);
	//puts(file);
#endif

	file[MAX_FILE_SIZE-1] = FILE_END_CODE;	// place end of file code. to be sage
	compute_stats();

#ifdef debug
#ifndef MB
	for(i=0; i<256; i++)
		printf("%d\t%u\n", i, stats[i]);
#else
	for(i=0; i<256; i++)
	{
		xil_printf("%d -> \n", i);
		xil_printf("%d\n", stats[i]);
	}
#endif
#endif

	// inplace trimming of "stats" (remove 0 count occurrences)
	for(i=0, j=0; i<256; i++)
	{
		if(stats[i] != 0)
		{
			stats[j] = stats[i];
			ascii[j] = i;
			j++;
		}
	}

	int size = j;

#ifdef debug
	puts("");
#ifndef MB
	for(i=0; i<j; i++)
		printf("%d\t%u\n", ascii[i], stats[i]);
#else
	for(i=0; i<256; i++)
	{
		xil_printf("%d -> ", ascii[i]);
		xil_printf("%d\n", stats[i]);
	}
#endif
#endif

	MinHeapNode* huffman_tree = buildHuffmanTree(ascii, (unsigned *) &stats, size);
	HuffmanPrint(huffman_tree, file);

	// let's reuse the stats array as a table for the codewords obtained from the tree
	char *encoding_table = (char *) stats;
	tree_to_table(huffman_tree, encoding_table, 0, 1);

	// encode the buffer
	unsigned outbuf_len = encode_file(file, file, encoding_table);

#ifndef MB
	write_file("outfile231431.txt", file, outbuf_len);
#endif

	// Print statistics of compression
	puts("----------------STATS---------------");
	putchar('\n');
	puts("Compressed file");

#ifndef MB
	printf("\tSize of compressed file (bits): %d\n", bits);
	printf("\tSize of compressed file (bytes): %u (without header)\n", outbuf_len);
#else
	xil_printf("\tSize of compressed file (bits): %d\n", bits);
	xil_printf("\tSize of compressed file (bytes): %u (without header)\n", outbuf_len);
#endif
	return 0;
}

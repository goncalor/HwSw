#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "huffman_code.h"
#include "define.h"
#include "file.h"

#ifdef MB
#include "htimer.h"
#include "fsl.h"	// ??
#endif

int main(int argc, char **argv)
{
	int i, j;
	char ascii[256];

	#ifndef MB
	char * file = malloc(MAX_FILE_SIZE*sizeof(char));
	char * out;
	#else
	char *file = (char *) (0xa8100000);
	u32 timeL[11], timeH[11];
	init_timer(1);
	start_timer(1);
	#endif


	#ifndef MB
	if(argc != 2)
	{
		printf("Usage: %s file\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	read_file(argv[1], file);
	//puts(file);
	#endif

	#ifdef MB
	timeH[0] = get_timer64_val(&(timeL[0]));
	#endif

	file[MAX_FILE_SIZE-1] = FILE_END_CODE;	// place end of file code. to be safe

	#ifndef MB
	compute_stats((char *) file);
	#else

	u32 * file_aux = (u32 *) file;

	cputfsl(FILE_END_CODE, 0);	// send FILE_END_CODE for the accelarator to recognise it

	while(((*file_aux & 0xFF000000)>>24 != FILE_END_CODE) &&
			((*file_aux & 0x00FF0000)>>16 != FILE_END_CODE) &&
			((*file_aux & 0x0000FF00)>>8 != FILE_END_CODE) &&
			((*file_aux & 0x000000FF) != FILE_END_CODE) )
	{
		putfsl(*file_aux, 0);
		file_aux++;
	}

	int tmp;
	for(i=0; i<256; i = i + 2)
	{
		getfsl(tmp, 0);

		stats[i] = tmp & 0xFFFF0000>>16;
		stats[i + 1] = tmp & 0x0000FFFF;

		xil_printf("stats %d -> %d\n", i, stats[i]);
		xil_printf("stats %d -> %d\n", i + 1, stats[i + 1]);
	}

	timeH[1] = get_timer64_val(&(timeL[1]));
	#endif

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

	#ifdef MB
	timeH[2] = get_timer64_val(&(timeL[2]));
	#endif

	MinHeapNode* huffman_tree = buildHuffmanTree(ascii, (unsigned *) &stats, size);

	#ifdef MB
	timeH[3] = get_timer64_val(&(timeL[3]));
	#endif

	#ifdef MB
	timeH[4] = get_timer64_val(&(timeL[4]));
	#endif

	//HuffmanPrint(huffman_tree, (char *)file);

	#ifdef MB
	timeH[5] = get_timer64_val(&(timeL[5]));
	#endif

	#ifdef MB
	timeH[6] = get_timer64_val(&(timeL[6]));
	#endif

	// let's reuse the stats array as a table for the codewords obtained from the tree
	char *encoding_table = (char *) stats;
	tree_to_table(huffman_tree, encoding_table, 0, 1);

	#ifdef MB
	timeH[7] = get_timer64_val(&(timeL[7]));
	#endif

	#ifdef MB
	timeH[8] = get_timer64_val(&(timeL[8]));
	#endif

	// encode the buffer
	unsigned outbuf_len = encode_file((char *)file, (char *)file, encoding_table);

	#ifdef MB
	timeH[9] = get_timer64_val(&(timeL[9]));
	#endif

	#ifndef MB
	out = strcat(argv[1], "_comp");
	write_file(out, file, outbuf_len);
	#else
	timeH[10] = get_timer64_val(&(timeL[10]));

	xil_printf("start compute_stats(): %d ms\n", (int) conv2_cycles_to_msecs(timeH[0], timeL[0]));
	xil_printf("finish compute_stats(): %d ms\n", (int) conv2_cycles_to_msecs(timeH[1], timeL[1]));
	xil_printf("start buildHuffmanTree(): %d ms\n", (int) conv2_cycles_to_msecs(timeH[2], timeL[2]));
	xil_printf("finish buildHuffmanTree(): %d ms\n", (int) conv2_cycles_to_msecs(timeH[3], timeL[3]));
	xil_printf("start HuffmanPrint(): %d ms\n", (int) conv2_cycles_to_msecs(timeH[4], timeL[4]));
	xil_printf("finish HuffmanPrint(): %d ms\n", (int) conv2_cycles_to_msecs(timeH[5], timeL[5]));
	xil_printf("start tree_to_table(): %d ms\n", (int) conv2_cycles_to_msecs(timeH[6], timeL[6]));
	xil_printf("finish tree_to_table(): %d ms\n", (int) conv2_cycles_to_msecs(timeH[7], timeL[7]));
	xil_printf("start encode_file(): %d ms\n", (int) conv2_cycles_to_msecs(timeH[8], timeL[8]));
	xil_printf("finish encode_file(): %d ms\n", (int) conv2_cycles_to_msecs(timeH[9], timeL[9]));
	xil_printf("time elapsed: %d ms\n", (int) conv2_cycles_to_msecs(timeH[10], timeL[10]));
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
	xil_printf("\tSize of compressed file (bytes): %d (without header)\n", outbuf_len);
	#endif
	return 0;
}

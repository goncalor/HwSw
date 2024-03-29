#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xparameters.h>
#include <xstatus.h>

#include "huffman_code.h"
#include "define.h"
#include "file.h"

#ifdef MB
#include "htimer.h"
#include "fsl.h"
#endif

#define DEBUG_CORE_ID 10

/**
 * Global to sync cores
 */
volatile char *sharedstate = (char *)XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR;
volatile char *sharedstate1 = (char *)XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 1;
volatile char *sharedstate2 = (char *)XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 2;
volatile char *sharedstate3 = (char *)XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 3;

/**
 * Base address to share results
 */
u32 * base_addr1 = (u32 *) (XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 4);
u32 * base_addr2 = (u32 *) (XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 0x400 + 4);

char * shared_tree = (char *) (XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR);

int main(int argc, char **argv)
{
	int i, j;
	char ascii[256];

	/**
	 * initial synch. core 3 is assumed to be the last to be launched
	 */
	while(*sharedstate != 0x3);

	#ifndef MB
	char * file = malloc(MAX_FILE_SIZE*sizeof(char));
	char * out;
	#else
	char *file = (char *) (XPAR_MCB_DDR2_S0_AXI_BASEADDR + 0x100000);
	char sizeoffile[10];
	u32 timeL[12], timeH[12];
	if(init_timer(1) == XST_FAILURE){
		xil_printf("timer :(\n");
		return 0;
	}
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

	i = 0;

	while(*file != LAST_DIGIT)
	{
		sizeoffile[i++] = *file;
		file++;
	}
	file++;
	//xil_printf("after LAST_DIGIT %d\n", *file);
	sizeoffile[i] = '\0';

	int size;
	int orig_size = atoi(sizeoffile);

	// Calculate the start pointer and end pointer
	size = orig_size/4 * XPAR_CPU_ID;

	char *begin = file + size;
	char *file_aux = file + size;
	char *end = file_aux + orig_size/4;

	//---------- start FSL ---------

	cputfsl(FILE_END_CODE, 0);	// send FILE_END_CODE for the accelarator to recognise it

	char to_send[4];

	for(i=0; file_aux < end; file_aux++, i++)
	{
		to_send[i] = *file_aux;

		if(i==3)
		{
			putfsl(*((u32*)to_send), 0);
			i=-1;
		}
	}

	// send remaining bytes
	to_send[i] = FILE_END_CODE;
	putfsl(*((u32*)to_send), 0);
	//xil_printf("Enviei o file end code\n");

	//------ receive results ------

	int tmp;
	for(i=0; i < 256; i = i + 2)
	{
		getfsl(tmp, 0);

		stats[i] = (tmp & 0xFFFF0000) >> 16;
		stats[i + 1] = tmp & 0x0000FFFF;

		//xil_printf("stats %d -> %d\n", i, stats[i]);
		//xil_printf("stats %d -> %d\n", i + 1, stats[i + 1]);
	}

	//------- END FSL -----------


	//------- start SYNC and SUM

	// Sync with core 1
	// Receive from core 1
	while(*sharedstate1 != 0x1)
		;
	*sharedstate1 = 0x0;

	u32 * section_aux = base_addr1;
	// Add core 1 results with local
	for(i = 0; i < 256; i++){
		stats[i] += *section_aux;
		section_aux++;
	}

#if XPAR_CPU_ID == DEBUG_CORE_ID
	xil_printf("counts on core %d after synching with core 1\n", XPAR_CPU_ID);
	for(i=0; i<256; i++) {
		xil_printf("%d -> ", i);
		xil_printf("%d\n", stats[i]);
	}
#endif

	// Sync with core 2
	// Receive from core 2
	while(*sharedstate3 != 0x1)
		;
	*sharedstate3 = 0x0;

	section_aux = base_addr2;
	// Add core 2 results with local
	for(i = 0; i < 256; i++){
		stats[i] += *section_aux;
		section_aux++;
	}

	stats[FILE_END_CODE] = 1;

#if XPAR_CPU_ID == DEBUG_CORE_ID
	xil_printf("counts on core %d after synching all\n", XPAR_CPU_ID);
	for(i=0; i<256; i++) {
		xil_printf("%d -> ", i);
		xil_printf("%d\n", stats[i]);
	}
#endif

	//------- END SYNC and SUM

	timeH[1] = get_timer64_val(&(timeL[1]));
	#endif

	#ifdef debug
	#ifndef MB
	for(i=0; i<256; i++)
		printf("%d\t%u\n", i, stats[i]);
	#else
	for(i=0; i<256; i++)
	{
		//xil_printf("%d -> \n", i);
		//xil_printf("%d\n", stats[i]);
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

	size = j;

	#ifdef debug
	puts("");
	#ifndef MB
	for(i=0; i<j; i++)
		printf("%d\t%u\n", ascii[i], stats[i]);
	#else
	for(i=0; i<256; i++)
	{
		//xil_printf("%d -> ", ascii[i]);
		//xil_printf("%d\n", stats[i]);
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
	//char *encoding_table = (char *) stats;
	tree_to_table(huffman_tree, shared_tree, 0, 1);

	// inform other cores the tree is ready to be used
	*sharedstate = 2;

	#ifdef MB
	timeH[7] = get_timer64_val(&(timeL[7]));
	#endif

	#ifdef MB
	timeH[8] = get_timer64_val(&(timeL[8]));
	#endif

	// encode the buffer
	unsigned outbuf_len = encode_file((char *) begin,
			(char *) file+orig_size+orig_size*XPAR_CPU_ID+4,
			shared_tree, orig_size/4);

	*(unsigned*)((char *) file+orig_size+orig_size*XPAR_CPU_ID) = outbuf_len;

	// wait until all cores finish encoding their parts
	while(*sharedstate1!=4 || *sharedstate2!=4 || *sharedstate3!=4)
		;

	#ifdef MB
	timeH[9] = get_timer64_val(&(timeL[9]));
	#endif

	// write (sample of) compressed memory sections to stdout
	char aux_mem_pos;
	for(i=0; i<4; i++)
	{
		xil_printf("compressed region %d. total size %d (bytes)\n", i,
				*(unsigned*)((char *) file+orig_size+orig_size*i));
		int k;
		for(k=0; k<10; k++)
		{
			int j;
			aux_mem_pos = *((char*) file+orig_size+orig_size*i+4+k);

			for(j=0; j<8; j++)
			{
				xil_printf("%d", (aux_mem_pos>>(7-j)&1));
			}
			xil_printf(" ");
		}
		xil_printf("\n\n");
	}

	#ifndef MB
	out = strcat(argv[1], "_comp");
	write_file(out, file, outbuf_len);
	#else

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
	xil_printf("time elapsed: %d ms\n", (int) conv2_cycles_to_msecs(timeH[9], timeL[9]));
	#endif

	// Print statistics of compression
	//xil_printf("----------------STATS---------------");
	//putchar('\n');
	//xil_printf("Compressed file");

	#ifndef MB
	printf("\tSize of compressed file (bits): %d\n", bits);
	printf("\tSize of compressed file (bytes): %u (without header)\n", outbuf_len);
	printf("SIZEOF: %lu\n", sizeof(heap_nodes));
	#else
	//xil_printf("\tSize of compressed file (bits): %d\n", bits);
	//xil_printf("\tSize of compressed file (bytes): %d (without header)\n", outbuf_len);
	#endif

	return 0;
}

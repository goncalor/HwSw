#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "huffman_code.h"
#include "define.h"
#include "file.h"

#ifdef MB
#include "htimer.h"
#include "fsl.h"
#endif

/**
 * Global to sync all cores
 */
volatile unsigned int *sharedstate = (unsigned int *)XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR;

/**
 * Global to sync core 0 and 1
 */
volatile unsigned int *sharedstate1 = (unsigned int *)XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR + 1;

/**
 * Global to sync core 0 and 2
 */
volatile unsigned int *sharedstate3 = (unsigned int *)XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR + 2;

/**
 * Base address to share results
 */
char * base_addr1 = (char *) (0xa8000000);
char * base_addr2 = (char *) (0xa800003F);

/* Must always be CPU0
   Code below relies on that */
#if XPAR_CPU_ID != 0
#undef XPAR_CPU_ID
#define XPAR_CPU_ID 0
#endif

int main(int argc, char **argv)
{
	int i, j;
	char ascii[256];
  *sharedstate = 0x1;

	#ifndef MB
	char * file = malloc(MAX_FILE_SIZE*sizeof(char));
	char * out;
	#else
	char *file = (char *) (0xa8100000);
  char sizeoffile[10];
	u32 timeL[12], timeH[12];
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

  i = 0;

  // Get size of file
  while(((*file_aux & 0xFF000000)>>24 != LAST_DIGIT) &&
      ((*file_aux & 0x00FF0000)>>16 != LAST_DIGIT) &&
      ((*file_aux & 0x0000FF00)>>8 != LAST_DIGIT) &&
      ((*file_aux & 0x000000FF) != LAST_DIGIT) )
  {
    sizeoffile[i] = file_aux[0];
    sizeoffile[i + 1] = file_aux[1];
    sizeoffile[i + 2] = file_aux[2];
    sizeoffile[i + 3] = file_aux[3];
    file_aux++;
    i = i + 4;
  }

  // Let it compute size if \n is in the last 32 bits read
  sizeoffile[i] = file_aux[0];
  sizeoffile[i + 1] = file_aux[1];
  sizeoffile[i + 2] = file_aux[2];
  sizeoffile[i + 3] = file_aux[3];
  file_aux++;

  size = atoi(sizeoffile);

  // Calculate the start pointer and end pointer
  size = size/4 * XPAR_CPU_ID;
  end = size + size/4;

	cputfsl(FILE_END_CODE, 0);	// send FILE_END_CODE for the accelarator to recognise it

	while(((*file_aux & 0xFF000000)>>24 != FILE_END_CODE) &&
			((*file_aux & 0x00FF0000)>>16 != FILE_END_CODE) &&
			((*file_aux & 0x0000FF00)>>8 != FILE_END_CODE) &&
			((*file_aux & 0x000000FF) != FILE_END_CODE) )
	{
		putfsl(*file_aux, 0);
		file_aux++;
	}

	putfsl(*file_aux, 0);	// put the last byte, which contains FILE_END_CODE

	int tmp;
	for(i=0; i<256; i = i + 2)
	{
		getfsl(tmp, 0);

		stats[i] = (tmp & 0xFFFF0000) >> 16;
		stats[i + 1] = tmp & 0x0000FFFF;

		//xil_printf("stats %d -> %d\n", i, stats[i]);
		//xil_printf("stats %d -> %d\n", i + 1, stats[i + 1]);
	}

  // Sync with core 1
  // Receive from core 1
  while(*sharedstate1 != 0x1);
  *sharedstate1 = 0x0;

  char * section_aux = base_addr1;

  // Add core 1 results with local
  for(i = 0; i < 256; i++){
    stats[i] += *section_aux;
    section_aux++;
  }

  // Sync with core 2
  // Receive from core 2
  while(*sharedstate3 != 0x1);
  *sharedstate3 = 0x0;

  section_aux = base_addr2;

  // Add core 2 results with local
  for(i = 0; i < 256; i++){
    stats[i] += *section_aux;
    section_aux++;
  }

  stats[FILE_END_CODE] = 1;

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

  // Sync with all cores
  *sharedstate = 0x0;
  while(*sharedstate != 0x1);

  // Put table in internal memory

	#ifdef MB
	timeH[7] = get_timer64_val(&(timeL[7]));
	#endif

	#ifdef MB
	timeH[8] = get_timer64_val(&(timeL[8]));
	#endif

	// encode the buffer
	unsigned outbuf_len = encode_file((char *)file, (char *)file, encoding_table);

  // Write owns outbuf_len to stdout
  // then read from core 2, 3 and 4 memory section

  // Print everything in order to stdout

	#ifdef MB
	timeH[9] = get_timer64_val(&(timeL[9]));
	#endif

	#ifndef MB
	out = strcat(argv[1], "_comp");
	write_file(out, file, outbuf_len);
	#else
	timeH[10] = get_timer64_val(&(timeL[10]));
	timeH[11] = get_timer64_val(&timeL[11]);

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
	xil_printf("time elapsed: %d ms\n", (int) conv2_cycles_to_msecs(timeH[11], timeL[11]));
	#endif

	// Print statistics of compression
	puts("----------------STATS---------------");
	putchar('\n');
	puts("Compressed file");

	#ifndef MB
	printf("\tSize of compressed file (bits): %d\n", bits);
	printf("\tSize of compressed file (bytes): %u (without header)\n", outbuf_len);
  printf("SIZEOF: %lu\n", sizeof(heap_nodes));
	#else
	xil_printf("\tSize of compressed file (bits): %d\n", bits);
	xil_printf("\tSize of compressed file (bytes): %d (without header)\n", outbuf_len);
	#endif
	return 0;
}

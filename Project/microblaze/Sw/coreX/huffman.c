#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xparameters.h>
#include <xstatus.h>

#include "huffman_code.h"
#include "define.h"

#include "fsl.h"

/**
 * Global to sync all cores
 */
#if XPAR_CPU_ID != 1
volatile unsigned int *sharedstate =
		(unsigned int *) XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR;
#endif

/**
 * Global to sync core 0 and 1
 */
#if XPAR_CPU_ID == 0
volatile unsigned int *sharedstate1 =
		(unsigned int *) XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 1;
#endif

/**
 * Global to sync core 0 and 2
 */
volatile unsigned int *sharedstate3 =
		(unsigned int *) XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 3;

/**
 * Global to sync core 2 and 3
 */
#if XPAR_CPU_ID == 2
volatile unsigned int *sharedstate2 = (unsigned int *)XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 2;
#elif XPAR_CPU_ID == 3
volatile unsigned int *sharedstate2 = (unsigned int *)XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 2;
#endif

/**
 * Base address to share results
 */
#if XPAR_CPU_ID == 0
u32 * base_addr = (u32 *) (XPAR_MCB_DDR2_S0_AXI_BASEADDR);
#elif XPAR_CPU_ID == 2
u32 * base_addr = (u32 *) (XPAR_MCB_DDR2_S0_AXI_BASEADDR + 0x400);
u32 * base_addr3 = (u32 *) (XPAR_MCB_DDR2_S0_AXI_BASEADDR + 0x800);
#elif XPAR_CPU_ID == 3
u32 * base_addr = (u32 *) (XPAR_MCB_DDR2_S0_AXI_BASEADDR + 0x800);
#endif

/* We are either CPU1, CPU2, CPU3 or CPU4.
 Code below relies on the value being one of these 4 */
#if XPAR_CPU_ID == 4
#undef XPAR_CPU_ID
#define XPAR_CPU_ID 3
#endif

int main(int argc, char **argv) {
	//char ascii[256];
	char sizeoffile[10];
	char *file = (char *) (XPAR_MCB_DDR2_S0_AXI_BASEADDR + 0x100000);

	u32 * file_aux = (u32 *) file;
	int i = 0;

	while (*file != LAST_DIGIT) {
		sizeoffile[i++] = *file;
		file++;
	}
	file++;
	sizeoffile[i] = '\0';
	file_aux = (u32 *) file;

	int size;
	int orig_size = atoi(sizeoffile);

	// Calculate the start pointer and end pointer
	size = orig_size / 4 * XPAR_CPU_ID;

	file_aux = file_aux + size;
	#if XPAR_CPU_ID == 3
	u32 *end = file_aux + size + orig_size / 4 + (orig_size % 4);
	#else
	u32 *end = file_aux + size + orig_size / 4;
	#endif

	  //---------- start FSL ---------

	cputfsl(FILE_END_CODE, 0);
	// send FILE_END_CODE for the accelarator to recognise it

	while ((((*file_aux & 0xFF000000) >> 24 != FILE_END_CODE) &&
			((*file_aux	& 0x00FF0000) >> 16 != FILE_END_CODE) &&
			((*file_aux & 0x0000FF00) >> 8 != FILE_END_CODE) &&
			((*file_aux & 0x000000FF) != FILE_END_CODE)))
	{
		putfsl(*file_aux, 0);
		file_aux++;
		if(file_aux >= end)
			break;
	}

	putfsl(FILE_END_CODE, 0);
	// put the last byte, which contains FILE_END_CODE

	int tmp;
	for (i = 0; i < 256; i = i + 2) {
		getfsl(tmp, 0);

		stats[i] = (tmp & 0xFFFF0000) >> 16;
		stats[i + 1] = tmp & 0x0000FFFF;

		//xil_printf("stats %d -> %d\n", i, stats[i]);
		//xil_printf("stats %d -> %d\n", i + 1, stats[i + 1]);
	}

	  //---------- END FSL ---------

	u32 *section_aux = (u32 *) base_addr;

#if XPAR_CPU_ID == 0
	// Send to core 0:
	// Write to memory section
	// Sync with core 0
	for (i = 0; i < 256; i++) {
		*section_aux = stats[i];
		section_aux++;
	}
	*sharedstate1 = 0x1;
	while (*sharedstate1 != 0x0)
		;
#endif

#if XPAR_CPU_ID == 3
	// Send to core 3:
	// Write to memory section
	// Sync with core 3
	for(i = 0; i < 256; i++) {
		*section_aux = stats[i];
		section_aux++;
	}
	*sharedstate2 = 0x1;
	while(*sharedstate2 != 0x0);
#elif XPAR_CPU_ID == 2
	while(*sharedstate2 != 0x1);
	*sharedstate2 = 0x0;
	// Add results from core 3 to core 2 and write to its memory section
	section_aux = base_addr3;
	u32 * section_aux_2 = (u32*) base_addr;
	for(i = 0; i < 256; i++) {
		*section_aux_2 = stats[i] + *section_aux;
		section_aux++;
		section_aux_2++;
	}
	*sharedstate3 = 0x1;
	while(*sharedstate3 != 0x0);
#endif

#ifdef debug
	for(i=0; i<256; i++) {
		xil_printf("%d -> \n", i);
		xil_printf("%d\n", stats[i]);
	}
#endif

	// Sync with core 0 (wait until table is built)
	while (*sharedstate != 0x0)
		;
	*sharedstate = 0x1;

	// ponteiro para a memória externa com a tabela de
	// codificação completa.
	char *encoding_table = NULL;	//TODO

	// encode the buffer
	unsigned outbuf_len = encode_file((char *) file, (char *) file,
			encoding_table);

	// Write outbuf_len to specific memory section so that core 1 can read and
	// print to screen

	return 0;
}

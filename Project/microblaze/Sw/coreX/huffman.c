#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xparameters.h>
#include <xstatus.h>

#include "huffman_code.h"
#include "define.h"

#include "fsl.h"

#define DEBUG_CORE_ID 2

/**
 * Global to sync all cores
 */
#if XPAR_CPU_ID != 1
volatile char *sharedstate =
		(char *) XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR;
#endif

/**
 * Global to sync core 0 and 1
 */
volatile char *sharedstate1 =
		(char *) XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 1;

/**
 * Global to sync core 0 and 2
 */
volatile char *sharedstate3 =
		(char *) XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 3;

/**
 * Global to sync core 2 and 3
 */
#if XPAR_CPU_ID == 2 || XPAR_CPU_ID == 3
volatile char *sharedstate2 = (char *)XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 2;
#endif

u32 * sync0 = (u32 *) XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 4;
u32 * sync1 = (u32 *) XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 5;
u32 * sync2 = (u32 *) XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 6;
u32 * sync3 = (u32 *) XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 7;

/**
 * Base address to share results
 */
#if XPAR_CPU_ID == 0
u32 * base_addr = (u32 *) (XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 4);
#elif XPAR_CPU_ID == 2
u32 * base_addr = (u32 *) (XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 0x400 + 4);
u32 * base_addr3 = (u32 *) (XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 0x800 + 4);
#elif XPAR_CPU_ID == 3
u32 * base_addr = (u32 *) (XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR + 0x800 + 4);
#endif


int main(int argc, char **argv) {
	//char ascii[256];
	char sizeoffile[10];
	char *file = (char *) (XPAR_MCB_DDR2_S0_AXI_BASEADDR + 0x100000);

	/*
#if XPAR_CPU_ID == 0
	*sync1 = 0x1;
#elif XPAR_CPU_ID == 2
	*sync2 = 0x1;
#elif XPAR_CPU_ID == 3
	*sync3 = 0x1;
#endif

	//while(*sync0 != 0x1 || *sync1 != 0x1 || *sync2 != 0x1 || *sync3 != 0x1);
*/

#if XPAR_CPU_ID == DEBUG_CORE_ID
	xil_printf("core %d antes do last digit\n", XPAR_CPU_ID);
#endif

#if XPAR_CPU_ID == DEBUG_CORE_ID
	char *debug_ptr;
	for(debug_ptr=file; *debug_ptr!=FILE_END_CODE; debug_ptr++)
		xil_printf("0x%x ", *debug_ptr);
	xil_printf("\n");
#endif

	int i = 0;
	while (*file != LAST_DIGIT) {
		sizeoffile[i++] = *file;
		file++;
	}
	file++;
	sizeoffile[i] = '\0';

	int size;
	int orig_size = atoi(sizeoffile);

#if XPAR_CPU_ID == DEBUG_CORE_ID
	xil_printf("orig_size %d\n", orig_size);
#endif

	// Calculate the start pointer and end pointer
	size = orig_size / 4 * XPAR_CPU_ID;

	char *file_aux = file + size;
	#if XPAR_CPU_ID == 3
	char *end = file_aux + orig_size/4 + orig_size%4;
	#else
	char *end = file_aux + orig_size/4;
	#endif

	  //---------- start FSL ---------

#if XPAR_CPU_ID == DEBUG_CORE_ID
	xil_printf("core %d começa FSL\n", XPAR_CPU_ID);
#endif

	// send FILE_END_CODE for the accelarator to recognise it
	cputfsl(FILE_END_CODE, 0);
	char to_send[4];

	for(i=0; file_aux < end; file_aux++, i++)
	{
		to_send[i] = *file_aux;

#if XPAR_CPU_ID == DEBUG_CORE_ID
	xil_printf("to_send[i] %x\n", to_send[i]);
#endif

		if(i==3)
		{
			putfsl(*((u32*)to_send), 0);
			i=-1;

#if XPAR_CPU_ID == DEBUG_CORE_ID
	xil_printf("to_send %x\n", *((u32*)to_send));
#endif
		}
	}

	// send remaining bytes
	to_send[i] = FILE_END_CODE;
	putfsl(*((u32*)to_send), 0);

	//------ receive results ------

#if XPAR_CPU_ID == DEBUG_CORE_ID
	xil_printf("core %d começa a receber resultados\n", XPAR_CPU_ID);
#endif

	int tmp;
	for (i = 0; i < 256; i = i + 2)
	{
		getfsl(tmp, 0);

		stats[i] = (tmp & 0xFFFF0000) >> 16;
		stats[i + 1] = tmp & 0x0000FFFF;

		//xil_printf("stats %d -> %d\n", i, stats[i]);
		//xil_printf("stats %d -> %d\n", i + 1, stats[i + 1]);
	}

	  //---------- END FSL ---------

	u32 *section_aux = base_addr;

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

#if XPAR_CPU_ID == 2

#if XPAR_CPU_ID == DEBUG_CORE_ID
	xil_printf("shared states seem by core %d\n", XPAR_CPU_ID);
	xil_printf("sharedstate0 %d\n", *sharedstate);
	xil_printf("sharedstate1 %d\n", *sharedstate1);
	xil_printf("sharedstate2 %d\n", *sharedstate2);
	xil_printf("sharedstate3 %d\n", *sharedstate3);
#endif

	// wait for core 3
	i=0;
	while(*sharedstate2 != 0x1)
	{
	/*	int k;
		for(k=0; k<30000;k++)
			;
		i%16 ? xil_printf("%d ", i++) : xil_printf("%d\n", i++);*/
	}
	*sharedstate2 = 0x0;

#if XPAR_CPU_ID == DEBUG_CORE_ID
	xil_printf("core %d synched with core 3\n", XPAR_CPU_ID);
#endif

#if XPAR_CPU_ID == DEBUG_CORE_ID
	for(debug_ptr=file; *debug_ptr!=FILE_END_CODE; debug_ptr++)
		xil_printf("0x%x ", *debug_ptr);
	xil_printf("\n");
#endif

	// Add results from core 3 to core 2 and write to its memory section
	section_aux = base_addr3;	// points to results written by core 3
	u32 * section_aux_2 = (u32*) base_addr;	// points to results written by core 2 itself
	for(i = 0; i < 256; i++) {
		*section_aux_2 = stats[i] + *section_aux;
		section_aux++;
		section_aux_2++;
	}

#if XPAR_CPU_ID == DEBUG_CORE_ID
	xil_printf("counts that core %d read from 3's shared memory\n", XPAR_CPU_ID);
	for(i=0; i<256; i++) {
		xil_printf("%d -> ", i);
		xil_printf("%d\n", base_addr3[i]);
	}

	xil_printf("results on core %d after summing with core 3 results\n", XPAR_CPU_ID);
	for(i=0; i<256; i++) {
		xil_printf("%d -> ", i);
		xil_printf("%d\n", base_addr[i]);
	}
#endif

	// to sync with core 0
	*sharedstate3 = 0x1;
	while(*sharedstate3 != 0x0)
		;

#if XPAR_CPU_ID == DEBUG_CORE_ID
	xil_printf("core %d synched with core 0. continue execution\n", XPAR_CPU_ID);
#endif

#endif

#if XPAR_CPU_ID == 3
	// Send to core 3:
	// Write to memory section
	// Sync with core 3
	for(i = 0; i < 256; i++) {
		*section_aux = stats[i];
		section_aux++;
	}

#if XPAR_CPU_ID == DEBUG_CORE_ID
	xil_printf("counts that core %d wrote to shared memory\n", XPAR_CPU_ID);
	for(i=0; i<256; i++) {
		xil_printf("%d -> ", i);
		xil_printf("%d\n", base_addr[i]);
	}
#endif

	*sharedstate2 = 0x1;
	while(*sharedstate2 != 0x0)
		;

#endif


#ifdef debug
	for(i=0; i<256; i++) {
		xil_printf("%d -> ", i);
		xil_printf("%d\n", stats[i]);
	}
#endif

	// Sync with core 0 (wait until table is built)
	/*while (*sharedstate != 0x0)
		;
	*sharedstate = 0x1;*/

	// ponteiro para a memÃ³ria externa com a tabela de
	// codificaÃ§Ã£o completa.
	char *encoding_table = NULL;	//TODO

	// encode the buffer
	//unsigned outbuf_len = encode_file((char *) file, (char *) file, encoding_table);

	// Write outbuf_len to specific memory section so that core 1 can read and
	// print to screen

	return 0;
}

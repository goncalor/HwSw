#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "huffman_code.h"
#include "define.h"
#include "file.h"

#include "htimer.h"
#include "fsl.h"

/**
 * Global to sync all cores
 */
#if XPAR_CPU_ID != 0
volatile unsigned int *sharedstate = (unsigned int *)XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR;
#endif

/**
 * Global to sync core 0 and 1
 */
#if XPAR_CPU_ID == 1
  volatile unsigned int *sharedstate1 = (unsigned int *)XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR + 1;
#endif

/**
 * Global to sync core 0 and 2
 */
volatile unsigned int *sharedstate3 = (unsigned int *)XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR + 2;

/**
 * Global to sync core 2 and 3
 */
#if XPAR_CPU_ID == 2
  volatile unsigned int *sharedstate2 = (unsigned int *)XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR + 2;
#else if XPAR_CPU_ID == 3
  volatile unsigned int *sharedstate2 = (unsigned int *)XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR + 2;
#endif

/**
 * Base address to share results
 */
#if XPAR_CPU_ID == 1
  char * base_addr1 = (char *) (0xa8000000);
#else if XPAR_CPU_ID == 2
  char * base_addr = (char *) (0xa800003F);
  char * base_addr3 = (char *) (0xa800007E);
#else if XPAR_CPU_ID == 3
  char * base_addr = (char *) (0xa800007E);
#endif

/* We are either CPU1, CPU2, CPU3 or CPU4.
   Code below relies on the value being one of these 4 */
#if XPAR_CPU_ID == 4
#undef XPAR_CPU_ID
#define XPAR_CPU_ID 3
#endif

int main(int argc, char **argv)
{
	int i, j, size, end;
	char ascii[256];
  char sizeoffile[10];
	char *file = (char *) (0xa8100000);

	u32 * file_aux = (u32 *) file;
  i = 0;

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
  sizeoffile[i + 4] = 0;

  size = atoi(sizeoffile);

  // Calculate the start pointer and end pointer
  size = size/4 * XPAR_CPU_ID;
  end = size + size/4;

	cputfsl(FILE_END_CODE, 0);	// send FILE_END_CODE for the accelarator to recognise it

	while((((*file_aux & 0xFF000000)>>24 != FILE_END_CODE) &&
			   ((*file_aux & 0x00FF0000)>>16 != FILE_END_CODE) &&
			   ((*file_aux & 0x0000FF00)>>8 != FILE_END_CODE) &&
			   ((*file_aux & 0x000000FF) != FILE_END_CODE)) ||
         size < end )
	{
		putfsl(*file_aux, 0);
		file_aux++;
    size = size + 4;
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

	stats[FILE_END_CODE] = 1;

  char * section_aux = base_addr;

  #if XPAR_CPU_ID == 1
    // Send to core 0:
    // Write to memory section
    // Sync with core 0
    for(i = 0; i < 256; i++){
      section_aux = stats[i];
      section_aux++;
    }
    *sharedstate1 = 0x1;
    while(*sharedstate1 != 0x0);
  #endif

  #if XPAR_CPU_ID == 3
    // Send to core 3:
    // Write to memory section
    // Sync with core 3
    for(i = 0; i < 256; i++){
      *section_aux = stats[i];
      section_aux++;
    }
    *sharedstate2 = 0x1;
    while(*sharedstate2 != 0x0);
  #else if XPAR_CPU_ID == 2
    while(*sharedstate2 != 0x1);
    *sharedstate2 = 0x0;
    // Add results from core 3 to core 2 and write to its memory section
    section_aux = base_addr3;
    char * section_aux_2 = base_addr;
    for(i = 0; i < 256; i++){
      *section_aux_2 = stats[i] + *section_aux;
      section_aux++;
      section_aux_2++;
    }
    *sharedstate3 = 0x1;
    while(*sharedstate3 != 0x0);
  #endif

	#ifdef debug
	for(i=0; i<256; i++){
		xil_printf("%d -> \n", i);
		xil_printf("%d\n", stats[i]);
	}
	#endif

  // Sync with core 0 (wait until table is built)
  while(*sharedstate != 0x0);
  *sharedstate = 0x1;

  // ponteiro para a memória externa com a tabela de
  // codificação completa.
  char *encoding_table = NULL;

	// encode the buffer
	unsigned outbuf_len = encode_file((char *)file, (char *)file, encoding_table);

  // Write outbuf_len to specific memory section so that core 1 can read and
  // print to screen

	return 0;
}

/*
 * fsl_test.c
 *
 *  Created on: 22 de Abr de 2015
 *      Author: hwsw
 */

#include <stdio.h>
//#include <stdlib.h>
#include "fsl.h"

#define FILE_END_CODE 0

int main(void)
{
	int i, j;
	//char file[] = "ACAGAATAGAGAX\0";
	//char file[] = "ACAG AATA GAGA X\0";

	//char file[] = "AGACAATAGAGAX\0";
	char file[] = "ABBCCCDDDD\0";

	unsigned * file_aux = (unsigned *) file;
	static unsigned stats[256];
	static unsigned tmp;

	cputfsl(FILE_END_CODE, 0);	// send FILE_END_CODE for the accelarator to recognise it

	while(((*file_aux & 0xFF000000)>>24 != FILE_END_CODE) &&
			((*file_aux & 0x00FF0000)>>16 != FILE_END_CODE) &&
			((*file_aux & 0x0000FF00)>>8 != FILE_END_CODE) &&
			((*file_aux & 0x000000FF) != FILE_END_CODE) )
	{
		putfsl(*file_aux, 0);
		file_aux++;
	}

	putfsl(*file_aux, 0);
//	putfsl(FILE_END_CODE, 0);

	// xil_printf("unsigned size: %d B\n\n", sizeof(unsigned));

	for(i=0; i<256; i = i + 2)
	{
		getfsl(tmp, 0);
		//xil_printf("i: %d %x\n", i, tmp);

		stats[i] = (tmp & 0xFFFF0000) >> 16;
		stats[i + 1] = tmp & 0x0000FFFF;

		//xil_printf("stats %d -> %d\n", i, tmp);
		//xil_printf("stats %d -> %d\n", i, stats[i]);
		//xil_printf("stats %d -> %d\n", i + 1, stats[i + 1]);
	}

	for (j = 0; j < 256; ++j) {
		xil_printf("stats %d -> %d\n", j, stats[j]);
	}
	return 0;
}

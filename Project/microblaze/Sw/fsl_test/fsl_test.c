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

	//char file[] = "ABBCCCDDDD\n\r\0";
	char file[] = {'A', 1, 2, 3, 4, 5, 6, 7, 8, 9,
			10, 10,
			11, 12, 13, 14, 15,
			16, 16,
			17, 17, 17,
			18, 18, 18, 18,
			19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
			33, 33,
			34, 34, 34,
			35, 35, 35, 35,
			36, 37, 0};

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

	// xil_printf("unsigned size: %d B\n\n", sizeof(unsigned));

	for(i=0; i<254; i = i + 2)
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

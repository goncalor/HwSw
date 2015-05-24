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

int main(int argc, char **argv)
{
	int i, j;
	char ascii[256];
	char *file = (char *) (0xa8100000);

	file[MAX_FILE_SIZE-1] = FILE_END_CODE;	// place end of file code. to be safe

	u32 * file_aux = (u32 *) file;

	cputfsl(FILE_END_CODE, 0);	// send FILE_END_CODE for the accelarator to recognise it

	while(((*file_aux & 0xFF000000)>>24 != FILE_END_CODE) &&
			((*file_aux & 0x00FF0000)>>16 != FILE_END_CODE) &&
			((*file_aux & 0x0000FF00)>>8 != FILE_END_CODE) &&
			((*file_aux & 0x000000FF) != FILE_END_CODE) ){
		putfsl(*file_aux, 0);
		file_aux++;
	}

	putfsl(*file_aux, 0);	// put the last byte, which contains FILE_END_CODE

	int tmp;
	for(i=0; i<256; i = i + 2){
		getfsl(tmp, 0);

		stats[i] = (tmp & 0xFFFF0000) >> 16;
		stats[i + 1] = tmp & 0x0000FFFF;

		//xil_printf("stats %d -> %d\n", i, stats[i]);
		//xil_printf("stats %d -> %d\n", i + 1, stats[i + 1]);
	}

	stats[FILE_END_CODE] = 1;
	timeH[1] = get_timer64_val(&(timeL[1]));
	#endif

	#ifdef debug
	for(i=0; i<256; i++){
		xil_printf("%d -> \n", i);
		xil_printf("%d\n", stats[i]);
	}
	#endif

	// encode the buffer
	unsigned outbuf_len = encode_file((char *)file, (char *)file, encoding_table);

	return 0;
}

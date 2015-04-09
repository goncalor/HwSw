#include "define.h"

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

void compute_stats(char *file)
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


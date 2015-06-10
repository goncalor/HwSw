#include <stdio.h>
#include <stdlib.h>

#include "huffman_code.h"
#include "define.h"

// encodes orig into dest. returns the number of bytes written
// to dest.
unsigned encode_file(char *orig, char *dest, char *codewords, unsigned len)
{
	unsigned orig_ptr, dest_ptr;
	unsigned short word_len = 8, code_len, bits_used = 0;
	unsigned char ascii, outword = 0, code;

	orig_ptr = 0;
	dest_ptr = 0;
	do
	{
		ascii = orig[orig_ptr];
		code = codewords[ascii<<2];	// retrieve the code for this ascii char
		code_len = codewords[(ascii<<2)+1];	// and the length of the code

		if(bits_used + code_len > word_len)
		{
			outword |= code >> bits_used;
			dest[dest_ptr] = outword;
			dest_ptr++;
			outword = code << (word_len - bits_used);
			bits_used = code_len + bits_used - word_len;
		}
		else
		{
			outword |= code >> bits_used;
			bits_used += code_len;
		}

		orig_ptr++;
	}
	while(orig_ptr != len);

	dest[dest_ptr] = outword;	// the last codeword has not been written yet. write it!
	return dest_ptr+1;
}

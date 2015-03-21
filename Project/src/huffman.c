// C program for Huffman Coding
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "read_char.h"
#include "misc.h"
#include "huffman_code.h"

// Driver program to test above functions
int main(int argc, char * argv[]){
    char * arr = NULL;
    int * freq = NULL;
    char * file = NULL;
    int size_arr = 0;

    if(argc > 2 || argc == 1 ||
       strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
      puts("Usage:");
      puts("\t./huffman name-of-file");
      exit(0);
    }else{
      file = argv[1];
    }

    size_arr = read_char(&arr, &freq, file);
    //print_array(arr, freq, size_arr);

    int size = size_arr/sizeof(arr[0]);
    HuffmanCodes(arr, freq, size, file);

    free(arr);
    free(freq);
    exit(0);
}

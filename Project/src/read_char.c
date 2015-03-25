/**
 * Reads file and gives statistics of each character/symbol
 */
#include <stdio.h>
#include <stdlib.h>

//#define ALPHABET_GLOBAL "\0000000\a\b\t\n\v\f\r000000000000000\e0000 !\"#$%&\'()*+,-./0123456789:;<=>\?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"

char ALPHABET_GLOBAL[256];

/**
 * Reads every character/symbol and generates its statistics
 * @param  file_alphabet file alphabet(characters/symbols that
 *                       are used in the file)
 * @param  freq          ocuurence of each letter/symbol
 * @return               size of alphabet
 */
int read_char(char ** file_alphabet, int ** freq, char * file){
  /**
   * Variable declarations
   */
  FILE * read_me;
  char read;
  int * freq_all, total = 0, add = 0, i;

  /**
   * Create Array to store each ocurrence of a letter/symbol
   */

  for(i=0; i<256; i++)
	  ALPHABET_GLOBAL[i] = i;

  freq_all = (int *) malloc(256 * sizeof(int));

  /**
   * Initialize Array
   */
  for(int j = 0; j < sizeof(freq_all); j++){
    freq_all[j] = 0;
  }

  /**
   * Create file descriptor
   */
  if((read_me = fopen(file, "r")) == NULL){
    perror("fopen: ");
    exit(1);
  }

  /**
   * Read file and count each occurrence of a letter/symbol
   * total -> how many letters are in the file
   */
  while((read = getc(read_me)) != EOF){
    if(read <= '~'){
      if(freq_all[(short) read] == 0){
        total++;
      }
      freq_all[(short) read]++;
    }
  }

  printf("Total: %d\n", total);

  /**
   * Allocate memory for the exit arrays
   */
  *freq = malloc(total * sizeof(int));
  if(*freq == NULL){
    perror("malloc(freq): ");
  }

  *file_alphabet = malloc(total * sizeof(char));
  if(*file_alphabet == NULL){
    perror("malloc(alphabet): ");
  }

  /**
   * Store each letter/symbol ocurrence and itself in the exit arrays
   */
  for(int i = 0; i < 256; i++){
    if(freq_all[i] != 0){
      (*freq)[add] = freq_all[i];
      (*file_alphabet)[add] = ALPHABET_GLOBAL[i];
      add++;
    }
  }

  /**
   * Close file descriptor
   */
  fclose(read_me);

  return total;
}

/**
 * Prints letter/symbol array aswell aseach ocurrence
 * @param file_alphabet file alphabet (letters/symbols used in FILE)
 * @param freq          number of ocurrences of each symbol/letter
 */
void print_array(char * file_alphabet, int * freq, int total){
  for(int i = 0; i < total; i++){
    printf("%c -> %d\n", file_alphabet[i], freq[i]);
  }
}

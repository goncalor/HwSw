#include <stdio.h>
#include <stdlib.h>

int main(){
  FILE * read_me;
  char read;
  int i, * freq, total = 0, add = 0;
  char alphabet_all[] =
  "\0000000\a\b\t\n\v\f\r000000000000000\e0000 !\"#$%&\'()*+,-./0123456789:;<=>\?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
  char * alphabet;
  int * freq_all;

  freq_all = (int *) malloc(sizeof(alphabet_all) * sizeof(int));

  for(i = 0; i < sizeof(freq_all); i++){
    freq_all[i] = 0;
  }

  if((read_me = fopen("read_me", "r")) == NULL){
    perror("fopen: ");
    exit(1);
  }

  while((read = getc(read_me)) != EOF){
    if(read <= '~'){
      if(freq_all[(short) read] == 0){
        total++;
      }
      freq_all[(short) read]++;
    }
  }

  printf("\nTotal chars: %d\n", total);
  freq = (int *) malloc(total * sizeof(int));
  if(freq == NULL){
    perror("malloc(freq): ");
  }

  alphabet = (char *) malloc(total * sizeof(char));
  if(alphabet == NULL){
    perror("malloc(alphabet): ");
  }

  for(i = 0; i < sizeof(alphabet_all); i++){
    if(freq_all[i] != 0){
      freq[add] = freq_all[i];
      alphabet[add] = alphabet_all[i];
      add++;
    }
  }

  for(i = 0; i < total; i++){
    printf("%c -> %d\n", alphabet[i], freq[i]);
  }

  free(freq);
  free(alphabet);
  fclose(read_me);

  exit(0);
}

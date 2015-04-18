/**
 * Creates the binary representation of a symbol/character
 * @param  bin input array
 * @param  a   symbol/character to convert
 * @return     binary of symbol/character
 */
int * int2bin(int * bin, unsigned int a){
  int i;
  int tmp = a;

  for(i = 0; i < 8; i++)
    bin[i] = 0;

  for(i = 0; i < 8 || tmp != 0; i++){
    bin[i] = tmp % 2;
    tmp = tmp / 2;
  }

  return bin;
}

/**
 * Receives int and returns its binary form also in int
 * @param  a int to convert
 * @return   binary number
 */
unsigned int int2bin(int a){
  if(a == 0) return 0;
  if(a == 1) return 1;
  return (a % 2) + 10 * int2bin(a/2);
}

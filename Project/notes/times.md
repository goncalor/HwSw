# Times
Huffman Print medido sem xil_printf de maneira a ter uma medida mais realista. Os resultados não devem ser escritos para o `stdout` mas sim para um ficheiro de saída, que neste caso é um espaço de memória dentro da memória externa.

## Resultados (sem Aceleração)
- tmp -> Total Time: 3 ms
  + Compute Stats   -> 1 ms
  + Build Tree      -> 0 ms
  + Tree to Table   -> 0 ms
  + Encode File     -> 0 ms

- teste -> Total Time: 8 ms
  + Compute Stats   -> 1 ms
  + Build Tree      -> 5 ms
  + Tree to Table   -> 1 ms
  + Encode File     -> 0 ms

- pdf -> Total Time: 6 ms
  + Compute Stats   -> 1 ms
  + Build Tree      -> 3 ms
  + Tree to Table   -> 0 ms
  + Encode File     -> 1 ms

- read_me -> Total Time: 80 ms
  + Compute Stats   -> 21 ms
  + Build Tree      -> 12 ms 
  + Tree to Table   -> 1 ms
  + Encode File     -> 43 ms

- BIG_READ -> Total Time:  ms
  + Compute Stats   -> 76584 ms (1,28 min)
  + Build Tree      -> 32 ms
  + Tree to Table   -> 3 ms
  + Encode File     -> 159882 ms (2,66 min)

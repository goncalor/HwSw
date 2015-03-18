Co-Projecto Hw/Sw: Huffman Encoding
===================================

Luís Fiolhais - Nº 74171  
Gonçalo Ribeiro - Nº 73294

##Especificação

O objectivo do nosso projecto é implementar compressão de texto usando codificação de Huffman estática. A codificação de Huffman atribui códigos mais curtos a caracteres que aparecem mais frequentemente e códigos mais longos a caracteres mais raros. Para começar a construir a árvore é necessário saber à priori o alfabeto do ficheiro e a frequência de cada caracter.

###1ª Parte

As estatísticas do ficheiro são carregadas para a memória externa bem como o ficheiro a comprimir. A partir das estatísticas é construída a árvore binária que corresponde à codificação de Huffman. Esta árvore é depois usada para reescrever o ficheiro na sua forma comprimida. Este ficheiro comprimido é colocado na memória externa.

Vamos começar por portar o código C de forma a funcionar no MicroBlaze. Após o seu correcto funcionamento vamos utilizar hardware de forma a melhorar o tempo de pesquisa na árvore para que a codificação do ficheiro seja mais rápida. Esperamos assim melhorar o desempenho da implementação.

###2ª parte

Nesta parte tencionamos que as estatísticas do ficheiro sejam produzidas pelo próprio sistema. Vamos também paralelizar a codificação do ficheiro através de vários MicroBlazes. Planeamos dividir o ficheiro em várias partes e pôr cada MicroBlaze a trabalhar sobre uma das partes do ficheiro (usando a mesma árvore).

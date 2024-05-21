#ifndef OP_CSVH
    #define OP_CSVH

    #include <stdlib.h>
    #include <stdio.h>
    #include "reg.h"

    // Nesse  .h se encontram todas as funções que fazem operações básicas em arquivos csv

    // Função que lê uma string de um arquivo csv, string é guardada em dest, retorno é o tamanho da string
    int lerStrCsv(FILE * file, char ** dest);
    // Função que lê um inteiro de um arquivo csv
    int lerIntCsv(FILE * file);
    // Função que lê um registro de dados de um arquivo csv
    void readRegDadoCsv(FILE * fIn, REG_DADO * dados);

#endif
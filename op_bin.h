#ifndef OP_BINH
    #define OP_BINH

    #include <stdio.h>
    #include <stdlib.h>
    #include "reg.h"

    // Nesse  .h se encontram todas as funções que fazem operações básicas em arquivos binários
    
    // Função que lê uma string de um arquivo binário
    void lerStrBin(FILE * file, char ** dest, int tam);
    //Função que lê um registro de cabeçalho de um arquivo binário, preenchendo o regCab
    void readRegCabBin(FILE * filebin, REG_CAB * regCab);
    //Função que lê um registro de dados de um arquivo binário, preenchendo o regDado
    void readRegDadoBin(FILE * filebin, REG_DADO * regDado);
    // Função que escreve um registro de cabeçalho num arquivo binário
    void writeRegCabBin(FILE * fOut, REG_CAB cabecalho);
    // Função que escreve um registro de dados num arquivo binário
    void writeRegDadoBin(FILE * file, REG_DADO dados);

#endif
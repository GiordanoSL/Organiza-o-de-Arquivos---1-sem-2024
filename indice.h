/*-----------------------------------------------------
Autores: Giordano Santorum Lorenzetto - nUSP 14574017
         Victor Moreli dos Santos - nUSP 14610514

Funções que manipulam índice:
    - carregamento: lê um arquivo de índices e o converte num vetor em memória primária
    - reescrita: reconstrói o arquivo de índices a partir do vetor em memória primária
    - insert_ordenado: faz inserção ordenada num vetor de índices
-------------------------------------------------------*/


#ifndef INDICEH
    #define INDICEH

    #include "op_bin.h"
    #include "util.h"
    #include "reg.h"

    //lê um arquivo de índices e o converte num vetor em memória primária
    REG_DADO_ID ** carregamento(FILE * fId, int nroRegArq, int numInsert);
    //reconstrói o arquivo de índices a partir do vetor em memória primária
    void reescrita(FILE * fId, REG_DADO_ID ** vetorId, int nroRegArq);
    //faz inserção ordenada num vetor de índices
    void insert_ordenado(REG_DADO_ID ** vetorId, REG_DADO_ID * regDadoId, int count);

#endif
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

    // Função que realiza carregamento na memória primária de um arquivo de índice em disco,
    // retorna um vetor com os registros de índice do arquivo
    //
    // Argumento numInsert: - caso o carregamento esteja sendo feito para realizar inserções, numInsert = números de inserções
    //                        que seram feitas, espaço já reservado no vetor;
    //                      - caso não, numInsert = 0
    //                      * O uso desse argumento evita reallocs nos casos de inserção  
    REG_DADO_ID ** carregamento(FILE * fId, int nroRegArq, int numInsert);
    //reconstrói o arquivo de índices a partir do vetor em memória primária
    void reescrita(FILE * fId, REG_DADO_ID ** vetorId, int nroRegArq);
    //faz inserção ordenada num vetor de índices
    void insert_ordenado(REG_DADO_ID ** vetorId, REG_DADO_ID * regDadoId, int count);

#endif
/*-----------------------------------------------------
Autores: Giordano Santorum Lorenzetto - nUSP 14574017
         Victor Moreli dos Santos - nUSP 14610514
-------------------------------------------------------*/

#ifndef ARVOREB_H

    #define ARVOREB_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include <string.h>

    #define ORDEM 4
    #define MAX_CHAVES (ORDEM - 1)
    #define MAX_DESCENDENTES ORDEM
    #define TAM_PAG 60
    #define TAM_REGISTRO TAM_PAG
    #define TAM_CAB_ARVOREB TAM_PAG

    struct reg_dado_arvoreb_ {
        int alturaNo;
        int nroChaves;
        int chaves[MAX_CHAVES];      // chave primária
        long byteOffset[MAX_CHAVES]; // byteoffset do registro no arquivo de dados
        int descendentes[MAX_DESCENDENTES];
    };

    struct reg_cabecalho_arvoreb_ {
        char status;       // 0 - inconsistente, 1 - consistente
        int noRaiz;
        int proxRRN;
        int nroChaves;
        char lixo[47];
        int altura;
    };

    typedef struct reg_dado_arvoreb_ DadoArvoreB;
    typedef struct reg_cabecalho_arvoreb_ CabecalhoArvoreB;

    typedef struct NoArvoreB_ {
        DadoArvoreB dados;
        long rrn; // Número do registro relativo no arquivo
    } NoArvoreB;

    typedef struct ArvoreB_ {
        CabecalhoArvoreB cabecalho;
    } ArvoreB;

    void inserirChave(ArvoreB *arvore, FILE *arquivo, int chave, long byteOffset);
    bool inicializarArvoreB(ArvoreB *arvore, const char *nomeArquivo);
    long buscarChave(ArvoreB *arvore, FILE *arquivo, long rrn, int chave);
    void printBTree(ArvoreB *arvore, const char *nomeArquivo);
    void escreverCabArvoreB(FILE * arquivo, CabecalhoArvoreB *cabecalho);
    void lerCabArvoreB(FILE * arquivo, CabecalhoArvoreB *cabecalho);

#endif
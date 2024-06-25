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

        // Estrutura que representa um nó da árvore B
    struct reg_dado_arvoreb_ {
        int alturaNo;                       // Altura do nó na árvore
        int nroChaves;                      // Número de chaves no nó
        int chaves[MAX_CHAVES];             // Chaves do nó
        long byteOffset[MAX_CHAVES];        // Byte offset dos registros no arquivo de dados
        int descendentes[MAX_DESCENDENTES]; // Ponteiros para os nós descendentes
    };

    // Estrutura que representa o cabeçalho da árvore B
    struct reg_cabecalho_arvoreb_ {
        char status;       // Status da árvore (0 - inconsistente, 1 - consistente)
        int noRaiz;        // RRN (Número do Registro Relativo) do nó raiz
        int proxRRN;       // Próximo RRN disponível para inserção
        int nroChaves;     // Número total de chaves na árvore
        char lixo[47];     // Espaço restante na página do cabecalho 
        int altura;        // Altura da árvore
    };

    typedef struct reg_dado_arvoreb_ DadoArvoreB;
    typedef struct reg_cabecalho_arvoreb_ CabecalhoArvoreB;

    // Estrutura que representa um nó da árvore B com seu RRN
    typedef struct NoArvoreB_ {
        DadoArvoreB dados;
        long rrn; // Número do registro relativo no arquivo
    } NoArvoreB;

    // Estrutura que representa a árvore B
    typedef struct ArvoreB_ {
        CabecalhoArvoreB cabecalho;
    } ArvoreB;

    // Função que realiza a inserção de uma chave num arquivo de índice com árvore B
    void inserirChave(ArvoreB *arvore, FILE *arquivo, int chave, long byteOffset);
    // Função para inicializar a árvore B
    bool inicializarArvoreB(ArvoreB *arvore, const char *nomeArquivo);
    // Função para buscar uma chave na árvore B, recebe o RRN do nó a ser buscado
    long buscarChave(ArvoreB *arvore, FILE *arquivo, long rrn, int chave);
    // Função para escrever o cabeçalho da árvore B no arquivo
    void escreverCabArvoreB(FILE * arquivo, CabecalhoArvoreB *cabecalho);
    // Função para ler o cabeçalho da árvore B do arquivo
    void lerCabArvoreB(FILE * arquivo, CabecalhoArvoreB *cabecalho);

#endif
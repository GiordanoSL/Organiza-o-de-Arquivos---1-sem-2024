#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "arvoreB.h"

#define ORDEM 4
#define MAX_CHAVES (ORDEM - 1)
#define MAX_DESCENDENTES ORDEM
#define TAM_PAG 60
#define TAM_REGISTRO TAM_PAG
#define TAM_CAB_ARVOREB TAM_PAG


// Função para criar nó com valores nulos
void criarNo(NoArvoreB * no){
    no->dados.alturaNo = -1;
    no->dados.nroChaves = 0;
    for (int i = 0; i < MAX_CHAVES; i++)
    {
        no->dados.chaves[i] = -1;
        no->dados.descendentes[i] = -1;
        no->dados.byteOffset[i] = -1;
    }
    no->dados.descendentes[MAX_DESCENDENTES - 1] = -1;
    no->rrn = -1;
}

// Função para escrever um nó no arquivo de índice
void escreverNo(FILE * arquivo, long rrn, NoArvoreB *no) {
    fseek(arquivo, rrn * TAM_REGISTRO + TAM_CAB_ARVOREB, SEEK_SET);
    fwrite(&(no->dados.alturaNo), sizeof(int), 1, arquivo);
    fwrite(&(no->dados.nroChaves), sizeof(int), 1, arquivo);
    for (int i = 0; i < MAX_CHAVES; i++) {
        fwrite(&(no->dados.chaves[i]), sizeof(int), 1, arquivo);
        fwrite(&(no->dados.byteOffset[i]), sizeof(long), 1, arquivo);
    }
    fwrite(no->dados.descendentes, sizeof(int), MAX_DESCENDENTES, arquivo);
}

// Função para ler um nó no arquivo de índice
void lerNo(FILE * arquivo, long rrn, NoArvoreB *no) {
    if(rrn == -1)
        return;

    fseek(arquivo, rrn * TAM_REGISTRO + TAM_CAB_ARVOREB, SEEK_SET);
    no->rrn = rrn;
    fread(&(no->dados.alturaNo), sizeof(int), 1, arquivo);
    fread(&(no->dados.nroChaves), sizeof(int), 1, arquivo);
    for (int i = 0; i < MAX_CHAVES; i++) {
        fread(&(no->dados.chaves[i]), sizeof(int), 1, arquivo);
        fread(&(no->dados.byteOffset[i]), sizeof(long), 1, arquivo);
    }
    fread(no->dados.descendentes, sizeof(int), MAX_DESCENDENTES, arquivo);

}

// Função para escrever o cabeçalho da árvore B no arquivo
void escreverCabArvoreB(FILE * arquivo, CabecalhoArvoreB *cabecalho) {
    fseek(arquivo, 0, SEEK_SET);    // já vai para o início do arquivo
    fwrite(&(cabecalho->status), sizeof(char), 1, arquivo);
    fwrite(&(cabecalho->noRaiz), sizeof(int), 1, arquivo);
    fwrite(&(cabecalho->proxRRN), sizeof(int), 1, arquivo);
    fwrite(&(cabecalho->nroChaves), sizeof(int), 1, arquivo);
    fwrite(cabecalho->lixo, sizeof(char), 47, arquivo);
}

// Função para ler o cabeçalho da árvore B do arquivo
void lerCabArvoreB(FILE * arquivo, CabecalhoArvoreB *cabecalho) {
    fseek(arquivo, 0, SEEK_SET);    // já vai para o início do arquivo
    fread(&(cabecalho->status), sizeof(char), 1, arquivo);
    fread(&(cabecalho->noRaiz), sizeof(int), 1, arquivo);
    fread(&(cabecalho->proxRRN), sizeof(int), 1, arquivo);
    fread(&(cabecalho->nroChaves), sizeof(int), 1, arquivo);
    fread(cabecalho->lixo, sizeof(char), 47, arquivo);
}


// Função para inicializar a árvore B
bool inicializarArvoreB(ArvoreB *arvore, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "wb");
    if (arquivo == NULL) {      
        perror("Falha no processamento do arquivo.");
        return false;
    }
    arvore->cabecalho.status = '1'; // Consistente
    arvore->cabecalho.noRaiz = -1;
    arvore->cabecalho.proxRRN = 0;
    arvore->cabecalho.nroChaves = 0;
    arvore->cabecalho.altura = 0;
    for (int i = 0; i < 47; i++)
        arvore->cabecalho.lixo[i] = '$';

    escreverCabArvoreB(arquivo, &arvore->cabecalho);    // Escreve cabecalho no arquivo
    fclose(arquivo);
    return true;
}

// Função para buscar uma chave na árvore B, recebe o RRN do nó a ser buscado
long buscarChave(ArvoreB *arvore, FILE *arquivo, long rrn, int chave) {
    NoArvoreB no;
    lerNo(arquivo, rrn, &no);   // Lê o nó

    int i = 0;
    while (i < no.dados.nroChaves && chave > no.dados.chaves[i]) {  // percorre o nó até encontrar chave maior ou igual a chave buscada
        i++;
    }

    if (i < no.dados.nroChaves && chave == no.dados.chaves[i]) {
        return no.dados.byteOffset[i]; // Chave encontrada
    } else if (no.dados.alturaNo == 0) {    // Chave não foi encontrada e está num nó folha
        return -1; // Não há descendentes para procurar
    } else {
        // Desce para o próximo nível da árvore
        return buscarChave(arvore, arquivo, no.dados.descendentes[i], chave);
    }
}

// Função para dividir um nó quando ele está cheio
void dividirNo(int chave, long byteOffset, NoArvoreB *no, int *chavePromovida, long *byteOffsetPromovido, int *descendenteDireita, NoArvoreB *novoNoDireita) {
    int tempChaves[MAX_CHAVES + 1];
    long tempByteOffsets[MAX_CHAVES + 1];
    int tempDescendentes[MAX_DESCENDENTES + 1];

    // Copia as chaves e os byte offsets para arrays temporários
    for (int i = 0; i < MAX_CHAVES; i++) {
        tempChaves[i] = no->dados.chaves[i];
        tempByteOffsets[i] = no->dados.byteOffset[i];
        tempDescendentes[i] = no->dados.descendentes[i];
    }
    tempDescendentes[MAX_CHAVES] = no->dados.descendentes[MAX_CHAVES];

    // Insere a nova chave na posição correta nos arrays temporários
    int i;
    for (i = no->dados.nroChaves - 1; i >= 0 && chave < tempChaves[i]; i--) {
        tempChaves[i + 1] = tempChaves[i];
        tempByteOffsets[i + 1] = tempByteOffsets[i];
        tempDescendentes[i + 2] = tempDescendentes[i + 1];
    }
    tempChaves[i + 1] = chave;
    tempByteOffsets[i + 1] = byteOffset;
    tempDescendentes[i + 2] =  *descendenteDireita;

    // Promove a chave do meio
    int meio = ((ORDEM % 2 == 0)? (ORDEM/2) - 1 : (ORDEM/2));
    *chavePromovida = tempChaves[meio];
    *byteOffsetPromovido = tempByteOffsets[meio];

    // Configurar o novo nó à direita
    novoNoDireita->dados.alturaNo = no->dados.alturaNo;
    novoNoDireita->dados.nroChaves = (ORDEM / 2);
    for (int j = 0; j < novoNoDireita->dados.nroChaves; j++) {
        novoNoDireita->dados.chaves[j] = tempChaves[meio + 1 + j];
        novoNoDireita->dados.byteOffset[j] = tempByteOffsets[meio + 1 + j];
        novoNoDireita->dados.descendentes[j] = tempDescendentes[meio + 1 + j];
    }
    novoNoDireita->dados.descendentes[novoNoDireita->dados.nroChaves] = tempDescendentes[meio + novoNoDireita->dados.nroChaves + 1];

    // Atualizar o nó original
    no->dados.nroChaves = meio;
    for (int j = 0; j < meio; j++) {
        no->dados.chaves[j] = tempChaves[j];
        no->dados.byteOffset[j] = tempByteOffsets[j];
        no->dados.descendentes[j] = tempDescendentes[j];
    }
    no->dados.descendentes[meio] = tempDescendentes[meio];

    // Limpar as posições restantes do nó original
    for (int j = meio; j < MAX_CHAVES; j++) {
        no->dados.chaves[j] = -1;
        no->dados.byteOffset[j] = -1;
        if(j != meio) no->dados.descendentes[j] = -1;
    }
    no->dados.descendentes[MAX_CHAVES] = -1;
}

// Função recursiva para inserir uma chave na árvore B
int inserirChaveRecursivo(FILE *arquivo, ArvoreB *arvore, int rrnAtual, int chave, long byteOffset, int *promoKey, long *promoOffset, int *promoRChild) {
    if (rrnAtual == -1) {   // Chegou no local da inserção
        *promoKey = chave;
        *promoOffset = byteOffset;
        *promoRChild = -1;
        return 1;           // Retorno 1 indica que irá averiguar espaço no nó superior, inserindo nele ou fazendo split
    }   

    NoArvoreB paginaAtual;
    lerNo(arquivo, rrnAtual, &paginaAtual); // Lê página atual

    int pos;
    for (pos = 0; pos < paginaAtual.dados.nroChaves && chave > paginaAtual.dados.chaves[pos]; pos++);   // Percorre nó até encontrar chave maior ou igual à chave que vai ser inserida

    if (pos < paginaAtual.dados.nroChaves && chave == paginaAtual.dados.chaves[pos]) {  // Se encontrou a chave que seria inserida, não insere chave duplicada
        printf("Erro: chave duplicada\n");
        return -1;
    }

    int retorno = inserirChaveRecursivo(arquivo, arvore, paginaAtual.dados.descendentes[pos], chave, byteOffset, promoKey, promoOffset, promoRChild); // Chama função recursiva de inserção
    if (retorno == 0 || retorno == -1) {    // Retorno = 0: Inserção ocorreu num nó onde havia espaço
        return retorno;                     // Retorno = -1: Erro
    }

    if (paginaAtual.dados.nroChaves < MAX_CHAVES) {     // Há espaço para inserção
        for (int j = paginaAtual.dados.nroChaves; j > pos; j--) {   // Desloca para criar espaço para a chave/byteffset
            paginaAtual.dados.chaves[j] = paginaAtual.dados.chaves[j - 1];
            paginaAtual.dados.byteOffset[j] = paginaAtual.dados.byteOffset[j - 1];
            paginaAtual.dados.descendentes[j + 1] = paginaAtual.dados.descendentes[j];
        }
        // Insere chave/byteoffset na posiçãom correta
        paginaAtual.dados.chaves[pos] = *promoKey;
        paginaAtual.dados.byteOffset[pos] = *promoOffset;
        paginaAtual.dados.descendentes[pos + 1] = *promoRChild;
        paginaAtual.dados.nroChaves++;

        escreverNo(arquivo, rrnAtual, &paginaAtual);   // Reescreve nó no arquivo de índices
        return 0;
    } else {    // Não há espaço para inserção, fazer split
        NoArvoreB novoNoDireita;
        criarNo(&novoNoDireita);    // Cria novo nó a direita

        // Chama função que realiza o split, separando as chaves/byteoffsets/descendentes
        dividirNo(*promoKey, *promoOffset, &paginaAtual, promoKey, promoOffset, promoRChild, &novoNoDireita);

        novoNoDireita.rrn = arvore->cabecalho.proxRRN++;    // Atualiza RRN do novo nó e proxRRN
        *promoRChild = novoNoDireita.rrn;   // Novo nó a direita é o 'filho direito' da chave promovida

        escreverNo(arquivo, rrnAtual, &paginaAtual);    // Reescreve nó da esquerda
        escreverNo(arquivo, novoNoDireita.rrn, &novoNoDireita); // Escreve nó da direita

        return 1;
    }
}

// Função que realiza a inserção de uma chave num arquivo de índice com árvore B
void inserirChave(ArvoreB *arvore, FILE *arquivo, int chave, long byteOffset) {
    int promoKey;
    long promoOffset;
    int promoRChild;

    // Chama função recursiva de inserção
    int retorno = inserirChaveRecursivo(arquivo, arvore, arvore->cabecalho.noRaiz, chave, byteOffset, &promoKey, &promoOffset, &promoRChild);

    // Retorno = 1 nesse ponto significa split da raiz, sendo necessária criar uma nova raiz para guardar a  chave promovida
    if (retorno == 1) {
        NoArvoreB novaRaiz;
        criarNo(&novaRaiz); // Criação da nova raiz
        novaRaiz.dados.alturaNo = arvore->cabecalho.noRaiz == -1 ? arvore->cabecalho.altura : ++arvore->cabecalho.altura;
        novaRaiz.dados.nroChaves = 1;  
        novaRaiz.dados.chaves[0] = promoKey;    // única chave é a promovida
        novaRaiz.dados.byteOffset[0] = promoOffset;
        novaRaiz.dados.descendentes[0] = arvore->cabecalho.noRaiz;
        novaRaiz.dados.descendentes[1] = promoRChild;
        novaRaiz.rrn = arvore->cabecalho.proxRRN++;
        arvore->cabecalho.noRaiz = novaRaiz.rrn;
        escreverNo(arquivo, novaRaiz.rrn, &novaRaiz);   // escreve nova raiz
    }

    arvore->cabecalho.nroChaves++;  // atualiza número de chaves
}


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// https://prepinsta.com/data-structures/b-tree-insertion/

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

// Funções auxiliares para leitura e escrita de nós no arquivo
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

void lerNo(FILE * arquivo, long rrn, NoArvoreB *no) {
    fseek(arquivo, rrn * TAM_REGISTRO + TAM_CAB_ARVOREB, SEEK_SET);
    fread(&(no->dados.alturaNo), sizeof(int), 1, arquivo);
    fread(&(no->dados.nroChaves), sizeof(int), 1, arquivo);
    for (int i = 0; i < MAX_CHAVES; i++) {
        fread(&(no->dados.chaves[i]), sizeof(int), 1, arquivo);
        fread(&(no->dados.byteOffset[i]), sizeof(long), 1, arquivo);
    }
    fread(no->dados.descendentes, sizeof(int), MAX_DESCENDENTES, arquivo);
}

void escreverCabArvoreB(FILE * arquivo, CabecalhoArvoreB *cabecalho) {
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&(cabecalho->status), sizeof(char), 1, arquivo);
    fwrite(&(cabecalho->noRaiz), sizeof(int), 1, arquivo);
    fwrite(&(cabecalho->proxRRN), sizeof(int), 1, arquivo);
    fwrite(&(cabecalho->nroChaves), sizeof(int), 1, arquivo);
    fwrite(cabecalho->lixo, sizeof(char), 47, arquivo);
}

void lerCabArvoreB(FILE * arquivo, CabecalhoArvoreB *cabecalho) {
    fseek(arquivo, 0, SEEK_SET);
    fread(&(cabecalho->status), sizeof(char), 1, arquivo);
    fread(&(cabecalho->noRaiz), sizeof(int), 1, arquivo);
    fread(&(cabecalho->proxRRN), sizeof(int), 1, arquivo);
    fread(&(cabecalho->nroChaves), sizeof(int), 1, arquivo);
    fread(cabecalho->lixo, sizeof(char), 47, arquivo);
}

// Funções de inicialização, busca, inserção, remoção, etc.
void inicializarArvoreB(ArvoreB *arvore, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w+b");
    if (arquivo == NULL) {
        perror("Erro ao abrir arquivo");
        exit(1);
    }
    arvore->cabecalho.status = '1'; // Consistente
    arvore->cabecalho.noRaiz = -1;
    arvore->cabecalho.proxRRN = 0;
    arvore->cabecalho.nroChaves = 0;
    for (int i = 0; i < 47; i++)
        arvore->cabecalho.lixo[i] = '$';

    escreverCabArvoreB(arquivo, &arvore->cabecalho);
    fclose(arquivo);
}

long buscarChave(ArvoreB *arvore, FILE *arquivo, long rrn, int chave) {
    NoArvoreB no;
    lerNo(arquivo, rrn, &no);

    int i = 0;
    while (i < no.dados.nroChaves && chave > no.dados.chaves[i]) {
        i++;
    }
    if (i < no.dados.nroChaves && chave == no.dados.chaves[i]) {
        return no.dados.byteOffset[i]; // Chave encontrada
    } else if (no.dados.alturaNo == 0) {
        return -1; // Não há descendentes para procurar
    } else {
        // Desce para o próximo nível da árvore
        return buscarChave(arvore, arquivo, no.dados.descendentes[i], chave);
    }
}

void dividirNo(int chave, long byteOffset, NoArvoreB *no, int *chavePromovida, long *byteOffsetPromovido, int *descendenteDireita, NoArvoreB *novoNoDireita) {
    int tempChaves[MAX_CHAVES + 1];
    long tempByteOffsets[MAX_CHAVES + 1];
    int tempDescendentes[MAX_DESCENDENTES + 1];

    // Copia as chaves e os byte offsets para arrays temporários
    for (int i = 0; i < no->dados.nroChaves; i++) {
        tempChaves[i] = no->dados.chaves[i];
        tempByteOffsets[i] = no->dados.byteOffset[i];
    }
    for (int i = 0; i <= no->dados.nroChaves; i++) {
        tempDescendentes[i] = no->dados.descendentes[i];
    }

    // Insere a nova chave na posição correta nos arrays temporários
    int i;
    for (i = no->dados.nroChaves - 1; i >= 0 && chave < tempChaves[i]; i--) {
        tempChaves[i + 1] = tempChaves[i];
        tempByteOffsets[i + 1] = tempByteOffsets[i];
    }
    tempChaves[i + 1] = chave;
    tempByteOffsets[i + 1] = byteOffset;

    int meio = ORDEM / 2;
    *chavePromovida = tempChaves[meio];
    *byteOffsetPromovido = tempByteOffsets[meio];

    novoNoDireita->dados.alturaNo = no->dados.alturaNo;
    novoNoDireita->dados.nroChaves = ORDEM - 1 - meio;
    for (int j = 0; j < novoNoDireita->dados.nroChaves; j++) {
        novoNoDireita->dados.chaves[j] = tempChaves[meio + 1 + j];
        novoNoDireita->dados.byteOffset[j] = tempByteOffsets[meio + 1 + j];
        novoNoDireita->dados.descendentes[j] = tempDescendentes[meio + 1 + j];
    }
    novoNoDireita->dados.descendentes[novoNoDireita->dados.nroChaves] = tempDescendentes[ORDEM];

    no->dados.nroChaves = meio;
    for (int j = 0; j < meio; j++) {
        no->dados.chaves[j] = tempChaves[j];
        no->dados.byteOffset[j] = tempByteOffsets[j];
        no->dados.descendentes[j] = tempDescendentes[j];
    }
    no->dados.descendentes[meio] = tempDescendentes[meio];

    for (int j = meio; j < MAX_CHAVES; j++){
        no->dados.chaves[j] = -1;
        no->dados.byteOffset[j] = -1;
        no->dados.descendentes[j] = -1;
    }
    no->dados.descendentes[MAX_CHAVES] = -1;

    
}

int inserirChaveRecursivo(FILE *arquivo, ArvoreB *arvore, int rrnAtual, int chave, long byteOffset, int *promoKey, long *promoOffset, int *promoRChild) {
    if (rrnAtual == -1) {
        *promoKey = chave;
        *promoOffset = byteOffset;
        *promoRChild = -1;
        return 1;
    }

    NoArvoreB paginaAtual;
    lerNo(arquivo, rrnAtual, &paginaAtual);

    int pos;
    for (pos = 0; pos < paginaAtual.dados.nroChaves && chave > paginaAtual.dados.chaves[pos]; pos++);

    if (pos < paginaAtual.dados.nroChaves && chave == paginaAtual.dados.chaves[pos]) {
        printf("Erro: chave duplicada\n");
        return -1;
    }

    int retorno = inserirChaveRecursivo(arquivo, arvore, paginaAtual.dados.descendentes[pos], chave, byteOffset, promoKey, promoOffset, promoRChild);
    if (retorno == 0 || retorno == -1) {
        return retorno;
    }

    if (paginaAtual.dados.nroChaves < MAX_CHAVES) {
        for (int j = paginaAtual.dados.nroChaves; j > pos; j--) {
            paginaAtual.dados.chaves[j] = paginaAtual.dados.chaves[j - 1];
            paginaAtual.dados.byteOffset[j] = paginaAtual.dados.byteOffset[j - 1];
            paginaAtual.dados.descendentes[j + 1] = paginaAtual.dados.descendentes[j];
        }
        paginaAtual.dados.chaves[pos] = *promoKey;
        paginaAtual.dados.byteOffset[pos] = *promoOffset;
        paginaAtual.dados.descendentes[pos + 1] = *promoRChild;
        paginaAtual.dados.nroChaves++;

        escreverNo(arquivo, rrnAtual, &paginaAtual);
        return 0;
    } else {
        NoArvoreB novoNoDireita;
        criarNo(&novoNoDireita);

        dividirNo(*promoKey, *promoOffset, &paginaAtual, promoKey, promoOffset, promoRChild, &novoNoDireita);

        novoNoDireita.rrn = arvore->cabecalho.proxRRN++;
        *promoRChild = novoNoDireita.rrn;

        escreverNo(arquivo, rrnAtual, &paginaAtual);
        escreverNo(arquivo, novoNoDireita.rrn, &novoNoDireita);

        return 1;
    }
}

void inserirChave(ArvoreB *arvore, const char *nomeArquivo, int chave, long byteOffset) {
    FILE *arquivo = fopen(nomeArquivo, "r+b");
    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo. 1 %d\n", chave);
        return;
    }

    lerCabArvoreB(arquivo, &arvore->cabecalho);
    if(arvore->cabecalho.status == '0'){
        printf("Falha no processamento do arquivo.2 %d\n", chave);
        fclose(arquivo);
        return;
    }

    arvore->cabecalho.status = '0';
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&arvore->cabecalho.status, sizeof(char), 1, arquivo);

    int promoKey;
    long promoOffset;
    int promoRChild;

    int retorno = inserirChaveRecursivo(arquivo, arvore, arvore->cabecalho.noRaiz, chave, byteOffset, &promoKey, &promoOffset, &promoRChild);

    if (retorno == 1) {
        NoArvoreB novaRaiz;
        criarNo(&novaRaiz);
        novaRaiz.dados.alturaNo = arvore->cabecalho.noRaiz == -1 ? 0 : (arvore->cabecalho.noRaiz + 1);
        novaRaiz.dados.nroChaves = 1;
        novaRaiz.dados.chaves[0] = promoKey;
        novaRaiz.dados.byteOffset[0] = promoOffset;
        novaRaiz.dados.descendentes[0] = arvore->cabecalho.noRaiz;
        novaRaiz.dados.descendentes[1] = promoRChild;
        novaRaiz.rrn = arvore->cabecalho.proxRRN++;
        arvore->cabecalho.noRaiz = novaRaiz.rrn;
        escreverNo(arquivo, novaRaiz.rrn, &novaRaiz);
    }

    arvore->cabecalho.nroChaves++;
    arvore->cabecalho.status = '1';
    escreverCabArvoreB(arquivo, &arvore->cabecalho);
    fclose(arquivo);
}

int main() {
    ArvoreB arvore;

    // Inicializa a árvore B e cria o arquivo de dados
    inicializarArvoreB(&arvore, "arvoreB.dat");

    // Insere algumas chaves na árvore B
    inserirChave(&arvore, "arvoreB.dat", 10, 100);
    inserirChave(&arvore, "arvoreB.dat", 20, 200);
    inserirChave(&arvore, "arvoreB.dat", 5, 50);
    inserirChave(&arvore, "arvoreB.dat", 15, 150);
    inserirChave(&arvore, "arvoreB.dat", 25, 250);
    inserirChave(&arvore, "arvoreB.dat", 30, 300);

    // Busca por algumas chaves na árvore B e imprime os resultados
    long result = buscarChave(&arvore, fopen("arvoreB.dat", "rb"), arvore.cabecalho.noRaiz, 10);
    if (result != -1) {
        printf("Chave 10 encontrada no byte offset: %ld\n", result);
    } else {
        printf("Chave 10 não encontrada.\n");
    }

    result = buscarChave(&arvore, fopen("arvoreB.dat", "rb"), arvore.cabecalho.noRaiz, 20);
    if (result != -1) {
        printf("Chave 20 encontrada no byte offset: %ld\n", result);
    } else {
        printf("Chave 20 não encontrada.\n");
    }

    result = buscarChave(&arvore, fopen("arvoreB.dat", "rb"), arvore.cabecalho.noRaiz, 25);
    if (result != -1) {
        printf("Chave 25 encontrada no byte offset: %ld\n", result);
    } else {
        printf("Chave 25 não encontrada.\n");
    }

    result = buscarChave(&arvore, fopen("arvoreB.dat", "rb"), arvore.cabecalho.noRaiz, 40);
    if (result != -1) {
        printf("Chave 40 encontrada no byte offset: %ld\n", result);
    } else {
        printf("Chave 40 não encontrada.\n");
    }

    return 0;
}

/*-----------------------------------------------------
Autores: Giordano Santorum Lorenzetto - nUSP 14574017
         Victor Moreli dos Santos - nUSP 14610514
-------------------------------------------------------*/


#include "indice.h"

// Função que realiza carregamento na memória primária de um arquivo de índice em disco,
// retorna um vetor com os registros de índice do arquivo
//
// Argumento numInsert: - caso o carregamento esteja sendo feito para realizar inserções, numInsert = números de inserções
//                        que seram feitas, espaço já reservado no vetor;
//                      - caso não, numInsert = 0
//                      * O uso desse argumento evita reallocs nos casos de inserção      

REG_DADO_ID ** carregamento(FILE * fId, int nroRegArq, int numInsert){
    if(fId == NULL){                    // arquivo de indice inválido
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }

    // retorna ao início do arquivo de índice, não importando assim para onde o ponteiro aponta no momento
    // em que a função é chamada
    fseek(fId, 0, SEEK_SET);    

    REG_CAB_ID regCabId;
    readRegCabId(fId, &regCabId);   // lê cabecalho do arquivo de índice

    if(regCabId.status == '0'){         // arquivo de indice inconsistente
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }

    regCabId.status = '0';  
    fseek(fId, 0, SEEK_SET);
    writeRegCabId(fId, regCabId);   // arquivo de índice aberto para carregamento -> arquivo *inconsistente*

    // alocação do vetor de registros de índice, tamanho nroRegArq + numInsert
    REG_DADO_ID **vetorId = (REG_DADO_ID **)malloc((nroRegArq + numInsert) * sizeof(REG_DADO_ID *));
    if(vetorId == NULL){
        printf("Falha na alocação de memória.");
        exit(1);
    } 
 
    // for que preenche o vetor com os registros de índice, inserindo ordenadamente
    for (int i = 0; i < nroRegArq; i++){
        REG_DADO_ID * regDadoId = (REG_DADO_ID *) malloc(sizeof(REG_DADO_ID));  // registro de índice que será lido do arquivo e adicionado ao vetor
        if(regDadoId == NULL){                                                  // falha na alocação de espaço para registro
            printf("Falha na alocação de memória.");
            exit(1);
        }                                                
            
        readRegDadoId(fId, regDadoId);           // leitura no arquivo
        vetorId[i] = regDadoId;                  // arquivo consistente => indíces ordenados, faz inserção normal
    }

    // arquivo só voltará a ser consistente se houver reescrita

    return vetorId;                 // retorna vetor
}

// Reconstrói o arquivo de índices a partir do vetor em memória primária 
void reescrita(FILE * fId, REG_DADO_ID ** vetorId, int nroRegArq){
    if(fId == NULL || vetorId == NULL){                    // arquivo de indice inválido ou vetor inválido
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    if(vetorId == NULL)
        exit(1);

    // retorna ao início do arquivo de índice, não importando assim para onde o ponteiro aponta no momento
    // em que a função é chamada
    fseek(fId, 0, SEEK_SET);    

    REG_CAB_ID regCabId;
    regCabId.status = '0';
    writeRegCabId(fId, regCabId);   // arquivo de índice aberto para reescrita -> arquivo *inconsistente*

    // for que percorre o vetor de registros de índice, reescrevendo o arquivo
    for (int i = 0; i < nroRegArq; i++){
        writeRegDadoId(fId, *(vetorId[i]));    // reescreve arquivo de índice
    }

    regCabId.status = '1';
    fseek(fId, 0, SEEK_SET);                // retorna para o início do arquivo
    writeRegCabId(fId, regCabId);           // muda status para consistente (atualizado)
    
}

// Faz inserção ordenada num vetor de índices
void insert_ordenado(REG_DADO_ID ** vetorId, REG_DADO_ID * regDadoId, int tam) {
    int esq = 0;
    int dir = tam - 1;
    int meio;
    int position = tam;  // Posição padrão: inserção no fim

    // Busca binária para encontrar a posição de inserção
    while (esq <= dir) {
        meio = (esq + dir) / 2;
        if (vetorId[meio]->id < regDadoId->id) {
            esq = meio + 1;
        } else {
            dir = meio - 1;
        }
    }
    position = esq;  // A posição de inserção será onde esq aponta

    // Shift no restante dos elementos para abrir espaço
    for (int i = tam; i > position; i--) {
        vetorId[i] = vetorId[i - 1];
    }

    // Insere o novo elemento na posição correta
    vetorId[position] = regDadoId;
}

void remove_indice(REG_DADO_ID ** vetorId, int id, int nroRegArq){
    int meio, dir, esq;
    esq = 0;
    dir = nroRegArq - 1;

    // Busca binária para encontrar o id que será removido
    while (esq <= dir) {
        meio = (esq + dir) / 2;

        if (vetorId[meio]->id < id)
            esq = meio + 1;

        else if(vetorId[meio]->id > id)
            dir = meio - 1;

        else
            break; // meio = posicao correta
    }


    // verifica se o id foi encontrado
    if(vetorId[meio]->id == id)
        // libera a memória utlizada pelo registro de indice
        free(vetorId[meio]);

        // faz o shift
        for (int i = meio; i < nroRegArq - 1; i++){
            vetorId[i] = vetorId[i + 1];        
        }

}

long get_offset(REG_DADO_ID ** vetorId, int id, int nroRegArq){
    int meio, dir, esq;
    esq = 0;
    dir = nroRegArq - 1;

    // Busca binária para encontrar a posição da chave primária
    while (esq <= dir) {
        meio = (esq + dir) / 2;

        if (vetorId[meio]->id < id)
            esq = meio + 1;

        else if(vetorId[meio]->id > id)
            dir = meio - 1;

        else
            break; // meio = posicao correta
        
    }

    if(vetorId[meio]->id == id) // verifica se o id foi encontrado, caso tenha sido, retorna o byteoffset associado a ele
        return vetorId[meio]->byteoffset;
    else 
        return -1;
}


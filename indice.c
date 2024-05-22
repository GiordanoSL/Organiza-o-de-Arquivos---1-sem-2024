#include "indice.h"

REG_DADO_ID ** carregamento(FILE * fId, int nroRegArq){
    if(fId == NULL){                    // arquivo de indice inválido
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }

    fseek(fId, 0, SEEK_SET);

    REG_CAB_ID regCabId;
    readRegCabId(fId, &regCabId);
    if(regCabId.status == '0'){         // arquivo de indice inconsistente
        printf("Falha no processamento do arquivo.\n");
        return NULL;
    }

    regCabId.status = '0';
    fseek(fId, 0, SEEK_SET);
    writeRegCabId(fId, regCabId);   // arquivo de índice aberto para carregamento -> arquivo inconsistente

    REG_DADO_ID **vetorId = (REG_DADO_ID **)malloc(nroRegArq * sizeof(REG_DADO_ID *));  // Corrigido o tamanho da alocação
 
    for (int i = 0; i < nroRegArq; i++){
        REG_DADO_ID * regDadoId = (REG_DADO_ID *) malloc(sizeof(REG_DADO_ID));
        readRegDadoId(fId, regDadoId);
        vetorId[i] = regDadoId;
    }

    return vetorId;
}

// deve ser chamada com fId após o cabeçalho
void reescrita(FILE * fId, REG_DADO_ID ** vetorId, int nroRegArq){
    if(fId == NULL || vetorId == NULL){                    // arquivo de indice inválido ou vetor inválido
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    //fseek(fId, 0, SEEK_SET);

    REG_CAB_ID regCabId;
    // readRegCabId(fId, &regCabId);

    for (int i = 0; i < nroRegArq; i++){
        writeRegDadoId(fId, *(vetorId[i]));    // reescreve arquivo de índice
    }

    regCabId.status = '1';
    fseek(fId, 0, SEEK_SET);
    writeRegCabId(fId, regCabId);           // muda status para consistente (atualizado)
    
}

void insert_ordenado(REG_DADO_ID ** vetorId, REG_DADO_ID * regDadoId, int count) {
    int left = 0;
    int right = count - 1;
    int mid;
    int position = count;  // Padrão: insere no fim

    // Busca binária para encontrar a posição de inserção
    while (left <= right) {
        mid = (left + right) / 2;
        if (vetorId[mid]->id < regDadoId->id) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    position = left;  // A posição de inserção será onde left aponta

    // Move os elementos para dar espaço para o novo elemento
    for (int i = count; i > position; i--) {
        vetorId[i] = vetorId[i - 1];
    }

    // Insere o novo elemento na posição correta
    vetorId[position] = regDadoId;
    // vetorId[position]->byteoffset = regDadoId->byteoffset;
    // vetorId[position]->id = regDadoId->id;
}

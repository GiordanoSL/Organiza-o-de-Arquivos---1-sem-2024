/*-----------------------------------------------------
Autores: Giordano Santorum Lorenzetto - nUSP 14574017
         Victor Moreli dos Santos - nUSP 14610514
-------------------------------------------------------*/


#include "delete.h"

void remove_reg_dado(FILE * fBin, REG_DADO_ID ** vetorIndices, long offset, REG_DADO * regDado, REG_CAB * regCab);

void delete_from_where(char * arquivoBinario, char * arquivoIndice, int numRem){

    // Abertura do arquivo binario para leitura e escrita
    FILE * fBin = fopen(arquivoBinario, "rb+");
    if(fBin == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Abertura do arquivo de indice para leitura
    FILE * fId = fopen(arquivoIndice, "rb");
    if(fId == NULL){
        create_index(arquivoBinario, arquivoIndice);  // caso o arquivo ainda nao exista cria o arquivo por meio da funcionalidade 4 (create_index)
        fId = fopen(arquivoIndice, "rb");
        if(fId == NULL){
            fclose(fBin);
            printf("Falha no processamento do arquivo.\n");
            return;
        }
    }

    REG_CAB regCab;
    REG_CAB_ID regCabId;

    readRegCabBin(fBin, &regCab);            // lê cabecalho do arquivo de dados
    readRegCabId(fId, &regCabId);            // lê cabecalho do arquivo de índices

    if (regCab.status == '0' || regCabId.status == '0') {  // se um dos dois estiver inconsistente, falha no processamento do arquivo
        fclose(fBin);
        fclose(fId);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // faz carregamento do arquivo de índices para a memória primária (vetorIndices), status = 0
    REG_DADO_ID ** vetorIndices = carregamento(fId, regCab.nroRegArq, 0);

    // Se o numero de arquivos registrados arquivados for zero, nao eh possível remover
    if(regCab.nroRegArq == 0){ 
        fclose(fBin);
        fclose(fId);
        return;
    }

    // Atualiza status do arquivo para inconsistente
    regCab.status = '0';
    fseek(fBin, 0, SEEK_SET);
    writeRegCabBin(fBin, regCab);

    // Declara structs de registro de dados auxiliares
    REG_DADO regDadoModelo; // struct que serve de modelo para a busca, inicia com valores nulos
    REG_DADO regDado; // struct q armazenará temporariamente registros de dados lidos do arquivo e entrada
    
    // iniciando as remoções
    for(int i = 0; i < numRem; i++){

        // lendo os campos e seus valores, e atribuindo eles ao registro modelo
        lerCamposReg(&regDadoModelo);

        // se o campo ID for especificado, o arquivo de indices é utilizado para a busca
        if(regDadoModelo.id != -1){
            // busca binaria do offset a partir do ID
            long offset = get_offset(vetorIndices, regDadoModelo.id, regCab.nroRegArq);
            // se o ID for encontrado faz a remoção
            if(offset != -1){
                // le o registro de dados no offset correto para comparar com os outros campos especificados
                fseek(fBin, offset, SEEK_SET);
                readRegDadoBin(fBin, &regDado);
                // só faz a remoção se o registro se encaixar em todas as especificações
                if(comparaRegDado(regDadoModelo, regDado)){
                    remove_reg_dado(fBin, vetorIndices, offset, &regDado, &regCab);
                }
            }
        }else{ // quando o ID não é especificado a busca é sequencial pelo arquivo
            while(readRegDadoBin(fBin, &regDado) != 0){ // enquanto a leitura de registros for possível vai lendo registro a registro do arquivo binario de entrada
            // se o registro lido nao estiver removido, compara com o registro modelo 
            if(regDado.removido == '0'){
                if(comparaRegDado(regDadoModelo, regDado)){
                    //REGISTRO PARA SER REMOVIDO ENCONTRADO

                    // volta o ponteiro de arquivo binario para o byteOffset inicial do registro que vai ser removido
                    fseek(fBin, -(regDado.tamanhoRegistro), SEEK_CUR); 
                    long offset = ftell(fBin); // byte offset do registro que será removido
                    remove_reg_dado(fBin, vetorIndices, offset, &regDado, &regCab);
                }                
            }
            //liberacao da memoria usada pelo registro de dados temporario
            free(regDado.nomeJogador);
            free(regDado.nacionalidade);
            free(regDado.nomeClube);              
            }
        }
        
        // liberação da memória usada pelo registro modelo
        if(regDadoModelo.nacionalidade != NULL) free(regDadoModelo.nacionalidade);
        if(regDadoModelo.nomeClube != NULL) free(regDadoModelo.nomeClube);
        if(regDadoModelo.nomeJogador != NULL) free(regDadoModelo.nomeJogador);

        fseek(fBin, TAM_REG_CAB, SEEK_SET); // volta para o inicio do arquivo para a próxima remoção (já pula o cabeçalho)

    }
    // Fecha o arquivo de índices para criá-lo novamente a partir do recarregamento do vetor de índices que está em memória primária
    fclose(fId);
    // Abertura do arquivo de indice, agora para escrita (exclui o anterior)
    fId = fopen(arquivoIndice, "wb");
    if(fId == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    // reescrita a partir do vetor de índices atualizado
    reescrita(fId, vetorIndices, regCab.nroRegArq);

    // Remoções realizadas com sucesso, status do arquivo volta para consistente 
    regCab.status = '1';    
    fseek(fBin, 0, SEEK_SET); // volta para o inicio do arquivo e atualiza o registro de cabeçalho
    writeRegCabBin(fBin, regCab);

    //fecha os arquivos
    fclose(fBin);
    fclose(fId);

    binarioNaTela(arquivoBinario);
    binarioNaTela(arquivoIndice);

}

// função auxiliar que remove um registro de dados do arquivo de dados
void remove_reg_dado(FILE * fBin, REG_DADO_ID ** vetorIndices, long offset, REG_DADO * regDado, REG_CAB * regCab){

    int tamanhoAux; // variável auxiliar para guardar o tamanho do registro atual na lista de removidos
    long proxAux; // variável auxiliar para guardar o byte offset do proximo registr na lista de removidos
    long offsetAux; // variável auxiliar para guardar o byte offset do registro atual na lista de removidos
    long offsetAnt; // variável auxiliar para guardar o byte offset do registro anterior na lista de removidos

    if(regCab->topo == -1){ // caso topo = -1 (nenhum registro foi removido) o registro removido se torna o topo
        // insercao no topo
        regDado->prox = regCab->topo;
        regCab->topo = offset;
    }else{

        // lista não está vazia, vai para o primeiro elemento
        fseek(fBin, regCab->topo + 1, SEEK_SET); // ja pula o char de removido

        offsetAux = regCab->topo;
        // le o tamanho do registro atual e o offset do proximo registro da lista de removidos
        fread(&tamanhoAux, sizeof(int), 1, fBin);
        fread(&proxAux, sizeof(long), 1, fBin);

        if(regDado->tamanhoRegistro <= tamanhoAux){ // insercao no topo
            regDado->prox = regCab->topo;
            regCab->topo = offset;

        }else{
            // 'anda' na lista de removidos uma primeira vez
            offsetAnt = offsetAux;
            offsetAux = proxAux;

            fseek(fBin, offsetAux + 1, SEEK_SET); // ja pula o char de removido

            // le o tamanho do registro atual e o offset do proximo registro da lista de removidos
            fread(&tamanhoAux, sizeof(int), 1, fBin);
            fread(&proxAux, sizeof(long), 1, fBin);

            while(tamanhoAux <= regDado->tamanhoRegistro && offsetAux != -1){
                // 'anda' na lista de removidos
                offsetAnt = offsetAux;
                offsetAux = proxAux;

                // le o tamanho do registro atual e o offset do proximo registro da lista de removidos
                fseek(fBin, offsetAux + 1, SEEK_SET); // ja pula o char de removido
                fread(&tamanhoAux, sizeof(int), 1, fBin);
                fread(&proxAux, sizeof(long), 1, fBin);
            }

            // atualiza o prox do registro anterior na lista de removidos (inserção do novo registro removido)
            fseek(fBin, offsetAnt + 5, SEEK_SET); // ja pula o char de removido e tamanho do registro
            fwrite(&offset, sizeof(long), 1, fBin);
            // atualiza o prox do registro que está sendo removido
            regDado->prox = offsetAux;
        }
    }
    // faz a remoção do indice no vetor de índices
    remove_indice(vetorIndices, regDado->id, regCab->nroRegArq);
    // atualiza char de removido do registro de dados
    regDado->removido = '1'; 
    // atualiza as informações do cabeçalho
    regCab->nroRegRem ++;
    regCab->nroRegArq --;
    fseek(fBin, offset, SEEK_SET); // volta para o offset do registro que está sendo removido
    writeRegDadoBin(fBin, *regDado); // atualiza o registro removido no arquivo
}
#include "insert.h"

void insert_into(char *arquivoDados, char *arquivoIndice, int numInsert) {
    FILE *fDados = fopen(arquivoDados, "rb+");
    if (fDados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE *fId = fopen(arquivoIndice, "rb+");
    if (fId == NULL) {
        create_index(arquivoDados, arquivoIndice);
        fId = fopen(arquivoIndice, "rb+");
    }

    REG_CAB regCabDados;
    REG_CAB_ID regCabId;

    readRegCabBin(fDados, &regCabDados);
    readRegCabId(fId, &regCabId);

    if (regCabDados.status == '0' || regCabId.status == '0') {
        //printf("Falha no processamento do arquivo.\n");
        fclose(fDados);
        fclose(fId);
        return;
    }

    REG_DADO_ID **vetorIndices = carregamento(fId, regCabDados.nroRegArq + numInsert);

    REG_DADO regDadoModelo;
    
    int tam;
    long prox;
    char removido;
    long pos, ant;
    char flagInsertFim = '0';
    

    for (int i = 1; i <= numInsert; i++) {
        // Inicializando e lendo os campos

        REG_DADO_ID * regDadoIdModelo = (REG_DADO_ID *) malloc(sizeof(REG_DADO_ID));
        
        char id[50];
        scan_quote_string(id);
        regDadoModelo.id = atoi(id);
        
        getchar();

        char idade[10];
        scan_quote_string(idade);
        if(strcmp(idade, "") == 0) strcpy(idade, "-1");
        regDadoModelo.idade = atoi(idade);
        getchar();

        regDadoModelo.nomeJogador = (char *)calloc(50, sizeof(char));
        if (regDadoModelo.nomeJogador == NULL) exit(1);
        scan_quote_string(regDadoModelo.nomeJogador);
        if(strcmp(regDadoModelo.nomeJogador, "") == 0)
            regDadoModelo.tamNomeJog = 0;
        else
            regDadoModelo.tamNomeJog= strlen(regDadoModelo.nomeJogador);
        getchar();

        regDadoModelo.nacionalidade = (char *)calloc(50, sizeof(char));
        if (regDadoModelo.nacionalidade == NULL) exit(1);
        scan_quote_string(regDadoModelo.nacionalidade);
        if(strcmp(regDadoModelo.nacionalidade, "") == 0)
            regDadoModelo.tamNacionalidade = 0;
        else
            regDadoModelo.tamNacionalidade = strlen(regDadoModelo.nacionalidade);
        getchar();

        regDadoModelo.nomeClube = (char *)calloc(50, sizeof(char));
        if (regDadoModelo.nomeClube == NULL) exit(1);
        scan_quote_string(regDadoModelo.nomeClube);
        if(strcmp(regDadoModelo.nomeClube, "") == 0)
            regDadoModelo.tamNomeClube = 0;
        else
            regDadoModelo.tamNomeClube = strlen(regDadoModelo.nomeClube);
        getchar();

        regDadoModelo.removido = '0';
        regDadoModelo.tamanhoRegistro = 33 + regDadoModelo.tamNomeJog + regDadoModelo.tamNacionalidade + regDadoModelo.tamNomeClube;
        regDadoModelo.prox = -1;

        regDadoIdModelo->id = regDadoModelo.id;

        if (regCabDados.nroRegRem == 0) {
            // Inserir no fim
            //printf("inserindo no fim\n");
            fseek(fDados, regCabDados.proxByteOffset, SEEK_SET);
            regDadoIdModelo->byteoffset = regCabDados.proxByteOffset;
            writeRegDadoBin(fDados, regDadoModelo);
            regCabDados.proxByteOffset += regDadoModelo.tamanhoRegistro;
            insert_ordenado(vetorIndices, regDadoIdModelo, regCabDados.nroRegArq + i - 1);   
        } else {
            // Inserir em espaço reaproveitado
            fseek(fDados, regCabDados.topo, SEEK_SET);
            ant = ftell(fDados);
            fread(&removido, sizeof(char), 1, fDados);
            fread(&tam, sizeof(int), 1, fDados);

            if (tam >= regDadoModelo.tamanhoRegistro) {
                // Inserir no topo
                //printf("inserindo no topo\n");
                int dif = tam - regDadoModelo.tamanhoRegistro;
                regDadoModelo.tamanhoRegistro = tam;
                fread(&prox, sizeof(long), 1, fDados);
                regCabDados.topo = prox;
                fseek(fDados, ant, SEEK_SET);
                writeRegDadoBin(fDados, regDadoModelo);
                if(dif > 0)
                    preencheLixo(fDados, dif);
                regDadoIdModelo->byteoffset = ant;
                insert_ordenado(vetorIndices, regDadoIdModelo, regCabDados.nroRegArq + i - 1);
                regCabDados.nroRegRem += -1;
            } else {
                while (tam < regDadoModelo.tamanhoRegistro) {
                    ant = ftell(fDados);
                    fread(&prox, sizeof(long), 1, fDados);
                    if (prox != -1) {
                        fseek(fDados, prox, SEEK_SET);
                        pos = ftell(fDados);
                        fread(&removido, sizeof(char), 1, fDados);
                        fread(&tam, sizeof(int), 1, fDados);
                    } else {
                        // Lista acabou, insere no fim
                        //printf("inserindo no fim pois a lista acabou\n");
                        fseek(fDados, regCabDados.proxByteOffset, SEEK_SET);
                        regDadoIdModelo->byteoffset = regCabDados.proxByteOffset;
                        writeRegDadoBin(fDados, regDadoModelo);
                        regCabDados.proxByteOffset += regDadoModelo.tamanhoRegistro;
                        flagInsertFim = '1';
                        insert_ordenado(vetorIndices, regDadoIdModelo, regCabDados.nroRegArq + i - 1);
                        break;
                    }
                }

                if (flagInsertFim == '1') continue;

                // Insere "no meio" da lista
                //printf("inserindo no meio\n");
                int dif = tam - regDadoModelo.tamanhoRegistro;
                regDadoModelo.tamanhoRegistro = tam;
                fread(&prox, sizeof(long), 1, fDados);
                fseek(fDados, ant, SEEK_SET);
                fwrite(&prox, sizeof(long), 1, fDados);
                fseek(fDados, pos, SEEK_SET);
                writeRegDadoBin(fDados, regDadoModelo);
                regCabDados.nroRegRem -= 1;
                if(dif > 0)
                    preencheLixo(fDados, dif);
                regDadoIdModelo->byteoffset = pos;
                insert_ordenado(vetorIndices, regDadoIdModelo, regCabDados.nroRegArq + i - 1);
            }
        }

        free(regDadoModelo.nomeJogador);
        free(regDadoModelo.nacionalidade);
        free(regDadoModelo.nomeClube);
    }

    regCabDados.nroRegArq += numInsert;
    fseek(fDados, 0, SEEK_SET);
    writeRegCabBin(fDados, regCabDados);

    fseek(fId, sizeof(char), SEEK_SET); // volta para a posição após o cabecalho para chamar reescrita
    reescrita(fId, vetorIndices, regCabDados.nroRegArq);

    fclose(fDados);
    fclose(fId);

    binarioNaTela(arquivoDados);
    binarioNaTela(arquivoIndice);
}

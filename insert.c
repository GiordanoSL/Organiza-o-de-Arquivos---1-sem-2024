/*-----------------------------------------------------
Autores: Giordano Santorum Lorenzetto - nUSP 14574017
         Victor Moreli dos Santos - nUSP 14610514
-------------------------------------------------------*/


#include "insert.h"

void insert_into(char *arquivoDados, char *arquivoIndice, int numInsert) {
    FILE *fDados = fopen(arquivoDados, "rb+");      // abre arquivo de dados para leitura e escrita
    if (fDados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE *fId = fopen(arquivoIndice, "rb+");        // abre arquivo de índice para leitura e escrita
    if (fId == NULL) {                              // se o arquivo de índices não existir
        create_index(arquivoDados, arquivoIndice);  // cria o arquivo por meio da funcionalidade 4 (create_index)
        fId = fopen(arquivoIndice, "rb+");
        if (fId == NULL) {                          // verifica se o arquivo foi aberto sem erros
            fclose(fDados);                         // fecha o arquivo que ja estava aberto
            printf("Falha no processamento do arquivo.\n");
            return;
        }
    }

    REG_CAB regCabDados;
    REG_CAB_ID regCabId;

    readRegCabBin(fDados, &regCabDados);            // lê cabecalho do arquivo de dados
    readRegCabId(fId, &regCabId);                   // lê cabecalho do arquivo de índices

    if (regCabDados.status == '0' || regCabId.status == '0') {  // se um dos dois estiver inconsistente, falha no processamento do arquivo
        printf("Falha no processamento do arquivo.\n");
        fclose(fDados);
        fclose(fId);
        return;
    }

    // faz carregamento do arquivo de índices para a memória primária (vetorIndices), status = 0
    REG_DADO_ID ** vetorIndices = carregamento(fId, regCabDados.nroRegArq, numInsert); // já aloca espaço para as inserções no vetor

    REG_DADO regDadoAux;    // registro de dados auxiliar para inserção no arquivo de dados
    
    // variáveis auxiliares para uso na inserção
    int tam;    // tamanho do arquivo onde a busca se encontra
    long prox;  // byteoffset do proximo da lista de removidos
    char removido;  // usado para pular char de removido dos registros
    long pos, ant;  // pos - byteoffset do atual, ant - byteoffset do anterior 
    char flagInsertFim = '0';   // flag de inserção no fim após percorrer toda a lista

    for (int i = 1; i <= numInsert; i++) {
        // Inicializando e lendo os campos fornecidos pelo usuário

        REG_DADO_ID * regDadoIdAux = (REG_DADO_ID *) malloc(sizeof(REG_DADO_ID));   // registro de índices auxiliar para inserção no vetor de índices
        
        // Lê o id
        char id[50];
        scan_quote_string(id);
        regDadoAux.id = atoi(id);

        // Preenche id do registro de índice auxiliar
        regDadoIdAux->id = regDadoAux.id;
        
        getchar();  // pula o espaço

        // Lê a idade
        char idade[10];
        scan_quote_string(idade);   // retorna "" se for NULO
        if(strcmp(idade, "") == 0) strcpy(idade, "-1"); // Se for "", preenche com -1
        regDadoAux.idade = atoi(idade);

        getchar();  // pula o espaço

        // Lê o nome do jogador e salva o tamanho
        readQuoteField(&(regDadoAux.nomeJogador), &(regDadoAux.tamNomeJog));

        // Lê a nacionalidade e salva o tamanho
        readQuoteField(&(regDadoAux.nacionalidade), &(regDadoAux.tamNacionalidade));

        // Lê o nome do clube e salva o tamanho
        readQuoteField(&(regDadoAux.nomeClube), &(regDadoAux.tamNomeClube));

        regDadoAux.removido = '0';  // não removido logicamente
        regDadoAux.tamanhoRegistro = 33 + regDadoAux.tamNomeJog + regDadoAux.tamNacionalidade + regDadoAux.tamNomeClube;    // calcula tamanho
        regDadoAux.prox = -1;       // inicializa prox com -1


        if (regCabDados.nroRegRem == 0) {   // Se não houver registros removidos, insere no fim
            // Inserir no fim
            
            fseek(fDados, regCabDados.proxByteOffset, SEEK_SET);    // pula para o próximo byte offset vazio para inserção (fim do arquivo) disponível no cabecalho
            regDadoIdAux->byteoffset = regCabDados.proxByteOffset;  // preenche byteoffset do registro de índice auxiliar
            writeRegDadoBin(fDados, regDadoAux);                    // escreve registro de dados
            regCabDados.proxByteOffset += regDadoAux.tamanhoRegistro; // atualiza próximo byteoffset vazio para inserção
            insert_ordenado(vetorIndices, regDadoIdAux, regCabDados.nroRegArq + i - 1);  // insere ordenado no vetor de índices
        } else {    // Se há registros removidos, possível reaproveitamento de espaço
            // Possível inserção em espaço reaproveitado
            fseek(fDados, regCabDados.topo, SEEK_SET);  // vai para o topo da lista dos logicamente removidos
            ant = regCabDados.topo;                     // salva o anterior
            fread(&removido, sizeof(char), 1, fDados);  // pula o removido
            fread(&tam, sizeof(int), 1, fDados);        // lê o tamanho

            // se o tamanho do registro logicamente removido for maior que o tamanho do registro que será inserido
            if (tam >= regDadoAux.tamanhoRegistro) {
                // Inserir no topo

                int dif = tam - regDadoAux.tamanhoRegistro; // calcula diferença de tamanho entre o espaço disponível e o registro
                regDadoAux.tamanhoRegistro = tam;           // novo tamanho do registro é o espaço disponível
                fread(&prox, sizeof(long), 1, fDados);      // lê o próximo
                regCabDados.topo = prox;                    // o topo recebe o próximo da lista de removidos
                fseek(fDados, ant, SEEK_SET);               // volto para o anterior (nesse caso, é o início do registro que estava no topo)
                writeRegDadoBin(fDados, regDadoAux);        // escreve registro
                preencheLixo(fDados, dif);                  // preenche lixo ($)
                regDadoIdAux->byteoffset = ant;             // preenche byteoffset do registro auxiliar de índice
                insert_ordenado(vetorIndices, regDadoIdAux, regCabDados.nroRegArq + i - 1); // insere ordenado no vetor de índices
                regCabDados.nroRegRem = regCabDados.nroRegRem - 1;  // diminui nro de removidos do arquivo de dados

            } else {        // inserção não ocorrerá no topo
                
                while (tam < regDadoAux.tamanhoRegistro) {      // enquanto o tamanho do registro onde se encontra é menor do que o que será inserido
                    
                    ant = ftell(fDados);    //ant guarda posição do anterior antes do campo prox 
                    fread(&prox, sizeof(long), 1, fDados);  // lê o próximo

                    if (prox != -1) {   // se não estou no fim da lista

                        fseek(fDados, prox, SEEK_SET);  // vai para o próximo
                        pos = prox;                     // atualiza posição atual
                        fread(&removido, sizeof(char), 1, fDados); // pula o removido
                        fread(&tam, sizeof(int), 1, fDados);       // lê o tamanho

                    } else {                // se estou no fim da lista

                        // Lista acabou, insere no fim
                        
                        fseek(fDados, regCabDados.proxByteOffset, SEEK_SET);    // pula para o próximo byte offset vazio para inserção (fim do arquivo) disponível no cabecalho
                        regDadoIdAux->byteoffset = regCabDados.proxByteOffset;  // preenche byteoffset do registro de índice auxiliar
                        writeRegDadoBin(fDados, regDadoAux);                    // escreve registro
                        regCabDados.proxByteOffset += regDadoAux.tamanhoRegistro;   // atualiza próximo byteoffset vazio para inserção
                        flagInsertFim = '1';          // flag para indicar inserção no fim quando a lista foi percorrida e não foi possível reaproveitar espaço
                        insert_ordenado(vetorIndices, regDadoIdAux, regCabDados.nroRegArq + i - 1); // insere ordenado no vetor de índices
                        break;  // sai do while

                    }
                }

                // saiu em posição disponível para reaproveitamento ou inseriu no fim após chegar no fim da lista

                if (flagInsertFim == '1'){  // inserção no fim da lista, liberado memória do registro auxiliar e vai pra próxima inserção
                    free(regDadoAux.nomeJogador);
                    free(regDadoAux.nacionalidade);
                    free(regDadoAux.nomeClube);
                    continue;   // próxima inserção
                }
                    
                
                // Insere no "meio" da lista, com aproveitamento de espaço
                
                int dif = tam - regDadoAux.tamanhoRegistro; // salva diferença entre espaço disponível e tamanho do registro
                regDadoAux.tamanhoRegistro = tam;   // novo tamanho do registro = espaço disponível

                // atualização da lista (proximo do anterior = proximo do atual)
                fread(&prox, sizeof(long), 1, fDados);  // pega o proximo do atual
                fseek(fDados, ant, SEEK_SET);           // vai para o anterior
                fwrite(&prox, sizeof(long), 1, fDados); // proximo do anterior = proximo do atual
                fseek(fDados, pos, SEEK_SET);           // volta para o atual

                writeRegDadoBin(fDados, regDadoAux); // escreve novo registro
                preencheLixo(fDados, dif);  // preenche lixo
                regCabDados.nroRegRem = regCabDados.nroRegRem - 1; // diminui nro de removidos do arquivo de dados 
                regDadoIdAux->byteoffset = pos; // preenche byteoffset do registro de índice auxiliar
                insert_ordenado(vetorIndices, regDadoIdAux, regCabDados.nroRegArq + i - 1); // insere ordenado no vetor de índices
            }
        }

        // libera memória nos casos inserção no topo, inserção no fim c/ nro de removidos = 0, inserção no "meio"
        free(regDadoAux.nomeJogador);
        free(regDadoAux.nacionalidade);
        free(regDadoAux.nomeClube);
    }

    regCabDados.nroRegArq += numInsert; // atualiza número de registros no arquivo
    fseek(fDados, 0, SEEK_SET);         // vai para o início
    writeRegCabBin(fDados, regCabDados);    // escreve novo cabeçalho

    reescrita(fId, vetorIndices, regCabDados.nroRegArq);    // reescreve arquivo de índices a partir do vetor, status = 1

    // liberação de memória do vetor de índices
    for (int i = 0; i < regCabDados.nroRegArq; i++)
    {
        free(vetorIndices[i]);
        vetorIndices[i] = NULL;
    }
    free(vetorIndices);
    vetorIndices = NULL;

    // fecha os arquivos
    fclose(fDados);
    fclose(fId);

    // binario na tela para os arquivos
    binarioNaTela(arquivoDados);
    binarioNaTela(arquivoIndice);
}

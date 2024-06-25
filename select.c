/*-----------------------------------------------------
Autores: Giordano Santorum Lorenzetto - nUSP 14574017
         Victor Moreli dos Santos - nUSP 14610514
-------------------------------------------------------*/


#include "select.h"

void select_from(char * arquivoDados){

    // Abertura do arquivo binario para leitura
    FILE * fileDados = fopen(arquivoDados, "rb");
    if(fileDados == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    REG_CAB regCab; // Declara registro de cabeçalho
    readRegCabBin(fileDados, &regCab); // Lê registro de cabeçalho

    // Se o arquivo estiver inconsistente, houve falha no processamento do arquivo
    if(regCab.status == '0'){
        printf("Falha no processamento do arquivo.\n");
        fclose(fileDados);
        return;
    }

    // Se o numero de arquivos registrados for zero, nao eh necessario fazer a busca
    if(regCab.nroRegArq == 0){
        printf("Registro inexistente.\n\n");
        fclose(fileDados);
        return;
    }

    REG_DADO regDado; // Declara registro de dados

    while(readRegDadoBin(fileDados, &regDado)){ // Lê registro de dados um por um até chegar ao fim do arquivo

        if(regDado.removido == '0'){ // se o registro lido não estiver removido, imprime na saida padrão
            printf("Nome do Jogador: %s\n", regDado.tamNomeJog == 0 ? "SEM DADO" : regDado.nomeJogador);
            printf("Nacionalidade do Jogador: %s\n", regDado.tamNacionalidade == 0 ? "SEM DADO" : regDado.nacionalidade);
            printf("Clube do Jogador: %s\n", regDado.tamNomeClube == 0 ? "SEM DADO" : regDado.nomeClube);
            printf("\n");
        }

        // liberação de memoria das strings de regDado
        free(regDado.nomeJogador);
        free(regDado.nacionalidade);
        free(regDado.nomeClube);
        
    }    
    
    // fechando o arquivo binário
    fclose(fileDados);
}

void select_from_where(char * arquivoDados, int num_buscas){

    // Abertura do arquivo binario para leitura
    FILE * fileDados = fopen(arquivoDados, "rb");
    if(fileDados == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    REG_CAB regCab; // Declara registro de cabeçalho
    readRegCabBin(fileDados, &regCab); // Lê registro de cabeçalho

    // Se o numero de arquivos registrados for zero ou se o arquivo estiver inconsistente,
    // nao eh necessario fazer a busca
    if(regCab.nroRegArq == 0 || regCab.status == '0'){
        printf("Registro inexistente.\n"); 
        fclose(fileDados);
        return;
    }

    // Declara structs de registro de dados auxiliares
    REG_DADO regDadoModelo; // struct que serve de modelo para a busca, inicia com valores nulos
    regDadoModelo.id = -1;
    regDadoModelo.idade = -1;
    regDadoModelo.nacionalidade = NULL;
    regDadoModelo.nomeClube = NULL;
    regDadoModelo.nomeJogador = NULL;

    REG_DADO regDado; // struct q armazenará temporariamente registros de dados lidos do arquivo e entrada

    char reg_encontrado = 0; // flag para determinar se ao menos 1 registro foi encontrado
    
    for(int i = 1; i <= num_buscas; i++){

        // Inicio da i-esima busca
        printf("Busca %d\n\n", i);
        
        // lendo os campos e seus valores, e atribuindo eles ao registro modelo
        lerCamposRegParcial(&regDadoModelo);

        // flag de registro encontrado = 0
        reg_encontrado = 0;
        while(readRegDadoBin(fileDados, &regDado)){ // Lê registro de dados um por um até chegar ao fim do arquivo

            // se o registro lido nao estiver removido, compara com o registro modelo 
            if(regDado.removido == '0'){
                if(comparaRegDado(regDadoModelo, regDado)){ // compara nomeJog caso o nomeJog do modelo nao for nulo
                    printf("Nome do Jogador: %s\n", regDado.tamNomeJog == 0 ? "SEM DADO" : regDado.nomeJogador);
                    printf("Nacionalidade do Jogador: %s\n", regDado.tamNacionalidade == 0 ? "SEM DADO" : regDado.nacionalidade);
                    printf("Clube do Jogador: %s\n", regDado.tamNomeClube == 0 ? "SEM DADO" : regDado.nomeClube);
                    printf("\n");        
                    reg_encontrado = 1;
                }                
            }

            //liberacao da memoria usada pelo registrador de dados temporario
            free(regDado.nomeJogador);
            free(regDado.nacionalidade);
            free(regDado.nomeClube);

            // se o id foi levado em consideraçao na busca e o registro foi encontrado a busca para pois o id é um campo unico  
            if(regDadoModelo.id != -1 && reg_encontrado == 1) 
                break;                 
        }
        
        // se nenhum registro foi encontrado, informamos o usuario
        if(reg_encontrado == 0) printf("Registro inexistente.\n\n");

        if(regDadoModelo.nacionalidade != NULL) free(regDadoModelo.nacionalidade);
        if(regDadoModelo.nomeClube != NULL) free(regDadoModelo.nomeClube);
        if(regDadoModelo.nomeJogador != NULL) free(regDadoModelo.nomeJogador);

        // resetando todos os campos do registro modelo com valores nulos deixando pronto para a proxima busca
        regDadoModelo.id = -1;
        regDadoModelo.idade = -1;
        regDadoModelo.nacionalidade = NULL;
        regDadoModelo.nomeClube = NULL;
        regDadoModelo.nomeJogador = NULL;

        // retorna o ponteiro do arquivo para o inicio dos registros de dados (pula o cabecalho)
        fseek(fileDados, TAM_REG_CAB, SEEK_SET);
    }    

    //fecha o arquivo
    fclose(fileDados);
}

void select_from_arvoreB_id(char * arquivoDados, char * arquivoArvore, int num_buscas){
    // Abertura do arquivo binário de dados para leitura
    FILE * fileDados = fopen(arquivoDados, "rb");
    if(fileDados == NULL){
        // Se o arquivo não for encontrado, exibe mensagem de erro e retorna
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Abertura do arquivo binário da árvore B para leitura
    FILE * fileArvB = fopen(arquivoArvore, "rb");
    if(fileArvB == NULL){
        // Se o arquivo não for encontrado, exibe mensagem de erro e retorna
        fclose(fileDados);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Leitura do cabeçalho da árvore B
    ArvoreB arvB;
    lerCabArvoreB(fileArvB, &(arvB.cabecalho));

    // Leitura do cabeçalho do arquivo de dados
    REG_CAB regCab;
    readRegCabBin(fileDados, &regCab);

    // Verificação de consistência dos arquivos
    if (regCab.status == '0' || arvB.cabecalho.status == '0') {
        // Se algum dos arquivos estiver inconsistente, exibe mensagem de erro e retorna
        fclose(fileDados);
        fclose(fileArvB);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Estrutura para armazenar o registro de dados lido do arquivo
    REG_DADO regDado;
    int id; // ID a ser buscado
    long byte_offset; // Offset do byte no arquivo de dados

    // Realiza a busca `num_buscas` vezes
    for (int i = 1; i <= num_buscas; i++){

        // Início da i-ésima busca
        printf("BUSCA %d\n\n", i);

        // Pulando caracteres "id " -> Lê e descarta 3 caracteres
        getchar();
        getchar();
        getchar(); 
    
        // Leitura do ID a ser buscado
        scanf(" %d", &id);

        // Busca a chave na árvore B e obtém o byte offset do registro no arquivo de dados
        byte_offset = buscarChave(&arvB, fileArvB, arvB.cabecalho.noRaiz, id);

        // Verifica se a chave foi encontrada
        if(byte_offset == -1L){
            // Se não foi encontrada, exibe mensagem de registro inexistente
            printf("Registro inexistente.\n\n");
            continue;
        }

        // Posiciona o ponteiro do arquivo no byte offset encontrado
        fseek(fileDados, byte_offset, SEEK_SET);
        // Lê o registro de dados do arquivo
        readRegDadoBin(fileDados, &regDado);

        // Exibe os campos do registro de dados lido
        printf("Nome do Jogador: %s\n", regDado.tamNomeJog == 0 ? "SEM DADO" : regDado.nomeJogador);
        printf("Nacionalidade do Jogador: %s\n", regDado.tamNacionalidade == 0 ? "SEM DADO" : regDado.nacionalidade);
        printf("Clube do Jogador: %s\n", regDado.tamNomeClube == 0 ? "SEM DADO" : regDado.nomeClube);
        printf("\n"); 
        
        // Liberação da memória alocada para as strings do registro de dados
        free(regDado.nomeJogador);
        free(regDado.nacionalidade);
        free(regDado.nomeClube);
    }    
    
    // Fechando os arquivos binários
    fclose(fileDados);
    fclose(fileArvB);
}


void select_from_arvoreB(char * arquivoDados, char * arquivoArvore, int num_buscas){
    // Abertura do arquivo binário de dados para leitura
    FILE * fileDados = fopen(arquivoDados, "rb");
    if(fileDados == NULL){
        // Se o arquivo não for encontrado, exibe mensagem de erro e retorna
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Abertura do arquivo da árvore B para leitura
    FILE * fileArvB = fopen(arquivoArvore, "rb");
    if(fileArvB == NULL){
        // Se o arquivo não for encontrado, exibe mensagem de erro e retorna
        fclose(fileDados);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Leitura do cabeçalho da árvore B
    ArvoreB arvB;
    lerCabArvoreB(fileArvB, &(arvB.cabecalho));

    // Leitura do cabeçalho do arquivo de dados
    REG_CAB regCab;
    readRegCabBin(fileDados, &regCab);

    // Verifica a consistência dos arquivos
    if (regCab.status == '0' || arvB.cabecalho.status == '0') {
        // Se algum dos arquivos estiver inconsistente, fecha os arquivos, exibe uma mensagem de erro e retorna
        fclose(fileDados);
        fclose(fileArvB);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Declaração de structs auxiliares para os registros de dados
    REG_DADO regDadoModelo; // Struct que serve de modelo para a busca
    REG_DADO regDado; // Struct para armazenar temporariamente registros de dados lidos do arquivo

    char reg_encontrado = 0; // Flag para determinar se ao menos um registro foi encontrado
    long byte_offset; // Variável para armazenar o offset do byte no arquivo de dados

    // Loop para realizar múltiplas buscas
    for(int i = 1; i <= num_buscas; i++){

        // Início da i-ésima busca
        printf("Busca %d\n\n", i);
        
        // Lê os campos e seus valores, atribuindo-os ao registro modelo
        lerCamposRegParcial(&regDadoModelo);

        // Reset da flag de registro encontrado
        reg_encontrado = 0;

        // Se o ID for especificado, procura a partir da árvore B
        if(regDadoModelo.id != -1){
            // Busca a chave na árvore B e obtém o byte offset do registro no arquivo de dados
            byte_offset = buscarChave(&arvB, fileArvB, arvB.cabecalho.noRaiz, regDadoModelo.id);
            if(byte_offset != -1){
                // Posiciona o ponteiro do arquivo no byte offset encontrado
                fseek(fileDados, byte_offset, SEEK_SET);
                // Lê o registro de dados do arquivo
                readRegDadoBin(fileDados, &regDado);
                // Compara o registro modelo com o registro lido
                if(comparaRegDado(regDadoModelo, regDado)){
                    // Exibe os campos do registro de dados lido
                    printf("Nome do Jogador: %s\n", regDado.tamNomeJog == 0 ? "SEM DADO" : regDado.nomeJogador);
                    printf("Nacionalidade do Jogador: %s\n", regDado.tamNacionalidade == 0 ? "SEM DADO" : regDado.nacionalidade);
                    printf("Clube do Jogador: %s\n", regDado.tamNomeClube == 0 ? "SEM DADO" : regDado.nomeClube);
                    printf("\n");        
                    reg_encontrado = 1;
                }
                // Liberação da memória usada pelo registrador de dados temporário
                free(regDado.nomeJogador);
                free(regDado.nacionalidade);
                free(regDado.nomeClube);
            }
        }else{ // Caso o ID não seja especificado, procura registro a registro
            // Lê registros de dados um por um até chegar ao fim do arquivo
            while(readRegDadoBin(fileDados, &regDado)){
                // Se o registro lido não estiver removido, compara com o registro modelo
                if(regDado.removido == '0'){
                    if(comparaRegDado(regDadoModelo, regDado)){
                        // Exibe os campos do registro de dados lido
                        printf("Nome do Jogador: %s\n", regDado.tamNomeJog == 0 ? "SEM DADO" : regDado.nomeJogador);
                        printf("Nacionalidade do Jogador: %s\n", regDado.tamNacionalidade == 0 ? "SEM DADO" : regDado.nacionalidade);
                        printf("Clube do Jogador: %s\n", regDado.tamNomeClube == 0 ? "SEM DADO" : regDado.nomeClube);
                        printf("\n");        
                        reg_encontrado = 1;
                    }                
                }

                // Liberação da memória usada pelo registrador de dados temporário
                free(regDado.nomeJogador);
                free(regDado.nacionalidade);
                free(regDado.nomeClube);

                // Se o ID foi levado em consideração na busca e o registro foi encontrado, a busca para pois o ID é um campo único  
                if(regDadoModelo.id != -1 && reg_encontrado == 1) 
                    break;                 
            }
        }
        
        // Se nenhum registro foi encontrado, informa o usuário
        if(reg_encontrado == 0) printf("Registro inexistente.\n\n");

        // Liberação da memória usada pelo registro modelo, se necessário
        if(regDadoModelo.nacionalidade != NULL) free(regDadoModelo.nacionalidade);
        if(regDadoModelo.nomeClube != NULL) free(regDadoModelo.nomeClube);
        if(regDadoModelo.nomeJogador != NULL) free(regDadoModelo.nomeJogador);

        // Reseta todos os campos do registro modelo com valores nulos, deixando pronto para a próxima busca
        regDadoModelo.id = -1;
        regDadoModelo.idade = -1;
        regDadoModelo.nacionalidade = NULL;
        regDadoModelo.nomeClube = NULL;
        regDadoModelo.nomeJogador = NULL;

        // Retorna o ponteiro do arquivo para o início dos registros de dados (pula o cabeçalho)
        fseek(fileDados, TAM_REG_CAB, SEEK_SET);
    }    

    // Fecha os arquivos
    fclose(fileDados);
    fclose(fileArvB);
}

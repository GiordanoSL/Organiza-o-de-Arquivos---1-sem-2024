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
    // Abertura do arquivo binario para leitura
    FILE * fileDados = fopen(arquivoDados, "rb");
    if(fileDados == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Abertura do arquivo da arvoreB para leitura
    FILE * fileArvB = fopen(arquivoArvore, "rb");
    if(fileArvB == NULL){
        // se nenhum arquivo for encontrado, cria o arquivo
        create_arvoreB(arquivoDados, arquivoArvore);
        fileArvB = fopen(arquivoArvore, "rb");
        if(fileArvB == NULL){
            fclose(fileDados);
            printf("Falha no processamento do arquivo.\n");
            return;
        }
    }

    ArvoreB arvB;
    lerCabArvoreB(fileArvB, &(arvB.cabecalho));

    REG_CAB regCab;

    readRegCabBin(fileDados, &regCab);            // lê cabecalho do arquivo de dados  

    if (regCab.status == '0' || arvB.cabecalho.status == '0') {  // se um dos arquivos estiver inconsistente, falha no processamento do arquivo
        fclose(fileDados);
        fclose(fileArvB);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    REG_DADO regDado;
    int id;
    long byte_offset;

    for (int i = 1; i <= num_buscas; i++){
        // Inicio da i-esima busca
        printf("BUSCA %d\n\n", i);

        // pula 3 caracteres ("id ")
        getchar();
        getchar();
        getchar();
    
        scanf(" %d", &id); // lê o id a ser buscado

        byte_offset = buscarChave(&arvB, fileArvB, arvB.cabecalho.noRaiz, id);

        // Verifica se alguma chave foi encontrada
        if(byte_offset == -1L){
            printf("Registro inexistente.\n\n");
            continue;
        }

        fseek(fileDados, byte_offset, SEEK_SET);
        readRegDadoBin(fileDados, &regDado);

        printf("Nome do Jogador: %s\n", regDado.tamNomeJog == 0 ? "SEM DADO" : regDado.nomeJogador);
        printf("Nacionalidade do Jogador: %s\n", regDado.tamNacionalidade == 0 ? "SEM DADO" : regDado.nacionalidade);
        printf("Clube do Jogador: %s\n", regDado.tamNomeClube == 0 ? "SEM DADO" : regDado.nomeClube);
        printf("\n"); 
        
        // liberação de memoria das strings de regDado
        free(regDado.nomeJogador);
        free(regDado.nacionalidade);
        free(regDado.nomeClube);
        
    }    
    
    // fechando o arquivo binário
    fclose(fileDados);
    // fechando o arquivo da arvore
    fclose(fileArvB);
}

void select_from_arvoreB(char * arquivoDados, char * arquivoArvore, int num_buscas){

    // Abertura do arquivo binario para leitura
    FILE * fileDados = fopen(arquivoDados, "rb");
    if(fileDados == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Abertura do arquivo da arvoreB para leitura
    FILE * fileArvB = fopen(arquivoArvore, "rb");
    if(fileArvB == NULL){
        fclose(fileDados);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    ArvoreB arvB;
    lerCabArvoreB(fileArvB, &(arvB.cabecalho));

    REG_CAB regCab;

    readRegCabBin(fileDados, &regCab);            // lê cabecalho do arquivo de dados  

    if (regCab.status == '0' || arvB.cabecalho.status == '0') {  // se um dos arquivos estiver inconsistente, falha no processamento do arquivo
        fclose(fileDados);
        fclose(fileArvB);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Declara structs de registro de dados auxiliares
    REG_DADO regDadoModelo; // struct que serve de modelo para a busca

    REG_DADO regDado; // struct q armazenará temporariamente registros de dados lidos do arquivo e entrada

    char reg_encontrado = 0; // flag para determinar se ao menos 1 registro foi encontrado
    long byte_offset;
    
    for(int i = 1; i <= num_buscas; i++){

        // Inicio da i-esima busca
        printf("Busca %d\n\n", i);
        
        // lendo os campos e seus valores, e atribuindo eles ao registro modelo
        lerCamposRegParcial(&regDadoModelo);

        // flag de registro encontrado = 0
        reg_encontrado = 0;

        if(regDadoModelo.id != -1){ // se o id for especificado, procura a partir da arvore b
            byte_offset = buscarChave(&arvB, fileArvB, arvB.cabecalho.noRaiz, regDadoModelo.id);
            if(byte_offset != -1){
                fseek(fileDados, byte_offset, SEEK_SET);
                readRegDadoBin(fileDados, &regDado);
                if(comparaRegDado(regDadoModelo, regDado)){ //compara os outros campos também, além do ID
                    printf("Nome do Jogador: %s\n", regDado.tamNomeJog == 0 ? "SEM DADO" : regDado.nomeJogador);
                    printf("Nacionalidade do Jogador: %s\n", regDado.tamNacionalidade == 0 ? "SEM DADO" : regDado.nacionalidade);
                    printf("Clube do Jogador: %s\n", regDado.tamNomeClube == 0 ? "SEM DADO" : regDado.nomeClube);
                    printf("\n");        
                    reg_encontrado = 1;
                }
                //liberacao da memoria usada pelo registrador de dados temporario
                free(regDado.nomeJogador);
                free(regDado.nacionalidade);
                free(regDado.nomeClube);
            }
        }else{ // Caso o id não for especificado procura de registro a registro
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

        }
        
        // se nenhum registro foi encontrado, informa o usuario
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
    fclose(fileArvB);
}
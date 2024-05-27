/*-----------------------------------------------------
Autores: Giordano Santorum Lorenzetto - nUSP 14574017
         Victor Moreli dos Santos - nUSP 14610514
-------------------------------------------------------*/


#include "select.h"

void select_from(char * arquivobin){

    // Abertura do arquivo binario para leitura
    FILE * filebin = fopen(arquivobin, "rb");
    if(filebin == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Determinação do tamaho do arquivo
    fseek(filebin, 0, SEEK_END);
    long int tamArquivo = ftell(filebin);
    fseek(filebin, 0, SEEK_SET);

    REG_CAB regCab; // Declara registro de cabeçalho
    readRegCabBin(filebin, &regCab); // Lê registro de cabeçalho

    // Se o arquivo estiver inconsistente, houve falha no processamento do arquivo
    if(regCab.status == '0'){
        printf("Falha no processamento do arquivo.\n");
        fclose(filebin);
        return;
    }

    // Se o numero de arquivos registrados for zero, nao eh necessario fazer a busca
    if(regCab.nroRegArq == 0){
        printf("Registro inexistente.\n\n");
        fclose(filebin);
        return;
    }

    REG_DADO regDado; // Declara registro de dados

    while(ftell(filebin) != tamArquivo){
        // Lê os registros de dados
        readRegDadoBin(filebin, &regDado); // Lê registro de dados

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
    fclose(filebin);
}

void select_from_where(char * arquivobin, int num_buscas){

    // Abertura do arquivo binario para leitura
    FILE * filebin = fopen(arquivobin, "rb");
    if(filebin == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Determinação do tamanho do arquivo
    fseek(filebin, 0, SEEK_END);
    long int tamArquivo = ftell(filebin);
    fseek(filebin, 0, SEEK_SET);

    REG_CAB regCab; // Declara registro de cabeçalho
    readRegCabBin(filebin, &regCab); // Lê registro de cabeçalho

    // Se o numero de arquivos registrados for zero ou se o arquivo estiver inconsistente,
    // nao eh necessario fazer a busca
    if(regCab.nroRegArq == 0 || regCab.status == '0'){
        printf("Registro inexistente.\n"); 
        fclose(filebin);
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


    int num_campos; // qtd de campos que serão lidos na busca
    char * campo; // nome do campo que será lido na busca
    char reg_encontrado = 0; // flag para determinar se ao menos 1 registro foi encontrado
    
    for(int i = 1; i <= num_buscas; i++){

        // Inicio da i-esima busca
        printf("Busca %d\n\n", i);

        // lendo o numero de campos que serao lidos
        scanf(" %d", &num_campos);
        getchar(); //descarta o ' ' da entrada
        
        // lendo os campos e seus valores, e atribuindo eles ao registro modelo
        for (int j = 0; j < num_campos; j++){
            campo = lerStr();
            if(!strcmp(campo, "id")){
                scanf(" %d", &regDadoModelo.id);
                getchar(); //descarta o ' ' ou '\n' da entrada
            }else if (!strcmp(campo, "idade")){
                scanf(" %d", &regDadoModelo.idade);
                getchar(); //descarta o ' ' ou '\n' da entrada
            }else if (!strcmp(campo, "nacionalidade")){
                //tenta alocar memoria para o campo caso nao consiga sai do programa
                if((regDadoModelo.nacionalidade = (char *) calloc(sizeof(char), 50)) == NULL) exit(1); 
                scan_quote_string(regDadoModelo.nacionalidade); // lendo a nacionalidade inserida pelo usuario
                getchar(); //descarta o ' ' ou '\n' da entrada
            }else if (!strcmp(campo, "nomeJogador")){
                //tenta alocar memoria para o campo caso nao consiga sai do programa
                if((regDadoModelo.nomeJogador = (char *) calloc(sizeof(char), 50)) == NULL) exit(1);
                scan_quote_string(regDadoModelo.nomeJogador); // lendo o nome do jogador inserido pelo usuario
                getchar(); //descarta o ' ' ou '\n' da entrada
            }else if (!strcmp(campo, "nomeClube")){
                //tenta alocar memoria para o campo caso nao consiga sai do programa
                if((regDadoModelo.nomeClube = (char *) calloc(sizeof(char), 50)) == NULL) exit(1);
                scan_quote_string(regDadoModelo.nomeClube); // lendo o nome do clube inserido pelo usuario
                getchar(); //descarta o ' ' ou '\n' da entrada
            }
            
            free(campo);
        }

        // flag de registro encontrado = 0
        reg_encontrado = 0;
        while(ftell(filebin) != tamArquivo){

            // Lê o registro de dados do arquivo binario
            readRegDadoBin(filebin, &regDado);

            // se o registro lido nao estiver removido, compara com o registro modelo 
            if(regDado.removido == '0'){
                if((regDadoModelo.id == -1 || regDado.id == regDadoModelo.id) && // compara id caso o id do modelo nao for nulo
                (regDadoModelo.idade == -1 || regDado.idade == regDadoModelo.idade)&& // compara idade caso a idade do modelo nao for nula
                (regDadoModelo.nacionalidade == NULL || !strcmp(regDado.nacionalidade, regDadoModelo.nacionalidade)) && // compara nacionalidade caso a nacionalidade do modelo nao for nula
                (regDadoModelo.nomeClube == NULL || !strcmp(regDado.nomeClube, regDadoModelo.nomeClube))&& // compara nomeClube caso o nomeClube do modelo nao for nulo
                (regDadoModelo.nomeJogador == NULL || !strcmp(regDado.nomeJogador, regDadoModelo.nomeJogador))) // compara nomeJog caso o nomeJog do modelo nao for nulo
                {
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
        fseek(filebin, TAM_REG_CAB, SEEK_SET);
    }    

    //fecha o arquivo
    fclose(filebin);
}

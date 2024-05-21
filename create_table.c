#include "create_table.h"


// Função que implementa a funcionalidade 1: CREATE TABLE
void create_table(char * arquivoIn, char * arquivoOut){
    FILE * fIn = fopen(arquivoIn, "r");     // arquivoIn: nome do arquivo csv de entrada
    if(fIn == NULL){
        printf("Erro na abertura do .csv\n");
        exit(1);
    }
    FILE * fOut = fopen(arquivoOut, "wb");  // arquivoOut: nome do arquivo bin de saída
    if(fOut == NULL){
        printf("Erro na abertura do .bin\n");
        exit(1);
    }

    // Calcula tamanho do arquivo para usar como condição de parada no loop que o percorre
    fseek(fIn, 0, SEEK_END);
    long int tamArquivo = ftell(fIn);
    fseek(fIn, 0, SEEK_SET);

    REG_CAB cabecalho;      // receberá o cabecalho
    REG_DADO dados;         // recebera cada registro de dados
    
    // deixa espaço para o registro de cabecalho
    fseek(fOut, TAM_REG_CAB, SEEK_CUR);

    cabecalho.nroRegArq = 0;

    //pula primeira linha do .csv (cabeçalho do csv)
    while(getc(fIn) != '\n'){
        continue;
    }

    //lendo todos os valores
    while (ftell(fIn) != tamArquivo){
        readRegDadoCsv(fIn, &dados);    // lê o registro de dados do csv
        writeRegDadoBin(fOut, dados);   // escreve o registro de dados lido no binario
        cabecalho.nroRegArq++;          // incrementa o numero de registros no arquivo

        // desaloca o que for necessario
        free(dados.nomeJogador);
        free(dados.nacionalidade);
        free(dados.nomeClube);
    }

    // preenche cabecalho em memoria primaria
    cabecalho.status = '1';
    cabecalho.topo = -1;
    cabecalho.proxByteOffset = ftell(fOut); // atualiza prox byte offset
    cabecalho.nroRegRem = 0;

    
    fseek(fOut, 0, SEEK_SET);           // volta para o inicio
    writeRegCabBin(fOut, cabecalho);    // escreve o cabecalho no arquivo binario

    // fecha os arquivos
    fclose(fIn);
    fclose(fOut);

    // chama função fornecida binarioNaTela
    binarioNaTela(arquivoOut);
}
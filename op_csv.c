#include "op_csv.h"


// Função que lê um inteiro de um arquivo csv
int lerIntCsv(FILE * file){
    int i = 0, result;
    char aux;
    char * intStr = (char *) malloc(sizeof(char)*9);
    //ler int
    while(((aux = (char) getc(file)) != ',') && aux != '\n' && aux != '\r'){ // lê até a ,
        if(i < 9)
            intStr[i] = aux; // enquanto couber, guarda os caracteres na string
        else{
            intStr = (char *) realloc(intStr, i + 2); // se não couber, aloca mais espaço
            intStr[i] = aux;
        }
        i++;
    }

    if(i == 0){
        intStr[i++] = '-';
        intStr[i++] = '1';
    }   // caso não haja número, deve ser retornado -1

    intStr[i] = '\0';
    result = atoi(intStr);  // resultado guarda o inteiro referente a string intStr
    free(intStr);
    return result;
}


// Função que lê uma string de um arquivo csv, string é guardada em dest, retorno é o tamanho da string
int lerStrCsv(FILE * file, char ** dest){
    int i = 0;
    char aux;
    char * str = (char *) malloc(1);    // aloca espaço para 1 caracter
    if(str == NULL){
        fprintf(stderr, "Erro ao alocar memoria na funcao lerStrCsv()\n");
        exit(1);
    }
    while(((aux = (char) getc(file)) != ',') && aux != '\n'){   // lê até a , ou até o \n
        if(aux == '\r') // ignora os '\r's
            continue;
        str = (char *) realloc(str, i + 1);     // aloca de caractere em caractere
        str[i] = aux;
        i++;
    }
    *dest = str;
    return i;
}


// Função que lê um registro de dados de um arquivo csv
void readRegDadoCsv(FILE * fIn, REG_DADO * dados){
    //ler id
    dados -> id = lerIntCsv(fIn);
    //ler idade
    dados -> idade = lerIntCsv(fIn);
    //ler nome
    dados -> tamNomeJog = lerStrCsv(fIn, &(dados -> nomeJogador));
    //ler nacionalidade
    dados -> tamNacionalidade = lerStrCsv(fIn, &(dados -> nacionalidade));
    //ler clube
    dados -> tamNomeClube = lerStrCsv(fIn, &(dados -> nomeClube));
    
    dados -> removido = '0';
    //calcula tamanho do registro
    dados -> tamanhoRegistro = 33 + dados -> tamNomeJog + dados -> tamNacionalidade + dados -> tamNomeClube;

    dados -> prox = -1;
}
    


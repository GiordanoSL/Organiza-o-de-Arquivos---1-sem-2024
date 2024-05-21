#include "op_bin.h"


// Função que lê uma string de um arquivo binário
void lerStrBin(FILE * filebin, char ** str, int tam){
    *str = (char *) malloc(tam + 1);
    for (int i = 0; i < tam; i++)
    {
        fread(*str + i, sizeof(char), 1, filebin); // lê byte a byte
    }
    (*str)[tam] = '\0'; // coloca o \0 no fim da string
}


//Função que lê um registro de cabeçalho de um arquivo binário, preenchendo o regCab
void readRegCabBin(FILE * filebin, REG_CAB * regCab){
    fread(&(regCab -> status), sizeof(char), 1, filebin);   // lê o status
    fread(&(regCab -> topo), sizeof(long), 1, filebin);     // lê o topo
    fread(&(regCab -> proxByteOffset), sizeof(long), 1, filebin);   // lê o proxByteOffset
    fread(&(regCab -> nroRegArq), sizeof(int), 1, filebin);     // lê o nroRegArq
    fread(&(regCab -> nroRegRem), sizeof(int), 1, filebin);     // lê o nroRegRem
}

// Função que escreve um registro de cabeçalho num arquivo binário
void writeRegCabBin(FILE * fOut, REG_CAB cabecalho){
    fwrite(&(cabecalho.status), sizeof(char), 1, fOut);     // escreve o status
    fwrite(&cabecalho.topo, sizeof(long), 1, fOut);         // escreve o topo
    fwrite(&cabecalho.proxByteOffset, sizeof(long), 1, fOut);   // escreve o proxByteOffset
    fwrite(&cabecalho.nroRegArq, sizeof(int), 1, fOut);     // escreve o nroRegArq
    fwrite(&cabecalho.nroRegRem, sizeof(int), 1, fOut);     // escreve o nroRegRem
}

//Função que lê um registro de dados de um arquivo binário, preenchendo o regDado
void readRegDadoBin(FILE * filebin, REG_DADO * regDado){
    // Lê o char de removido
    fread(&(regDado -> removido), sizeof(char), 1, filebin);
    // Lê o tamanho do registro
    fread(&(regDado -> tamanhoRegistro), sizeof(int), 1, filebin);
    // Lê o prox
    fread(&(regDado -> prox), sizeof(long), 1, filebin);
    // Lê o id
    fread(&(regDado -> id), sizeof(int), 1, filebin);
    // Lê a idade
    fread(&(regDado -> idade), sizeof(int), 1, filebin);
    // Lê o tamanho do nome do jogador
    fread(&(regDado -> tamNomeJog), sizeof(int), 1, filebin);
    // Lê o nome do jogador
    lerStrBin(filebin, &(regDado -> nomeJogador), regDado -> tamNomeJog);
    // Lê o tamanho da nacionalidade
    fread(&(regDado -> tamNacionalidade), sizeof(int), 1, filebin);
    // Lê a nacionalidade
    lerStrBin(filebin, &(regDado -> nacionalidade), regDado -> tamNacionalidade);
    // Lê o tamanho do nome do clube
    fread(&(regDado -> tamNomeClube), sizeof(int), 1, filebin);
    // Lê o nome do clube
    lerStrBin(filebin, &(regDado -> nomeClube), regDado -> tamNomeClube);
}


// Função que escreve um registro de dados num arquivo binário
void writeRegDadoBin(FILE * file, REG_DADO dados){
    // escrever removido
    fwrite(&(dados.removido), sizeof(char), 1, file);
    // escrever tamanho registro
    fwrite(&(dados.tamanhoRegistro), sizeof(int), 1, file);
    // escrever prox
    fwrite(&(dados.prox), sizeof(long), 1, file);
    // escrever id
    fwrite(&(dados.id), sizeof(int), 1, file);
    // escrever idade
    fwrite(&(dados.idade), sizeof(int), 1, file);
    // escrever tamanho nome jogador
    fwrite(&(dados.tamNomeJog), sizeof(int), 1, file);
    // escrever nome jogador
    fwrite(dados.nomeJogador, sizeof(char), dados.tamNomeJog, file);
    // escrever tamanho nacionalidade
    fwrite(&(dados.tamNacionalidade), sizeof(int), 1, file);
    // escrever nacionalidade
    fwrite(dados.nacionalidade, sizeof(char), dados.tamNacionalidade, file);
    // escrever tamanho nome clube
    fwrite(&(dados.tamNomeClube), sizeof(int), 1, file);
    // escrever nome clube
    fwrite(dados.nomeClube, sizeof(char), dados.tamNomeClube, file);
}


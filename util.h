#ifndef UTILH
    #define UTILH

    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <string.h>

    //funções fornecidas
    void binarioNaTela(char *nomeArquivoBinario);
    void scan_quote_string(char *str);

    // lê string do arquivo de entrada
    char * lerStr();

    void preencheLixo(FILE * fDados, int espaco);

#endif
/*-----------------------------------------------------
Autores: Giordano Santorum Lorenzetto - nUSP 14574017
         Victor Moreli dos Santos - nUSP 14610514

Funções de select
-------------------------------------------------------*/


#ifndef SELECTH
    #define SELECTH

    #include "op_bin.h"
    #include "op_csv.h"
    #include "util.h"
    #include "reg.h"
    #include "create.h"
    #include "arvoreB.h"

    // le todos os registros de dados no arquivo <arquivoIn> e os imprime na saida padrão
    void select_from(char * arquivoIn);
    
    /* faz num_buscas buscas diferente por registros de dados no arquivo <arquivoIn>
     cada uma dessas buscas recebe um numero m que deterina quantos campos serão especi-
     ficados como requisitos da busca seguidos de m nomes de campos e seus valores
     Exemplo: 3 id 29382 nacionalide "RUSSIA"*/
    void select_from_where(char * arquivoIn, int num_buscas);

    /* faz num_buscas buscas diferente por ids de jogadores, cada busca é feita através 
    de uma Arvore-B */
    void select_from_arvoreB_id(char * arquivoBin, char * arquivoArvore, int num_buscas);

    /* faz num_buscas buscas diferente por registros de dados no arquivo <arquivoIn>
     cada uma dessas buscas recebe um numero m que deterina quantos campos serão especi-
     ficados como requisitos da busca seguidos de m nomes de campos e seus valores.
     Se o id for especificado, utiliza a estrutura de indexação arvoreB para fazer a busca
     Exemplo: 3 id 29382 nacionalide "RUSSIA"*/
    void select_from_arvoreB(char * arquivoBin, char * arquivoArvore, int num_buscas);

#endif
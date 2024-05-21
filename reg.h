#ifndef REGH
    #define REGH

    // Esse arquivo define as structs do registro de cabecalho e do registro de dados
    
    #define TAM_REG_CAB 25      // tamanho do registro de cabecalho
    
    typedef struct reg_cabecalho_ REG_CAB;
    typedef struct reg_dados_ REG_DADO;

    struct reg_cabecalho_{
        char status;            // 0 - inconsistente, 1 - consistente
        long topo;              // primeiro registro removido da lista
        long proxByteOffset;    // proxima posicao vazia para insercao
        int nroRegArq;          // nro de registros do arquivo
        int nroRegRem;          // nro de registros removidos do arquivo
    };

    struct reg_dados_{
        //Controle
        char removido;
        int tamanhoRegistro;
        long prox;

        //Campos de tamanho fixo
        int id;
        int idade;

        //Indicadores de tamanho
        int tamNomeJog;
        int tamNacionalidade;
        int tamNomeClube;

        //Campos de tamanho variável
        char * nomeJogador;
        char * nacionalidade;
        char * nomeClube;
    };

#endif
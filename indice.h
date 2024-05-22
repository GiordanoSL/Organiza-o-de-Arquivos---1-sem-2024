#ifndef INDICEH
    #define INDICEH

    #include "op_bin.h"
    #include "util.h"
    #include "reg.h"

    REG_DADO_ID ** carregamento(FILE * fId, int nroRegArq);
    void reescrita(FILE * fId, REG_DADO_ID ** vetorId, int nroRegArq);
    void insert_ordenado(REG_DADO_ID ** vetorId, REG_DADO_ID * regDadoId, int count);

#endif
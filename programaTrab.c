/*-----------------------------------------------------
Autores: Giordano Santorum Lorenzetto - nUSP 14574017
         Victor Moreli dos Santos - nUSP 14610514

Programa Principal
-------------------------------------------------------*/


#include "select.h"
#include "create.h"
#include "insert.h"
#include "delete.h"


int main(void){

    int comando;

    scanf(" %d", &comando);
    
    getchar();//descarta o '\n' da entrada

    // strings para nomes de arquivos
    char * arquivoIn;
    char * arquivoOut;
    char * arquivoIndice;
    char * arquivoDados;
    int num_buscas; // numero de buscas

    switch (comando){
    case 1: // caso 1 = create_table

        // lendo o nome dos arquivos
        arquivoIn = lerStr();// arquivo de entrada [nome].csv
        arquivoOut = lerStr();// arquivo de saida  [nome].bin

        //chamada da funcao
        create_table(arquivoIn, arquivoOut);

        //liberacao da memoria
        free(arquivoIn);
        free(arquivoOut);
        break;
    case 2: //caso 2 = select_from
        
        // lendo o nome dos arquivos
        arquivoIn = lerStr();// arquivo de entrada [nome].bin

        //chamada da funcao
        select_from(arquivoIn);

        //liberacao da memoria
        free(arquivoIn);
        break;
    case 3: //caso 3 = select_from_where 

        // lendo o nome dos arquivos
        arquivoIn = lerStr();// arquivo de entrada [nome].bin

        //lendo o numero de buscas
        scanf(" %d", &num_buscas);
        getchar(); // descarta o '\n' da entrada

        //chamada da funcao
        select_from_where(arquivoIn, num_buscas);
        
        //liberacao da memoria
        free(arquivoIn);
        break;
    case 4: //caso 4 = create index 

        // lendo o nome dos arquivos
        arquivoIn = lerStr();// arquivo de entrada [nome].csv
        arquivoOut = lerStr();// arquivo de saida  [nome].bin

        //chamada da funcao
        if(create_index(arquivoIn, arquivoOut))
            binarioNaTela(arquivoOut);  // chama função fornecida binarioNaTela

        //liberacao da memoria
        free(arquivoIn);
        free(arquivoOut);
        break;
    case 5: //caso 5 = delete_from_where 

        int numRem; // numero de remoções que serão feitas

        // lendo o nome dos arquivos
        arquivoDados = lerStr();// arquivo de dados [nome].bin
        arquivoIndice = lerStr();// arquivo de indice [nome].bin

        //lendo o numero de remoções
        scanf(" %d", &numRem);
        getchar(); // descarta o '\n' da entrada

        //chamada da funcao
        delete_from_where(arquivoDados, arquivoIndice, numRem);
        
        //liberacao da memoria
        free(arquivoDados);
        free(arquivoIndice);
        break;
    case 6: //caso 6 = insert_into 

        int numInsert; // numero de insercoes que serão feitas

        // lendo o nome dos arquivos
        arquivoDados = lerStr();// arquivo de dados [nome].bin
        arquivoIndice = lerStr();// arquivo de indice [nome].bin

        //lendo o numero de insercoes
        scanf(" %d", &numInsert);
        getchar(); // descarta o '\n' da entrada

        //chamada da funcao
        insert_into(arquivoDados, arquivoIndice, numInsert);
        
        //liberacao da memoria
        free(arquivoDados);
        free(arquivoIndice);
        break;
    case 7: //caso 7 = create index arvoreB

        // lendo o nome dos arquivos
        arquivoIn = lerStr();// arquivo de entrada [nome].csv
        arquivoOut = lerStr();// arquivo de saida  [nome].bin

        //chamada da funcao
        if(create_arvoreB(arquivoIn, arquivoOut))
            binarioNaTela(arquivoOut);  // chama função fornecida binarioNaTela

        //liberacao da memoria
        free(arquivoIn);
        free(arquivoOut);
        break;
    case 8: //caso 8 = select from arvoreB

        // lendo o nome dos arquivos
        arquivoDados = lerStr();// arquivo de entrada [nome].csv
        arquivoIndice = lerStr();// arquivo de saida  [nome].bin
        scanf(" %d", &num_buscas);

        //chamada da funcao
        select_from_arvoreB_id(arquivoDados, arquivoIndice, num_buscas);

        //liberacao da memoria
        free(arquivoIn);
        free(arquivoOut);
        break;
    case 9: //caso 8 = select from arvoreB

        // lendo o nome dos arquivos
        arquivoDados = lerStr();// arquivo de entrada [nome].csv
        arquivoIndice = lerStr();// arquivo de saida  [nome].bin
        scanf(" %d", &num_buscas);

        //chamada da funcao
        select_from_arvoreB(arquivoDados, arquivoIndice, num_buscas);

        //liberacao da memoria
        free(arquivoIn);
        free(arquivoOut);
        break;
    case 10: //caso 8 = select from arvoreB

        // lendo o nome dos arquivos
        arquivoDados = lerStr();// arquivo de entrada [nome].csv
        arquivoIndice = lerStr();// arquivo de saida  [nome].bin
        scanf(" %d", &num_buscas);

        //chamada da funcao
        insert_into_arvB(arquivoDados, arquivoIndice, num_buscas);

        //liberacao da memoria
        free(arquivoIn);
        free(arquivoOut);
        break;
    }

    
    return 0;
}
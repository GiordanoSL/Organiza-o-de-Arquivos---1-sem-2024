#include "select.h"
#include "create_table.h"

// int main(void){
//     //createTable("dado2.csv", "naotaprontoainda.bin");

//     select_from("binario3.bin");
    
//     return 0;
// }

int main(void){

    int comando;

    scanf(" %d", &comando);
    getchar();//descarta o '\n' da entrada

    // strings para nomes de arquivos
    char * arquivoIn;
    char * arquivoOut;

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

        int numBuscas; // numerode buscas que serão feitas

        // lendo o nome dos arquivos
        arquivoIn = lerStr();// arquivo de entrada [nome].bin

        //lendo o numero de buscas
        scanf(" %d", &numBuscas);
        getchar(); // descarta o '\n' da entrada

        //chamada da funcao
        select_from_where(arquivoIn, numBuscas);
        
        //liberacao da memoria
        free(arquivoIn);
        break;
    default:
        break;
    }

    
    return 0;
}
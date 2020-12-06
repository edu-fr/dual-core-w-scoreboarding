#include "helper.h"
#include "conversor.c"
#include "config.c"
#include "processador.c"


// Inicializa os valores da memoria
void init(int memoria[], int tamanho_memoria){ 
    for(int i  = 0; i<tamanho_memoria; i++){
        memoria[i] = -1;
    }
}

// Mostra como utilizar o programa implementado
void mostrarAjuda(char *name) { // ok
    fprintf(stderr, "\
            [uso] %s <opcoes>\n\
            -h              		mostra ajuda.\n\
            -c CONFIG       		nome do arquivo de configuracao.\n\
            -o SAIDA        		nome do arquivo de saida.\n\
            -p PROGRAMA 1    		nome do 1o programa a ser executado .\n\
            -r PROGRAMA 2    		nome do 2o programa a ser executado .\n\
            -m TAMANHO DA MEMORIA 1	inteiro correspondente ao numero de instrucoes no arquivo de programa 1. \n\
            -n TAMANHO DA MEMORIA 2	inteiro correspondente ao numero de instrucoes no arquivo de programa 2. \n", name) ;
    exit(-1) ;
}

int main(int argc, char **argv){ 
    int opt, tamanho_memoria[2];
    char *config, *saida, *programa_1, *programa_2, *str_tamanho_memoria_1, *str_tamanho_memoria_2;

    if ( argc < 2 ) mostrarAjuda(argv[0]);

    while( (opt = getopt(argc, argv, "hc:o:p:r:m:n")) > 0 ) {
        switch ( opt ) {
            case 'h': /* help */
                mostrarAjuda(argv[0]);
                break;
            case 'c': /* opção -c */
                config = optarg;
                break;
            case 'o': /* opção -o */
                saida = optarg;
                break;
            case 'p': /* opção -p */
                programa_1 = optarg;
                break;

            case 'r': /* opção -r */
                programa_2 = optarg;
                break;
            
            case 'm': //opcao -m
                str_tamanho_memoria_1 = optarg;
                break;

            case 'n': //opcao -n
                str_tamanho_memoria_2 = optarg;
                break;
            
            default:
                fprintf(stderr, "Opcao invalida ou faltando argumento: `%c'\n", optopt) ;
                return -1;
        }
    }

    tamanho_memoria[0] = atoi(str_tamanho_memoria_1);
    
    tamanho_memoria[1] = atoi(str_tamanho_memoria_2);

    int memoria_1[tamanho_memoria[0]];
    
    int memoria_2[tamanho_memoria[1]];
    
    init(memoria_1, tamanho_memoria[0]);

    init(memoria_2, tamanho_memoria[1]);

    char linhas_instrucoes_1[tamanho_memoria[0]][64];

    char linhas_instrucoes_2[tamanho_memoria[1]][64];

    conversor(programa_1, memoria_1, linhas_instrucoes_1, tamanho_memoria[0]);

    conversor(programa_2, memoria_2, linhas_instrucoes_2, tamanho_memoria[1]);

    lerArquivoConfiguracao(config);

    processador(memoria_1, memoria_2, tamanho_memoria[0], tamanho_memoria[1], linhas_instrucoes_1,   linhas_instrucoes_2, saida)

    scoreboarding(memoria_1, tamanho_memoria[0], saida, linhas_instrucoes_1);
    
    
}
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
            -p PROGRAMA     		nome do programa a ser executado .\n\
            -m TAMANHO DA MEMORIA 	inteiro correspondente ao numero de instrucoes no arquivo de programa. \n", name) ;
    exit(-1) ;
}


int main(int argc, char **argv){ 
    int opt, tamanho_memoria;
    char *config, *saida, *programa, *str_tamanho_memoria;

    if ( argc < 2 ) mostrarAjuda(argv[0]);

    while( (opt = getopt(argc, argv, "hc:o:p:m:")) > 0 ) {
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
                programa = optarg;
                break;
            case 'm': //opcao -m
                str_tamanho_memoria = optarg;
                break;
            default:
                fprintf(stderr, "Opcao invalida ou faltando argumento: `%c'\n", optopt) ;
                return -1;
        }
    }

    tamanho_memoria = atoi(str_tamanho_memoria);
    
    int memoria[tamanho_memoria];
    
    init(memoria, tamanho_memoria);

    char linhas_instrucoes[tamanho_memoria][64];

    conversor(programa, memoria, linhas_instrucoes, tamanho_memoria);

    lerArquivoConfiguracao(config);

    scoreboarding(memoria, tamanho_memoria, saida, linhas_instrucoes);
    
}



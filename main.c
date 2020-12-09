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

void tInfo_init(tInfo* t_info, int thread_num, int *memoria, int tamanho_memoria, char* nome_arq_saida, char **linhas_instrucoes){
    t_info->thread_num = thread_num;
    t_info->memoria = memoria;
    t_info->tamanho_memoria = tamanho_memoria;
    t_info->nome_arq_saida = nome_arq_saida;

    t_info->linhas_instrucoes = malloc(sizeof(char*) * tamanho_memoria);
    
    for(int i = 0; i < tamanho_memoria; i++){
        t_info->linhas_instrucoes[i] = malloc(64);
    }

    printf("\nPRINT INIT\n");
    for(int i = 0; i < tamanho_memoria; i++){
        strcpy(t_info->linhas_instrucoes[i], linhas_instrucoes[i]);
        printf("linhas_instrucoes[%d]: %s\n", i, linhas_instrucoes[i]);
    }
    printf("\nFIM DO PRINT INIT\n");
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
    int s, opt, tamanho_memoria_1, tamanho_memoria_2;
    char *config, *saida[2], *programa[2], *str_tamanho_memoria[2];
    void *res;
    pthread_attr_t attr;
    if ( argc < 2 ) mostrarAjuda(argv[0]);

    while( (opt = getopt(argc, argv, "hc:o:q:p:r:m:n:")) > 0 ) {
        switch ( opt ) {
            case 'h': /* help */
                mostrarAjuda(argv[0]);
                break;
            case 'c': /* opção -c */
                config = optarg;
                break;
            case 'o': /* opção -o */
                saida[0] = optarg;
                break;
            case 'q': /* opção -q */
                saida[1] = optarg;
                break;
            case 'p': /* opção -p */
                programa[0] = optarg;
                break;
            case 'r': /* opção -r */
                programa[1] = optarg;
            break;
            case 'm': //opcao -m
                str_tamanho_memoria[0] = optarg;
                break;
            case 'n': //opcao -m
                str_tamanho_memoria[1] = optarg;
            break;
            default:
                fprintf(stderr, "Opcao invalida ou faltando argumento: `%c'\n", opt) ;
                return -1;
        }
    }
    printf("to na main \n");
    s = pthread_attr_init(&attr);
    

    tamanho_memoria_1 = atoi(str_tamanho_memoria[0]);
    tamanho_memoria_2 = atoi(str_tamanho_memoria[1]);

    int memoria_1[tamanho_memoria_1];
    
    int memoria_2[tamanho_memoria_2];

    init(memoria_1, tamanho_memoria_1);

    init(memoria_2, tamanho_memoria_2);

    char **linhas_instrucoes_1, **linhas_instrucoes_2;
    
    linhas_instrucoes_1 = malloc(sizeof(char*) * tamanho_memoria_1);
    
    linhas_instrucoes_2 = malloc(sizeof(char*) * tamanho_memoria_2);
    
    for(int i = 0; i < tamanho_memoria_1; i++){
        linhas_instrucoes_1[i] = malloc(64);
       
    }

    for(int i = 0; i < tamanho_memoria_2; i++){
        
        linhas_instrucoes_2[i] = malloc(64);
    }


    printf("entrando na conversao \n");
    conversor(programa[0], memoria_1, linhas_instrucoes_1, tamanho_memoria_1);

    printf("Programa 2 : %s \n",programa[1]);
    
    conversor(programa[1], memoria_2, linhas_instrucoes_2, tamanho_memoria_2);

    printf("saindo  na conversao \n");
    lerArquivoConfiguracao(config);
    
    /*
    printf("Tamanho memoria: 1: %d, 2: %d\n", tamanho_memoria_1, tamanho_memoria_2);
    printf("Programa 1: %s, 2: %s\n", programa[0], programa[1]);  
    printf("Config: %s", config);
    */
    
    tInfo* t_info = malloc(sizeof(tInfo) * 2);
    
    tInfo_init(&t_info[FLUXO_1], FLUXO_1, memoria_1, tamanho_memoria_1, saida[FLUXO_1], linhas_instrucoes_1);
    tInfo_init(&t_info[FLUXO_2], FLUXO_2, memoria_2, tamanho_memoria_2, saida[FLUXO_2], linhas_instrucoes_2);
    
    printf("Nome saida: %s\n\n",t_info[FLUXO_1].nome_arq_saida);
    printf("Nome saida: %s\n\n",t_info[FLUXO_2].nome_arq_saida);
    
    for(int i = 0; i < 2; i++) {
        s = pthread_create(&t_info[i].thread_id, &attr,
		       &scoreboarding, &t_info[i]);
        if(s != 0){
            printf("Erro ao criar a thread %d!\n", i);
        }

    } 

    s = pthread_attr_destroy(&attr);
    if (s != 0)
        printf("Erro pthread destroy");

    for(int i = 0; i < 2; i++) {
        s = pthread_join(t_info[i].thread_id, &res);
        if(s != 0){
            printf("Erro ao esperar a thread %d terminar!\n", i);
        }
        free(res);
    }   

    free(t_info);
}
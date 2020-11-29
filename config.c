#include "config.h"

extern int num_ciclos_ADD, num_ciclos_ADDI, num_ciclos_AND,
    num_ciclos_ANDI, num_ciclos_OR, num_ciclos_ORI, num_ciclos_SLT,
    num_ciclos_SUB, num_ciclos_MULT, num_ciclos_DIV, num_ciclos_LI, 
    num_ciclos_MOVE;


// Abre o arq de config, salva as variaveis e fecha o arq
void lerArquivoConfiguracao(char *nome_arq){
    //Abrir o arquivo
    FILE *arquivo_configuracao;
    arquivo_configuracao = fopen(nome_arq, "r");

    lerConfiguracoes(arquivo_configuracao);
    fclose(arquivo_configuracao);
}

//programa que le o arquivo de configuração
void lerConfiguracoes(FILE* arq_config){
    // Variavel auxiliar que guarda a string da linha
    char buffer[64];

    // loop pra ler cada linha do arquivo
    while(fgets(buffer, 64 , arq_config) != NULL){
        lerNumCiclos(buffer);
    }
}

// Salva o numero de ciclos de acordo com o opcode
void registrarCiclo(char *opcode, int ciclos){
    if(strcmp("add ", opcode) == 0)
        num_ciclos_ADD = ciclos;
    else if(strcmp("addi ", opcode) == 0)
        num_ciclos_ADDI = ciclos;
    else if(strcmp("and ", opcode) == 0)
        num_ciclos_AND = ciclos;
    else if(strcmp("andi ", opcode) == 0)
        num_ciclos_ANDI = ciclos;
    else if(strcmp("or ", opcode) == 0)
        num_ciclos_OR = ciclos;
    else if(strcmp("ori ", opcode) == 0)
        num_ciclos_ORI = ciclos;
    else if(strcmp("slt ", opcode) == 0)
        num_ciclos_SLT = ciclos;
    else if(strcmp("sub ", opcode) == 0)
        num_ciclos_SUB = ciclos;
    else if(strcmp("mult ", opcode) == 0)
        num_ciclos_MULT = ciclos;
    else if(strcmp("div ", opcode) == 0)
        num_ciclos_DIV = ciclos;
    else if(strcmp("li ", opcode) == 0)
        num_ciclos_LI = ciclos;
    else if(strcmp("mov ", opcode) == 0)
        num_ciclos_MOVE = ciclos;
    else {
        printf("Opcode inválido: %s \n", opcode);
        exit(1);
    }
}

// Le o numero de ciclos expecificado no arquivo, e armazena nas variaveis de opcode
void lerNumCiclos(char linha[64]){
    char *token_config, *opcode = NULL, *ciclos = NULL;
    token_config = strtok(linha, "=");
    opcode = token_config;
    token_config = strtok(NULL, "=");
    ciclos = token_config;
    registrarCiclo(opcode, atoi(ciclos));
}




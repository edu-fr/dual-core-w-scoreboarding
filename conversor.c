/* PROGRAMA CONVERSOR DO ARQUIVO */
#define COD_REG "$" 
#include "conversor.h"

bool flag_opcode_imediate = false;
bool flag_opcode_move = false;
bool flag_opcode_li = false;

void conversor(char *nome_arq, int *memoria, char linhas_instrucoes[][64], int tamanho_memoria){
    //Abrir o arquivo
    FILE *arquivo_instrucao;
    arquivo_instrucao = fopen(nome_arq, "r");
    if(arquivo_instrucao == NULL){
        printf("Falha ao abrir arquivo de instrucoes\n");
        exit(1);
	}
    lerArquivo(arquivo_instrucao, memoria, linhas_instrucoes, tamanho_memoria);
    fclose(arquivo_instrucao);
}


void lerArquivo(FILE *arquivo_instrucao, int* memoria, char linhas_instrucoes[][64], int tamanho_memoria){
    // Variavel auxiliar que guarda a string da linha
    char buffer_instrucao[64];
    int numero_instrucao = 0;
    // loop pra ler cada linha do arquivo
    while(fgets(buffer_instrucao,64 , arquivo_instrucao) != NULL){
        buffer_instrucao[strcspn(buffer_instrucao, "\r\n")] = 0; //retira o '\n' do final da string
        strcpy(linhas_instrucoes[numero_instrucao], buffer_instrucao);
        numero_instrucao++;
        lerInstrucao(buffer_instrucao, memoria, tamanho_memoria);
    }
}

int registradorParaBinario(char token_instrucao[3]){
    char buffer[2];
    for(int i = 0; i < 2; i++){
        token_instrucao[i] = token_instrucao[i+1];
        buffer[i] = token_instrucao[i];
    }
    return identificaRegistrador(buffer);
}

void lerInstrucao(char *instrucao, int *memoria, int tamanho_memoria){
    unsigned int bin_instrucao;               // Instrucao completa.
    int i = 0;                              
    unsigned int bin_opcode;                // valor correspondente ao OPCODE da instrucao
    unsigned int bin_registradores[3];     // valores correspondentes aos 3 registrados da instrucao
    unsigned int bin_imediate;            // valor correspondente ao valor imediato de uma instrucao
    char *token_instrucao; 
    char aux_registradores[3][3];       // Pra guardar cada registrador lido com strtok                         
    // Dividimos a linha inteira em tokens, que ficam "armazenadas" na token_instrucao
    
    token_instrucao = strtok(instrucao, " ");       // Divide pelo espaco
    bin_opcode = opcodeParaBinario(token_instrucao);
    
    for(int k = 0; k < 3; k++){
        if(flag_opcode_move && k==2){ 
            break;
        }
        if(flag_opcode_li && k == 2){
            break;
        }
        token_instrucao = strtok(NULL, " ");
        strcpy(aux_registradores[k], token_instrucao); 
    }

    if(flag_opcode_li){ // opcode LI
        bin_registradores[0] = registradorParaBinario(aux_registradores[0]);
        bin_registradores[1] = 0;
        bin_registradores[2] = 0;
        bin_imediate = atoi(aux_registradores[1]);
        bin_instrucao = (bin_opcode << 26) + (bin_registradores[0] << 21) + (bin_registradores[1] << 16) + bin_imediate;
    }else if(flag_opcode_imediate){ //opcode de instrucao que usa imediato
        bin_registradores[0] = registradorParaBinario(aux_registradores[0]);
        bin_registradores[1] = registradorParaBinario(aux_registradores[1]);
        bin_registradores[2] = -1;
        bin_imediate = atoi(aux_registradores[2]);
        bin_instrucao = (bin_opcode << 26) + (bin_registradores[0] << 21) + (bin_registradores[1] << 16) + bin_imediate;
    }else if(flag_opcode_move){ // opcode move
        bin_registradores[0] = registradorParaBinario(aux_registradores[0]);
        bin_registradores[1] = registradorParaBinario(aux_registradores[1]);
        bin_registradores[2] = 0;
        bin_instrucao = (bin_opcode << 26) + (bin_registradores[0] << 21) + (bin_registradores[1] << 16) + (bin_registradores[2] << 11) + (0 << 0);
    }else{ //opcode comum
        bin_registradores[0] = registradorParaBinario(aux_registradores[0]);
        bin_registradores[1] = registradorParaBinario(aux_registradores[1]);
        bin_registradores[2] = registradorParaBinario(aux_registradores[2]);
        bin_instrucao = (bin_opcode << 26) + (bin_registradores[0] << 21) + (bin_registradores[1] << 16) + (bin_registradores[2] << 11) + (0 << 0); 
    }
    salvarNaMemoriaDeInstrucoes(bin_instrucao, memoria, tamanho_memoria);
    flag_opcode_imediate = false;
    flag_opcode_move = false;
    flag_opcode_li = false;
}

void salvarNaMemoriaDeInstrucoes(int instrucao, int *memoria, int tamanho_memoria){
    //funcao que guarda a instrucao binaria na memoria
    //primeiro encontramos a ultima posicao da "memoria" de instrucoes
    int i = 0;
    for(i; i < tamanho_memoria; i++){
        if(memoria[i] == -1){
            break;
        }
    }
    memoria[i] = instrucao;
}

int8_t opcodeParaBinario(char *opcode){
    // Verificacao para saber qual o valor do opcode
    if(strcmp(opcode, "add") == 0){
        return ADD;
    }
    else if(strcmp(opcode, "addi") == 0){
        flag_opcode_imediate = true;
        return ADDI;
    }
    else if(strcmp(opcode, "and") == 0){
        return AND;
    }
    else if(strcmp(opcode, "andi") == 0){
        flag_opcode_imediate = true;
        return ANDI;
    }
    else if(strcmp(opcode, "or") == 0){
        return OR;
    }
    else if(strcmp(opcode, "ori") == 0){
        flag_opcode_imediate = true;
        return ORI;
    }
    else if(strcmp(opcode, "slt") == 0){
        return SLT;
    }
    else if(strcmp(opcode, "sub") == 0){
        return SUB;
    }
    else if(strcmp(opcode, "mult") == 0){
        return MULT;
    }
    else if(strcmp(opcode, "div") == 0){
        return DIV;
    }
    else if(strcmp(opcode, "li") == 0){ 
        flag_opcode_li = true;
        return LI;
    }
    else if(strcmp(opcode, "move") == 0){
        flag_opcode_move = true;
        return MOVE;
    }
    printf("Funcao nao suportada! %s\n", opcode);
    exit(1);
    return 0;
}



int identificaRegistrador(char *buffer){
    if(!strcmp(buffer, "zero")){
        return zero;
    }else if(!strcmp(buffer, "at")){
        return at;
    }else if(!strcmp(buffer, "v0")){
        return v0;
    }else if(!strcmp(buffer, "v1")){
        return v1;
    }else if(!strcmp(buffer, "a0")){
        return a0;
    }else if(!strcmp(buffer, "a1")){
        return a1;
    }else if(!strcmp(buffer, "a2")){
        return a2;
    }else if(!strcmp(buffer, "a3")){
        return a3;
    }else if(!strcmp(buffer, "t0")){
        return t0;
    }else if(!strcmp(buffer, "t1")){
        return t1;
    }else if(!strcmp(buffer, "t2")){
        return t2;
    }else if(!strcmp(buffer, "t3")){
        return t3;
    }else if(!strcmp(buffer, "t4")){
        return t4;
    }else if(!strcmp(buffer, "t5")){
        return t5;
    }else if(!strcmp(buffer, "t6")){
        return t6;
    }else if(!strcmp(buffer, "t7")){
        return t7;
    }else if(!strcmp(buffer, "s0")){
        return s0;
    }else if(!strcmp(buffer, "s1")){
        return s1;
    }else if(!strcmp(buffer, "s2")){
        return s2;
    }else if(!strcmp(buffer, "s3")){
        return s3;
    }else if(!strcmp(buffer, "s4")){
        return s4;
    }else if(!strcmp(buffer, "s5")){
        return s5;
    }else if(!strcmp(buffer, "s6")){
        return s6;
    }else if(!strcmp(buffer, "s7")){
        return s7;
    }else if(!strcmp(buffer, "t8")){
        return t8;
    }else if(!strcmp(buffer, "t9")){
        return t9;
    }else if(!strcmp(buffer, "k0")){
        return k0;
    }else if(!strcmp(buffer, "k1")){
        return k1;
    }else if(!strcmp(buffer, "gp")){
        return gp;
    }else if(!strcmp(buffer, "sp")){
        return sp;
    }else if(!strcmp(buffer, "fp")){
        return fp;
    }else if(!strcmp(buffer, "ra")){
        return ra;
    }else{
        printf("Registrador invalido!\n");
        exit(1);
    }
}

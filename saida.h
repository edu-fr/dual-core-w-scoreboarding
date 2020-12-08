

#ifndef SAIDA_H_INCLUDED
#define SAIDA_H_INCLUDED  

//ARQUIVO SAIDA.C
char* converteNomeRegistrador(int cod_reg);

char* converteNomeUF(int codigo_UF);

void converteStatusRegistradores(char nomes_UF[32][16]);

void escrever_saida(FILE* arq, int PC, char* instrucoes);

void saida(FILE* arq_saida, int PC, char** instrucoes, int* memoria, int tam_mem);

void converteInstrucao(char **instrucoes, int PC, int *memoria, int tam_mem);

char* converter(int instrucao);

int recuperaCampo(int instruct, int size, int starting_bit);

char* opcodeParaString(int opcode);

#endif
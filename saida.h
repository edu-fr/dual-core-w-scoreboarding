

#ifndef SAIDA_H_INCLUDED
#define SAIDA_H_INCLUDED  

//ARQUIVO SAIDA.C
char* converteNomeRegistrador(int cod_reg);

char* converteNomeUF(int codigo_UF);

void converteStatusRegistradores(char nomes_UF[32][16]);

void escrever_saida(FILE* arq, int PC, char** instrucoes);

void saida(FILE* arq_saida, int PC, char** lista_instrucoes, int instrucao);

void converteInstrucao(char **instrucoes, int PC, int instrucao);

void converter(int instrucao, char *string);

int recuperaCampo(int instruct, int size, int starting_bit);

char* opcodeParaString(int opcode);

#endif
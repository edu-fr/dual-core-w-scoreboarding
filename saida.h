

#ifndef SAIDA_H_INCLUDED
#define SAIDA_H_INCLUDED  

//ARQUIVO SAIDA.C
char* converteNomeRegistrador(int cod_reg);

char* converteNomeUF(int codigo_UF);

void converteStatusRegistradores(char nomes_UF[32][16]);

void escrever_saida(FILE* arq, int PC[2], char linhas_instrucoes[][][64]);

void saida(FILE *arq_saida, int PC, char linhas_instrucoes[][64]);

void escrever_status_instrucoes(FILE* arq, int PC, char linhas_instrucoes[][64]);

void escrever_status_UF(FILE* arq, int PC);

void escrever_status_registradores(FILE* arq, int PC);

#endif
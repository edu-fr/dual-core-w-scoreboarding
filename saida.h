

#ifndef SAIDA_H_INCLUDED
#define SAIDA_H_INCLUDED  

//ARQUIVO SAIDA.C
char* converteNomeRegistrador(int cod_reg);

char* converteNomeUF(int codigo_UF);

void converteStatusRegistradores(char nomes_UF[32][16]);

void escrever_saida(FILE* arq, int PC, char linhas_instrucoes[][64]);

void saida(FILE *arq_saida, int PC, char linhas_instrucoes[][64]);

#endif
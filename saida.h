

#ifndef SAIDA_H_INCLUDED
#define SAIDA_H_INCLUDED  

//ARQUIVO SAIDA.C
void saida(FILE* arq_saida, int PC[2], char linhas_instrucoes_1[][64], char linhas_instrucoes_2[][64]);

void escrever_status_instrucoes(FILE* arq, int PC, char linhas_instrucoes[][64]);

void escrever_status_UF(FILE* arq, int PC);

void escrever_status_registradores(FILE* arq, int PC, char nomes_UF[32][16]);

void escrever_saida(FILE* arq, int PC[2], char linhas_instrucoes_1[][64], char linhas_instrucoes_2[][64]);

char* converteNomeUF(int codigo_UF);

char* converteNomeRegistrador(int cod_reg);

void converteStatusRegistradores(char nomes_UF[32][16]);

#endif
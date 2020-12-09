

#ifndef SAIDA_H_INCLUDED
#define SAIDA_H_INCLUDED  

//ARQUIVO SAIDA.C

char* converteNomeRegistrador(int cod_reg);

char* converteNomeUF(int codigo_UF);

void converteStatusRegistradores(char nomes_UF[32][16], enum UF status_dos_registradores[32]);

void escrever_saida(FILE* arq, int PC, char linhas_instrucoes[][64], int clock_processador,
statusInst status_instrucoes, UnidadeFuncional vetor_UF[5], enum UF status_dos_registradores[32],
int banco_registradores[32], bool flag_registradores[32]);


void saida(FILE* arq_saida, int PC, char linhas_instrucoes[][64], int clock_processador,
statusInst status_instrucoes, UnidadeFuncional vetor_UF[5], enum UF status_dos_registradores[32], 
int banco_registradores[32], bool flag_registradores[32]);
#endif
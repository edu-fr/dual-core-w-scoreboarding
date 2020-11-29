
#ifndef SCOREBOARDING_CONVERSOR_H
#define SCOREBOARDING_CONVERSOR_H

extern bool flag_opcode_imediate;

// ARQUIVO CONVERSOR.C
void lerArquivo(FILE *arquivo_instrucao, int *memoria, char linhas_instrucoes[][64], int tamanho_memoria);

int registradorParaBinario(char *token_instrucao);

void lerInstrucao(char *instrucao, int *memoria, int tamanho_memoria);

int8_t opcodeParaBinario(char *opcode);

void salvarNaMemoriaDeInstrucoes(int instrucao, int *memoria, int tamanho_memoria);

void conversor(char *nome_arq, int *memoria, char linhas_intrucoes[][64], int tamanho_memoria);

int identificaRegistrador(char *buffer);
#endif //SCOREBOARDING_CONVERSOR_H

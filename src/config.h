
#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

// ARQUIVO CONFIG.C
void registrarCiclo(char *opcode, int ciclos);

void lerNumCiclos(char linha[64]);

void lerConfiguracoes(FILE* arq_config);

void lerArquivoConfiguracao(char *nome_arq);

#endif


#ifndef PROCESSADOR_H_INCLUDED
#define PROCESSADOR_H_INCLUDED


// ARQUIVO PROCESSADOR.C

void limpaBusca(instrucaoBuscada *instrucao_buscada);

void limpaDecodificacao(instrucaoDecodificada *instrucao_decodificada);

int busca(int *memoria,int PC);

void decodificacao(instrucaoBuscada instrucao_buscada, instrucaoDecodificada *instrucao_decodificada);

bool verificaWAW(int destino_nova, UnidadeFuncional vetor_UF[5]);

bool emissao(instrucaoDecodificada instrucao_decodificada, 
int clock_processador, statusInst *status_instrucoes, UnidadeFuncional vetor_UF[5],
enum UF status_dos_registradores[32], emissoes *lista_emissoes);

void leituraOperandos(listaExecucao instrucoes_prontas[5], int PC, int *memoria, 
int clock_processador, statusInst *status_instrucoes, UnidadeFuncional vetor_UF[5],
emissoes *lista_emissoes, int clock_instrucoes_lidas[5], int banco_registradores[32]);

void execucao(listaExecucao instrucoes_prontas[5], 
instrucaoExecutando lista_instrucoes_executando[5], resultadoExec lista_resultados[5], 
int clock_processador, statusInst *status_instrucoes, int banco_registradores[32]);

void executaUF(listaExecucao *instrucao_pronta, instrucaoExecutando *instrucao_executando, 
resultadoExec *resultado, int clock_processador, 
statusInst *status_instrucoes, int banco_registradores[32]);


bool liberaLeitura(int i, UnidadeFuncional vetor_UF[5]);

bool verificaWAR(int reg_destino, int indice_UF_atual,
 int clock_processador, UnidadeFuncional vetor_UF[5], 
 emissoes lista_emissoes, int clock_instrucoes_lidas[5]);


void escritaResultados(resultadoExec lista_resultados[5],
 int instrucoes_escritas[5], int clock_processador, 
 statusInst *status_instrucoes, int banco_registradores[32],
 UnidadeFuncional vetor_UF[5],  bool flag_registradores[32],
  emissoes lista_emissoes, int clock_instrucoes_lidas[5]);

void limpaEstruturas(int instrucoes_escritas[5], resultadoExec lista_resultados[5], 
listaExecucao instrucoes_prontas[5], instrucaoExecutando lista_instrucoes_executando[5],
UnidadeFuncional vetor_UF[5], enum UF status_dos_registradores[32], int clock_instrucoes_lidas[5]);

void atualizaDependencias(UnidadeFuncional vetor_UF[5], emissoes lista_emissoes);

int verificaRegistradorOcupado(int8_t registrador, int indice_UF, 
UnidadeFuncional vetor_UF[5], emissoes lista_emissoes);

void preencheStatusRegistradorRk(int indice, int8_t registrador, UnidadeFuncional vetor_UF[5], emissoes lista_emissoes);

void preencheStatusRegistradorRj(int indice, int8_t registrador, UnidadeFuncional vetor_UF[5], emissoes lista_emissoes);

void limpaAuxiliares(int indice, resultadoExec lista_resultados[5], 
listaExecucao instrucoes_prontas[5],
instrucaoExecutando lista_instrucoes_executando[5],
int clock_instrucoes_lidas[5]);

void limpaUF(int indice_UF, UnidadeFuncional vetor_UF[5]);

int executaInstrucao(listaExecucao instrucao, int banco_registradores[32]);

void preencheStatusInstrucoes(int etapa, int PC, 
int clock_processador, statusInst *status_instrucoes);

int recuperaCampo(int instruct, int size, int starting_bit);

int opcodeParaUF(int opcode);

char *opcodeParaOperacao(int opcode);

bool verificaImediato(int opcode);

bool verificaMove(int opcode);

bool verificaLi(int opcode);

void preencheStatusUF(int indice_UF, int Fi, int Fj, int Fk, int opcode, 
UnidadeFuncional vetor_UF[5], emissoes lista_emissoes);

void preencheOcupacaoStatusUF(int indice_UF, UnidadeFuncional vetor_UF[5], emissoes lista_emissoes);

bool verificaTermino(int PC, int tamanho_memoria, instrucaoDecodificada instrucao_decodificada, UnidadeFuncional vetor_UF[5]);

int opcodeParaNumCiclos(int opcode);


void initScoreboarding(int *memoria, int tamanho_memoria, listaExecucao instrucoes_prontas[5], 
bool *acabou_de_executar, instrucaoExecutando lista_instrucoes_executando[5], 
resultadoExec lista_resultados[5], int instrucoes_escritas[5], int *clock_processador,
UnidadeFuncional vetor_UF[5], enum UF status_dos_registradores[32],bool flag_registradores[32],
emissoes *lista_emissoes, int clock_instrucoes_lidas[5], int banco_registradores[32],statusInst *status_instrucoes);

#endif


#ifndef PROCESSADOR_H_INCLUDED
#define PROCESSADOR_H_INCLUDED


// ARQUIVO PROCESSADOR.C

void processador(int memoria[2], int tamanho_memoria[2], char linhas_instrucoes_1[][64], char linhas_instrucoes_2[][64], char *saida);

int busca(int *memoria,int PC);

void decodificacao(instrucaoBuscada instrucao_buscada,instrucaoDecodificada *instrucao_decodificada);

bool emissao(instrucaoDecodificada instrucao_decodificada, int tnum);

void leituraOperandos(listaExecucao instrucoes_prontas[5], int PC, int* memoria, int tnum);

void execucao(listaExecucao instrucoes_prontas[5], instrucaoExecutando lista_instrucoes_executando[5], resultadoExec lista_resultados[5], int tnum);

void escritaResultados(resultadoExec lista_resultados[5], int instrucoes_escritas[5], int tnum);

int executaInstrucao(listaExecucao instrucao);

void preencheStatusInstrucoes(int etapa, int PC, int tnum);

int tamanhoMemoria(int memoria[]);

int recuperaCampo(int instruct, int size, int starting_bit);

int opcodeParaUF(int opcode);

char* opcodeParaOperacao(int opcode);

bool verificaImediato(int opcode);

int verificaRegistradorOcupado(int8_t registrador, int indice, int tnum);

void preencheStatusRegistradorRj(int indice, int8_t registrador, int tnum);

void preencheStatusUF(int indice_UF, int instrucao, int opcode, int tnum);

bool verificaTermino(int PC, int tamanho_memoria, instrucaoDecodificada instrucao_decodificada, int tnum);

int opcodeParaNumCiclos(int opcode);

void scoreboarding(threadInfo thread_info);

bool verificaMove(int opcode);

void preencheStatusRegistradorRk(int indice, int8_t registrador, int tnum);

void preencheOcupacaoStatusUF(int indice_UF, int tnum);

void limpaEstruturas(int instrucoes_escritas[5], resultadoExec lista_resultados[5], listaExecucao instrucoes_prontas[5],
    instrucaoExecutando lista_instrucoes_executando[5], int tnum);

void limpaUF(int indice_UF, int tnum);

bool verificaLi(int opcode);

bool liberaLeitura(int i, int tnum);

void limpaAuxiliares(int indice,resultadoExec lista_resultados[5], listaExecucao instrucoes_prontas[5],
             instrucaoExecutando lista_instrucoes_executando[5]);

void atualizaDependencias();

void limpaBusca(instrucaoBuscada *instrucao_buscada);

void limpaDecodificacao(instrucaoDecodificada *instrucao_decodificada);

void executaUF(listaExecucao *instrucao_pronta, instrucaoExecutando *instrucao_executando,resultadoExec *resultado, int tnum);

bool verificaWAW(int destino_nova, int tnum);

void initProcessador(int PC[2]);

void initNucleo(int *memoria, int tamanho_memoria, threadInfo thread_info, int PC);

void thread_start(threadInfo thread_info);

#endif
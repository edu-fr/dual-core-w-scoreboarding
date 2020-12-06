

#ifndef PROCESSADOR_H_INCLUDED
#define PROCESSADOR_H_INCLUDED


// ARQUIVO PROCESSADOR.C

void processador(int memoria[2], int tamanho_memoria[2], int linhas_instrucoes_[2], char *saida);

int busca(int *memoria,int PC);

void decodificacao(instrucaoBuscada instrucao_buscada,instrucaoDecodificada *instrucao_decodificada);

bool emissao(instrucaoDecodificada instrucao_decodificada);

void leituraOperandos(listaExecucao instrucoes_prontas[5], int PC, int* memoria);

void execucao(listaExecucao instrucoes_prontas[5], instrucaoExecutando lista_instrucoes_executando[5], resultadoExec lista_resultados[5]);

void escritaResultados(resultadoExec lista_resultados[5], int instrucoes_escritas[5]);

int executaInstrucao(listaExecucao instrucao);

void preencheStatusInstrucoes();

int tamanhoMemoria(int memoria[]);

int recuperaCampo(int instruct, int size, int starting_bit);

int opcodeParaUF(int opcode);

char* opcodeParaOperacao(int opcode);

bool verificaImediato(int opcode);

int verificaRegistradorOcupado(int8_t registrador, int indice);

void preencheStatusRegistradorRj(int indice, int8_t registrador);

void preencheStatusUF(int indice_UF, int instrucao, int opcode);

bool verificaTermino(int PC, int tamanho_memoria, instrucaoDecodificada instrucao_decodificada);

int opcodeParaNumCiclos(int opcode);

void scoreboarding(threadInfo thread_info);

bool verificaMove(int opcode);

void preencheStatusRegistradorRk(int indice, int8_t registrador);

void preencheOcupacaoStatusUF(int indice_UF);

void limpaEstruturas(int instrucoes_escritas[5], resultadoExec lista_resultados[5], listaExecucao instrucoes_prontas[5],
    instrucaoExecutando lista_instrucoes_executando[5]);

void limpaUF(int indice_UF);

bool verificaLi(int opcode);

bool liberaLeitura(int i);

void limpaAuxiliares(int indice,resultadoExec lista_resultados[5], listaExecucao instrucoes_prontas[5],
             instrucaoExecutando lista_instrucoes_executando[5]);

void initScoreboarding(int* memoria, int tamanho_memoria, listaExecucao instrucoes_prontas[5], bool *acabou_de_executar,
                       instrucaoExecutando lista_instrucoes_executando[5],resultadoExec lista_resultados[5], 
                       int instrucoes_escritas[5]);

void atualizaDependencias();

void limpaBusca(instrucaoBuscada *instrucao_buscada);

void limpaDecodificacao(instrucaoDecodificada *instrucao_decodificada);

void executaUF(listaExecucao *instrucao_pronta, instrucaoExecutando *instrucao_executando,resultadoExec *resultado);

bool verificaWAW(int destino_nova);

#endif
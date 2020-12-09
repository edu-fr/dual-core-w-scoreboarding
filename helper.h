#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#define FLUXO_1 0
#define FLUXO_2 1

enum opcode {ADD = 32, ADDI = 8, AND = 36, ANDI = 12, OR = 37, ORI = 13, SLT = 42, SUB = 34, MULT = 24, DIV = 26, 
LI = 20, MOVE = 30};
enum UF {Mult1 = 0, Mult2, Add, Div, Log};
enum etapa {EMISSAO = 0, LEITURA_OPERANDOS = 1, EXECUCAO = 2, ESCRITA = 3};
enum registradores{ zero = 0, at, v0, v1, a0, a1, a2, a3, t0, t1, t2, t3, t4, t5, t6, t7, s0, s1, s2, s3, s4, s5, s6, s7, 
t8, t9, k0, k1, gp, sp, fp, ra};

// Definicao das estruturas auxiliares
typedef struct {
    int instrucao;
    int PC_busca;
}instrucaoBuscada;

typedef struct {
    int opcode;
    int indice_UF;
    int instrucao_completa;
    int PC_busca;
}instrucaoDecodificada;

typedef struct{
    int *emissao; 
    int *leituraOP;
    int *execucao;
    int *escrita;
    int *instrucoes; // Representa todas as intrucoes
}statusInst; // Struct do 'scoreboard'

//char
typedef struct{ // Definimos UnidadeFuncional como uma struct e criamos uma unidade funcional pra cada 
    enum UF FU;
    char busy[10];
    char op[20];
    int8_t Fi;
    int8_t Fj;
    int8_t Fk;
    enum UF Qj;
    enum UF Qk;
    char Rj[10];
    char Rk[10];
} UnidadeFuncional;

typedef struct{
    int PC;
    int resultado;
    int ciclo_termino;
    int reg_destino;
    int instrucao;
} resultadoExec;

typedef struct{ // Representação dos barramentos
    int instrucao;
    int PC;
    int clock_lido;
    int opcode;
    int destino;
    int operando1;
    int operando2;
    bool ja_executou;
} listaExecucao;

typedef struct{
    int clock_inicio;
    int ciclos_restantes;
} instrucaoExecutando;

typedef struct{
    int opcodes_emitidos[5];    // indice de acordo com a UF que será utilizada
    int instrucoes_emitidas[5]; // indice de acordo com a UF que será utilizada
    int PC_emitido[5]; 
} emissoes;

/* Pthread */ 

typedef struct{
    pthread_t thread_id;        /* ID returned by pthread_create() */
    int thread_num;
    int *memoria;
    int tamanho_memoria;
    char* nome_arq_saida;
    char** linhas_instrucoes;
} tInfo;

/***/

int num_ciclos_ADD, num_ciclos_ADDI, num_ciclos_AND,
    num_ciclos_ANDI, num_ciclos_OR, num_ciclos_ORI, num_ciclos_SLT, num_ciclos_SUB,
    num_ciclos_MULT, num_ciclos_DIV, num_ciclos_LI, num_ciclos_MOVE;


// variaveis auxiliares
extern int clock_processador;
extern int F[32]; // Banco de registradores
extern UnidadeFuncional vetor_UF[5];
extern enum UF status_dos_registradores[32];
extern emissoes lista_emissoes;

extern int opcodes_emitidos[5];    // indice de acordo com a UF que será utilizada
extern int instrucoes_emitidas[5]; // indice de acordo com a UF que será utilizada

extern listaExecucao *lista_execucao;
extern resultadoExec *vetor_resultados;
extern statusInst status_instrucoes; // Criamos a struct q representa os status das instucoes do scoreboard
#include "operacoes.c"
#include "saida.c"
#include "processador.h"
#include <math.h>

void* scoreboarding(void *arg){
    tInfo *t_info = arg;

    // VARIAVEIS EX-GLOBAIS
    int clock_processador;
    statusInst status_instrucoes;           // Status das instrucoes
    int banco_registradores[32];            // Banco dos registradores
    UnidadeFuncional vetor_UF[5];           // Unidades Funcionais
    enum UF status_dos_registradores[32];   // Status dos Registradores
    bool flag_registradores[32];
    // Estrutura Auxiliar da Emissao
    emissoes lista_emissoes;
    int clock_instrucoes_lidas[5];

    /* Dados t_info */
    int *memoria = t_info->memoria;
    int tamanho_memoria = t_info->tamanho_memoria;
    char *nome_arq_saida;
    nome_arq_saida = t_info->nome_arq_saida;

    printf("Nome saida do t_info: %s e thread NUM : %d\n\n",t_info->nome_arq_saida, t_info->thread_num);
    
    
    char linhas_instrucoes[tamanho_memoria][64];

    for (int i = 0; i < tamanho_memoria; i++)
    {
        strcpy(linhas_instrucoes[i], t_info->linhas_instrucoes[i]);
    }
    /**

    for (int i = 0; i < tamanho_memoria; i++)
    {
        printf("Memoria posicao %d: %d\n", i, memoria[i]);
        printf("Em texto: %s\n", linhas_instrucoes[i]);
    }
    printf("Tamanho da memoria: %d\n", tamanho_memoria);
    printf("Nome do arquivo de saida: %s\n", nome_arq_saida);
    */
    //Estruturas Auxiliares
    listaExecucao instrucoes_prontas[5];
    instrucaoExecutando lista_instrucoes_executando[5];
    resultadoExec lista_resultados[5];
    int instrucoes_escritas[5];
    bool acabou_de_executar;
    instrucaoBuscada instrucao_buscada;
    instrucaoDecodificada instrucao_decodificada;
    // init estruturas auxiliares
    limpaBusca(&instrucao_buscada);
    limpaDecodificacao(&instrucao_decodificada);

    initScoreboarding(memoria, tamanho_memoria, instrucoes_prontas, &acabou_de_executar, 
    lista_instrucoes_executando, lista_resultados, instrucoes_escritas, &clock_processador,
    vetor_UF, status_dos_registradores, flag_registradores,
     &lista_emissoes, clock_instrucoes_lidas, banco_registradores, &status_instrucoes);
    int PC = 0;
    FILE *arq_saida;
    arq_saida = fopen(nome_arq_saida, "w");

    do
    {
        //printf("\n======== CLOCK %d ========\n", clock_processador);
        escritaResultados(lista_resultados, instrucoes_escritas, 
        clock_processador, &status_instrucoes, banco_registradores, 
        vetor_UF, flag_registradores, lista_emissoes, clock_instrucoes_lidas);
        execucao(instrucoes_prontas, lista_instrucoes_executando, 
        lista_resultados, clock_processador, &status_instrucoes, banco_registradores);
        leituraOperandos(instrucoes_prontas, PC - 1, memoria,
        clock_processador, &status_instrucoes, vetor_UF, &lista_emissoes, clock_instrucoes_lidas);

        if (PC < tamanho_memoria)
        {
            if (instrucao_buscada.instrucao == 0)
            {
                instrucao_buscada.instrucao = busca(memoria, PC);
                instrucao_buscada.PC_busca = PC;

                decodificacao(instrucao_buscada, &instrucao_decodificada);
            }
        }

        if (instrucao_decodificada.instrucao_completa != 0)
        { // só emite se necessario
            if (emissao(instrucao_decodificada, clock_processador, 
            &status_instrucoes, vetor_UF, status_dos_registradores, &lista_emissoes))
            {
                limpaBusca(&instrucao_buscada);
                limpaDecodificacao(&instrucao_decodificada);
                PC += 1;
            }
        }
        printf("\nClock do processador (%d) %d \n \n",t_info->thread_num,clock_processador);
        saida(arq_saida, PC, linhas_instrucoes, clock_processador,
        status_instrucoes, vetor_UF, status_dos_registradores, 
        banco_registradores, flag_registradores);
        limpaEstruturas(instrucoes_escritas, lista_resultados, 
        instrucoes_prontas, lista_instrucoes_executando, vetor_UF,
        status_dos_registradores, clock_instrucoes_lidas);
        atualizaDependencias(vetor_UF, lista_emissoes);
        //checa se alguma Unidade Funcional ainda tem processos a fazer
        acabou_de_executar = verificaTermino(PC, tamanho_memoria, instrucao_decodificada,vetor_UF);

        clock_processador += 1;
    } while (!acabou_de_executar);
    
    saida(arq_saida, PC, linhas_instrucoes, clock_processador,
        status_instrucoes, vetor_UF, status_dos_registradores, 
        banco_registradores, flag_registradores);
    fclose(arq_saida);
    
    return 0;
}

void limpaBusca(instrucaoBuscada *instrucao_buscada)
{
    instrucao_buscada->instrucao = 0;
    instrucao_buscada->PC_busca = -1;
}

void limpaDecodificacao(instrucaoDecodificada *instrucao_decodificada)
{
    instrucao_decodificada->opcode = 0;
    instrucao_decodificada->indice_UF = 0;
    instrucao_decodificada->instrucao_completa = 0;
    instrucao_decodificada->PC_busca = 0;
}

int busca(int *memoria, int PC)
{ // retorna a instrucao na posicao de pc
    int instrucao_buscada = memoria[PC];
    return instrucao_buscada;
}

void decodificacao(instrucaoBuscada instrucao_buscada, instrucaoDecodificada *instrucao_decodificada)
{
    instrucao_decodificada->opcode = recuperaCampo(instrucao_buscada.instrucao, 6, 32); //extrai o opcode da instruçao
    instrucao_decodificada->indice_UF = opcodeParaUF(instrucao_decodificada->opcode);   //extrai a UF em que a instrucao deve ir
    instrucao_decodificada->instrucao_completa = instrucao_buscada.instrucao;
    instrucao_decodificada->PC_busca = instrucao_buscada.PC_busca;
}

bool verificaWAW(int destino_nova, UnidadeFuncional vetor_UF[5])
{
    for (int i = 0; i < 5; i++)
    {
        if (vetor_UF[i].Fi == destino_nova)
        {
            return true;
        }
    }
    return false;
}

bool emissao(instrucaoDecodificada instrucao_decodificada, 
int clock_processador, statusInst *status_instrucoes, UnidadeFuncional vetor_UF[5],
enum UF status_dos_registradores[32], emissoes *lista_emissoes)
{
    int destino_nova = recuperaCampo(instrucao_decodificada.instrucao_completa, 5, 26);

    if (instrucao_decodificada.indice_UF == 0)
    { // É uma instrucao mult
        if (strcmp(vetor_UF[0].busy, "nao") == 0)
        { // Se UF Mult1 está livre, então
            // indice UF continua sendo 0
        }
        else
        { // se UF Mult1 está ocupada, testa UF Mult2
            if (strcmp(vetor_UF[1].busy, "nao") == 0)
            {
                instrucao_decodificada.indice_UF = 1;
            }
        }
    }

    if (strcmp(vetor_UF[instrucao_decodificada.indice_UF].busy, "sim") == 0)
    {
        return false;
    }
    if (verificaWAW(destino_nova, vetor_UF))
    {
        return false;
    }
    else
    {
        preencheStatusUF(instrucao_decodificada.indice_UF, 
        instrucao_decodificada.instrucao_completa,
         instrucao_decodificada.opcode, vetor_UF, *lista_emissoes); //
        lista_emissoes->opcodes_emitidos[instrucao_decodificada.indice_UF] = instrucao_decodificada.opcode;
        lista_emissoes->instrucoes_emitidas[instrucao_decodificada.indice_UF] = instrucao_decodificada.instrucao_completa; //
        lista_emissoes->PC_emitido[instrucao_decodificada.indice_UF] = instrucao_decodificada.PC_busca;
        preencheStatusInstrucoes(EMISSAO, instrucao_decodificada.PC_busca, clock_processador, status_instrucoes);
        status_dos_registradores[vetor_UF[instrucao_decodificada.indice_UF].Fi] = instrucao_decodificada.indice_UF;
        return true;
    }
}

void leituraOperandos(listaExecucao instrucoes_prontas[5], int PC, int *memoria, 
int clock_processador, statusInst *status_instrucoes, UnidadeFuncional vetor_UF[5],
emissoes *lista_emissoes, int clock_instrucoes_lidas[5])
{
    for (int i = 0; i < 5; i++)
    {
        //percorre as UF's e ve qual pode ser lida
        if (strcmp(vetor_UF[i].busy, "sim") == 0)
        { // só le operandos se tiver instrucao omitida na UF
            if (liberaLeitura(i, vetor_UF) && lista_emissoes->opcodes_emitidos[i] != -1)
            {
                //ambas sao true entao pode ler
                instrucoes_prontas[i].opcode = lista_emissoes->opcodes_emitidos[i];
                instrucoes_prontas[i].PC = lista_emissoes->PC_emitido[i];
                lista_emissoes->opcodes_emitidos[i] = -1; // Garante que essa instrucao nao sera lida novamente
                instrucoes_prontas[i].destino = vetor_UF[i].Fi;
                instrucoes_prontas[i].instrucao = memoria[PC];
                instrucoes_prontas[i].operando1 = vetor_UF[i].Fj;
                instrucoes_prontas[i].operando2 = vetor_UF[i].Fk;
                instrucoes_prontas[i].ja_executou = false;
                instrucoes_prontas[i].clock_lido = clock_processador;
                clock_instrucoes_lidas[i] = clock_processador;
                //printf("\nInstrucao n %d foi lida no clock %d! \n", PC, instrucoes_prontas[i].clock_lido);
                preencheStatusInstrucoes(LEITURA_OPERANDOS, lista_emissoes->PC_emitido[i],
                 clock_processador, status_instrucoes);
            }
        }
    }
}

void execucao(listaExecucao instrucoes_prontas[5], 
instrucaoExecutando lista_instrucoes_executando[5], resultadoExec lista_resultados[5], 
int clock_processador, statusInst *status_instrucoes, int banco_registradores[32])
{
    /* Testar D+ */
    executaUF(&instrucoes_prontas[Mult1], &lista_instrucoes_executando[Mult1],
     &lista_resultados[Mult1], clock_processador, status_instrucoes, banco_registradores);
    executaUF(&instrucoes_prontas[Mult2], &lista_instrucoes_executando[Mult2], 
    &lista_resultados[Mult2], clock_processador, status_instrucoes, banco_registradores);
    executaUF(&instrucoes_prontas[Add], &lista_instrucoes_executando[Add], 
    &lista_resultados[Add], clock_processador, status_instrucoes, banco_registradores);
    executaUF(&instrucoes_prontas[Div], &lista_instrucoes_executando[Div],
     &lista_resultados[Div],clock_processador,  status_instrucoes, banco_registradores);
    executaUF(&instrucoes_prontas[Log], &lista_instrucoes_executando[Log],
     &lista_resultados[Log], clock_processador, status_instrucoes, banco_registradores);
}

void executaUF(listaExecucao *instrucao_pronta, instrucaoExecutando *instrucao_executando, 
resultadoExec *resultado, int clock_processador, statusInst *status_instrucoes, 
int banco_registradores[32])
{

    if (instrucao_pronta->PC != -1 && instrucao_pronta->ja_executou == false)
    {
        instrucao_executando->ciclos_restantes = opcodeParaNumCiclos(instrucao_pronta->opcode) - 1;
        instrucao_executando->clock_inicio = instrucao_pronta->clock_lido + 1;
        instrucao_pronta->ja_executou = true; // comecou a execucao
    }

    if (instrucao_executando->ciclos_restantes == 0)
    {
        resultado->resultado = executaInstrucao(*instrucao_pronta, banco_registradores);
        resultado->ciclo_termino = clock_processador;
        resultado->reg_destino = instrucao_pronta->destino;
        resultado->PC = instrucao_pronta->PC;
        preencheStatusInstrucoes(EXECUCAO, instrucao_pronta->PC, 
        clock_processador, status_instrucoes);

        instrucao_executando->ciclos_restantes -= 1;
    }
    else if (instrucao_executando->ciclos_restantes > 0)
    {
        instrucao_executando->ciclos_restantes -= 1;
    }
}

bool liberaLeitura(int i, UnidadeFuncional vetor_UF[5])
{
    if ((strcmp(vetor_UF[i].Rj, "nao")) && (strcmp(vetor_UF[i].Rk, "nao")))
    {
        return true;
    }
    return false;
}

bool verificaWAR(int reg_destino, int indice_UF_atual,
 int clock_processador, UnidadeFuncional vetor_UF[5], 
 emissoes lista_emissoes, int clock_instrucoes_lidas[5])
{
    for (int i = 0; i < 5; i++)
    { // verifica se algum dos outros 4 registradores vai utilizar na leitura o reg destino atual
        if (i == indice_UF_atual)
        { // não conflita com ele mesmo
            continue;
        }
        if (lista_emissoes.PC_emitido[indice_UF_atual] > lista_emissoes.PC_emitido[i])
        { // só verifica a leitura em inst passadas
            
            if (reg_destino == vetor_UF[i].Fj || reg_destino == vetor_UF[i].Fk)
            { // encontrou war

                if (!strcmp(vetor_UF[i].Rj, "sim") && !strcmp(vetor_UF[i].Rk, "sim"))
                { //
                    
                    if (clock_processador > clock_instrucoes_lidas[i])
                    {
                        if (clock_instrucoes_lidas[i] != -1)
                        {
                            
                            return false;
                        }
                        
                        return true;
                    }
                    return true;
                }
                return true;
            }
        }
    }
    return false;
}

void escritaResultados(resultadoExec lista_resultados[5],
 int instrucoes_escritas[5], int clock_processador, 
 statusInst *status_instrucoes, int banco_registradores[32],
 UnidadeFuncional vetor_UF[5],  bool flag_registradores[32],
  emissoes lista_emissoes, int clock_instrucoes_lidas[5])
{
    for (int i = 0; i < 5; i++)
    {
        if (lista_resultados[i].ciclo_termino == -1)
        {
            continue;
        }
        if (verificaWAR(vetor_UF[i].Fi, i, clock_processador, 
        vetor_UF, lista_emissoes, clock_instrucoes_lidas))
        {
            converteNomeUF(i);
            continue;
        }
        instrucoes_escritas[i] = 1;
        banco_registradores[lista_resultados[i].reg_destino] = lista_resultados[i].resultado;
        flag_registradores[lista_resultados[i].reg_destino] = true;
        preencheStatusInstrucoes(ESCRITA, lista_resultados[i].PC, 
        clock_processador, status_instrucoes);
    }
}

void limpaEstruturas(int instrucoes_escritas[5], resultadoExec lista_resultados[5], 
listaExecucao instrucoes_prontas[5], instrucaoExecutando lista_instrucoes_executando[5],
UnidadeFuncional vetor_UF[5], enum UF status_dos_registradores[32], int clock_instrucoes_lidas[5])
{
    for (int i = 0; i < 5; i++)
    {
        if (instrucoes_escritas[i] == 1)
        {
            limpaUF(i, vetor_UF);
            status_dos_registradores[lista_resultados[i].reg_destino] = -1;
            limpaAuxiliares(i, lista_resultados, instrucoes_prontas, 
            lista_instrucoes_executando, clock_instrucoes_lidas);
            instrucoes_escritas[i] = -1;
        }
    }
}

void atualizaDependencias(UnidadeFuncional vetor_UF[5], emissoes lista_emissoes)
{
    for (int i = 0; i < 5; i++)
    {
        preencheStatusRegistradorRj(i, vetor_UF[i].Fj, vetor_UF, lista_emissoes);
        preencheStatusRegistradorRk(i, vetor_UF[i].Fk, vetor_UF, lista_emissoes);
    }
}

int verificaRegistradorOcupado(int8_t registrador, int indice_UF, 
UnidadeFuncional vetor_UF[5], emissoes lista_emissoes)
{
    for (int i = 0; i < 5; i++)
    {
        if (lista_emissoes.PC_emitido[indice_UF] > lista_emissoes.PC_emitido[i])
        {
            if (vetor_UF[i].Fi == registrador && vetor_UF[i].Fi != -1)
            {
                if (i == indice_UF)
                {
                    continue; //indice igual o da propria UF -> registrador destino eh a fonte
                }
                return i;
            }
        }
    }
    return -1;
}
void preencheStatusRegistradorRk(int indice, int8_t registrador, UnidadeFuncional vetor_UF[5], emissoes lista_emissoes)
{
    int registrador_ocupado = verificaRegistradorOcupado(registrador, indice, 
    vetor_UF, lista_emissoes);
    if (strcmp(vetor_UF[indice].busy, "nao"))
    {
        if (registrador_ocupado != -1)
        {
            vetor_UF[indice].Qk = registrador_ocupado;
            strcpy(vetor_UF[indice].Rk, "nao");
        }
        else
        {
            if (registrador == -1)
            {
                strcpy(vetor_UF[indice].Rk, "");
            }
            else
            {
                strcpy(vetor_UF[indice].Rk, "sim");
            }
        }
    }
}

void preencheStatusRegistradorRj(int indice, int8_t registrador, UnidadeFuncional vetor_UF[5], emissoes lista_emissoes)
{
    int registrador_ocupado = verificaRegistradorOcupado(registrador, indice, 
    vetor_UF, lista_emissoes);
    if (strcmp(vetor_UF[indice].busy, "nao"))
    {
        if (registrador_ocupado != -1)
        {
            vetor_UF[indice].Qj = registrador_ocupado;
            strcpy(vetor_UF[indice].Rj, "nao");
        }
        else
        {
            if (registrador == -1)
            {
                strcpy(vetor_UF[indice].Rj, "");
            }
            else
            {
                strcpy(vetor_UF[indice].Rj, "sim");
            }
        }
    }
}

void limpaAuxiliares(int indice, resultadoExec lista_resultados[5], 
listaExecucao instrucoes_prontas[5],
instrucaoExecutando lista_instrucoes_executando[5],
int clock_instrucoes_lidas[5])
{

    lista_resultados[indice].ciclo_termino = -1;
    lista_resultados[indice].instrucao = -1;
    lista_resultados[indice].PC = -1;
    lista_resultados[indice].reg_destino = -1;
    lista_resultados[indice].resultado = -1;

    instrucoes_prontas[indice].clock_lido = -1;
    instrucoes_prontas[indice].destino = -1;
    instrucoes_prontas[indice].instrucao = -1;
    instrucoes_prontas[indice].ja_executou = false;
    instrucoes_prontas[indice].opcode = -1;
    instrucoes_prontas[indice].operando1 = -1;
    instrucoes_prontas[indice].operando2 = -1;
    instrucoes_prontas[indice].PC = -1;

    clock_instrucoes_lidas[indice] = -1;

    lista_instrucoes_executando[indice].ciclos_restantes = -1;
    lista_instrucoes_executando[indice].clock_inicio = -1;
}

void limpaUF(int indice_UF, UnidadeFuncional vetor_UF[5])
{
    vetor_UF[indice_UF].FU = -1;
    vetor_UF[indice_UF].Fi = -1;
    vetor_UF[indice_UF].Fj = -1;
    vetor_UF[indice_UF].Fk = -1;
    vetor_UF[indice_UF].Qj = -1;
    vetor_UF[indice_UF].Qk = -1;
    strcpy(vetor_UF[indice_UF].Rj, "");
    strcpy(vetor_UF[indice_UF].Rk, "");
    strcpy(vetor_UF[indice_UF].busy, "nao");
    strcpy(vetor_UF[indice_UF].op, "");
}

int executaInstrucao(listaExecucao instrucao, int banco_registradores[32])
{
    int resultado;
    switch (instrucao.opcode)
    {
    case ADD:
        resultado = add(banco_registradores[instrucao.operando1], banco_registradores[instrucao.operando2]);
        break;
    case ADDI:
        resultado = addi(banco_registradores[instrucao.operando1], recuperaCampo(instrucao.instrucao, 16, 16));
        break;
    case AND:
        resultado = and(banco_registradores[instrucao.operando1], banco_registradores[instrucao.operando2]);
        break;
    case ANDI:
        resultado = andi(banco_registradores[instrucao.operando1], recuperaCampo(instrucao.instrucao, 16, 16));
        break;
    case OR:
        resultado = or (banco_registradores[instrucao.operando1], banco_registradores[instrucao.operando2]);
        break;
    case ORI:
        resultado = ori(banco_registradores[instrucao.operando1], recuperaCampo(instrucao.instrucao, 16, 16));
        break;
    case SLT:
        resultado = slt(banco_registradores[instrucao.operando1], banco_registradores[instrucao.operando2]);
        break;
    case SUB:
        resultado = sub(banco_registradores[instrucao.operando1], banco_registradores[instrucao.operando2]);
        break;
    case MULT:
        resultado = mult(banco_registradores[instrucao.operando1], banco_registradores[instrucao.operando2]);
        break;
    case DIV:
        resultado = divi(banco_registradores[instrucao.operando1], banco_registradores[instrucao.operando2]);
        if (resultado == -1)
        {
            printf("Erro: Divisao por 0 -> infinito. Abortando...\n");
            exit(1);
        }
        break;
    case LI:
        resultado = li(recuperaCampo(instrucao.instrucao, 16, 16));
        break;
    case MOVE:
        resultado = move(banco_registradores[instrucao.operando1]);
        break;
    default:
        printf("Operacao: %d nao encontrada.\n", instrucao.opcode);
        return -1;
    }
    return resultado;
}

void preencheStatusInstrucoes(int etapa, int PC, 
int clock_processador, statusInst *status_instrucoes)
{
    switch (etapa)
    {
    case EMISSAO:
        status_instrucoes->emissao[PC] = clock_processador;
        break;
    case LEITURA_OPERANDOS:
        status_instrucoes->leituraOP[PC] = clock_processador;
        break;
    case EXECUCAO:
        status_instrucoes->execucao[PC] = clock_processador;
        break;
    case ESCRITA:
        status_instrucoes->escrita[PC] = clock_processador;
        break;
    }
}

// Retorna os campos da instrucao (Ja transformada em 32bits)
int recuperaCampo(int instruct, int size, int starting_bit)
{
    int32_t mask_2 = pow(2, size) - 1; // corresponde ao inteiro de 32 bits cujos seis bits mais significativos eh 1
    int mask = 0;
    for (int i = 0; i < 32; i++)
    {
        if (i < size)
        {
            mask = mask << 1 | 1;
        }
        if (i >= size && i < starting_bit)
        {
            mask = mask << 1;
        }
    }
    int operator= mask & instruct;
    for (int i = starting_bit; i > size; i--)
        operator= operator>> 1 | 0;
    operator= operator& mask_2;
    return operator;
}

int opcodeParaUF(int opcode)
{
    if (opcode == ADD || opcode == ADDI || opcode == SUB)
        return Add;
    else if (opcode == DIV)
        return Div;
    else if (opcode == AND || opcode == ANDI || opcode == OR || 
    opcode == ORI || opcode == SLT || opcode == LI || opcode == MOVE)
        return Log;
    else if (opcode == MULT){
        return Mult1;
    }
    return -1;
}

char *opcodeParaOperacao(int opcode)
{
    switch (opcode)
    {
    case ADD:
    case ADDI:
        return "add";
    case SUB:
        return "subt";
    case AND:
    case ANDI:
        return "and";
    case OR:
    case ORI:
        return "or";
    case SLT:
        return "slt";
    case MULT:
        return "mult";
    case DIV:
        return "divi";
    case LI:
        return "load";
    case MOVE:
        return "move";
    }
    printf("Erro, operacao nao encontrada para o opcode %d!\n", opcode);
    exit(1);
    return "Erro";
}

bool verificaImediato(int opcode)
{
    if (opcode == ADDI || opcode == ORI || opcode == LI || opcode == ANDI)
    {
        return true;
    }
    return false;
}

bool verificaMove(int opcode)
{
    if (opcode == MOVE)
    {
        return true;
    }
    return false;
}

bool verificaLi(int opcode)
{
    if (opcode == LI)
    {
        return true;
    }
    return false;
}

void preencheStatusUF(int indice_UF, int instrucao, int opcode, 
UnidadeFuncional vetor_UF[5], emissoes lista_emissoes)
{
    /* Preenche a linha do status das unidades funcionais */
    strcpy(vetor_UF[indice_UF].busy, "sim");
    strcpy(vetor_UF[indice_UF].op, opcodeParaOperacao(opcode));
    vetor_UF[indice_UF].Fi = recuperaCampo(instrucao, 5, 26);
    vetor_UF[indice_UF].Fj = recuperaCampo(instrucao, 5, 21);
    preencheStatusRegistradorRj(indice_UF, vetor_UF[indice_UF].Fj, vetor_UF,lista_emissoes);

    if (verificaLi(opcode))
    { // se for Li
        vetor_UF[indice_UF].Fj = -1;
        strcpy(vetor_UF[indice_UF].Rj, "");
        vetor_UF[indice_UF].Fk = -1;
        strcpy(vetor_UF[indice_UF].Rk, "");
    }
    else if (verificaMove(opcode))
    { //move
        vetor_UF[indice_UF].Fk = -1;
        strcpy(vetor_UF[indice_UF].Rk, "");
    }
    else if (!verificaImediato(opcode))
    { //se for normal
        vetor_UF[indice_UF].Fk = recuperaCampo(instrucao, 5, 16);
        preencheStatusRegistradorRk(indice_UF, vetor_UF[indice_UF].Fk, vetor_UF,lista_emissoes);
    }
    else
    { // se for imediato
        vetor_UF[indice_UF].Fk = -1;
        strcpy(vetor_UF[indice_UF].Rk, "");
    }
    preencheOcupacaoStatusUF(indice_UF, vetor_UF, lista_emissoes);
}

void preencheOcupacaoStatusUF(int indice_UF, UnidadeFuncional vetor_UF[5], emissoes lista_emissoes)
{
    int indice_ocupado;
    // Verifica se Fj está ocupado
    indice_ocupado = verificaRegistradorOcupado(vetor_UF[indice_UF].Fj, 
    indice_UF, vetor_UF, lista_emissoes);
    if (indice_ocupado > -1)
    { // Está ocupado
        vetor_UF[indice_UF].Qj = indice_ocupado;
        strcpy(vetor_UF[indice_UF].Rj, "nao");
    }
    else
    {
        strcpy(vetor_UF[indice_UF].Rj, "sim");
    }
    // Verifica se Fk está ocupado
    indice_ocupado = verificaRegistradorOcupado(vetor_UF[indice_UF].Fk,
     indice_UF, vetor_UF, lista_emissoes);
    if (indice_ocupado > -1)
    { // Está ocupado
        vetor_UF[indice_UF].Qk = indice_ocupado;
        strcpy(vetor_UF[indice_UF].Rk, "nao");
    }
    else
    {
        strcpy(vetor_UF[indice_UF].Rk, "sim");
    }
}

bool verificaTermino(int PC, int tamanho_memoria, instrucaoDecodificada instrucao_decodificada,
 UnidadeFuncional vetor_UF[5])
{
    for (int j = 0; j < 5; j++)
    {
        if (!strcmp(vetor_UF[j].busy, "sim") || (PC < tamanho_memoria) || (instrucao_decodificada.instrucao_completa != 0))
        {
            return false;
        }
    }
    return true;
}

int opcodeParaNumCiclos(int opcode)
{
    if (opcode == ADD)
        return num_ciclos_ADD;
    else if (opcode == ADDI)
        return num_ciclos_ADDI;
    else if (opcode == AND)
        return num_ciclos_AND;
    else if (opcode == ANDI)
        return num_ciclos_ANDI;
    else if (opcode == OR)
        return num_ciclos_OR;
    else if (opcode == ORI)
        return num_ciclos_ORI;
    else if (opcode == SLT)
        return num_ciclos_SLT;
    else if (opcode == SUB)
        return num_ciclos_SUB;
    else if (opcode == MULT)
        return num_ciclos_MULT;
    else if (opcode == DIV)
        return num_ciclos_DIV;
    else if (opcode == LI)
        return num_ciclos_LI;
    else if (opcode == MOVE)
        return num_ciclos_MOVE;
}

void initScoreboarding(int *memoria, int tamanho_memoria, listaExecucao instrucoes_prontas[5], 
bool *acabou_de_executar, instrucaoExecutando lista_instrucoes_executando[5], 
resultadoExec lista_resultados[5], int instrucoes_escritas[5], int *clock_processador,
UnidadeFuncional vetor_UF[5], enum UF status_dos_registradores[32],bool flag_registradores[32],
emissoes *lista_emissoes, int clock_instrucoes_lidas[5], int banco_registradores[32], statusInst *status_instrucoes)
{
    printf("Cheguei na init Score !! \n");
    status_instrucoes->instrucoes = memoria; // recebe todas as instrucoes
    status_instrucoes->emissao = (int *)calloc(sizeof(int), tamanho_memoria);
    status_instrucoes->leituraOP = (int *)calloc(sizeof(int), tamanho_memoria);
    status_instrucoes->execucao = (int *)calloc(sizeof(int), tamanho_memoria);
    status_instrucoes->escrita = (int *)calloc(sizeof(int), tamanho_memoria);

    *clock_processador = 1;
    
    *acabou_de_executar = false;

    for (int i = 0; i < 5; i++)
    {
        limpaUF(i, vetor_UF);

        instrucoes_prontas[i].PC = -1;
        instrucoes_prontas[i].clock_lido = -1;
        instrucoes_prontas[i].destino = -1;
        instrucoes_prontas[i].instrucao = -1;
        instrucoes_prontas[i].ja_executou = false;
        instrucoes_prontas[i].opcode = -1;
        instrucoes_prontas[i].operando1 = -1;
        instrucoes_prontas[i].operando2 = -1;

        lista_instrucoes_executando[i].ciclos_restantes = -1;
        lista_instrucoes_executando[i].clock_inicio = -1;

        lista_resultados[i].ciclo_termino = -1;
        lista_resultados[i].instrucao = -1;
        lista_resultados[i].PC = -1;
        lista_resultados[i].reg_destino = -1;
        lista_resultados[i].resultado = -1;

        lista_emissoes->instrucoes_emitidas[i] = -1;
        lista_emissoes->opcodes_emitidos[i] = -1;
        lista_emissoes->PC_emitido[i] = -1;

        instrucoes_escritas[i] = -1;
        clock_instrucoes_lidas[i] = -1;
    }

    for (int i = 0; i < 32; i++)
    {
        banco_registradores[i] = 0;
        status_dos_registradores[i] = -1;
        flag_registradores[i] = false;
    }
}

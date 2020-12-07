#include "operacoes.c"
#include "saida.c"
#include "processador.h"
#include <math.h>


int clock_processador;
statusInst status_instrucoes[2];         // Status das Instrucoes
int banco_registradores[2][32];          // Banco de registradores
UnidadeFuncional vetor_UF[2][5];         // Unidades Funcionais
enum UF status_dos_registradores[2][32]; // Status dos Registradores
int clock_instrucoes_lidas[2][5];
bool flag_registradores[2][32];
bool verifica_termino[2];
emissoes lista_emissoes[2];

void processador(int memoria[2], int tamanho_memoria[2], char linhas_instrucoes_1[][64], char linhas_instrucoes_2[][64], char *nome_saida)
{
    //Estruturas Auxiliares
    threadInfo thread_info[2];
    int s;
    int PC[2];
    pthread_attr_t attr;
    void *res;

    FILE *arq_saida;

    arq_saida = fopen(nome_saida, "w");

    initProcessador(PC);

    initNucleo(memoria[FLUXO_1], tamanho_memoria[FLUXO_1], thread_info[FLUXO_1], PC[FLUXO_1]);
    initNucleo(memoria[FLUXO_2], tamanho_memoria[FLUXO_2], thread_info[FLUXO_2], PC[FLUXO_2]);

    s = pthread_attr_init(&attr);   
    if (s != 0)
        handle_error_en(s, "pthread_attr_init");

    for (int tnum = 0; tnum < 2; tnum++) { // Criacao dos pthread's
        thread_info[tnum].thread_num = tnum;
        s = pthread_create(&thread_info[tnum].thread_id, &attr,
                &thread_start(thread_info[tnum]), &thread_info[tnum]);

        if (s != 0)
            handle_error_en(s, "pthread_create");
        }

    s = pthread_attr_destroy(&attr);
    if (s != 0)
        handle_error_en(s, "pthread_attr_destroy");
    
    for (int i = 0; i < 2; i++) {
        s = pthread_join(thread_info[i].thread_id, &res);
        if (s != 0)
         handle_error_en(s, "pthread_join");

        free(res);      /* Free memory allocated by thread */
    }


    fclose(arq_saida);
    
}


void thread_start(threadInfo thread_info)
{
    bool acabou_de_executar;
    do{
        scoreboarding(thread_info);
        //rodou o programa
        saida(arq_saida, PC, linhas_instrucoes_1, linhas_instrucoes_2);
        limpaEstruturas(thread_info.instrucoes_escritas, thread_info.lista_resultados, thread_info.instrucoes_prontas, thread_info.lista_instrucoes_executando);
        atualizaDependencias(thread_info.thread_num);
        //checa se alguma Unidade Funcional ainda tem processos a fazer
        acabou_de_executar = verificaTermino(PC, thread_info.tamanho_memoria, thread_info.instrucao_decodificada, thread_info.thread_num);
        clock_processador++;
         
    }while(!acabou_de_executar);

    return 0;
}   



void scoreboarding(threadInfo thread_info)
{
    int tnum = thread_info.thread_num;
    printf("\n======== CLOCK %d ========\n", clock_processador);
    escritaResultados(thread_info.lista_resultados, thread_info.instrucoes_escritas, tnum);
    execucao(thread_info.instrucoes_prontas, thread_info.lista_instrucoes_executando, thread_info.lista_resultados, tnum);
    leituraOperandos(thread_info.instrucoes_prontas, thread_info.PC - 1, thread_info.memoria, tnum);

    if (thread_info.PC < thread_info.tamanho_memoria)
    {
        if (thread_info.instrucao_buscada.instrucao == 0)
        {
            thread_info.instrucao_buscada.instrucao = busca(thread_info.memoria, thread_info.PC);
            thread_info.instrucao_buscada.PC_busca = thread_info.PC;

            decodificacao(thread_info.instrucao_buscada, &thread_info.instrucao_decodificada);
        }
    }

    if (thread_info.instrucao_decodificada.instrucao_completa != 0)
    { // só emite se necessario
        if (emissao(thread_info.instrucao_decodificada, tnum))
        {
            limpaBusca(&thread_info.instrucao_buscada);
            limpaDecodificacao(&thread_info.instrucao_decodificada);
            thread_info.PC += 1;
        }
    }

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

bool verificaWAW(int destino_nova, int tnum)
{
    for (int i = 0; i < 5; i++)
    {
        if (vetor_UF[tnum][i].Fi == destino_nova)
        {
            return true;
        }
    }
    return false;
}

bool emissao(instrucaoDecodificada instrucao_decodificada, int tnum)
{
    int destino_nova = recuperaCampo(instrucao_decodificada.instrucao_completa, 5, 26);

    if (instrucao_decodificada.indice_UF == 0)
    { // É uma instrucao mult
        if (strcmp(vetor_UF[tnum][0].busy, "nao") == 0)
        { // Se UF Mult1 está livre, então
            // indice UF continua sendo 0
        }
        else
        { // se UF Mult1 está ocupada, testa UF Mult2
            if (strcmp(vetor_UF[tnum][1].busy, "nao") == 0)
            {
                instrucao_decodificada.indice_UF = 1;
            }
        }
    }

    if (strcmp(vetor_UF[tnum][instrucao_decodificada.indice_UF].busy, "sim") == 0)
    {
        return false;
    }
    if (verificaWAW(destino_nova, tnum))
    {
        return false;
    }
    else
    {
        preencheStatusUF(instrucao_decodificada.indice_UF, instrucao_decodificada.instrucao_completa, instrucao_decodificada.opcode, tnum); //
        lista_emissoes[tnum].opcodes_emitidos[instrucao_decodificada.indice_UF] = instrucao_decodificada.opcode;
        lista_emissoes[tnum].instrucoes_emitidas[instrucao_decodificada.indice_UF] = instrucao_decodificada.instrucao_completa; //
        lista_emissoes[tnum].PC_emitido[instrucao_decodificada.indice_UF] = instrucao_decodificada.PC_busca;
        preencheStatusInstrucoes(EMISSAO, instrucao_decodificada.PC_busca, tnum);
        status_dos_registradores[tnum][vetor_UF[tnum][instrucao_decodificada.indice_UF].Fi] = instrucao_decodificada.indice_UF;
        return true;
    }
}

void leituraOperandos(listaExecucao instrucoes_prontas[5], int PC, int *memoria, int tnum)
{
    for (int i = 0; i < 5; i++)
    {
        //percorre as UF's e ve qual pode ser lida
        if (strcmp(vetor_UF[tnum][i].busy, "sim") == 0)
        { // só le operandos se tiver instrucao omitida na UF
            if (liberaLeitura(i, tnum) && lista_emissoes[tnum].opcodes_emitidos[i] != -1)
            {
                //ambas sao true entao pode ler
                instrucoes_prontas[i].opcode = lista_emissoes[tnum].opcodes_emitidos[i];
                instrucoes_prontas[i].PC = lista_emissoes[tnum].PC_emitido[i];
                lista_emissoes[tnum].opcodes_emitidos[i] = -1; // Garante que essa instrucao nao sera lida novamente
                instrucoes_prontas[i].destino = vetor_UF[tnum][i].Fi;
                instrucoes_prontas[i].instrucao = memoria[PC];
                instrucoes_prontas[i].operando1 = vetor_UF[tnum][i].Fj;
                instrucoes_prontas[i].operando2 = vetor_UF[tnum][i].Fk;
                instrucoes_prontas[i].ja_executou = false;
                instrucoes_prontas[i].clock_lido = clock_processador;
                clock_instrucoes_lidas[tnum][i] = clock_processador;
                printf("\nInstrucao n %d foi lida no clock %d! \n", PC, instrucoes_prontas[i].clock_lido);
                preencheStatusInstrucoes(LEITURA_OPERANDOS, lista_emissoes[tnum].PC_emitido[i], tnum);
            }
        }
    }
}

void execucao(listaExecucao instrucoes_prontas[5], instrucaoExecutando lista_instrucoes_executando[5], resultadoExec lista_resultados[5], int tnum)
{
    /* Testar D+ */
    executaUF(&instrucoes_prontas[Mult1], &lista_instrucoes_executando[Mult1], &lista_resultados[Mult1], tnum);
    executaUF(&instrucoes_prontas[Mult2], &lista_instrucoes_executando[Mult2], &lista_resultados[Mult2], tnum);
    executaUF(&instrucoes_prontas[Add], &lista_instrucoes_executando[Add], &lista_resultados[Add], tnum);
    executaUF(&instrucoes_prontas[Div], &lista_instrucoes_executando[Div], &lista_resultados[Div], tnum);
    executaUF(&instrucoes_prontas[Log], &lista_instrucoes_executando[Log], &lista_resultados[Log], tnum);
}

void executaUF(listaExecucao *instrucao_pronta, instrucaoExecutando *instrucao_executando, resultadoExec *resultado, int tnum)
{

    if (instrucao_pronta->PC != -1 && instrucao_pronta->ja_executou == false)
    {
        instrucao_executando->ciclos_restantes = opcodeParaNumCiclos(instrucao_pronta->opcode) - 1;
        instrucao_executando->clock_inicio = instrucao_pronta->clock_lido + 1;
        instrucao_pronta->ja_executou = true; // comecou a execucao
    }

    if (instrucao_executando->ciclos_restantes == 0)
    {
        resultado->resultado = executaInstrucao(*instrucao_pronta);
        resultado->ciclo_termino = clock_processador;
        resultado->reg_destino = instrucao_pronta->destino;
        resultado->PC = instrucao_pronta->PC;
        preencheStatusInstrucoes(EXECUCAO, instrucao_pronta->PC, tnum);

        instrucao_executando->ciclos_restantes -= 1;
    }
    else if (instrucao_executando->ciclos_restantes > 0)
    {
        instrucao_executando->ciclos_restantes -= 1;
    }
}

bool liberaLeitura(int i, int tnum)
{
    if ((strcmp(vetor_UF[tnum][i].Rj, "nao")) && (strcmp(vetor_UF[tnum][i].Rk, "nao")))
    {
        return true;
    }
    return false;
}

bool verificaWAR(int reg_destino, int indice_UF_atual, int tnum)
{
    for (int i = 0; i < 5; i++)
    { // verifica se algum dos outros 4 registradores vai utilizar na leitura o reg destino atual
        if (i == indice_UF_atual)
        { // não conflita com ele mesmo
            continue;
        }
        if (lista_emissoes[tnum].PC_emitido[indice_UF_atual] > lista_emissoes[tnum].PC_emitido[i])
        { // só verifica a leitura em inst passadas
            printf("Entrei sim i = %d \n", i);
            if (reg_destino == vetor_UF[tnum][i].Fj || reg_destino == vetor_UF[tnum][i].Fk)
            { // encontrou war
                printf("Encontrou WAR na op que está na UF %s\n", converteNomeUF(indice_UF_atual));

                if (!strcmp(vetor_UF[tnum][i].Rj, "sim") && !strcmp(vetor_UF[tnum][i].Rk, "sim"))
                { //
                    printf("CLOCK LIDO = %d CLOCK ATUAL = %d \n", clock_instrucoes_lidas[i], clock_processador);
                    if (clock_processador > clock_instrucoes_lidas[i])
                    {
                        if (clock_instrucoes_lidas[i] != -1)
                        {
                            printf("Deu certo, falsezada\n");
                            return false;
                        }
                        printf("Motivo: clock_lido == -1\n");
                        return true;
                    }
                    printf("Motivo: clock lido < clock atual, pois %d (clock lido) é menor que %d (clock atual)\n", clock_instrucoes_lidas[i], clock_processador);
                    return true;
                }
                printf("Porque o strcmp deu errado!\n");
                return true;
            }
        }
    }
    return false;
}

void escritaResultados(resultadoExec lista_resultados[5], int instrucoes_escritas[5], int tnum)
{
    for (int i = 0; i < 5; i++)
    {
        if (lista_resultados[i].ciclo_termino == -1)
        {
            continue;
        }
        if (verificaWAR(vetor_UF[tnum][i].Fi, i))
        {
            printf("Tem WAR na instrucao que está na UF %s, no clock %d\n", converteNomeUF(i), clock_processador);
            continue;
        }
        instrucoes_escritas[i] = 1;
        banco_registradores[tnum][lista_resultados[i].reg_destino] = lista_resultados[i].resultado;
        flag_registradores[tnum][lista_resultados[i].reg_destino] = true;
        preencheStatusInstrucoes(ESCRITA, lista_resultados[i].PC);
    }
}

void limpaEstruturas(int instrucoes_escritas[5], resultadoExec lista_resultados[5], listaExecucao instrucoes_prontas[5],
                     instrucaoExecutando lista_instrucoes_executando[5], int tnum)
{
    for (int i = 0; i < 5; i++)
    {
        if (instrucoes_escritas[i] == 1)
        {
            limpaUF(i, tnum);
            status_dos_registradores[tnum][lista_resultados[i].reg_destino] = -1;
            limpaAuxiliares(i, lista_resultados, instrucoes_prontas, lista_instrucoes_executando);
            instrucoes_escritas[i] = -1;
        }
    }
}

void atualizaDependencias(int tnum)
{
    for (int i = 0; i < 5; i++)
    {
        preencheStatusRegistradorRj(i, vetor_UF[tnum][i].Fj);
        preencheStatusRegistradorRk(i, vetor_UF[tnum][i].Fk);
    }
}

int verificaRegistradorOcupado(int8_t registrador, int indice_UF, int tnum)
{
    for (int i = 0; i < 5; i++)
    {
        if (lista_emissoes[tnum].PC_emitido[indice_UF] > lista_emissoes[tnum].PC_emitido[i])
        {
            if (vetor_UF[tnum][i].Fi == registrador && vetor_UF[tnum][i].Fi != -1)
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
void preencheStatusRegistradorRk(int indice, int8_t registrador, int tnum)
{
    int registrador_ocupado = verificaRegistradorOcupado(registrador, indice);
    if (strcmp(vetor_UF[tnum][indice].busy, "nao"))
    {
        if (registrador_ocupado != -1)
        {
            vetor_UF[tnum][indice].Qk = registrador_ocupado;
            strcpy(vetor_UF[tnum][indice].Rk, "nao");
        }
        else
        {
            if (registrador == -1)
            {
                strcpy(vetor_UF[tnum][indice].Rk, "");
            }
            else
            {
                strcpy(vetor_UF[tnum][indice].Rk, "sim");
            }
        }
    }
}

void preencheStatusRegistradorRj(int indice, int8_t registrador, int tnum)
{
    int registrador_ocupado = verificaRegistradorOcupado(registrador, indice);
    if (strcmp(vetor_UF[tnum][indice].busy, "nao"))
    {
        if (registrador_ocupado != -1)
        {
            vetor_UF[tnum][indice].Qj = registrador_ocupado;
            strcpy(vetor_UF[tnum][indice].Rj, "nao");
        }
        else
        {
            if (registrador == -1)
            {
                strcpy(vetor_UF[tnum][indice].Rj, "");
            }
            else
            {
                strcpy(vetor_UF[tnum][indice].Rj, "sim");
            }
        }
    }
}

void limpaAuxiliares(int indice, resultadoExec lista_resultados[5], listaExecucao instrucoes_prontas[5],
                     instrucaoExecutando lista_instrucoes_executando[5])
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

void limpaUF(int indice_UF, int tnum)
{
    vetor_UF[tnum][indice_UF].FU = -1;
    vetor_UF[tnum][indice_UF].Fi = -1;
    vetor_UF[tnum][indice_UF].Fj = -1;
    vetor_UF[tnum][indice_UF].Fk = -1;
    vetor_UF[tnum][indice_UF].Qj = -1;
    vetor_UF[tnum][indice_UF].Qk = -1;
    strcpy(vetor_UF[tnum][indice_UF].Rj, "");
    strcpy(vetor_UF[tnum][indice_UF].Rk, "");
    strcpy(vetor_UF[tnum][indice_UF].busy, "nao");
    strcpy(vetor_UF[tnum][indice_UF].op, "");
}

int executaInstrucao(listaExecucao instrucao)
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

void preencheStatusInstrucoes(int etapa, int PC, int tnum)
{
    switch (etapa)
    {
    case EMISSAO:
        status_instrucoes[tnum].emissao[PC] = clock_processador;
        break;
    case LEITURA_OPERANDOS:
        status_instrucoes[tnum].leituraOP[PC] = clock_processador;
        break;
    case EXECUCAO:
        status_instrucoes[tnum].execucao[PC] = clock_processador;
        break;
    case ESCRITA:
        status_instrucoes[tnum].escrita[PC] = clock_processador;
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
    else if (opcode == AND || opcode == ANDI || opcode == OR || opcode == ORI || opcode == SLT || opcode == LI || opcode == MOVE)
        return Log;
    else if (opcode == MULT)
    {
        /*
        if(strcmp(vetor_UF[Mult1].busy, "sim") == 0){
            return Mult2;
        }
        */
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

void preencheStatusUF(int indice_UF, int instrucao, int opcode, int tnum)
{
    /* Preenche a linha do status das unidades funcionais */
    strcpy(vetor_UF[tnum][indice_UF].busy, "sim");
    strcpy(vetor_UF[tnum][indice_UF].op, opcodeParaOperacao(opcode));
    vetor_UF[tnum][indice_UF].Fi = recuperaCampo(instrucao, 5, 26);
    vetor_UF[tnum][indice_UF].Fj = recuperaCampo(instrucao, 5, 21);
    preencheStatusRegistradorRj(indice_UF, vetor_UF[tnum][indice_UF].Fj);

    if (verificaLi(opcode))
    { // se for Li
        vetor_UF[tnum][indice_UF].Fj = -1;
        strcpy(vetor_UF[tnum][indice_UF].Rj, "");
        vetor_UF[tnum][indice_UF].Fk = -1;
        strcpy(vetor_UF[tnum][indice_UF].Rk, "");
    }
    else if (verificaMove(opcode))
    { //move
        vetor_UF[tnum][indice_UF].Fk = -1;
        strcpy(vetor_UF[tnum][indice_UF].Rk, "");
    }
    else if (!verificaImediato(opcode))
    { //se for normal
        vetor_UF[tnum][indice_UF].Fk = recuperaCampo(instrucao, 5, 16);
        preencheStatusRegistradorRk(indice_UF, vetor_UF[tnum][indice_UF].Fk);
    }
    else
    { // se for imediato
        vetor_UF[tnum][indice_UF].Fk = -1;
        strcpy(vetor_UF[tnum][indice_UF].Rk, "");
    }
    preencheOcupacaoStatusUF(indice_UF);
}

void preencheOcupacaoStatusUF(int indice_UF, int tnum)
{
    int indice_ocupado;
    // Verifica se Fj está ocupado
    indice_ocupado = verificaRegistradorOcupado(vetor_UF[tnum][indice_UF].Fj, indice_UF);
    if (indice_ocupado > -1)
    { // Está ocupado
        vetor_UF[tnum][indice_UF].Qj = indice_ocupado;
        strcpy(vetor_UF[tnum][indice_UF].Rj, "nao");
    }
    else
    {
        strcpy(vetor_UF[tnum][indice_UF].Rj, "sim");
    }
    // Verifica se Fk está ocupado
    indice_ocupado = verificaRegistradorOcupado(vetor_UF[tnum][indice_UF].Fk, indice_UF);
    if (indice_ocupado > -1)
    { // Está ocupado
        vetor_UF[tnum][indice_UF].Qk = indice_ocupado;
        strcpy(vetor_UF[tnum][indice_UF].Rk, "nao");
    }
    else
    {
        strcpy(vetor_UF[tnum][indice_UF].Rk, "sim");
    }
}

bool verificaTermino(int PC, int tamanho_memoria, instrucaoDecodificada instrucao_decodificada, int tnum)
{
    for (int j = 0; j < 5; j++)
    {
        if (!strcmp(vetor_UF[tnum][j].busy, "sim") || (PC < tamanho_memoria) || (instrucao_decodificada.instrucao_completa != 0))
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

void initProcessador(int PC[2])
{
    clock_processador = 1;

    PC[0] = 0;
    PC[1] = 0;
    verifica_termino[0] = false;
    verifica_termino[1] = false;
    for (int thread_num = 0; thread_num < 2; thread_num++)
    {
        status_instrucoes[thread_num].instrucoes = thread_info.memoria; // recebe todas as instrucoes
        status_instrucoes[thread_num].emissao = (int *)calloc(sizeof(int), tamanho_memoria);
        status_instrucoes[thread_num].leituraOP = (int *)calloc(sizeof(int), tamanho_memoria);
        status_instrucoes[thread_num].execucao = (int *)calloc(sizeof(int), tamanho_memoria);
        status_instrucoes[thread_num].escrita = (int *)calloc(sizeof(int), tamanho_memoria);

        for (int i = 0; i < 32; i++)
        {
            banco_registradores[thread_num][i] = 0;
            status_dos_registradores[thread_num][i] = -1;
            flag_registradores[thread_num][i] = false;
        }
    }
}

void initNucleo(int *memoria, int tamanho_memoria, threadInfo thread_info, int PC)
{

    for (int i = 0; i < 5; i++)
    {
        limpaUF(i, thread_info.thread_num);

        thread_info.instrucoes_prontas[i].PC = -1;
        thread_info.instrucoes_prontas[i].clock_lido = -1;
        thread_info.instrucoes_prontas[i].destino = -1;
        thread_info.instrucoes_prontas[i].instrucao = -1;
        thread_info.instrucoes_prontas[i].ja_executou = false;
        thread_info.instrucoes_prontas[i].opcode = -1;
        thread_info.instrucoes_prontas[i].operando1 = -1;
        thread_info.instrucoes_prontas[i].operando2 = -1;

        thread_info.lista_instrucoes_executando[i].ciclos_restantes = -1;
        thread_info.lista_instrucoes_executando[i].clock_inicio = -1;

        thread_info.lista_resultados[i].ciclo_termino = -1;
        thread_info.lista_resultados[i].instrucao = -1;
        thread_info.lista_resultados[i].PC = -1;
        thread_info.lista_resultados[i].reg_destino = -1;
        thread_info.lista_resultados[i].resultado = -1;

        thread_info.instrucoes_escritas[i] = -1;

    }

    for(int i = 0;  i < 2; i++){
        for(int j = 0; j < 5; j++){
            lista_emissoes[i].instrucoes_emitidas[j] = -1;
            lista_emissoes[i].opcodes_emitidos[j] = -1;
            lista_emissoes[i].PC_emitido[j] = -1;  
            clock_instrucoes_lidas[j][i] = -1;
            clock_instrucoes_lidas[j][i] = -1;
        
        }
    }
    thread_info.PC = PC;
    thread_info.tamanho_memoria = tamanho_memoria;
    thread_info.memoria = memoria;

    // init estruturas auxiliares
    limpaBusca(&thread_info.instrucao_buscada);
    limpaDecodificacao(&thread_info.instrucao_decodificada);
}

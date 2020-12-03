#include "operacoes.c"
#include "saida.c"
#include "processador.h"
#include <math.h>

int clock;
statusInst status_instrucoes; // Status das Instrucoes
int banco_registradores[32]; // Banco de registradores
UnidadeFuncional vetor_UF[5]; // Unidades Funcionais
enum UF status_dos_registradores[32]; // Status dos Registradores

bool flag_registradores[32];


// Estrutura Auxiliar da Emissao
emissoes lista_emissoes;

void scoreboarding(int *memoria, int tamanho_memoria, char* nome_arq_saida, char linhas_instrucoes[][64]) {    
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

    initScoreboarding(memoria,tamanho_memoria, instrucoes_prontas, &acabou_de_executar, lista_instrucoes_executando, lista_resultados,instrucoes_escritas);
    int PC = 0;
    FILE* arq_saida;
    arq_saida = fopen(nome_arq_saida, "w");
    
    
    do {
        escritaResultados(lista_resultados, instrucoes_escritas);
        execucao(instrucoes_prontas, lista_instrucoes_executando, lista_resultados);
        leituraOperandos(instrucoes_prontas, PC - 1, memoria);

        
        if(PC < tamanho_memoria){
            if(instrucao_buscada.instrucao == 0){
                instrucao_buscada.instrucao = busca(memoria,PC); 
                instrucao_buscada.PC_busca = PC;
    
                decodificacao(instrucao_buscada,&instrucao_decodificada);
            }
        }

        if(instrucao_decodificada.instrucao_completa != 0){ // só emite se necessario
            if(emissao(instrucao_decodificada)){
                    limpaBusca(&instrucao_buscada);
                    limpaDecodificacao(&instrucao_decodificada);
                    PC+=1;
                }
                else {
            }
        }

        saida(arq_saida, PC, linhas_instrucoes);
        limpaEstruturas(instrucoes_escritas, lista_resultados, instrucoes_prontas, lista_instrucoes_executando);
        atualizaDependencias();
        //checa se alguma Unidade Funcional ainda tem processos a fazer
        acabou_de_executar = verificaTermino(PC, tamanho_memoria, instrucao_decodificada);

        clock += 1;        
    } while(!acabou_de_executar);

    saida(arq_saida, tamanho_memoria, linhas_instrucoes);
    fclose(arq_saida);
}

void limpaBusca(instrucaoBuscada *instrucao_buscada){
    instrucao_buscada->instrucao = 0;
    instrucao_buscada->PC_busca = -1;
}

void limpaDecodificacao(instrucaoDecodificada *instrucao_decodificada){
    instrucao_decodificada->opcode = 0;
    instrucao_decodificada->indice_UF = 0;
    instrucao_decodificada->instrucao_completa = 0;
    instrucao_decodificada->PC_busca = 0;
}

int busca(int *memoria,int PC){ // retorna a instrucao na posicao de pc
    int instrucao_buscada = memoria[PC];
    return instrucao_buscada;
}

void decodificacao(instrucaoBuscada instrucao_buscada,instrucaoDecodificada *instrucao_decodificada){ 
    instrucao_decodificada->opcode = recuperaCampo(instrucao_buscada.instrucao, 6, 32); //extrai o opcode da instruçao
    instrucao_decodificada->indice_UF = opcodeParaUF(instrucao_decodificada->opcode); //extrai a UF em que a instrucao deve ir
    instrucao_decodificada->instrucao_completa = instrucao_buscada.instrucao;
    instrucao_decodificada->PC_busca = instrucao_buscada.PC_busca;
}

bool verificaWAW(int destino_nova){
    for(int i  =  0; i < 5; i++){
        if(vetor_UF[i].Fi == destino_nova){
            return true;
        }
    }
    return false;
}

bool emissao(instrucaoDecodificada instrucao_decodificada){
    int destino_nova = recuperaCampo(instrucao_decodificada.instrucao_completa, 5, 26);
    if (strcmp(vetor_UF[instrucao_decodificada.indice_UF].busy, "sim") == 0) {
        return false;
    }if (verificaWAW(destino_nova)){
        return false;
    } else {
        preencheStatusUF(instrucao_decodificada.indice_UF, instrucao_decodificada.instrucao_completa, instrucao_decodificada.opcode);//
        lista_emissoes.opcodes_emitidos[instrucao_decodificada.indice_UF] = instrucao_decodificada.opcode;
        lista_emissoes.instrucoes_emitidas[instrucao_decodificada.indice_UF] = instrucao_decodificada.instrucao_completa;//
        lista_emissoes.PC_emitido[instrucao_decodificada.indice_UF] = instrucao_decodificada.PC_busca;
        preencheStatusInstrucoes(EMISSAO, instrucao_decodificada.PC_busca);
        status_dos_registradores[vetor_UF[instrucao_decodificada.indice_UF].Fi] =  instrucao_decodificada.indice_UF;
        return true;
    }
}

void leituraOperandos(listaExecucao instrucoes_prontas[5], int PC, int* memoria){
    for(int i = 0; i < 5; i++){
        //percorre as UF's e ve qual pode ser lida
        if(strcmp(vetor_UF[i].busy, "sim") == 0){ // só le operandos se tiver instrucao omitida na UF
            if(liberaLeitura(i) && lista_emissoes.opcodes_emitidos[i] != -1){
                //ambas sao true entao pode ler
                instrucoes_prontas[i].opcode = lista_emissoes.opcodes_emitidos[i];
                instrucoes_prontas[i].PC = lista_emissoes.PC_emitido[i];
                lista_emissoes.opcodes_emitidos[i] = -1; // Garante que essa instrucao nao sera lida novamente
                instrucoes_prontas[i].destino = vetor_UF[i].Fi;
                instrucoes_prontas[i].instrucao = memoria[PC];
                instrucoes_prontas[i].operando1 = vetor_UF[i].Fj;
                instrucoes_prontas[i].operando2 = vetor_UF[i].Fk;
                instrucoes_prontas[i].ja_executou = false;
                instrucoes_prontas[i].clock_lido = clock;
                preencheStatusInstrucoes(LEITURA_OPERANDOS, lista_emissoes.PC_emitido[i]);
            }
        } 
    }          
}

void execucao(listaExecucao instrucoes_prontas[5], instrucaoExecutando lista_instrucoes_executando[5], resultadoExec lista_resultados[5]){
    /* Testar D+ */
    executaUF(&instrucoes_prontas[Mult1], &lista_instrucoes_executando[Mult1], &lista_resultados[Mult1]);
    executaUF(&instrucoes_prontas[Mult2], &lista_instrucoes_executando[Mult2], &lista_resultados[Mult2]);
    executaUF(&instrucoes_prontas[Add], &lista_instrucoes_executando[Add], &lista_resultados[Add]);
    executaUF(&instrucoes_prontas[Div], &lista_instrucoes_executando[Div], &lista_resultados[Div]);
    executaUF(&instrucoes_prontas[Log], &lista_instrucoes_executando[Log], &lista_resultados[Log]);
}

void executaUF(listaExecucao *instrucao_pronta, instrucaoExecutando *instrucao_executando,resultadoExec *resultado){

    if(instrucao_pronta->PC != -1 && instrucao_pronta->ja_executou == false) {
        instrucao_executando->ciclos_restantes = opcodeParaNumCiclos(instrucao_pronta->opcode) - 1;
        instrucao_executando->clock_inicio = instrucao_pronta->clock_lido + 1;
        instrucao_pronta->ja_executou = true; // comecou a execucao
    }

    if(instrucao_executando->ciclos_restantes == 0){
        resultado->resultado = executaInstrucao(*instrucao_pronta);
        resultado->ciclo_termino = clock;
        resultado->reg_destino = instrucao_pronta->destino;
        resultado->PC = instrucao_pronta->PC;
        preencheStatusInstrucoes(EXECUCAO, instrucao_pronta->PC);

        instrucao_executando->ciclos_restantes -= 1;
    }else if(instrucao_executando->ciclos_restantes > 0){
        instrucao_executando->ciclos_restantes -= 1;
    }
}


bool liberaLeitura(int i){
    if((strcmp(vetor_UF[i].Rj, "nao")) && (strcmp(vetor_UF[i].Rk, "nao"))){
         return true;
    }
    return false;
}

bool verificaWAR(int reg_destino, int indice_UF){
    for(int i = 0; i < 5; i++){
        if(i != indice_UF){
            if(lista_emissoes.PC_emitido[indice_UF] > lista_emissoes.PC_emitido[i]){
                if(reg_destino == vetor_UF[i].Fj || reg_destino == vetor_UF[i].Fk){
                    printf("%d destino", reg_destino);
                    if(!strcmp(vetor_UF[i].Rj, "sim") && !strcmp(vetor_UF[i].Rk, "sim")){
                        return false;
                    }
                    return true;
                }
            }
        }
    }
    return false;
}


void escritaResultados(resultadoExec lista_resultados[5], int instrucoes_escritas[5]){
    for (int i = 0; i < 5; i++) {
        if(lista_resultados[i].ciclo_termino == -1){
            continue;
        }
        printf("vetor_UF %d\n", vetor_UF[i].Fi);
        if(verificaWAR(vetor_UF[i].Fi, i)){
            printf("sai do WAR\n");
            continue;
        }
        instrucoes_escritas[i] = 1;
        banco_registradores[lista_resultados[i].reg_destino] = lista_resultados[i].resultado;
        flag_registradores[lista_resultados[i].reg_destino] = true;
        preencheStatusInstrucoes(ESCRITA, lista_resultados[i].PC);
    }
}

void limpaEstruturas(int instrucoes_escritas[5], resultadoExec lista_resultados[5], listaExecucao instrucoes_prontas[5],
    instrucaoExecutando lista_instrucoes_executando[5]){
    for (int i = 0; i < 5; i++) {
        if(instrucoes_escritas[i] == 1){
            limpaUF(i);
            status_dos_registradores[lista_resultados[i].reg_destino] = -1;
            limpaAuxiliares(i, lista_resultados, instrucoes_prontas, lista_instrucoes_executando);
            instrucoes_escritas[i] = -1;
        }
    }
}

void atualizaDependencias(){
    for(int i = 0; i < 5; i++){
        preencheStatusRegistradorRj(i, vetor_UF[i].Fj);
        preencheStatusRegistradorRk(i, vetor_UF[i].Fk);
    }
}

int verificaRegistradorOcupado(int8_t registrador, int indice_UF){
    for(int i = 0; i < 5; i++){
        if(lista_emissoes.PC_emitido[indice_UF] > lista_emissoes.PC_emitido[i]){
            if(vetor_UF[i].Fi == registrador && vetor_UF[i].Fi != -1){
                if(i == indice_UF){
                    continue; //indice igual o da propria UF -> registrador destino eh a fonte 
                }
                return i;
            }
        }
    }
    return -1;
}
void preencheStatusRegistradorRk(int indice, int8_t registrador){
    int registrador_ocupado = verificaRegistradorOcupado(registrador, indice);
    if(strcmp(vetor_UF[indice].busy, "nao")){
        if(registrador_ocupado != -1){
            vetor_UF[indice].Qk = registrador_ocupado;
            strcpy(vetor_UF[indice].Rk, "nao");
        }else{
            if(registrador == -1){
                strcpy(vetor_UF[indice].Rk, "");
            }
            else{
                strcpy(vetor_UF[indice].Rk, "sim");
            }
        }
    }
}

void preencheStatusRegistradorRj(int indice, int8_t registrador){
    int registrador_ocupado = verificaRegistradorOcupado(registrador, indice);
    if(strcmp(vetor_UF[indice].busy, "nao")){
        if(registrador_ocupado != -1){
            vetor_UF[indice].Qj = registrador_ocupado;
            strcpy(vetor_UF[indice].Rj, "nao");
        }else{
            if(registrador == -1){
                strcpy(vetor_UF[indice].Rj, "");
            }
            else{
                strcpy(vetor_UF[indice].Rj, "sim");
            }
        }
    }
}

void limpaAuxiliares(int indice,resultadoExec lista_resultados[5], listaExecucao instrucoes_prontas[5],
    instrucaoExecutando lista_instrucoes_executando[5]){
       
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

        lista_instrucoes_executando[indice].ciclos_restantes = -1;
        lista_instrucoes_executando[indice].clock_inicio = -1;
}

void limpaUF(int indice_UF){
    vetor_UF[indice_UF].FU = -1;
    vetor_UF[indice_UF].Fi = -1;
    vetor_UF[indice_UF].Fj = -1;
    vetor_UF[indice_UF].Fk = -1;
    vetor_UF[indice_UF].Qj = -1; 
    vetor_UF[indice_UF].Qk = -1; 
    strcpy(vetor_UF[indice_UF].Rj, "");
    strcpy(vetor_UF[indice_UF].Rk, "");
    strcpy(vetor_UF[indice_UF].busy,"nao");
    strcpy(vetor_UF[indice_UF].op, "");
}

int executaInstrucao(listaExecucao instrucao){
    int resultado;
    switch(instrucao.opcode){
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
            resultado = or(banco_registradores[instrucao.operando1], banco_registradores[instrucao.operando2]);
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
            if(resultado == -1){
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

void preencheStatusInstrucoes(int etapa, int PC){
    switch(etapa){
        case EMISSAO:
            status_instrucoes.emissao[PC] = clock;
            break;
        case LEITURA_OPERANDOS:
            status_instrucoes.leituraOP[PC] = clock;
            break;
        case EXECUCAO:
            status_instrucoes.execucao[PC] = clock;
            break;
        case ESCRITA:
            status_instrucoes.escrita[PC] = clock;
            break;
    }
}

// Retorna os campos da instrucao (Ja transformada em 32bits)
int recuperaCampo(int instruct, int size, int starting_bit){
    int32_t mask_2 = pow(2, size) - 1; // corresponde ao inteiro de 32 bits cujos seis bits mais significativos eh 1
    int mask = 0;
    for(int i =0; i < 32; i++){
        if(i < size){
            mask = mask << 1 | 1;
        }
        if(i >= size && i < starting_bit){
            mask = mask << 1;
        }
    }
    int operator = mask & instruct;
    for(int i = starting_bit; i > size; i--)
        operator = operator >> 1 | 0;
    operator = operator & mask_2; 
    return operator;
}

int opcodeParaUF(int opcode){
    if (opcode == ADD || opcode == ADDI || opcode == SUB) return Add;
    else if(opcode == DIV) return Div;
    else if(opcode == AND || opcode == ANDI || opcode == OR || opcode == ORI || opcode == SLT || opcode == LI || opcode == MOVE) return Log;
    else if(opcode == MULT){
        if(strcmp(vetor_UF[Mult1].busy, "sim") == 0){
            return Mult2;
        }
        return Mult1;
    }
    return -1;
}

char* opcodeParaOperacao(int opcode){
    switch(opcode){
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

bool verificaImediato(int opcode){
    if(opcode == ADDI || opcode == ORI || opcode == LI || opcode == ANDI){
        return true;
    }
    return false;
}

bool verificaMove(int opcode){
    if(opcode == MOVE){
        return true;
    }
    return false;
}

bool verificaLi(int opcode){
    if(opcode == LI){
        return true;
    }
    return false;
}


void preencheStatusUF(int indice_UF, int instrucao, int opcode) {
    /* Preenche a linha do status das unidades funcionais */
    strcpy(vetor_UF[indice_UF].busy, "sim");
    strcpy(vetor_UF[indice_UF].op, opcodeParaOperacao(opcode));
    vetor_UF[indice_UF].Fi = recuperaCampo(instrucao, 5, 26);
    vetor_UF[indice_UF].Fj = recuperaCampo(instrucao, 5, 21);
    preencheStatusRegistradorRj(indice_UF, vetor_UF[indice_UF].Fj);
    
    if(verificaLi(opcode)){ // se for Li
        vetor_UF[indice_UF].Fj = -1;
        strcpy(vetor_UF[indice_UF].Rj, "");
        vetor_UF[indice_UF].Fk = -1;
        strcpy(vetor_UF[indice_UF].Rk, "");
    }else if(verificaMove(opcode)){ //move
        vetor_UF[indice_UF].Fk = -1;
        strcpy(vetor_UF[indice_UF].Rk, "");
    }else if(!verificaImediato(opcode)){ //se for normal
        vetor_UF[indice_UF].Fk = recuperaCampo(instrucao, 5, 16);
        preencheStatusRegistradorRk(indice_UF, vetor_UF[indice_UF].Fk);
    }else{ // se for imediato 
        vetor_UF[indice_UF].Fk = -1;
        strcpy(vetor_UF[indice_UF].Rk, "");
    }
    preencheOcupacaoStatusUF(indice_UF);
}

void preencheOcupacaoStatusUF(int indice_UF){
    int indice_ocupado;
    // Verifica se Fj está ocupado
    indice_ocupado = verificaRegistradorOcupado(vetor_UF[indice_UF].Fj, indice_UF);
    if(indice_ocupado > -1){ // Está ocupado
        vetor_UF[indice_UF].Qj = indice_ocupado;
        strcpy(vetor_UF[indice_UF].Rj, "nao");
    } else {
        strcpy(vetor_UF[indice_UF].Rj, "sim");
    }
    // Verifica se Fk está ocupado
    indice_ocupado = verificaRegistradorOcupado(vetor_UF[indice_UF].Fk, indice_UF);
    if(indice_ocupado > -1){ // Está ocupado
        vetor_UF[indice_UF].Qk = indice_ocupado;
        strcpy(vetor_UF[indice_UF].Rk, "nao");
    } else {
        strcpy(vetor_UF[indice_UF].Rk, "sim");
    }
}

bool verificaTermino(int PC, int tamanho_memoria, instrucaoDecodificada instrucao_decodificada){
    for(int j = 0; j<5; j++){
        if(!strcmp(vetor_UF[j].busy, "sim") || (PC < tamanho_memoria) || (instrucao_decodificada.instrucao_completa != 0)){
            return false;
        } 
    }
    return true;
}

int opcodeParaNumCiclos(int opcode){
    if(opcode == ADD) return num_ciclos_ADD;
    else if(opcode == ADDI) return num_ciclos_ADDI;
    else if(opcode == AND) return num_ciclos_AND;
    else if(opcode == ANDI) return num_ciclos_ANDI;
    else if(opcode == OR) return num_ciclos_OR;
    else if(opcode == ORI) return num_ciclos_ORI;
    else if(opcode == SLT) return num_ciclos_SLT;
    else if(opcode == SUB) return num_ciclos_SUB;
    else if(opcode == MULT) return num_ciclos_MULT;
    else if(opcode == DIV) return num_ciclos_DIV;
    else if(opcode == LI) return num_ciclos_LI;
    else if(opcode == MOVE) return num_ciclos_MOVE;
}

void initScoreboarding(int* memoria, int tamanho_memoria, listaExecucao instrucoes_prontas[5], bool *acabou_de_executar, instrucaoExecutando lista_instrucoes_executando[5],resultadoExec lista_resultados[5], int instrucoes_escritas[5]){
    status_instrucoes.instrucoes = memoria;         // recebe todas as instrucoes
    status_instrucoes.emissao = (int *) calloc(sizeof(int), tamanho_memoria);
    status_instrucoes.leituraOP = (int *) calloc(sizeof(int),tamanho_memoria);
    status_instrucoes.execucao = (int *) calloc(sizeof(int),tamanho_memoria);
    status_instrucoes.escrita = (int *) calloc(sizeof(int),tamanho_memoria);
    
    clock = 1;
    *acabou_de_executar = false;

    for(int i = 0; i < 5; i++){
        limpaUF(i);

        instrucoes_prontas[i].PC = -1;
        instrucoes_prontas[i].clock_lido = -1;
        instrucoes_prontas[i].destino =  -1;
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

        lista_emissoes.instrucoes_emitidas[i] = -1;
        lista_emissoes.opcodes_emitidos[i] = -1;
        lista_emissoes.PC_emitido[i] = -1;

        instrucoes_escritas[i] = -1;
    }

    for(int i = 0; i < 32; i++){
        banco_registradores[i] = 0;
        status_dos_registradores[i] = -1;
        flag_registradores[i] = false;
    }
}


#include "saida.h"

extern int clock;
extern statusInst status_instrucoes; 
extern UnidadeFuncional vetor_UF[5];
extern enum UF status_dos_registradores[32];
extern int banco_registradores[32];
extern bool flag_registradores[32];



void saida(FILE* arq_saida, int PC, char** instrucoes, int* memoria, int tam_mem){
    converteInstrucao(instrucoes, PC, memoria, tam_mem);
    fseek(arq_saida, 0, SEEK_END); //aponta pro final do arquivo
    escrever_saida(arq_saida, PC, instrucoes);
}

void escrever_saida(FILE* arq, int PC, char* instrucoes){
    char nomes_UF[32][16];
    converteStatusRegistradores(nomes_UF);
    
    fprintf(arq, "--------------------- ciclo %d ----------------------- \n \n", clock); //escreveu primeira linha
    fprintf(arq, "1) status das instrucoes \n \n");
    fprintf(arq, "\t\t\t\temissao\t|\tleitura dos operandos\t|\texecucao\t|\tescrita dos resultados\t\n");   
    for(int i = 0; i < PC; i++){
        fprintf(arq, "%s\t\t\t", linhas_instrucoes[i]);
        if(status_instrucoes.emissao[i] != 0){
            fprintf(arq, "%d\t\t\t", status_instrucoes.emissao[i]);
        }
        if(status_instrucoes.leituraOP[i] != 0){ 
            fprintf(arq, "%d\t\t\t\t", status_instrucoes.leituraOP[i]);
        }
        if(status_instrucoes.execucao[i] != 0){
            fprintf(arq, "%d\t\t\t", status_instrucoes.execucao[i]);
        }
        if(status_instrucoes.escrita[i] != 0){
            fprintf(arq, "%d", status_instrucoes.escrita[i]);
        }
        fprintf(arq, "\n");
    }
    fprintf(arq, "\n2) status das unidades funcionais\n\n");
    fprintf(arq, "\nuf\t|busy\t|op\t|fi\t|fj\t|fk\t|qj\t|qk\t|rj\t|rk\n");
    fprintf(arq, "mult1\t|%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", vetor_UF[0].busy,
        vetor_UF[0].op, converteNomeRegistrador(vetor_UF[0].Fi), converteNomeRegistrador(vetor_UF[0].Fj), 
        converteNomeRegistrador(vetor_UF[0].Fk), converteNomeUF(vetor_UF[0].Qj), converteNomeUF(vetor_UF[0].Qk), 
        vetor_UF[0].Rj, vetor_UF[0].Rk);
    fprintf(arq, "mult2\t|%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", vetor_UF[1].busy,
        vetor_UF[1].op, converteNomeRegistrador(vetor_UF[1].Fi), converteNomeRegistrador(vetor_UF[1].Fj),
        converteNomeRegistrador(vetor_UF[1].Fk), converteNomeUF(vetor_UF[1].Qj),
        converteNomeUF(vetor_UF[1].Qk), vetor_UF[1].Rj, vetor_UF[1].Rk);
    fprintf(arq, "add\t|%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", vetor_UF[2].busy,
        vetor_UF[2].op, converteNomeRegistrador(vetor_UF[2].Fi), converteNomeRegistrador(vetor_UF[2].Fj),
        converteNomeRegistrador(vetor_UF[2].Fk), converteNomeUF(vetor_UF[2].Qj),
        converteNomeUF(vetor_UF[2].Qk), vetor_UF[2].Rj, vetor_UF[2].Rk);
    fprintf(arq, "div\t|%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", vetor_UF[3].busy,
        vetor_UF[3].op, converteNomeRegistrador(vetor_UF[3].Fi), converteNomeRegistrador(vetor_UF[3].Fj), 
        converteNomeRegistrador(vetor_UF[3].Fk), converteNomeUF(vetor_UF[3].Qj),
        converteNomeUF(vetor_UF[3].Qk), vetor_UF[3].Rj, vetor_UF[3].Rk);
    fprintf(arq, "log\t|%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", vetor_UF[4].busy,
        vetor_UF[4].op, converteNomeRegistrador(vetor_UF[4].Fi), converteNomeRegistrador(vetor_UF[4].Fj), 
        converteNomeRegistrador(vetor_UF[4].Fk), converteNomeUF(vetor_UF[4].Qj),
        converteNomeUF(vetor_UF[4].Qk), vetor_UF[4].Rj, vetor_UF[4].Rk);
    fprintf(arq, "\n");
    fprintf(arq, "3) status dos registradores \n \n");
    fprintf(arq, "\t$t0\t|$t1\t|$t2\t|$t3\t|$t4\t|$t5\t|$t6\t|$t7\t|$s0\t|$s1\t|$s2\t|$s3\t|$s4\t|$s5\t|$s6\t|$s7\t|$t8\t|$t9\t\n");
    fprintf(arq, "uf\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
    nomes_UF[8], nomes_UF[9], nomes_UF[10], nomes_UF[11],nomes_UF[12],nomes_UF[13], 
    nomes_UF[14], nomes_UF[15], nomes_UF[16], nomes_UF[17], nomes_UF[18], 
    nomes_UF[19],nomes_UF[20],nomes_UF[21], nomes_UF[22], nomes_UF[23], 
    nomes_UF[24], nomes_UF[25]);
    fprintf(arq, "\n");
    fprintf(arq, "4) banco de registradores \n \n");
    fprintf(arq, "$t0\t|$t1\t|$t2\t|$t3\t|$t4\t|$t5\t|$t6\t|$t7\t|$s0\t|$s1\t|$s2\t|$s3\t|$s4\t|$s5\t|$s6\t|$s7\t|$t8\t|$t9\t\n");
    for(int i = 8; i < 26; i++){
        if(flag_registradores[i]){
            fprintf(arq, "%d\t", banco_registradores[i]);
        }else{
            fprintf(arq, "\t");
        }
    }
    fprintf(arq, "\n\n");
    
}

char* converteNomeUF(int codigo_UF){
    switch(codigo_UF){
        case(0):
            return "MULT1";
        case(1):
            return "MULT2";
        case(2):
            return "ADD";
        case(3):
            return "DIV";
        case(4):
            return "LOG";
        default:
            return "";
    }
}

char* converteNomeRegistrador(int cod_reg){
    switch(cod_reg){
        case(0):
            return "$zero";
        case(1):
            return "$at";
        case(2):
            return "$v0";
        case(3):
            return "$v1";
        case(4):
            return "$a0";
        case(5):
            return "$a1";
        case(6):
            return "$a2";
        case(7):
            return "$a3";
        case(8):
            return "$t0";
        case(9):
            return "$t1";
        case(10):
            return "$t2";
        case(11):
            return "$t3";
        case(12):
            return "$t4";
        case(13):
            return "$t5";
        case(14):
            return "$t6";
        case(15):
            return "$t7";
        case(16):
            return "$s0";
        case(17):
            return "$s1";
        case(18):
            return "$s2";
        case(19):
            return "$s3";
        case(20):
            return "$s4";
        case(21):
            return "$s5";
        case(22):
            return "$s6";
        case(23):
            return "$s7";
        case(24):
            return "$t8";
        case(25):
            return "$t9";
        case(26):
            return "$k0";
        case(27):
            return "$k1";
        case(28):
            return "$gp";
        case(29):
            return "$sp";
        case(30):
            return "$fp";
        case(31):
            return "$ra";
        default:
            return "";
    }
}

void converteStatusRegistradores(char nomes_UF[32][16]){
    for(int i = 0; i < 32; i++){
        if(status_dos_registradores[i] == -1){
            strcpy(nomes_UF[i], "");
        } else {
            strcpy(nomes_UF[i], converteNomeUF(status_dos_registradores[i]));
        }
    }
}


void converteInstrucao(char **instrucoes, int PC, int *memoria, int tam_mem){
    for(int i = 0; i < tam_mem; i++){
        if(i < PC){
            instrucoes[i] = converter(memoria[i]);
        }else{
            break;
        }
    }
}

char* converter(int instrucao){
    //aqui temos o int da instrucao
    //hora de converter :)

    int opcode = recuperaCampo(instrucao, 6, 32);
    int Fi = recuperaCampo(instrucao, 5, 26);
    int Fj = recuperaCampo(instrucao, 5, 21);
    int Fk = recuperaCampo(instrucao, 5, 16);

    //separamos a instrucao em inteiros agora bora colocar como string

    char *str_opcode, *str_fi, *str_fj, *str_fk;
    str_opcode = opcodeParaString(opcode);
    str_fi = converteNomeRegistrador(Fi);
    str_fj = converteNomeRegistrador(Fj);
    str_fk = converteNomeRegistrador(Fk);

    char str_instrucao[50];
    strcat(str_instrucao, str_opcode);
    strcat(str_instrucao, " ");
    strcat(str_instrucao, str_fi);
    strcat(str_instrucao, " ");
    strcat(str_instrucao, str_fj);
    strcat(str_instrucao, " ");
    strcat(str_instrucao, str_fk);

    return str_instrucao;
}

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


char* opcodeParaString(int opcode){
    switch(opcode){
        case(ADD):
            return "add";
        case(ADDI):
            return "addi";
        case(AND):
            return "and";
        case(ANDI):
            return "andi";
        case(OR):
            return "or";
        case(ORI):
            return "ori";
        case(SLT):
            return "slt";
        case(SUB):
            return "sub";
        case(MULT):
            return "mult";
        case(DIV):
            return "div";
        case(LI):
            return "li";
        case(MOVE):
            return "move";
            
}

#include "operacoes.h"
int add(int x, int y){
    return x + y;
}

int addi(int x, int16_t y){
    return x + y;
}

int and(int x, int y){
    return x & y;
}

int andi(int x, int16_t y){
    return x & y;
}

int or(int x, int y){
    return x | y;
}

int ori(int x, int16_t y){
    return x | y;
}

int slt(int x, int y){
    return x < y;
}

int sub(int x, int y){
    return x - y;
}

int mult(int x, int y){
    return x * y;
}

int divi(int x, int y){
    if(y == 0){
        return -1;
    }
    return (int) (x / y);
}

int li(int16_t y){
    return ori(0, y);
}

int move(int16_t x){
    return x;
}
#include <stdio.h> 
#include <stdint.h> //biblioteca usada para chamar unsigned int 32 bits(uint32_t)
#include <math.h> 

void decodificar_telemetria(unsigned char *ptr) {
    // Combina os bytes em um inteiro de 32 bits (considerando little-endian)
    uint32_t valor = (ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0]; 

    // Extrai o bit de sinal
    int s = (valor >> 31) & 0x1; //testandogit

    // Extrai o expoente (8 bits)
    int E = (valor >> 23) & 0xFF;

    // Extrai a mantissa (23 bits)
    uint32_t M_bits = valor & 0x7FFFFF;

    // Converte a mantissa para valor fracionário
    double M = 0.0;
    for (int i = 0; i < 23; i++) {
        if (M_bits & (1 << (22 - i))) {
            M += pow(2, - (i + 1));
        }
    }

    // Aplica a fórmula IEEE-754
    double V;
    if (E == 255) {
        if (M == 0) {
            V = (s == 0) ? INFINITY : -INFINITY; // infinito
        } else {
            V = NAN; // NaN
        }
    } else if (E == 0) {
        // Número subnormal
        V = pow(-1, s) * M * pow(2, -126); //regra do IEEE-754
    } else { //caso E não seja nem 0 nem 255
        // Número normalizado
        V = pow(-1, s) * (1 + M) * pow(2, E - 127);
    }

    printf("Sinal: %d\n", s);
    printf("Expoente (Bias corrigido): %d\n", E);
    printf("Mantissa (hex): 0x%06X\n", M_bits);
    printf("Valor final: %f\n", (float)V);
}

int main() {
    // Exemplo de buffer com bytes, numeros finais de matricula: 1 e 7 
    unsigned char buffer_memoria[4] = {0x01, 0x07, 0x20, 0xC1}; 

    decodificar_telemetria(buffer_memoria);

    return 0; //teste
}
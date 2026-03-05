#include <stdio.h> 
#include <stdint.h> //biblioteca usada para chamar unsigned int 32 bits(uint32_t)
#include <math.h> 

void decodificar_telemetria(unsigned char *ptr) {
    // Combina os bytes em um inteiro de 32 bits (considerando big-endian)
    uint32_t valor = (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3]; 

    // Extrai o bit de sinal
    int s = (valor >> 31) & 0x1;

    // Extrai o expoente (8 bits)
    int E = (valor >> 23) & 0xFF;

    // Extrai a mantissa (23 bits)
    uint32_t M_bits = valor & 0x7FFFFF;

    // Converte a mantissa para valor fracionário
    double M = 0.0;
    for (int i = 0; i < 23; i++) {
        if (M_bits & (1 << (22 - i))) {
            M += pow(2, -(i + 1));
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
    printf("Expoente: %d\n", E);
    printf("Mantissa (hex): 0x%06X\n", M_bits);
    printf("Valor final: %f\n", (float)V);
}

int main() {
    // Exemplo de buffer com bytes recebidos
    unsigned char buffer[4] = {0x3E, 0x20, 0x00, 0x00}; // valor que representa 50.0

    decodificar_telemetria(buffer);

    return 0;
}
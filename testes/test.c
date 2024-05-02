#include <stdio.h>
#include <stdlib.h>

int power(int b, int e) {

    int result = 1;
    for (int i=0; i<e; i++)
        result *= b;

    return result;
}

void c(int i, int s, int *v) {

    int aux = power(2, s);
    int highest_potency = 0;
    //determinando o tamanho mínimo do cluster que contém esse id
    while (i > power(2, highest_potency))
        highest_potency++;

    int pivo;
    int reduced_id = i;
    int potencias = power(2, highest_potency);

    //reduzindo o i para o equivalente da posição no cluster de menor id possível
    while (aux <= reduced_id) {
        if (reduced_id >= potencias) {
            reduced_id -= potencias;
        }
        potencias = potencias/2;
    }

    //tendo sua posição na árvore simplificada, usa o simétrico em relação cluster s para encontrar o primeiro testador
    if (reduced_id < aux/2)
        pivo = i+aux/2;
    else
        pivo = i-aux/2;

    //aqui, damos o nome de pivô para o primeiro candidato a testador
    //inicia-se nele, e percorre todos os id dentro do cluster s dele

    int j = pivo % (aux/2);
    int base = pivo - j;
    int count = 0;

    while (count < aux/2) {

        v[count] = base+j;

        j = (j+1) % (aux/2);
        count++;
    }
    v[count] = -2;      //indica fim da fila

}

int main() {

    int p=5;
    int *v = (int *)malloc(sizeof(int)*power(2, p));

    for (int i=0; i<power(2, p); i++) {

        printf("%d: (", i);
        for (int j=1; j<=p; j++) {
            printf("[");
            c(i, j, v);
            int count = 0;
            while (v[count] != -2) {
                printf("%d, ", v[count]);
                count++;
            }

            printf("]");
        }
        printf(")\n");

    }

}

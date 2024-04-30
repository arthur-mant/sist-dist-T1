#include <stdio.h>

int power(int b, int e) {

    int result = 1;
    for (int i=0; i<e; i++)
        result *= b;

    return result;
}

int c(int i, int s) {

    if (s == 0)
        return i;

    int aux = power(2, s);
    int highest_potency = 0;
    while (i > power(2, highest_potency))
        highest_potency++;

    int reduced_id = i;
    int potencias = power(2, highest_potency);

    //reduzindo
    while (aux <= reduced_id) {
        if (reduced_id >= potencias) {
            reduced_id -= potencias;
        }
        potencias = potencias/2;
    }

    if (reduced_id < aux/2)
        return i+aux/2;
    else
        return i-aux/2;

}

int main() {

    int p=3;
    for (int i=0; i<power(2, p); i++) {

        printf("%d: (", i);
        for (int j=0; j<p; j++)
            printf("%d, ", c(i, j+1));
        printf(")\n");

    }

    printf("11, 2: %d\n", c(11, 2));

}

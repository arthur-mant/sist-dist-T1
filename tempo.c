/* Autor: Elias P. Duarte Jr.
   Data da Ãšltima ModificaÃ§Ã£o: 11/abril/2024
   DescriÃ§Ã£o: Nosso primeiro programa de simulaÃ§Ã£o da disciplina Sistemas DistribuÃ­dos.
     Vamos simular N processos, cada um conta o â€œtempoâ€ independentemente
    Um exemplo simples e significativo para captar o â€œespÃ­ritoâ€ da simulaÃ§Ã£o */

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"

// Vamos definir os EVENTOS 
#define test 1
#define fault 2
#define recovery 3

// Vamos definir o descritor do processo

typedef struct{ 
    int id;            // identificador de facility do SMPL
    int *state;
    int **old_state;
   // outras variÃ¡veis locais dos processos sÃ£o declaradas aqui!
} TipoProcesso;

TipoProcesso *processo;

//funções auxiliares

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

int main (int argc, char *argv[]) {
   
   static int N,   // nÃºmero de processos
            token,  // indica o processo que estÃ¡ executando
            event, r, i,
            MaxTempoSimulac = 250;

   static char fa_name[5];

   if (argc != 2) {
       puts("Uso correto: tempo <nÃºmero de processos>");
       exit(1);
    }

   N = atoi(argv[1]);

   smpl(0, "Um Exemplo de SimulaÃ§Ã£o");
   reset();
   stream(1);

   // inicializar processos

   processo = (TipoProcesso *) malloc(sizeof(TipoProcesso)*N);

   for (i=0; i<N; i++) {
      memset(fa_name, '\0', 5);
      sprintf(fa_name, "%d", i);
      processo[i].id = facility(fa_name,1);

      processo[i].state = (int *) malloc(sizeof(int)*N);
      for (int j = 0; j<N; j++) {
        processo[i].state[j] = -1;
      }
      processo[i].state[i] = 0;

      processo[i].old_state = (int **) malloc(sizeof(int *)*N);
      for (int j = 0; j<N; j++) {
        processo[i].old_state[j] = (int *) malloc(sizeof(int)*N);
        for (int k = 0; k < N; k++)
            processo[i].old_state[j][k] = -1;
        processo[i].old_state[j][i] = 0;
        
      }

   }
    // vamos fazer o escalonamento inicial de eventos

    // nossos processos vÃ£o executar testes em intervalos de testes
    // o intervalo de testes vai ser de 30 unidades de tempo
    // a simulaÃ§Ã£o comeÃ§a no tempo 0 (zero) e vamos escalonar o primeiro teste de todos os    
    //       processos para o tempo 30.0

    for (i=0; i<N; i++) {
       schedule(test, 30.0, i); 
    }
    schedule(fault, 31.0, 0);
    schedule(fault, 62.0, 1);
    schedule(fault, 93.0, 2);
    schedule(fault, 124.0, 3);
    schedule(recovery, 151.0, 0);

    // agora vem o loop principal do simulador

    puts("===============================================================");
    puts("           Sistemas DistribuÃ­- Prof. Elias");
    puts("          LOG do Trabalho PrÃ¡tico 1");
    printf("   Este programa foi executado para: N=%d processos.\n", N); 
    printf("           Tempo Total de SimulaÃ§Ã£o = %d\n", MaxTempoSimulac);
    puts("===============================================================");

    int max_s = 0;
    while (N > power(2, max_s))
        max_s++;

    printf("max_s para N = %d eh %d\n", N, max_s);

    int count = 0;
    int st;
    int *saida = (int *)malloc(sizeof(int)*N);
    int *testar = (int *)malloc(sizeof(int)*N);

    while(time() < MaxTempoSimulac) {
         cause(&event, &token);
         switch(event) {
           case test: 
                if (status(processo[token].id) !=0) break; // se o processo estÃ¡ falho, nÃ£o testa!

                //printf("start %d\n", token);
                for (int i=0; i<N; i++) {
                    if (i != token) {
                        for (int s=1; s<=max_s; s++) {
                            c(i, s, saida);
                            count = 0;

                            //printf("P%d: c(%d, %d) = [", token, i, s);
                            //encontrando testador para o processo i
                            while ((saida[count] != -2) && (processo[token].state[saida[count]] != 0)) {
                                //printf("%d, ", saida[count]);
                                count++;
                            }
                            //if (processo[token].state[saida[count]] == 0)
                            //    printf("%d", saida[count]);
                            //printf("]\n");
                            if  (token == saida[count]) {
                                //testa processo i
                                st = status(processo[i].id);

                                printf("O processo %d testou o processo %d ", token, i);
                                if (st == 0) {
                                    //processo i verifica quais informações são novas e passa para o processo token
                                    printf("correto e adquire [");
                                    for (int j=0; j<N; j++) {
                                        if ((processo[i].state[j] != processo[i].old_state[token][j]) &&
                                            (j != token) && (j != i)) {
                                            processo[token].state[j] = processo[i].state[j];
                                            printf("%d, ", j);
                                        }
                                        //atualiza processo[i].old_state[token]
                                        processo[i].old_state[token][j] = processo[i].state[j];
                                    }

                                    printf("] ");
                                }
                                else if (st == 1)
                                    printf("falho ");
                                else
                                    printf("desconhecido ");

                                printf("no tempo %4.1f\n", time());
                                processo[token].state[i] = st;
                            }
                        }
                    }
                }
                schedule(test, 30.0, token);
                break;
           case fault:
                r = request(processo[token].id, token, 0);
                printf("Socooorro!!! Sou o processo %d  e estou falhando no tempo %4.1f\n", token, time());
                break;
           case recovery:
                release(processo[token].id, token);
                printf("Viva!!! Sou o processo %d e acabo de recuperar no tempo %4.1f\n", token, time());
                schedule(test, 1.0, token);
                break;
          } // switch
    } // while
} // tempo.c

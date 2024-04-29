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
   // outras variÃ¡veis locais dos processos sÃ£o declaradas aqui!
} TipoProcesso;

TipoProcesso *processo;

int main (int argc, char *argv[]) {
   
   static int N,   // nÃºmero de processos
            token,  // indica o processo que estÃ¡ executando
            event, r, i,
            MaxTempoSimulac = 150;

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
   }
    // vamos fazer o escalonamento inicial de eventos

    // nossos processos vÃ£o executar testes em intervalos de testes
    // o intervalo de testes vai ser de 30 unidades de tempo
    // a simulaÃ§Ã£o comeÃ§a no tempo 0 (zero) e vamos escalonar o primeiro teste de todos os    
    //       processos para o tempo 30.0

    for (i=0; i<N; i++) {
       schedule(test, 30.0, i); 
    }
    schedule(fault, 31.0, 1);
    schedule(fault, 32.0, 0);
    schedule(recovery, 61.0, 1);

    // agora vem o loop principal do simulador

    puts("===============================================================");
    puts("           Sistemas DistribuÃ­dos Prof. Elias");
    puts("          LOG do Trabalho PrÃ¡tico 0, Tarefa 1");
    puts("      Digitar, compilar e executar o programa tempo.c");
    printf("   Este programa foi executado para: N=%d processos.\n", N); 
    printf("           Tempo Total de SimulaÃ§Ã£o = %d\n", MaxTempoSimulac);
    puts("===============================================================");

    while(time() < 150.0) {
         cause(&event, &token);
         switch(event) {
           case test: 
                if (status(processo[token].id) !=0) break; // se o processo estÃ¡ falho, nÃ£o testa!

                int st = 1;
                int nxt_process = (token+1) % N;
                int correct_process = token;

                int *aux_v = (int *) malloc(sizeof(int)*N);
                for (int i=0; i<N; i++)
                    aux_v[i] = -2;
                aux_v[token] = 0;

                while ((st != 0) && (nxt_process != token)) {
                    st = status(processo[nxt_process].id);
    
                    aux_v[nxt_process] = st;

                    printf("O processo %d testou o processo %d ", token, nxt_process);
                    if (st == 0) {
                        printf("correto ");
                        correct_process = nxt_process;
                    }
                    else if (st == 1)
                        printf("falho ");
                    else
                        printf("desconhecido ");
                    printf("no tempo %4.1f\n", time());
                    nxt_process = (nxt_process+1) % N;
                }

                for (int i = 0; i<N; i++)
                    if (aux_v[i] != -2)
                        processo[token].state[i] = aux_v[i];
                    else
                        processo[token].state[i] = processo[correct_process].state[i];
                //printf("Processo %d: estou testando no tempo %4.1f\n", token, time());
                printf("Vetor state do processo %d: [%d", token, processo[token].state[0]);
                for (int i = 1; i<N; i++)
                    printf(", %d", processo[token].state[i]);
                printf("]\n");

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

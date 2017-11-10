#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>


#define REINDEERS 9

//Declarando variaveis necessarias
int N_Elves;
int elves = 0, reindeer = 0; //argReindeer;
//Declarando semaforos
sem_t reindeerSem, santaSem, elfSem;
//Declarando Mutex
sem_t mutex,elfAux;
int cont_mutex = 1;
int aux_elves;


void prepareSleigh(){
    printf("\nSanta is preparing the sleigh... \n");
    return;
}
void helpElves(){
    printf("\nSanta is helping the elves... \n");
    return;
}
void getHitched(){
    printf("\nSanta is hitching the reindeers... \n");
    return;
}
void getHelp(){
    printf("\nThe elf is asking Santa for help... \n");
    return;
}


//Funcao Santa
void *Santa(){
    /* code */

  	printf("\n =====ENTROU Thread Santa=====\n\n");

  	printf("Santa dormindo\n");
  	sem_wait(&santaSem); // Estará dormindo até ser acordado por algum semáforo
  	printf("Santa acordou\n");
  	sem_wait(&mutex); // Precisa do acesso exclusivo aos contadores pra ver quem o acordou

    do{
    	if(reindeer == REINDEERS){ // As renas tem prioridade aos elfos, então elas são verificadas primeiro
    	        prepareSleigh(); // Se as renas forem == 9, o trenó é preparado
              reindeer = 0;
    	        int i;
    	        for (i=0; i<REINDEERS; i++){
    	        	sem_post(&reindeerSem);
    	        }
    	      
              printf("\n \n -----RENAS FINALIZADAS----- \n\n");
              sem_post(&mutex);



    	}
      if (elves == 3){

    		int j;
    	        for (j=0; j<3; j++){
    	        	sem_post(&elfAux);
    	        }
    	    N_Elves = N_Elves-3;

    		helpElves();
    		sleep(5);
    		printf("\n \n -----3 ELFOS FINALIZADAS----- \n\n");
        	sem_post(&elfAux);
        	sem_post(&mutex);
    	}
      //printf("elves = %d, reindeers = %d\n",elves,reindeer);
    } while (N_Elves !=0 || reindeer != 0);
      /* code */

  	//Desbloqueando mutex

  	printf("\n =====SAIU Thread Santa===== \n\n");
    pthread_exit(0);

}
//Funcao Reindeer
void *Reindeer(){

	printf("\n=====ENTROU Thread Reindeer===== \n\n");
	sem_wait(&mutex);//Bloqueando essa parte do codigo com mutex

	reindeer++;
	printf("xxxxxxxxxxx NUMEROS DE RENAS: %d xxxxxxxxxxxxxxx\n",reindeer);
	if(reindeer == REINDEERS){
		sem_post(&santaSem);
	}

	//Desbloqueando o mutex
  	sem_post(&mutex);
	sem_wait(&reindeerSem);
	sleep(5);
	getHitched();
  	pthread_exit(0);
}

/*//Funcao Elves
void *Elves(){
	printf("=====ENTROU Thread Elves===== \n\n");

	sem_wait(&elfTex);
	sem_wait(&mutex);

	elves++;

	printf("xxxxxxxxxxx NUMEROS DE ELFOS APOS ADICIONAR: %d xxxxxxxxxxxxxxx\n",elves);
	if (elves == 3){
		printf("Já há 3 elfos\n");
		sem_post(&santaSem);
	}else{
		printf("Ainda não há 3 elfos\n");
		sem_post( &elfTex);
	}

	sem_post(&mutex);

	getHelp();

	sem_wait(&mutex);

	elves--;

	printf("xxxxxxxxxxx NUMEROS DE ELFOS APOS RETIRAR: %d xxxxxxxxxxxxxxx\n",elves);
	if(elves == 0){
		sem_post(&elfTex);
	}

	sem_post(&mutex);
}*/

//Funcao Elves
void *Elves(){
	printf("=====ENTROU Thread Elves===== \n\n");
	sem_wait(&elfSem);
  printf("Mutex bloqueado em Elves %d\n",--cont_mutex);
	sem_wait(&mutex);

	elves++;

	printf("xxxxxxxxxxx NUMEROS DE ELFOS APOS ADICIONAR: %d xxxxxxxxxxxxxxx\n",elves);
	if (elves == 3){
		printf("Já há 3 elfos\n");
		sem_post(&santaSem);
	}else{
		printf("Ainda não há 3 elfos\n");
		sem_post( &elfSem);
	}

  printf("Mutex desbloqueado em Elves %d\n",++cont_mutex);
  sem_post(&mutex);
	getHelp();
  sem_wait(&elfAux);
  printf("Mutex bloqueado em Elves %d\n",--cont_mutex);
	sem_wait(&mutex);

	elves--;

	printf("xxxxxxxxxxx NUMEROS DE ELFOS APOS RETIRAR: %d xxxxxxxxxxxxxxx\n",elves);
	if(elves == 0){
		sem_post(&elfSem);
	}
  printf("Mutex bloqueado em Elves %d\n",++cont_mutex);
	sem_post(&mutex);
  pthread_exit(0);
}

int main(int argc, char *argv[]){

  if (argc == 1) {
    printf("Forneça o numero de elfos desejados como argumento.\n");
    return 1;
  }

	if (argc == 2){
    aux_elves = N_Elves = atoi(argv[1]);
  } else{
    printf("Forneça apenas o numero de elfos desejados como argumento.\n");
    return 1;
  }

	//declarando threads
	pthread_t thr_claus, thr_reindeer, thr_elf;

	//inicializando semaforos e mutex
	sem_init(&reindeerSem, 0, 0);
	sem_init(&elfSem, 0, 1);
	sem_init(&santaSem,0,0);
  sem_init(&mutex,0,1);
  sem_init(&elfAux,0,0);



	//criando as threads

	pthread_create(&thr_claus, NULL, &Santa, NULL);
  //pthread_create(&thr_reindeer, NULL, &Reindeer, NULL);
  //pthread_create(&thr_elf, NULL, &Elves, NULL);


  srand(time(NULL));
  int aux_random = REINDEERS + N_Elves;

  int elves_cont = N_Elves;
  int rein_cont = REINDEERS;
  printf("rein_cont: %d\n",rein_cont);
  printf("elves_cont: %d\n",elves_cont);

  for(int i=0; i < aux_random;i++){
    int r = rand()%2;
    printf("i = %d, rein_cont = %d, elves_cont = %d, r = %d\n",i, rein_cont, elves_cont,r);
    if (r == 0){
        if (elves_cont > 0){
          elves_cont--;
        printf("elves_cont de dentro: %d\n",elves_cont);
        pthread_create(&thr_elf, NULL, &Elves, NULL);
      } else {
        rein_cont--;
        printf("rein_cont de dentro: %d\n",rein_cont);
        pthread_create(&thr_reindeer, NULL, &Reindeer, NULL);
      }
	  }else{
      if (rein_cont > 0){
        rein_cont--;
        printf("rein_cont de dentro: %d\n",rein_cont);
        pthread_create(&thr_reindeer, NULL, &Reindeer, NULL);
      } else {
        elves_cont--;
      printf("elves_cont de dentro: %d\n",elves_cont);
      pthread_create(&thr_elf, NULL, &Elves, NULL);
      }
    }
  }
  pthread_join(thr_claus, NULL);
  printf("FINAL elves = %d, reindeers = %d\n",elves,reindeer);
  return 0;

}

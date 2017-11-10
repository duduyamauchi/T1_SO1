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
sem_t reindeerSem, santaSem, elfMutex;
//Declarando Mutex
sem_t mutex,elfSem;
int cont_mutex = 1;
int aux_elves;


void prepareSleigh(){
    printf("\n\nSanta is preparing the sleigh... \n\n");
    return;
}
void helpElves(){
    printf("\n\nSanta is helping the elves... \n\n");
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


    //do{
	while(1){
  	sem_wait(&santaSem); // Estará dormindo até ser acordado por algum semáforo
  	
  	sem_wait(&mutex); // Precisa do acesso exclusivo aos contadores pra ver quem o acordou

    	if(reindeer == REINDEERS){ // As renas tem prioridade aos elfos, então elas são verificadas primeiro
    			reindeer = 0;

				prepareSleigh();

    	        int i;
    	        for (i=0; i<REINDEERS; i++){
    	        	sem_post(&reindeerSem);
    	        }
    	        

    	}
      if (elves == 3){

    		int j;
    	        for (j=0; j<3; j++){
    	        	sem_post(&elfSem);
    	        }
    	    
    	    	N_Elves = N_Elves - 3;
		
				helpElves();
    	}

    	sem_post(&mutex);
	} //while (N_Elves !=0 && reindeer != 0);
      /* code */

  	//Desbloqueando mutex
    	pthread_exit(0);

}
//Funcao Reindeer
void *Reindeer(){

	sem_wait(&mutex);//Bloqueando essa parte do codigo com mutex
	
	reindeer++;
	if(reindeer == REINDEERS){
		sem_post(&santaSem);
	}

	//Desbloqueando o mutex
  	sem_post(&mutex);
	sem_wait(&reindeerSem);
	
	getHitched();

  	pthread_exit(0);
}


//Funcao Elves
void *Elves(){
	
	sem_wait(&elfMutex);
	sem_wait(&mutex);

	elves++;

	if (elves == 3){
		sem_post(&santaSem);
	}else{
		sem_post(&elfMutex);
	}

  	sem_post(&mutex);
  	sem_wait(&elfSem);
	
   	getHelp();

	sem_wait(&mutex);

	elves--;

	if(elves == 0){
		sem_post(&elfMutex);
	}
	
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
  sem_init(&elfSem,0,0);
  sem_init(&santaSem,0,0);

  sem_init(&elfMutex, 0, 1);
  sem_init(&mutex,0,1);




  //criando as threads

  pthread_create(&thr_claus, NULL, &Santa, NULL);

  srand(time(NULL));
  int aux_random = REINDEERS + N_Elves;

  int elves_cont = N_Elves;
  int rein_cont = REINDEERS;
  
  printf("rein_cont: %d\n",rein_cont);
  printf("elves_cont: %d\n",elves_cont);

  for(int i=0; i < aux_random;i++){
    int r = rand()%2;
    
    if (r == 0){
        
      if (elves_cont > 0){
        elves_cont--;
        pthread_create(&thr_elf, NULL, &Elves, NULL);
      } else {
        rein_cont--;
        pthread_create(&thr_reindeer, NULL, &Reindeer, NULL);
      }

    }else{
      
      if (rein_cont > 0){
        rein_cont--;
        pthread_create(&thr_reindeer, NULL, &Reindeer, NULL);
      } else {
        elves_cont--;
      pthread_create(&thr_elf, NULL, &Elves, NULL);
      }

    }
  }
  
  pthread_join(thr_claus, NULL);
  
  return 0;

}

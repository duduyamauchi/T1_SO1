#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

//Declarando variaveis necessarias
int elves, reindeer; //argReindeer;
//Declarando semaforos
sem_t reindeerSem, santaSem, mutex, elfTex;
//declarando threads
pthread_t thr_claus, thr_reindeer, thr_elf;


void prepareSleigh(){
    printf("Santa is preparing the sleigh... \n");
    return;
}
void helpElves(){
    printf("Santa is helping the elves... \n");
    return;
}
void getHitched(){
    printf("Santa is hitching the reindeers... \n");
    return;
}
void getHelp(){
    printf("The elf is asking Santa for help... \n");
    return;
}


//Funcao Santa
void *Santa(){
	//while(1){//elves != 0 && reindeer != 0){
		printf("ENTROU Thread Santa: Elves = %d, Reindeer = %d \n\n", elves, reindeer);

		printf("Santa wait\n");
		sem_wait(&santaSem); // Estará dormindo até ser acordado por algum semáforo
		printf("Santa go\n");
		sem_wait( &mutex); // Precisa do acesso exclusivo aos contadores pra ver quem o acordou
	    if(reindeer >= 9){ // As renas tem prioridade aos elfos, então elas são verificadas primeiro
	        prepareSleigh(); // Se as renas forem == 9, o trenó é preparado
	        //reindeerSem.signal(9)
	        int i;
	        for (i=0; i<9; i++){
	        sem_post(&reindeerSem);
	        }
	        reindeer -=9;
	        printf("if Elves = %d, Reindeer = %d \n", elves, reindeer);
	    }else if (elves == 3){
	        /*int i;
	        for (i=0; i<3; i++){
	        sem_wait(&elfSem);
	        }*/
	        helpElves();
	    }
		sem_post( &mutex);
		printf("SAIU Thread Santa: Elves = %d, Reindeer = %d \n\n", elves, reindeer);
//	}

}
//Funcao Reindeer
void *Reindeer(){

	printf("ENTROU Thread Reindeer: Elves = %d, Reindeer = %d \n\n", elves, reindeer);

	sem_wait(&mutex);
	reindeer += 1;
	if(reindeer == 9){
		//santaSem.signal()
		sem_post(&santaSem);
	}
	sem_post( &mutex);
	sem_wait(&reindeerSem);
	getHitched();
	printf("SAIU Thread Reindeer: Elves = %d, Reindeer = %d \n\n", elves, reindeer);
}

//Funcao Elves
void *Elves(){
	printf("ENTROU Thread Elves: Elves = %d, Reindeer = %d \n\n", elves, reindeer);
	sem_wait( &mutex);
	elves += 1;
	if (elves == 3){
		sem_wait( &elfTex);
		sem_post(&santaSem);
	}else{
		sem_post( &elfTex);
	}
	sem_post( &mutex);
	getHelp();
	sem_wait( &mutex);
	elves -= 1;
	if(elves == 0){
		sem_post(&elfTex);
	}
	sem_post( &mutex);
	printf("SAIU Thread Elves: Elves = %d, Reindeer = %d \n\n", elves, reindeer);
}

int main(){

	printf("elves and reindeers: \n");
	scanf("%d %d", &elves, &reindeer);


	//inicializando semaforos e mutex
	sem_init(&reindeerSem, 0, 0);
	sem_init(&elfTex,0,1);
	sem_init(&santaSem,0,0);
	sem_init(&mutex,0,1);

	//criando as threads

	pthread_create(&thr_claus, NULL, &Santa, NULL);
	pthread_create(&thr_reindeer, NULL, &Reindeer, NULL);
	pthread_create(&thr_elf, NULL, &Elves, NULL);


	//loop do papai noel
	while(1){
    	//iniciando as threads
    
    	//printf("inicial Elves = %d, Reindeer = %d \n", elves, reindeer);
		pthread_join(thr_claus, NULL);
		pthread_join(thr_reindeer, NULL);
		pthread_join(thr_elf, NULL);
	}

}


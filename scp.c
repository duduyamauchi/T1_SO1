#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

//Declarando variaveis necessarias
int N_Reindeer, N_Elves;
int elves = 0, reindeer = 0; //argReindeer;
//Declarando semaforos
sem_t reindeerSem, santaSem;
//Declarando Mutex
pthread_mutex_t mutex, elfTex;


void prepareSleigh(){
    printf("\n Santa is preparing the sleigh... \n");
    return;
}
void helpElves(){
    printf("\n Santa is helping the elves... \n");
    return;
}
void getHitched(){
    printf("\n Santa is hitching the reindeers... \n");
    return;
}
void getHelp(){
    printf("\n The elf is asking Santa for help... \n");
    return;
}


//Funcao Santa
void *Santa(){
	
	printf("\n =====ENTROU Thread Santa=====\n\n");

	printf("Santa dormindo\n");
	sem_wait(&santaSem); // Estará dormindo até ser acordado por algum semáforo
	printf("Santa acordou\n");

	printf("Mutex bloqueado em Santa\n");	
	pthread_mutex_lock(&mutex); // Precisa do acesso exclusivo aos contadores pra ver quem o acordou
	

	if(reindeer == 9){ // As renas tem prioridade aos elfos, então elas são verificadas primeiro
	        prepareSleigh(); // Se as renas forem == 9, o trenó é preparado
	        int i;
	        for (i=0; i<=9; i++){
	        	sem_post(&reindeerSem);
			printf("reindeer n: %d preparada\n",i);
	        }
	        reindeer = 0;
	        printf("-----Finalizando renas----- \n");
	}else if (elves == 3){
		helpElves();
	}
		
	pthread_mutex_unlock(&mutex);//Desbloqueando mutex
	printf("Mutex desbloqueado em Santa\n");

	printf("\n =====SAIU Thread Santa===== \n\n");

}
//Funcao Reindeer
void *Reindeer(){

	printf("\n=====ENTROU Thread Reindeer===== \n\n");
	
	pthread_mutex_lock(&mutex);//Bloqueando essa parte do codigo com mutex
	
	reindeer++;
	printf("xxxxxxxxxxx NUMEROS DE RENAS: %d xxxxxxxxxxxxxxx\n",reindeer);
	if(reindeer == 9){
		printf("Já há 9 reindeers\n");
		sem_post(&santaSem);
		printf("Reindeer acordando papai noel\n");
	}
	
	pthread_mutex_unlock( &mutex);//Desbloqueando o mutex
		
	sem_wait(&reindeerSem);

	getHitched();
	
	printf("=====SAIU Thread Reindeer===== \n\n");
}

//Funcao Elves
void *Elves(){
	printf("=====ENTROU Thread Elves===== \n\n");
	
	printf("elfTex bloqueado em Elves\n");
	pthread_mutex_lock(&elfTex);
	
	printf("mutex bloqueado em Elves\n");
	pthread_mutex_lock(&mutex);
	
	elves++;
	printf("xxxxxxxxxxx NUMEROS DE ELFOS: %d xxxxxxxxxxxxxxx\n",elves);
	if (elves == 3){
		printf("Já há 3 elfos\n");		
		sem_post(&santaSem);		
	}else{
		printf("Ainda não há 3 elfos\n");		
		pthread_mutex_unlock( &elfTex);		
	}

	pthread_mutex_unlock(&mutex);
	printf("Mutex desbloqueado em Elves\n");
	getHelp();
	
	printf("Mutex bloqueado em Elves\n");
	pthread_mutex_lock(&mutex);
	elves--;
	if(elves == 0){
		pthread_mutex_unlock(&elfTex);
		printf("elfTex desbloqueado em elves\n");
	}
	pthread_mutex_unlock(&mutex);
	printf("Mutex desbloqueado em Elves\n");

	printf("=====SAIU Thread Elves: Elves===== \n\n");
}

int main(){

	printf("elves and reindeers: \n");
	scanf("%d %d", &N_Elves, &N_Reindeer);

	//declarando threads
	pthread_t thr_claus, thr_reindeer[N_Reindeer], thr_elf[N_Elves];
	
	//inicializando semaforos e mutex
	sem_init(&reindeerSem, 0, 0);
	sem_init(&santaSem,0,0);	

	pthread_mutex_init(&mutex,NULL);
	pthread_mutex_init(&elfTex,NULL);

	//criando as threads

	pthread_create(&thr_claus, NULL, &Santa, NULL);

	for(int i=0; i < N_Reindeer;i++){
		pthread_create(&thr_reindeer[i], NULL, &Reindeer, NULL);
	}	
	
	for(int j=0; j < N_Elves; j++){
		pthread_create(&thr_elf[j], NULL, &Elves, NULL);
	}
	
	
	pthread_join(thr_claus, NULL);
	
}


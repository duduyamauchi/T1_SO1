#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

//Definindo um numero fixo de renas
#define REINDEERS 9

//Declarando variaveis necessarias
//Variavel responsavel pela entrada do programa
int N_Elves;
//Contadores
int elves = 0, reindeer = 0;
//Declarando semaforos
sem_t reindeerSem, santaSem, elfMutex;
//Declarando Mutex
sem_t mutex,elfSem;
//Auxilia na contagem do elfos
int aux_elves;

//Métodos a serem chamados pelas threads
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


//Funcao do Papai Noel
void *Santa(){
    //Loop infinito no qual o papai noel realiza todas as suas funções
	while(1){
  	sem_wait(&santaSem); // Estará dormindo até ser acordado por algum semáforo
  	
  	sem_wait(&mutex); // Precisa do acesso exclusivo aos contadores pra ver quem o acordou

    	if(reindeer == REINDEERS){ // As renas tem prioridade aos elfos, então elas são verificadas primeiro
    			reindeer = 0;

				prepareSleigh(); //Prepara o trenó 

    	        int i;
    	        for (i=0; i<REINDEERS; i++){
    	        	sem_post(&reindeerSem); //Envia o sinal para as 9 renas
    	        }
    	        

    	}
      if (elves == 3){ // Caso as renas ainda não estiverem prontas, atender os elfos

    		int j;
    	        for (j=0; j<3; j++){
    	        	sem_post(&elfSem); //Mandado o sinal para os 3 elfos
    	        }
		
				helpElves(); //Papai noel ajuda os elfos
    	}

    	sem_post(&mutex); //Libera o acesso aos contadores
	} 
    	pthread_exit(0);//Sai da thread

}
//Funcao das renas
void *Reindeer(){

	sem_wait(&mutex);//Acesso exclusivo aos contadores 
	
	reindeer++; //Incrementando o numero de renas
	if(reindeer == REINDEERS){ //Se o numero de renas for igual a 9, acorde o papai noel
		sem_post(&santaSem);// Mandando o sinal para o papai noel acordar
	}

  	sem_post(&mutex); //Libera o acesso aos contadores
	sem_wait(&reindeerSem); //Rena espera o sinal do papai noel para ser amarrada ao trenó
	
	getHitched();//Rena é amarrada ao trenó

  	pthread_exit(0);//Sai da thread
}


//Funcao dos elfos
void *Elves(){
	
	sem_wait(&elfMutex); //Bloqueia a criação de novos elfos, não permite a entrada de novos elfos que precisam de ajuda do papai noel
	sem_wait(&mutex); //Acesso exclusivo aos contadores

	elves++;//Incrementando o numero de elfos

	if (elves == 3){ //Se o numero de elfos for igual a 3, acorde o papai noel
		sem_post(&santaSem);//Mandando sinal para o papai noel acordar
	}else{ //Caso contrario permitir a entrada de novos elfos, ou seja a criação de mais elfos
		sem_post(&elfMutex);//Desbloqueia o mutex responsavel por permitir a criação de novos elfos
	}

  	sem_post(&mutex);//Libera o acesso aos contadores
  	sem_wait(&elfSem);//Elfo espera o sinal do papai noel o ajudando
	
   	getHelp();//Elfo pede ajuda

	sem_wait(&mutex); //Acesso exclusivo aos contadores

	elves--;//Decrementando o numero de elfos que já receberam ajuda

	if(elves == 0){ //Quando o numero de elfos pedindo ajudar for 0, libera a entrada de novos elfos pedindo ajuda
		sem_post(&elfMutex);//Desbloqueia o mutex responsavel por permitir a criação de novos elfos
	}
	
	sem_post(&mutex);//Libera o acesso aos contadores
  
	pthread_exit(0);//Sai da thread
}

int main(int argc, char *argv[]){

  if (argc == 1) {// O primeiro argumento de argv é sempre o nome do programa, se tiver só ele tá errado
    printf("Forneça o numero de elfos desejados como argumento.\n");
    return 1; //retorna algo diferente de 1, para indicar que o programa nao obteve exito
  }

  if (argc == 2){
    aux_elves = N_Elves = atoi(argv[1]);//a conversão é forçada para evitar um cast
  } else{
    printf("Forneça apenas o numero de elfos desejados como argumento.\n");
    return 1;//retorna algo diferente de 1, para indicar que o programa nao obteve exito
  }

  //declarando threads
  pthread_t thr_claus, thr_reindeer, thr_elf;//Threads envolvidas no problema

  //inicializando semaforos e mutex
  sem_init(&reindeerSem, 0, 0);//Semaforo das renas
  sem_init(&elfSem,0,0);//Semaforo dos elfos
  sem_init(&santaSem,0,0);// Semaforo do Papai noel

  sem_init(&elfMutex, 0, 1); //Mutex que impede a criação de novos elfos
  sem_init(&mutex,0,1); //Mutex que controla a manipulação das variveis elves e reeinder




  //criando as threads

  pthread_create(&thr_claus, NULL, &Santa, NULL);

  srand(time(NULL));//inicia uma seed de numeros aleatórios
  int aux_random = REINDEERS + N_Elves;// variável para controlar o número de threads de elfos e renas que devem ser criadas

  int elves_cont = N_Elves;//variável para monitorar o numero de elfos que ainda tem que ser gerados;
  int rein_cont = REINDEERS; // variável para monitorar o numero de duendes que ainda tem que ser gerados
  
  printf("rein_cont: %d\n",rein_cont);
  printf("elves_cont: %d\n",elves_cont);

  for(int i=0; i < aux_random;i++){
    int r = rand()%2;//seletor para escolher entre duendes e renas,
    
    if (r == 0){// se r = 0, tenta criar uma thread de duendes
        
      if (elves_cont > 0){// se ainda houver duendes pra serem criados
        elves_cont--;
        pthread_create(&thr_elf, NULL, &Elves, NULL);
      } else { //se não houver, uma rena deve ser gerada, já que aux_random garante que alguém deve ser criado
        rein_cont--;
        pthread_create(&thr_reindeer, NULL, &Reindeer, NULL);
      }

    }else{// se r = 1, tenta criar uma thread de rena
      
      if (rein_cont > 0){//se  ainda houver renas para serem criadas
        rein_cont--;
        pthread_create(&thr_reindeer, NULL, &Reindeer, NULL);
      } else {//se não houver, um duende deve ser gerado, já que aux_random garante que alguém deve ser criado
        elves_cont--;
      pthread_create(&thr_elf, NULL, &Elves, NULL);
      }

    }
  }
  
  pthread_join(thr_claus, NULL);//papai noel requer join pois ele tem que esperar as outras threads, já que lida com elas
  
  return 0;// retorno bem sucedido

}

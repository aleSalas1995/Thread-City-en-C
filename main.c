#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "start_thread.h"
#include "scheduler.h"
#include "my_mutex.h"

#define IN 99
#define N 35 //numero de nodos existentes en el mapa(cada nodo es una cuadra)

my_mutex fieldLock;

char matriz[5][7];//matriz que se utiliza para representar la ubicacion de los vehiculos en el mapa
int semaforo;//variable global que se usa para mantener el estado del semaforo(rojo o verde)

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/* funcion que pone todos los nodos de la matriz en cero
 */
int revisarMatriz(){
	int i,j;
	int bool=0;
	for(i=0;i < 5; i++){
		for(j=0;j < 7; j++){
			if(matriz[i][j]=='C' || matriz[i][j]=='A' || matriz[i][j]=='B'){
				return 1;
			}
		}
	}
	
	return 0;
}

/*funcion que imprime la matriz en el terminal
 */
void imprimirMatriz(){
    char puente[2] = "||";
    int i,j;
    for(i=0;i < 5; i++){
        for(j=0;j < 7; j++){
            printf("     ");
            if(matriz[i][j] == 'C'){
                printf(ANSI_COLOR_YELLOW    "%c   ",matriz[i][j]);
                printf(ANSI_COLOR_RESET);
            }
            else if(matriz[i][j] == 'B'){
                printf(ANSI_COLOR_GREEN    "%c   ",matriz[i][j]);
                printf(ANSI_COLOR_RESET);
            }
            else if(matriz[i][j] == 'A'){
                printf(ANSI_COLOR_RED    "%c   ",matriz[i][j]);
                printf(ANSI_COLOR_RESET);
            }
            else if(i==2 && j==1 || i==2 && j==3 || i==2 && j==5 ){
                printf("%s  ",puente);
            }
            else if(i==2 && j==0 || i==2 && j==2 || i==2 && j==4 ||i==2 && j==6 ){
                printf(ANSI_COLOR_BLUE    "%c   ",matriz[i][j]);
                printf(ANSI_COLOR_RESET);
            }
            else{
                printf("%c   ",matriz[i][j]);
            }
            
        
        }
        printf("\n\n");
    }
    printf("     *******************************************************");
    printf("\n\n");
    //usleep(50000); //milisegundos
     

    my_thread_yield();
}


/*funcion que busca el camino mas corto de un nodo a otro
 * utilza el algoritmo de dijkstra
 */
int dijsktra(int source,int target, int ruta[N]){
		int cost[35][35] = {{0,99,99,99,99,99,99,1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//0
									{99,0,1,99,99,99,99,99,1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//1
									{99,1,0,1,99,99,99,99,99,1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//2
									{99,99,1,0,1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//3
									{99,99,99,1,0,1,99,99,99,99,99,1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//4
									{99,99,99,99,1,0,1,99,99,99,99,99,1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//5
									{99,99,99,99,99,1,0,99,99,99,99,99,99,1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//6
									{1,99,99,99,99,99,99,0,1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//7
									{99,1,99,99,99,99,99,1,0,99,99,99,99,99,99,1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//8
									{99,99,1,99,99,99,99,99,99,0,1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//9
									{99,99,99,99,99,99,99,99,99,1,0,1,99,99,99,99,99,1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//10
									{99,99,99,99,1,99,99,99,99,99,1,0,1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//11
									{99,99,99,99,99,1,99,99,99,99,99,1,0,1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//12
									{99,99,99,99,99,99,1,99,99,99,99,99,1,0,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//13
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,0,1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//14
									{99,99,99,99,99,99,99,99,1,99,99,99,99,99,1,0,1,99,99,99,99,1,99,99,99,99,99,99,99,99,99,99,99,99,99},//15
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,1,0,1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//16
									{99,99,99,99,99,99,99,99,99,99,1,99,99,99,99,99,1,0,1,99,99,99,99,99,1,99,99,99,99,99,99,99,99,99,99},//17
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,1,0,1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//18
									{99,99,99,99,99,99,99,99,99,99,99,99,1,99,99,99,99,99,1,0,1,99,99,99,99,99,1,99,99,99,99,99,99,99,99},//19
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,1,0,99,99,99,99,99,99,99,99,99,99,99,99,99,99},//20
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,0,1,99,99,99,99,99,1,99,99,99,99,99,99},//21
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,1,99,99,99,99,99,1,0,1,99,99,99,99,99,99,99,99,99,99,99},//22
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,1,0,1,99,99,99,99,99,99,99,99,99,99},//23
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,1,99,99,99,99,99,1,0,1,99,99,99,99,99,1,99,99,99},//24
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,1,0,99,99,99,99,99,99,1,99,99},//25
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,1,99,99,99,99,99,99,0,1,99,99,99,99,99,1,99},//26
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,1,0,99,99,99,99,99,99,1},//27
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,1,99,99,99,99,99,99,0,99,99,99,99,99,99},//28
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,0,1,99,99,99,99},//29
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,1,0,1,99,99,99},//30
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,1,99,99,99,99,99,1,0,99,99,99},//31
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,1,99,99,99,99,99,99,0,1,99},//32
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,1,99,99,99,99,99,1,0,99},//33
									{99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,1,99,99,99,99,99,99,0}};//34
									
    int dist[N],prev[N],selected[N]={0},i,m,min,start,d,j;
    int path[N]={-1};
    for(i=0;i< N;i++)
    {
        dist[i] = IN;
        prev[i] = -1;
    }
    start = source;
    selected[start]=1;
    dist[start] = 0;
    while(selected[target] ==0)
    {
        min = IN;
        m = 0;
        for(i=0;i< N;i++)
        {
            d = dist[start] +cost[start][i];
            if(d< dist[i]&&selected[i]==0)
            {
                dist[i] = d;
                prev[i] = start;
            }
            if(min>dist[i] && selected[i]==0)
            {
                min = dist[i];
                m = i;
            }
        }
        start = m;
        selected[start] = 1;
    }
    start = target;
    j = 0;
    while(start != -1)
    {
        path[j] = start;
        start = prev[start];
        j++;
    }
    
    
    //path[j]='\0';
    //strrev(path);
    
    
    int p,q=0;
	for(p=dist[target];p >= 0; p--){
		ruta[q] = path[p];
		//printf("%i->",path[p]);
		q++;
	}
	
	printf("\n");
    //printf("%s", path);
    return dist[target];
}

/*funcion que mueve el vehiculo a travez del grafo
 */
void moverVehiculo(int ruta[N],int r,int r2,char tipo){
	int len=dijsktra(r,r2,ruta);
	int x=0,y=0,z;
	int posAux=0;
	srand(time(NULL));
	//inicia el ciclo por el cual se va a realizar el recorrido nodo a nodo
	for(z=0;z<len+1;z++){
		
		usleep(9000000);

		int pos =ruta[z];
		if(posAux==17 && pos==10){
			//hacer ceda
			usleep(900000);
		}
		
		
		
		if( posAux==26 && pos==19 || posAux==19 && pos==12 || posAux==12 && pos==19 || posAux==19 && pos==26){
			//semaforo
			while(semaforo!=1){
			
			}
		}

		if(pos==8 || pos==22){
				//revisar si va pasar un barco
		}
		
		//calcula la ruta en la matriz
		matriz[x][y]='0';
		posAux = pos;
		x = pos/7;
		y = pos%7;
		
		matriz[x][y]=tipo;//ubica el vehiculo
		
		printf("Vehiculo %c se encuentra en x: %i,y: %i\n",tipo,x,y);
		
	}
	usleep(900000);
	matriz[x][y]='0';
}

/*funcion que inicia el movimiento del carro por el mapa
 * utiliza un random para generar el spawm del vehiculo y el destino
 */
void iniciarCarro(){
	int ruta[N];
	
	time_t t;
   
   //srand((unsigned) time(&t));

	int r = rand() % 35;
	int r2 = rand() % 35;
	
	//si las ubicaciones generadas por los random no son validas,
	//invalidas: pertencen a un rio o puente
	while(r==r2 || r>=14 && r<=20 || r2>=14 && r2<=20){
		r2 = rand() % 35;
		r = rand() % 35;
	}
	
	printf("Carro c: %i-%i",r,r2);
	moverVehiculo(ruta,r,r2,'C');
	
}

/*funcion que inicia el movimiento del barco por el mapa
 * utiliza un random para generar el spawm del vehiculo y el destino
 */
void iniciarBarco(){
	int ruta[N];
	time_t t;
   
    //srand((unsigned) time(&t));
	
	int r = rand() % 3 + 14;
	int r2 = rand() % 3 + 14;
	
	//si las ubicaciones generadas por los random no son validas,
	//invalidas: pertencen a una calle o un rio 
	while(r==r2 || r==15 || r2==15){
		r2 = rand() % 3 + 14;
		r = rand() % 3 + 14;
	}
	
	printf("Barco B: %i-%i",r,r2);
	moverVehiculo(ruta,r,r2,'B');	
}

/*funcion que inicia el movimiento de la ambulancia por el mapa
 * utiliza un random para generar el spawm del vehiculo y el destino
 */
void iniciarAmbulancia(){
	int ruta[N];
	time_t t;
   
    //srand((unsigned) time(&t));
	
	int r = rand() % 35;
	int r2 = rand() % 35;
	
	//si las ubicaciones generadas por los random no son validas,
	//invalidas: pertencen a un rio o puente
	
	while(r==r2 || r>=14 && r<=20 || r2>=14 && r2<=20){
		r2 = rand() % 35;
		r = rand() % 35;
	}
	
	printf("Ambulancia A: %i-%i",r,r2);
	moverVehiculo(ruta,r,r2,'A');
}

/*funcion que inicializa el semaforo ubicado en el puente
 */
void iniciarSemaforo(){
	semaforo=0;
	
	while(1){
		sleep(4);
		if(semaforo==0){
			semaforo=1;
		}else{
			semaforo=0;
		}
	}
	usleep(111250000); //milisegundos
}

/*funcion que realiza un ciclo para que se imprima en el terminal el mapa constantemente
 */
void cicloImpresion(){
	//char matriz_aux[5][7] = matriz;
	while(1){
		
		usleep(1000); //milisegundos00
		//system("clear");
		//~ int bool = revisarMatriz();
		//~ if(bool==0){
			imprimirMatriz();
		//~ }
		//~ else{
			//~ break;
		//~ }

	}
	
	//return NULL;
}

/*funcion principal
 */
int main(int argc,char *argv[]){
	int i,j;
	for(i=0;i < 5; i++){
		for(j=0;j < 7; j++){
			matriz[i][j]='0';
			
		
		}
	}
	
	setup();
	my_thread_chsched("RoundRobin");
	
	while(1){
	
		my_thread_create(iniciarSemaforo,NULL,5,5);
		for (int i = 0; i < 1; i++){
			my_thread_create(iniciarCarro,NULL,5,5);
		}
		for (int i = 0; i < 1; i++){
			my_thread_create(iniciarAmbulancia,NULL,5,5);
		}
		for (int i = 0; i < 1; i++){
			my_thread_create(iniciarBarco,NULL,5,5);
		}
		
		my_thread_create(cicloImpresion,NULL,5,5);
		
		run_threads();
		
	}
	
	
}



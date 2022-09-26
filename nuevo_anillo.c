#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char **argv){	
	int virtual_pid, n, buffer[1], s, p = __INT_MAX__, pid;

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
	n = atoi(argv[1]);
	buffer[0] = atoi(argv[2]);
	s = atoi(argv[3]);
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], s);
    
	//quiero que el hijo s tenga conexion con el padre; creo el pipe padre y los hijos lo heredan
	int pipe_padre[2];
	pipe(pipe_padre);	
	//quiero que entre 2 hijos consecutivos haya un pipe que los conecte
	int pipe_hijos[n][2];
	

	// armo la estructura de pipes de hijos
	for(int i = 0; i < n; i++)
		pipe(pipe_hijos[i]);
	
	
	//creo n procesos hijos y le doy una virtual_pid a cada proceso
	for(virtual_pid = 0; virtual_pid < n; virtual_pid++){
		pid_t pid = fork();				
		if(pid == 0){

			// si no soy el hijo s, cierro mi pipe
			if(virtual_pid != s-1){
				close(pipe_padre[0]);
				close(pipe_padre[1]);
			}
			//quiero leer del pipe i y escribir en el pipe i+1; itero en el array de pipes
			for(int j = 0; j < n; j++){
							
				if(j == virtual_pid)
					//si quiero leer del pipe i cierro el canal de escritura
					close(pipe_hijos[j][1]);
				else if(j == (virtual_pid + 1) % n)
					//si quiero escribir en el pipe i+1 cierro el canal de lectura
					close(pipe_hijos[j][0]);
				else{
					//si soy el hijo i no necesito estar conectado al resto de los pipes
					close(pipe_hijos[j][0]);
					close(pipe_hijos[j][1]);
				}
			}
			// si soy el hijo salgo del proceso; no quiero tener mas hijos
			
			break;				
		}
	}

	//si soy el padre envio el mensaje al hijo s, el unico conectado al pipe_padre
	//	leo el mensaje luego de ser pasado por todos mis hijos;

	if(virtual_pid == n){
		write(pipe_padre[1], &buffer[0], sizeof(buffer[0]));
		close(pipe_padre[1]);
		read(pipe_padre[0], &buffer[0], sizeof(buffer[0]));
		close(pipe_padre[0]);
	}

	// si soy el hijo s leo el mensaje de mi padre por el pipe_padre, 
	// genero mi numero secreto y lo envio por mi pipe de escritura que es el siguiente
				//printf("ID: %d Elem: %d \n", virtual_pid, buffer[0]);
	if(virtual_pid == s){
		read(pipe_padre[0], &buffer[0], sizeof(buffer[0]));
		p = buffer[0] + 5;
		printf("ID: %d Elem: %d p: %d \n", (virtual_pid % n), buffer[0], p);
		write(pipe_hijos[(virtual_pid + 1)%n][1], &buffer[0], sizeof(buffer[0]));
	}

	//creo un while infinito por el que los hijos se pasan el mensaje con el array de pipes y sus direcciones virtuales
	while(1){
		read(pipe_hijos[virtual_pid][0], &buffer[0], sizeof(buffer[0]));
		
		if(buffer[0] > p)
			break;

		buffer[0]++;

		write(pipe_hijos[(virtual_pid + 1) % n][1], &buffer[0], sizeof(buffer[0]));
		printf("ID: %d Elem: %d p: %d \n", (virtual_pid), buffer[0], p);
	}

	//solo el hijo s llega aca
		 //printf("ID: %d Elem: %d \n", virtual_pid, buffer[0]);
	write(pipe_padre[1],&buffer[0],sizeof(buffer[0]));
	
	/* COMPLETAR */
    //FALTA MATAR A TODOS LOS HIJOS EN LINEA 70 Y CERRAR TODOS LOS PIPES DE LOS HIJOS ANTES DE MTARLOS
    /* COMPLETAR */
    
    
}

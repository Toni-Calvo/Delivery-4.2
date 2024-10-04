#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>

int main(int argc, char *argv[])
{ int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char peticion[512];
	char respuesta[512];
	
	// socket open
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error en el socket");
	
	// bind port & start the server
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	
	// any IP
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	// port: 9050
	serv_adr.sin_port = htons(9050);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error en el bind");
	
	// max queue: 4
	if (listen(sock_listen, 4) < 0)
		printf("Error en la escucha");
	
	while (true) {
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("Conectado\n");
		
		while (true) {
			ret=read(sock_conn,peticion, sizeof(peticion));
			printf ("Recibida una peticion\n");
			peticion[ret]='\0';
			
			printf ("La peticion es: %s\n",peticion);
			char *p = strtok(peticion, "/");
			int codigo =  atoi (p);
			
			// Disconect
			if (codigo == 0)
				break;
			
			p = strtok(NULL, "/");
			char nombre[20];
			strcpy (nombre, p);
			printf ("Codigo: %d, Nombre: %s\n", codigo, nombre);
			
			// Longitud
			if (codigo ==1)
				sprintf (respuesta, "%d",strlen (nombre));
			
			// Bonito
			else if (codigo == 2)
				if ((nombre[0]=='T') || (nombre[0]=='U') || ((nombre[0]=='Q') || (nombre[0]=='P')))
					strcpy(respuesta, "Si");
				else
					strcpy(respuesta,"No");
			
			// Altura
			else if (codigo == 3) {
				p = strtok(NULL, "/");
				float altura = atof(p);
								
				if (altura > 1.8)
					sprintf(respuesta, "%s eres alto", nombre);
				else
					sprintf(respuesta, "%s eres bajo", nombre);
			}
			
			// Palindromo
			else if (codigo == 4) {
				int cond = 1;
				
				// # de letras par
				if (strlen(nombre) % 2 == 0) {
					for (int i = 0; i < (strlen(nombre)/2); i++){
						if (nombre[i] != nombre[strlen(nombre) - 1 - i]) {
							cond = 0;
							break;
						}
					}
				}
				
				// # de letras impar
				else {
					for (int i = 0; i < (strlen(nombre) - 1)/2; i++) {
						if (nombre[i] != nombre[strlen(nombre) - 1 - i]) {
							cond = 0;
							break;
						}
					}
				}
				
				if (cond == 1)
					strcpy(respuesta, "Si");
				else
					strcpy(respuesta, "No");
			}
			
			// Mayusculas
			else if (codigo == 5) {
				for (int i = 0; i < strlen(nombre); i++) {
					respuesta[i] = toupper(nombre[i]);
				}
				respuesta[strlen(nombre)] = '\0';
			}
			
			// Enviamos la respuesta
			write (sock_conn,respuesta, strlen(respuesta));
		}
		
		// Se acabo el servicio para este cliente
		close(sock_conn); 
	}
}
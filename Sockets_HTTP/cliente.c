/*

** Fichero: servidor.c
** Autores:
** Raúl Melgosa Salvador DNI 70925689Z
** Juan Carlos Velasco Sánchez DNI 70959966K
*/


/*
 *			C L I E N T C P
 *
 *	This is an example program that demonstrates the use of
 *	stream sockets as an IPC mechanism.  This contains the client,
 *	and is intended to operate in conjunction with the server
 *	program.  Together, these two programs
 *	demonstrate many of the features of sockets, as well as good
 *	conventions for using these features.
 *
 *
 */
 
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

extern int errno;

#define ADDRNOTFOUND	0xffffffff
#define PUERTO 12250
#define TAM_BUFFER 100
#define RETRIES	5
#define TIMEOUT 6
#define TAMANNO_MAXIMO_TCP 32000
#define TAMANNO_MAXIMO_UDP 400

/*
 *			M A I N
 *
 *	This routine is the client which request service from the remote.
 *	It creates a connection, sends a number of
 *	requests, shuts down the connection in one direction to signal the
 *	server about the end of data, and then receives all of the responses.
 *	Status will be written to stdout.
 *
 *	The name of the system to which the requests will be sent is given
 *	as a parameter to the command.
 */
 
struct estructura_mensaje_cliente 
{
 	char mensaje[500];
 	char orden_usuario[500];
	char hostname[100];
	char connection[100];
};

void handler()
{
 	printf("Alarma recibida \n");
}
 
int main(argc, argv)
int argc;
char *argv[];
{
    int s;				/* connected socket descriptor */
   	struct addrinfo hints, *res;
    long timevar;			/* contains time returned by time() */
    struct sockaddr_in myaddr_in;	/* for local socket address */
    struct sockaddr_in servaddr_in;	/* for server socket address */
    struct in_addr reqaddr;
	int addrlen, i, j, errcode, n_retry;
    /* This example uses TAM_BUFFER byte messages. */
	char buf[TAM_BUFFER];
	char cabecera[3] = "\r\n";
	
	struct sigaction vec;

	if (strcmp(argv[2], "TCP") == 0) 
	{
		/* Create the socket. */
		s = socket (AF_INET, SOCK_STREAM, 0);
		if (s == -1) 
		{
			perror(argv[0]);
			fprintf(stderr, "%s: unable to create socket\n", argv[0]);
			exit(1);
		}
	
		/* clear out address structures */
		memset ((char *)&myaddr_in, 0, sizeof(struct sockaddr_in));
		memset ((char *)&servaddr_in, 0, sizeof(struct sockaddr_in));

		/* Set up the peer address to which we will connect. */
		servaddr_in.sin_family = AF_INET;
	
		/* Get the host information for the hostname that the
		* user passed in. */
		memset (&hints, 0, sizeof (hints));
		hints.ai_family = AF_INET;
 	 	
		/* esta funci�n es la recomendada para la compatibilidad con IPv6 gethostbyname queda obsoleta*/
    	errcode = getaddrinfo (argv[1], NULL, &hints, &res); 
    	if (errcode != 0)
		{
			/* Name was not found.  Return a
			 * special value signifying the error. */
			fprintf(stderr, "%s: No es posible resolver la IP de %s\n", argv[0], argv[1]);
			exit(1);
        }
   		else 
		{
			/* Copy address of host */
			servaddr_in.sin_addr = ((struct sockaddr_in *) res->ai_addr)->sin_addr;
	    }
    
		freeaddrinfo(res);

		/* puerto del servidor en orden de red*/
		servaddr_in.sin_port = htons(PUERTO);

		/* Try to connect to the remote server at the address
		 * which was just built into peeraddr.
		 */
		if (connect(s, (const struct sockaddr *)&servaddr_in, sizeof(struct sockaddr_in)) == -1) 
		{
			perror(argv[0]);
			fprintf(stderr, "%s: unable to connect to remote\n", argv[0]);
			exit(1);
		}
		/* Since the connect call assigns a free address
		 * to the local end of this connection, let's use
		 * getsockname to see what it assigned.  Note that
		 * addrlen needs to be passed in as a pointer,
		 * because getsockname returns the actual length
		 * of the address.
		 */
		addrlen = sizeof(struct sockaddr_in);
		if (getsockname(s, (struct sockaddr *)&myaddr_in, &addrlen) == -1) 
		{
			perror(argv[0]);
			fprintf(stderr, "%s: unable to read socket address\n", argv[0]);
			exit(1);
		}

		/* Print out a startup message for the user. */
		time(&timevar);
		/* The port number must be converted first to host byte
		* order before printing.  On most hosts, this is not
		* necessary, but the ntohs() call is included here so
		* that this program could easily be ported to a host
		* that does require it.
		*/
		printf("Connected to %s on port %u at %s", argv[1], ntohs(myaddr_in.sin_port), (char *) ctime(&timevar));

		//ENTRADA DE TEXTO
		struct estructura_mensaje_cliente mensaje_cliente;
		FILE *fp;

		fp = fopen(argv[3], "r");

		int a = 0;

		//TCP se cierra todo con ADIOS pero en UDP no
	
		FILE * fpe;
		char nombre_fichero[100];
		sprintf(nombre_fichero, "%u", ntohs(myaddr_in.sin_port));
		strcat(nombre_fichero, ".txt");
		fpe = fopen(nombre_fichero, "a");


		while (a == 0) 
		{
			bzero( (char *) &mensaje_cliente, sizeof(mensaje_cliente));
		
			fprintf(fpe, "\n=======================================================================================================================\n");
			if (fgets(mensaje_cliente.orden_usuario, sizeof(mensaje_cliente.orden_usuario), fp) == NULL) 
			{
				exit(1);
			}
		
			for (int k = 0; k < sizeof(mensaje_cliente.orden_usuario); k++) 
			{
				if (mensaje_cliente.orden_usuario[k] == '\n') 
				{
					mensaje_cliente.orden_usuario[k] = '\0';
				}
			}
		

			if (strcmp(mensaje_cliente.orden_usuario, "ADIOS") == 0) 
			{
				if (shutdown(s, 1) == -1) 
				{
					perror(argv[0]);
					fprintf(stderr, "%s: unable to shutdown socket\n", argv[0]);
					exit(1);
				}
				break;		
			}
			else 
			{						
				int status;
				status = getnameinfo((struct sockaddr *)&myaddr_in,sizeof(myaddr_in), mensaje_cliente.hostname,sizeof(mensaje_cliente.hostname),NULL,0,0);
     			if (status)
				{
					if (inet_ntop(AF_INET, &(myaddr_in.sin_addr), mensaje_cliente.hostname, sizeof(mensaje_cliente.hostname)) == NULL) 
					{
						perror(" inet_ntop \n");
					}
            	}
			
				int i;
				strcpy(mensaje_cliente.connection, "close");
				for (i = 0; i < sizeof(mensaje_cliente.orden_usuario); i++) 
				{
		            if (mensaje_cliente.orden_usuario[i] == 'k' && mensaje_cliente.orden_usuario[i-1] == ' ') 
					{
		                strcpy(mensaje_cliente.connection, "keep-alive");
		                break;
		            }
				}
				for (i = 0; i < sizeof(mensaje_cliente.orden_usuario); i++) 
				{
					if (mensaje_cliente.orden_usuario[i] == '/') 
					{
						while (mensaje_cliente.orden_usuario[i] != ' ') 
						{
							i++;
						}
						mensaje_cliente.orden_usuario[i] = '\0';		
						break;
					}				
				}
			
				strcat(mensaje_cliente.mensaje, mensaje_cliente.orden_usuario);
				strcat(mensaje_cliente.mensaje, " HTTP/1.1");
				strcat(mensaje_cliente.mensaje, "\r\n");			
				strcat(mensaje_cliente.mensaje, "Host: ");
				strcat(mensaje_cliente.mensaje, mensaje_cliente.hostname);
				strcat(mensaje_cliente.mensaje, "\r\n");
				strcat(mensaje_cliente.mensaje, "Connection: ");
				strcat(mensaje_cliente.mensaje, mensaje_cliente.connection);
				strcat(mensaje_cliente.mensaje, "\r\n");
				strcat(mensaje_cliente.mensaje, "\r\n");
			
				fprintf(fpe, "El mensaje enviado por el cliente es: %s", mensaje_cliente.mensaje);
				fflush(stdout);
			
				if (send(s, mensaje_cliente.mensaje, sizeof(mensaje_cliente.mensaje), 0) != sizeof(mensaje_cliente.mensaje)) 
				{
					fprintf(stderr, "%s: Connection aborted on error ",	argv[0]);
					fprintf(stderr, "on send number %d\n", i);
					exit(1);
				}
			} //FIN else if (strcmp(mensaje_cliente.orden_usuario, "ADIOS") == 0) 
	

			/* Now, shutdown the connection for further sends.
			* This will cause the server to receive an end-of-file
			* condition after it has received all the requests that
			* have just been sent, indicating that we will not be
			* sending any further requests.
			*/
	

			/* Now, start receiving all of the replys from the server.
			* This loop will terminate when the recv returns zero,
			* which is an end-of-file condition.  This will happen
			* after the server has sent all of its replies, and closed
			* its end of the connection.
			*/
		 
			char pagina[100] = "Paginas_web";	 
			int longitud_url_pagina;	
			int empieza = 0; 
			
			for(i = 0; i < sizeof(mensaje_cliente.orden_usuario); i++) 
			{
				if (mensaje_cliente.orden_usuario[i] == '/') 
				{
					longitud_url_pagina = 0;
					empieza = i;
				}
				if (empieza != 0) 
				{
					if (mensaje_cliente.orden_usuario[i] != ' ') 
					{
						longitud_url_pagina = longitud_url_pagina + 1;
					}
					else 
					{
						break;
					}
				}		
			}	
	
			strncat(pagina, &mensaje_cliente.orden_usuario[empieza], longitud_url_pagina);
	
			int num_trozos = 1;
						
			struct stat st;
			stat(pagina, &st);	 
			num_trozos = num_trozos + st.st_size/ TAMANNO_MAXIMO_TCP;

			char respuesta[32500];	
			printf("\n");
			for (int t = 0; t < num_trozos; t++) 
			{ 
				i = recv(s, respuesta, sizeof(respuesta), 0);
				if (i == -1) 
				{
					perror(argv[0]);
					fprintf(stderr, "%s: error reading result\n", argv[0]);
					exit(1);
				}
				/* The reason this while loop exists is that there
				* is a remote possibility of the above recv returning
				* less than TAM_BUFFER bytes.  This is because a recv returns
				* as soon as there is some data, and will not wait for
				* all of the requested data to arrive.  Since TAM_BUFFER bytes
				* is relatively small compared to the allowed TCP
				* packet sizes, a partial receive is unlikely.  If
				* this example had used 2048 bytes requests instead,
				* a partial receive would be far more likely.
				* This loop will keep receiving until all TAM_BUFFER bytes
				* have been received, thus guaranteeing that the
				* next recv at the top of the loop will start at
				* the begining of the next reply.
				*/

				/* Print out message indicating the identity of this reply. */

			
				for (i = 0; i < strlen(respuesta); i++) 
				{
					if (respuesta[i] == '\r' && respuesta[i+1] == '\n' && respuesta[i+2] == '\r' && respuesta[i+3] == '\n') 
					{
						i = i + 4;
						break;
					}
				}
				
				fprintf(fpe, "\nEl mensaje recibido del servidor es: %s", respuesta);
				fflush(stdout);
		
				printf("%s", &respuesta[i]);
				fflush(stdout);
				if (strcmp(&respuesta[i], "<html><body><h1>501 Not Implemented</h1></body></html>\n") == 0) 
				{
					break;
				}
		
				bzero(respuesta, sizeof(respuesta));
			} // FIN for (int t = 0; t < num_trozos; t++) 
		
			if (strcmp(mensaje_cliente.connection, "close") == 0) 
			{
				if (shutdown(s, 1) == -1) {
					perror(argv[0]);
					fprintf(stderr, "%s: unable to shutdown socket\n", argv[0]);
					exit(1);
				}
				break;	
			}
     	} // FIN while (a == 0)
     
     	fclose(fp);
     	fclose(fpe);
     	close(s);

    	/* Print message indicating completion of task. */
		time(&timevar);
		printf("All done at %s", (char *)ctime(&timevar));
	
	} //FIN if (strcmp(argv[2], "TCP") == 0) 	
    else if (strcmp(argv[2], "UDP") == 0) 
	{
     	s = socket (AF_INET, SOCK_DGRAM, 0);
		if (s == -1) 
		{
			perror(argv[0]);
			fprintf(stderr, "%s: unable to create socket\n", argv[0]);
			exit(1);
		}
	
    	/* clear out address structures */
		memset ((char *)&myaddr_in, 0, sizeof(struct sockaddr_in));
		memset ((char *)&servaddr_in, 0, sizeof(struct sockaddr_in));
	
		/* Bind socket to some local address so that the
		* server can send the reply back.  A port number
		* of zero will be used so that the system will
		* assign any available port number.  An address
		* of INADDR_ANY will be used so we do not have to
		* look up the internet address of the local host.
		*/
	
		myaddr_in.sin_family = AF_INET;
		myaddr_in.sin_port = 0;
		myaddr_in.sin_addr.s_addr = INADDR_ANY;
		if (bind(s, (const struct sockaddr *) &myaddr_in, sizeof(struct sockaddr_in)) == -1) 
		{
			perror(argv[0]);
			fprintf(stderr, "%s: unable to bind socket\n", argv[0]);
			exit(1);
	   	}
    
		addrlen = sizeof(struct sockaddr_in);
		if (getsockname(s, (struct sockaddr *)&myaddr_in, &addrlen) == -1) 
		{
			perror(argv[0]);
			fprintf(stderr, "%s: unable to read socket address\n", argv[0]);
			exit(1);
		}

        /* Print out a startup message for the user. */
    	time(&timevar);
        /* The port number must be converted first to host byte
        * order before printing.  On most hosts, this is not
        * necessary, but the ntohs() call is included here so
        * that this program could easily be ported to a host
        * that does require it.
        */
    
		printf("Connected to %s on port %u at %s", argv[1], ntohs(myaddr_in.sin_port), (char *) ctime(&timevar));

		/* Set up the server address. */
		servaddr_in.sin_family = AF_INET;
		/* Get the host information for the server's hostname that the
		 * user passed in.
		 */
      
	  	memset (&hints, 0, sizeof (hints));
      	hints.ai_family = AF_INET;
 	 	
		  /* esta funci�n es la recomendada para la compatibilidad con IPv6 gethostbyname queda obsoleta*/
    	errcode = getaddrinfo (argv[1], NULL, &hints, &res); 
    	if (errcode != 0)
		{
			/* Name was not found.  Return a
			 * special value signifying the error. */
			fprintf(stderr, "%s: No es posible resolver la IP de %s\n", argv[0], argv[1]);
			exit(1);
      	}
    	else 
		{
			/* Copy address of host */
			servaddr_in.sin_addr = ((struct sockaddr_in *) res->ai_addr)->sin_addr;
	 	}
     	
		 freeaddrinfo(res);
     	/* puerto del servidor en orden de red*/
	 	servaddr_in.sin_port = htons(PUERTO);

   		/* Registrar SIGALRM para no quedar bloqueados en los recvfrom */
    	vec.sa_handler = (void *) handler;
    	vec.sa_flags = 0;
    	if ( sigaction(SIGALRM, &vec, (struct sigaction *) 0) == -1) 
		{
            perror(" sigaction(SIGALRM)");
            fprintf(stderr,"%s: unable to register the SIGALRM signal\n", argv[0]);
            exit(1);
        }

		struct estructura_mensaje_cliente mensaje_cliente;
		bzero( (char *) &mensaje_cliente, sizeof(mensaje_cliente));
	
		FILE *fp;

		fp = fopen(argv[3], "r");
	
		FILE * fpe;
		char nombre_fichero[100];
		sprintf(nombre_fichero, "%u", ntohs(myaddr_in.sin_port));
		strcat(nombre_fichero, ".txt");
		fpe = fopen(nombre_fichero, "a");

        n_retry = RETRIES;
           
        if (fgets(mensaje_cliente.orden_usuario, sizeof(mensaje_cliente.orden_usuario), fp) == NULL) 
		{
			exit(1);
		}
        
        for (int k = 0; k < sizeof(mensaje_cliente.orden_usuario); k++) 
		{
            if(mensaje_cliente.orden_usuario[k] == '\n') 
			{
                mensaje_cliente.orden_usuario[k] = '\0';
            }
        }
           
        while (n_retry > 0) 
		{  
            int status;
            status = getnameinfo((struct sockaddr *)&myaddr_in,sizeof(myaddr_in), mensaje_cliente.hostname,sizeof(mensaje_cliente.hostname),NULL,0,0);
            if (status)
            {
            	if (inet_ntop(AF_INET, &(myaddr_in.sin_addr), mensaje_cliente.hostname, sizeof(mensaje_cliente.hostname)) == NULL) 
				{
                    perror(" inet_ntop \n");
                }
            }

            strcat(mensaje_cliente.mensaje, mensaje_cliente.orden_usuario);
            strcat(mensaje_cliente.mensaje, " HTTP/1.1");
            strcat(mensaje_cliente.mensaje, "\r\n");            
            strcat(mensaje_cliente.mensaje, "Host: ");
            strcat(mensaje_cliente.mensaje, mensaje_cliente.hostname);
            strcat(mensaje_cliente.mensaje, "\r\n");
            strcat(mensaje_cliente.mensaje, "Connection: close");
            strcat(mensaje_cliente.mensaje, "\r\n");
            strcat(mensaje_cliente.mensaje, "\r\n");
                
            fprintf(fpe, "El mensaje enviado por el cliente es: %s", mensaje_cliente.mensaje);
            fflush(stdout);

            if (sendto (s, mensaje_cliente.mensaje, strlen(mensaje_cliente.mensaje), 0, (struct sockaddr *)&servaddr_in, sizeof(struct sockaddr_in)) == -1) 
			{   
                perror(argv[0]);
                fprintf(stderr, "%s: unable to send request\n", argv[0]);
                exit(1);
            }

        	char pagina[100] = "Paginas_web";	 
			int longitud_url_pagina;	
			int empieza = 0; 
			
			for(i = 0; i < sizeof(mensaje_cliente.orden_usuario); i++) 
			{
				if (mensaje_cliente.orden_usuario[i] == '/') 
				{
					longitud_url_pagina = 0;
					empieza = i;
				}
				if (empieza != 0) 
				{
					if (mensaje_cliente.orden_usuario[i] != ' ') 
					{
						longitud_url_pagina = longitud_url_pagina + 1;
					}
					else 
					{
						break;
					}
				}		
			}	
	
			strncat(pagina, &mensaje_cliente.orden_usuario[empieza], longitud_url_pagina);
	
			int num_trozos = 1;
     			 
			struct stat st;
			stat(pagina, &st);	 
	
			num_trozos = num_trozos + st.st_size/ TAMANNO_MAXIMO_UDP;
            /* Set up a timeout so I don't hang in case the packet
            * gets lost.  After all, UDP does not guarantee
            * delivery.
            */
            for (int t = 0; t < num_trozos; t++) {
		    	alarm(TIMEOUT);
		    	/* Wait for the reply to come in. */
		    	char respuesta[1000];
		    	if (recvfrom (s, respuesta, sizeof(respuesta), 0,(struct sockaddr *)&servaddr_in, &addrlen) == -1) 
				{
					if (errno == EINTR) 
					{
						/* Alarm went off and aborted the receive.
						* Need to retry the request if we have
						* not already exceeded the retry limit.
						*/
						printf("attempt %d (retries %d).\n", n_retry, RETRIES);
						n_retry--; 
		            } 
		        	else  
					{
						printf("Unable to get response from");
						exit(1); 
		            }
		        } 
		    	else 
				{
					alarm(0);
					/* Print out response. */
					if (reqaddr.s_addr == ADDRNOTFOUND) 
					{
						printf("Host %s unknown by nameserver %s\n", argv[2], argv[1]);
					}
					else 
					{
						/* inet_ntop para interoperatividad con IPv6 */
						//if (inet_ntop(AF_INET, &reqaddr, hostname, MAXHOST) == NULL)
						//perror(" inet_ntop \n");
						//printf("Address for %s is %s\n", argv[2], hostname);
		           		for (i = 0; i < strlen(respuesta); i++) 
						{
							if (respuesta[i] == '\r' && respuesta[i+1] == '\n' && respuesta[i+2] == '\r' && respuesta[i+3] == '\n') 
							{
								i = i + 4;
								break;
							}
			     		}
			     		fprintf(fpe, "\nEl mensaje recibido del servidor es: %s", respuesta);
						fflush(stdout);
			     			     
			     		printf("%s", &respuesta[i]);
			     		fflush(stdout);
			     
			     		if (strcmp(&respuesta[i], "<html><body><h1>501 Not Implemented</h1></body></html>\n") == 0) 
						{
							break;
			     		}
		        	}      
		    	}		    
		    } //FIN for (int t = 0; t < num_trozos; t++) {
		    break;
		} //FIN while (n_retry > 0) 

		if (n_retry == 0) 
    	{
			printf("Unable to get response from");
			printf(" %s after %d attempts.\n", argv[1], RETRIES);
    	}
		
		close(s);
		fclose(fp);
		fclose(fpe);     	
    } //fin else if (strcmp(argv[2], "UDP") == 0) 
    else 
	{
     	fprintf(stderr, "Usage:  %s <nameserver> <TCP/UDP> <fichero-ordenes>\n", argv[0]);
		exit(1);
    }
}

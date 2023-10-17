/*

** Fichero: servidor.c
** Autores:
** Raúl Melgosa Salvador DNI 70925689Z
** Juan Carlos Velasco Sánchez DNI 70959966K
*/



/*
 *          		S E R V I D O R
 *
 *	This is an example program that demonstrates the use of
 *	sockets TCP and UDP as an IPC mechanism.  
 *
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#define PUERTO 12250
#define ADDRNOTFOUND	0xffffffff	/* return address for unfound host */
#define BUFFERSIZE	1024	/* maximum size of packets to be received */
#define TAM_BUFFER 500
#define MAXHOST 128
#define TAMANNO_MAXIMO_TCP 32000
#define TAMANNO_MAXIMO_UDP 400

extern int errno;

/*
 *			M A I N
 *
 *	This routine starts the server.  It forks, leaving the child
 *	to do all the work, so it does not have to be run in the
 *	background.  It sets up the sockets.  It
 *	will loop forever, until killed by a signal.
 *
 */
 
struct estructura_mensajes_log 
{
	char comunicacion_realizada[500];
	char comunicacion_finalizada[500];
	char nombre_ejecutable[50];
	char hostname[100];
	char hostname_IP[100];
	char puerto_efimero_cliente[50];
	char mensaje[500];
};

struct estructura_mensaje_servidorTCP 
{
	char mensaje_respuesta_servidor[TAMANNO_MAXIMO_TCP + 500]; //este buffer va a contener el mensaje de vuelta al cliente. Siempre empeiza por HTTP/1.1
	char orden_cliente[100]; //este buffer va a contener la orden que ha realizado el cliente
	char orden_implementada[10]; //este buffer contiene la orden que implementa el servidor. En nuestro caso solo implementamos la orden GET
	char paginaHTML[100]; //este buffer va a contener el nombre de la pagina html a obtener
	char directoriopaginaHTML[100]; //este buffer va a contener la ruta complata de la pagina html para poder abrila con fopen
	char cabecera[3]; //este buffer contiene la cabecera \r\n que llevan todas las lineas //puede que sea eliminado
	char servidor[100]; //este buffer seguramente sea eliminado		
	char connection[100]; //este buffer contiene el tipo de conxion (close o keep-alive)
	char contenido[100]; //este buffer seguramente sea eliminado
	char longitud_contenido[100]; //este buffer contiene la longitud de la pagina html enviada
	char contenido_paginaHTML[TAMANNO_MAXIMO_TCP]; //este buffer va a contener el contenido de la pagina HTML
};

struct estructura_mensaje_servidorUDP {
	char mensaje_respuesta_servidor[512]; //este buffer va a contener el mensaje de vuelta al cliente. Siempre empeiza por HTTP/1.1
	char orden_cliente[100]; //este buffer va a contener la orden que ha realizado el cliente
	char orden_implementada[10]; //este buffer contiene la orden que implementa el servidor. En nuestro caso solo implementamos la orden GET
	char paginaHTML[100]; //este buffer va a contener el nombre de la pagina html a obtener
	char directoriopaginaHTML[100]; //este buffer va a contener la ruta complata de la pagina html para poder abrila con fopen
	char cabecera[3]; //este buffer contiene la cabecera \r\n que llevan todas las lineas //puede que sea eliminado
	char servidor[100]; //este buffer seguramente sea eliminado		
	char connection[100]; //este buffer contiene el tipo de conxion (close o keep-alive)
	char contenido[100]; //este buffer seguramente sea eliminado
	char longitud_contenido[100]; //este buffer contiene la longitud de la pagina html enviada
	char contenido_paginaHTML[TAMANNO_MAXIMO_UDP]; //este buffer va a contener el contenido de la pagina HTML
};
 
void serverTCP(int s, struct sockaddr_in peeraddr_in, char * argv);
void serverUDP(int s, char * buf, struct sockaddr_in clientaddr_in, char * argv);
void errout(char *);		/* declare error out routine */

int FIN = 0;             /* Para el cierre ordenado */
void finalizar()
{ 
	FIN = 1; 
}

int main(argc, argv)
int argc;
char *argv[];
{

    int s_TCP, s_UDP;		/* connected socket descriptor */
    int ls_TCP;				/* listen socket descriptor */
    
    int cc;				    /* contains the number of bytes read */
     
    struct sigaction sa = {.sa_handler = SIG_IGN}; /* used to ignore SIGCHLD */
    
    struct sockaddr_in myaddr_in;	/* for local socket address */
    struct sockaddr_in clientaddr_in;	/* for peer socket address */
	int addrlen;
	
    fd_set readmask;
    int numfds,s_mayor;
    
    char buffer[BUFFERSIZE];	/* buffer for packets to be read into */
    
    struct sigaction vec;

		/* Create the listen socket. */
	ls_TCP = socket (AF_INET, SOCK_STREAM, 0);
	if (ls_TCP == -1) 
	{
		perror(argv[0]);
		fprintf(stderr, "%s: unable to create socket TCP\n", argv[0]);
		exit(1);
	}
	
	/* clear out address structures */
	memset ((char *)&myaddr_in, 0, sizeof(struct sockaddr_in));
   	memset ((char *)&clientaddr_in, 0, sizeof(struct sockaddr_in));

    addrlen = sizeof(struct sockaddr_in);

		/* Set up address structure for the listen socket. */
	myaddr_in.sin_family = AF_INET;
		/* The server should listen on the wildcard address,
		 * rather than its own internet address.  This is
		 * generally good practice for servers, because on
		 * systems which are connected to more than one
		 * network at once will be able to have one server
		 * listening on all networks at once.  Even when the
		 * host is connected to only one network, this is good
		 * practice, because it makes the server program more
		 * portable.
		 */
	myaddr_in.sin_addr.s_addr = INADDR_ANY;
	myaddr_in.sin_port = htons(PUERTO);

	/* Bind the listen address to the socket. */
	if (bind(ls_TCP, (const struct sockaddr *) &myaddr_in, sizeof(struct sockaddr_in)) == -1) 
	{
		perror(argv[0]);
		fprintf(stderr, "%s: unable to bind address TCP\n", argv[0]);
		exit(1);
	}
		/* Initiate the listen on the socket so remote users
		 * can connect.  The listen backlog is set to 5, which
		 * is the largest currently supported.
		 */
	if (listen(ls_TCP, 5) == -1) 
	{
		perror(argv[0]);
		fprintf(stderr, "%s: unable to listen on socket\n", argv[0]);
		exit(1);
	}
	
	
	/* Create the socket UDP. */
	s_UDP = socket (AF_INET, SOCK_DGRAM, 0);
	if (s_UDP == -1) 
	{
		perror(argv[0]);
		printf("%s: unable to create socket UDP\n", argv[0]);
		exit(1);
	}
	
	/* Bind the server's address to the socket. */
	if (bind(s_UDP, (struct sockaddr *) &myaddr_in, sizeof(struct sockaddr_in)) == -1) 
	{
		perror(argv[0]);
		printf("%s: unable to bind address UDP\n", argv[0]);
		exit(1);
	}

		/* Now, all the initialization of the server is
		 * complete, and any user errors will have already
		 * been detected.  Now we can fork the daemon and
		 * return to the user.  We need to do a setpgrp
		 * so that the daemon will no longer be associated
		 * with the user's control terminal.  This is done
		 * before the fork, so that the child will not be
		 * a process group leader.  Otherwise, if the child
		 * were to open a terminal, it would become associated
		 * with that terminal as its control terminal.  It is
		 * always best for the parent to do the setpgrp.
		 */
	setpgrp();

	switch (fork()) 
	{
		case -1:		/* Unable to fork, for some reason. */
			perror(argv[0]);
			fprintf(stderr, "%s: unable to fork daemon\n", argv[0]);
			exit(1);

		case 0:     /* The child process (daemon) comes here. */

				/* Close stdin and stderr so that they will not
				* be kept open.  Stdout is assumed to have been
				* redirected to some logging file, or /dev/null.
				* From now on, the daemon will not report any
				* error messages.  This daemon will loop forever,
				* waiting for connections and forking a child
				* server to handle each one.
				*/
			fclose(stdin);
			fclose(stderr);

			/* Set SIGCLD to SIG_IGN, in order to prevent
			 * the accumulation of zombies as each child
			 * terminates.  This means the daemon does not
			 * have to make wait calls to clean them up.
			 */
			if ( sigaction(SIGCHLD, &sa, NULL) == -1) 
			{
				perror(" sigaction(SIGCHLD)");
				fprintf(stderr,"%s: unable to register the SIGCHLD signal\n", argv[0]);
				exit(1);
			}
            
		    /* Registrar SIGTERM para la finalizacion ordenada del programa servidor */
			vec.sa_handler = (void *) finalizar;
			vec.sa_flags = 0;
			if ( sigaction(SIGTERM, &vec, (struct sigaction *) 0) == -1) 
			{
				perror(" sigaction(SIGTERM)");
				fprintf(stderr,"%s: unable to register the SIGTERM signal\n", argv[0]);
				exit(1);
			}
        
			while (!FIN) 
			{
				/* Meter en el conjunto de sockets los sockets UDP y TCP */
				FD_ZERO(&readmask);
				FD_SET(ls_TCP, &readmask);
				FD_SET(s_UDP, &readmask);
            
				/* 
				Seleccionar el descriptor del socket que ha cambiado. Deja una marca en 
				el conjunto de sockets (readmask)
				*/ 
				if (ls_TCP > s_UDP) s_mayor=ls_TCP;
				else s_mayor=s_UDP;

				if ( (numfds = select(s_mayor+1, &readmask, (fd_set *)0, (fd_set *)0, NULL)) < 0) 
				{
					if (errno == EINTR) 
					{
						FIN = 1;
						close (ls_TCP);
						close (s_UDP);
						perror("\nFinalizando el servidor. Se�al recibida en elect\n "); 
					}
				}
           		else 
				{ 
                	/* Comprobamos si el socket seleccionado es el socket TCP */
					if (FD_ISSET(ls_TCP, &readmask)) 
					{
						/* Note that addrlen is passed as a pointer
						* so that the accept call can return the
						* size of the returned address.
						*/
						/* This call will block until a new
						* connection arrives.  Then, it will
						* return the address of the connecting
						* peer, and a new socket descriptor, s,
						* for that connection.
						*/
						s_TCP = accept(ls_TCP, (struct sockaddr *) &clientaddr_in, &addrlen);
						if (s_TCP == -1) 
						{
							exit(1);	
						}
						
    					switch (fork()) 
						{
        					case -1:	/* Can't fork, just exit. */
        						exit(1);
        			
							case 0:		/* Child process comes here. */
                    			close(ls_TCP); /* Close the listen socket inherited from the daemon. */
        						serverTCP(s_TCP, clientaddr_in, argv[0]);
        						exit(0);
        			
							default:	/* Daemon process comes here. */
								/* The daemon needs to remember
								* to close the new accept socket
								* after forking the child.  This
								* prevents the daemon from running
								* out of file descriptor space.  It
								* also means that when the server
								* closes the socket, that it will
								* allow the socket to be destroyed
								* since it will be the last close.
								*/
        						close(s_TCP);
        				}
             		} /* De TCP*/
         
		 			 /* Comprobamos si el socket seleccionado es el socket UDP */
          			if (FD_ISSET(s_UDP, &readmask)) 
					{
						/* This call will block until a new
						* request arrives.  Then, it will
						* return the address of the client,
						* and a buffer containing its request.
						* BUFFERSIZE - 1 bytes are read so that
						* room is left at the end of the buffer
						* for a null character.
						*/
						cc = recvfrom(s_UDP, buffer, BUFFERSIZE - 1, 0, (struct sockaddr *)&clientaddr_in, &addrlen);
                		if ( cc == -1) 
						{
							perror(argv[0]);
							printf("%s: recvfrom error\n", argv[0]);
							exit (1);
                    	}
                    
						/* Make sure the message received is
						* null terminated.
						*/
						buffer[cc]='\0';
						serverUDP (s_UDP, buffer, clientaddr_in, argv[0]);
                	}
          		}
			}   /* Fin del bucle infinito de atenci�n a clientes */
			
			/* Cerramos los sockets UDP y TCP */
			close(ls_TCP);
			close(s_UDP);
    
        	printf("\nFin de programa servidor!\n");
        
		default:		/* Parent process comes here. */
			exit(0);
	}
}

/*
 *				S E R V E R T C P
 *
 *	This is the actual server routine that the daemon forks to
 *	handle each individual connection.  Its purpose is to receive
 *	the request packets from the remote client, process them,
 *	and return the results to the client.  It will also write some
 *	logging information to stdout.
 *
 */
void serverTCP(int s, struct sockaddr_in clientaddr_in, char *argv)
{
	int reqcnt = 0;		/* keeps count of number of requests */
	char buf[TAM_BUFFER];		/* This example uses TAM_BUFFER byte messages. */
	char hostname_decimal_punto[MAXHOST];
	char hostname[MAXHOST];		/* remote host's name string */

	int len, len1, status;
    struct hostent *hp;		/* pointer to host info for remote host */
    long timevar;			/* contains time returned by time() */
    
    struct linger linger;		/* allow a lingering, graceful close; */
    				            /* used when setting SO_LINGER */
    
    struct estructura_mensajes_log mensaje_log;
    struct estructura_mensaje_servidorTCP mensaje_servidor;
    bzero( (char *)&mensaje_log, sizeof(mensaje_log));
    bzero( (char *)&mensaje_servidor, sizeof(mensaje_servidor));

    				
	/* Look up the host information for the remote host
	 * that we have connected with.  Its internet address
	 * was returned by the accept call, in the main
	 * daemon loop above.
	 */


	 
    status = getnameinfo((struct sockaddr *)&clientaddr_in,sizeof(clientaddr_in), hostname,MAXHOST,NULL,0,0);
    if (status)
	{
        /* The information is unavailable for the remote
		* host.  Just format its internet address to be
		* printed out in the logging information.  The
		* address will be shown in "internet dot format".
		*/
		/* inet_ntop para interoperatividad con IPv6 */
    	if (inet_ntop(AF_INET, &(clientaddr_in.sin_addr), hostname, MAXHOST) == NULL)
		{
			perror(" inet_ntop \n");
		}
    }
    
	/* Log a startup message. */
    time (&timevar);
		/* The port number must be converted first to host byte
		 * order before printing.  On most hosts, this is not
		 * necessary, but the ntohs() call is included here so
		 * that this program could easily be ported to a host
		 * that does require it.
		 */
	
	printf("Startup from %s port %u at %s", hostname, ntohs(clientaddr_in.sin_port), (char *) ctime(&timevar));
		

		/* Set the socket for a lingering, graceful close.
		 * This will cause a final close of this socket to wait until all of the
		 * data sent on it has been received by the remote host.
		 */
	linger.l_onoff  =1;
	linger.l_linger =1;
	if (setsockopt(s, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger)) == -1) 
	{
		errout(hostname);
	}

		/* Go into a loop, receiving requests from the remote
		 * client.  After the client has sent the last request,
		 * it will do a shutdown for sending, which will cause
		 * an end-of-file condition to appear on this end of the
		 * connection.  After all of the client's requests have
		 * been received, the next recv call will return zero
		 * bytes, signalling an end-of-file condition.  This is
		 * how the server will know that no more requests will
		 * follow, and the loop will be exited.
		 */
		 		 
	FILE * flog;	 
	flog = fopen("peticiones.log", "a");	
	
	char comunicacion_realizada[500] = "Comunicacion realizada: ";
	char puerto_efimero_cliente[50];	 
	
	fwrite((char *) ctime(&timevar), strlen(ctime(&timevar)), 1, flog);
	fwrite(argv, strlen(argv), 1, flog);
	fwrite("\n", 1, 1, flog);
	
	strcat(mensaje_log.comunicacion_realizada, "\nComunicacion realizada: ");
	strcat(mensaje_log.comunicacion_realizada, hostname);
	
	if (inet_ntop(AF_INET, &(clientaddr_in.sin_addr), hostname_decimal_punto, MAXHOST) == NULL) {
		perror(" inet_ntop \n");	
	}
	strcat(mensaje_log.comunicacion_realizada, " ");
	strcat(mensaje_log.comunicacion_realizada, hostname_decimal_punto);
	strcat(mensaje_log.comunicacion_realizada, " ");
	strcat(mensaje_log.comunicacion_realizada, "TCP");
	strcat(mensaje_log.comunicacion_realizada, " ");
	sprintf(mensaje_log.puerto_efimero_cliente, "%u", ntohs(clientaddr_in.sin_port));
	strcat(mensaje_log.comunicacion_realizada, mensaje_log.puerto_efimero_cliente);	
	fwrite(mensaje_log.comunicacion_realizada, strlen(mensaje_log.comunicacion_realizada), 1, flog);
	fwrite("\n", 1, 1, flog);
	
				 
	while (len = recv(s, buf, TAM_BUFFER, 0)) 
	{
		if (len == -1) 
		{
			errout(hostname);
		}   /* error from recv */
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
			 * the begining of the next request.
			 */
			 	 
		bzero( (char *)&mensaje_log, sizeof(mensaje_log));
        bzero( (char *)&mensaje_servidor, sizeof(mensaje_servidor));	 			 
						
		int i,j, indice = 0;

	        
	    for (i = 0; i < sizeof(buf); i++) 
		{
		    if (buf[i] == '/') 
			{
		        indice = i;
		        strncpy(mensaje_servidor.orden_cliente, buf, indice-1);
		        break; 
		    }
		}
	            
		//extraer el directorio
        indice = indice + 1;
        i = 0;
        while (buf[indice] != ' ') 
		{
            mensaje_servidor.paginaHTML[i] = buf[indice];
            indice++;
            i++;
        }
                   
        //buscar el directorio
        FILE *fp;
                   
        strcat(mensaje_servidor.directoriopaginaHTML, "Paginas_web/");
        strcat(mensaje_servidor.directoriopaginaHTML, mensaje_servidor.paginaHTML);
        fp = fopen(mensaje_servidor.directoriopaginaHTML, "r");
                   
        int num_trozos = 1;
		char * todo;
		struct stat st;
		stat(mensaje_servidor.directoriopaginaHTML, &st);
		num_trozos = num_trozos + st.st_size / TAMANNO_MAXIMO_TCP;
		for (int t = 0; t < num_trozos; t++) 
		{
		
			for (i = 0; i < sizeof(buf); i++) 
			{
				if (buf[i] == '/') 
				{
					indice = i;
					strncpy(mensaje_servidor.orden_cliente, buf, indice-1);
					break; 
				}
			}
		
			if (strcmp(mensaje_servidor.orden_cliente, "GET") == 0) 
			{                                     
                if (fp == NULL) 
				{                  	
                  	sprintf(mensaje_log.mensaje, "\n404 Not Found. La pagina %s no se ha encontrado", mensaje_servidor.paginaHTML);
                   	fwrite(mensaje_log.mensaje, strlen(mensaje_log.mensaje), 1, flog); 
                   	
                   	strcat(mensaje_servidor.contenido_paginaHTML, "<html><body><h1>404 Not Found</h1></body></html>\n");
                   	sprintf(mensaje_servidor.longitud_contenido, "%ld", strlen(mensaje_servidor.contenido_paginaHTML));
                   	              
                   	strcat(mensaje_servidor.mensaje_respuesta_servidor, "HTTP/1.1 ");   
                   	strcat(mensaje_servidor.mensaje_respuesta_servidor, "404 Not Found");
                   	strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
                   	strcat(mensaje_servidor.mensaje_respuesta_servidor, "Server: ");
                   	strcat(mensaje_servidor.mensaje_respuesta_servidor, hostname);
                   	strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
                   	
                   	                
                   	int veces = 0;
                   	int indice = 0;
                   	for (i = 0; i < sizeof(buf); i++) 
					{ //buscamos el tipo de conexion para devolver lo mismo
                       	if (buf[i] == '\r' && buf[i+1] == '\n') 
						{
		         			veces++; //se ha leido la primera linea
		        		}
		        		if (buf[i] == '\r' && buf[i+1] == '\n' && veces == 2) 
						{
		          			i = i + 2;
		          			break; 
		        		}
		   			}
		   	
			   		int j = i;
		   	
			   		while (buf[i] != '\r' && buf[i+1] != '\n') 
					{
		       			indice++;
		       			i++;
		   			}		  				
					strncpy(mensaje_servidor.connection, &buf[j], indice);
						
				
					strcat(mensaje_servidor.mensaje_respuesta_servidor, mensaje_servidor.connection);
					strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
				
					strcat(mensaje_servidor.mensaje_respuesta_servidor, "Content_Length: ");
					strcat(mensaje_servidor.mensaje_respuesta_servidor, mensaje_servidor.longitud_contenido);
					strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
					strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
					strcat(mensaje_servidor.mensaje_respuesta_servidor, mensaje_servidor.contenido_paginaHTML); 
                   	
                }
                else 
				{                   	
                   	if (t == 0) 
					{
                   		sprintf(mensaje_log.mensaje, "\n200 OK. La pagina %s se ha encontrado correctamente", mensaje_servidor.paginaHTML);
                   		fwrite(mensaje_log.mensaje, strlen(mensaje_log.mensaje), 1, flog); 
                   		todo = malloc(st.st_size);
                   		bzero(todo, sizeof(todo));
                   		fread(todo, st.st_size, 1, fp); 
                   	}
                   	
					if (num_trozos != t+1) 
					{
						strncpy(mensaje_servidor.contenido_paginaHTML, &todo[TAMANNO_MAXIMO_TCP * t], TAMANNO_MAXIMO_TCP);
					}
					else 
					{
						strncpy(mensaje_servidor.contenido_paginaHTML, &todo[TAMANNO_MAXIMO_TCP * t], st.st_size - (TAMANNO_MAXIMO_TCP * t));
					}
                   	
                   	fread(mensaje_servidor.contenido_paginaHTML, sizeof(mensaje_servidor.contenido_paginaHTML), 1, fp);	
                   	sprintf(mensaje_servidor.longitud_contenido, "%ld", strlen(mensaje_servidor.contenido_paginaHTML));	          
                   
                    strcat(mensaje_servidor.mensaje_respuesta_servidor, "HTTP/1.1 "); 
                   	strcat(mensaje_servidor.mensaje_respuesta_servidor, "200 OK");
                   	strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
                   	strcat(mensaje_servidor.mensaje_respuesta_servidor, "Server: ");
                   	strcat(mensaje_servidor.mensaje_respuesta_servidor, hostname);
                   	strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
                   
                   	int veces = 0;
                   	int indice = 0;
                   	for (i = 0; i < sizeof(buf); i++) 
					{ //buscamos el tipo de conexion para devolver lo mismo
                       	if (buf[i] == '\r' && buf[i+1] == '\n') 
						{
		         			veces++; //se ha leido la primera linea
		        		}
		        		if (buf[i] == '\r' && buf[i+1] == '\n' && veces == 2) 
						{
							i = i + 2;
							break; 
		        		}
		   			}
		   	
			   		int j = i;
		   	
			   		while (buf[i] != '\r' && buf[i+1] != '\n') 
					{
						indice++;
						i++;
		   			}
		   				
					strncpy(mensaje_servidor.connection, &buf[j], indice);
						
				
					strcat(mensaje_servidor.mensaje_respuesta_servidor, mensaje_servidor.connection);
					strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
				
					strcat(mensaje_servidor.mensaje_respuesta_servidor, "Content_Length: ");
					strcat(mensaje_servidor.mensaje_respuesta_servidor, mensaje_servidor.longitud_contenido);
					strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
					strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
					strcat(mensaje_servidor.mensaje_respuesta_servidor, mensaje_servidor.contenido_paginaHTML);   						              	
                }
            } //Fin if (strcmp(mensaje_servidor.orden_cliente, "GET") == 0) 
			else 
			{
				num_trozos = 1;		    
                   
                sprintf(mensaje_log.mensaje, "\n505 Not Implemented. La orden %s no esta implementada", mensaje_servidor.orden_cliente);
                fwrite(mensaje_log.mensaje, strlen(mensaje_log.mensaje), 1, flog); 
                   
                strcat(mensaje_servidor.contenido_paginaHTML, "<html><body><h1>501 Not Implemented</h1></body></html>\n");
                sprintf(mensaje_servidor.longitud_contenido, "%ld", strlen(mensaje_servidor.contenido_paginaHTML));
                   
                strcat(mensaje_servidor.mensaje_respuesta_servidor, "HTTP/1.1 "); 
                strcat(mensaje_servidor.mensaje_respuesta_servidor, "501 Not Implemented");
                strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
                strcat(mensaje_servidor.mensaje_respuesta_servidor, "Server: ");
                strcat(mensaje_servidor.mensaje_respuesta_servidor, hostname);
                strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
                   
                int veces = 0;
                int indice = 0;
                for (i = 0; i < sizeof(buf); i++) //buscamos el tipo de conexion para devolver lo mismo
				{ 
                    if (buf[i] == '\r' && buf[i+1] == '\n') 
					{
		          		veces++; //se ha leido la primera linea
		       		}
		       		if (buf[i] == '\r' && buf[i+1] == '\n' && veces == 2) 
					{
		          		i = i + 2;
		          		break; 
		       		}
		   		}
		   
		   		int j = i;
		   
		   		while (buf[i] != '\r' && buf[i+1] != '\n') 
				{
		       		indice++;
		       		i++;
		   		}
		   				
				strncpy(mensaje_servidor.connection, &buf[j], indice);
						
				
				strcat(mensaje_servidor.mensaje_respuesta_servidor, mensaje_servidor.connection);
				strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
				
				strcat(mensaje_servidor.mensaje_respuesta_servidor, "Content_Length: ");
				strcat(mensaje_servidor.mensaje_respuesta_servidor, mensaje_servidor.longitud_contenido);
				strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
				strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
				strcat(mensaje_servidor.mensaje_respuesta_servidor, mensaje_servidor.contenido_paginaHTML);                   
			}
                              
            usleep(10000); //simulamos el procesamiento real de un servidor
               
            if (send(s, mensaje_servidor.mensaje_respuesta_servidor, sizeof(mensaje_servidor.mensaje_respuesta_servidor), 0) != sizeof(mensaje_servidor.mensaje_respuesta_servidor)) 
			{
				errout(hostname);		
	      	}
	       
		   	bzero(mensaje_servidor.mensaje_respuesta_servidor, sizeof(mensaje_servidor.mensaje_respuesta_servidor));
         	bzero(mensaje_servidor.contenido_paginaHTML, sizeof(mensaje_servidor.contenido_paginaHTML));
	    } //FIN for (int t = 0; t < num_trozos; t++) 
	       

	    for (i = 0; i < strlen(mensaje_servidor.connection); i++) 
		{
	       	if (mensaje_servidor.connection[i] == ':') 
			{
	       	    i = i + 2;
	       	    break;
	       	}
	    }
	       
	       
	    if (strcmp(mensaje_servidor.connection + i, "keep-alive") != 0) 
		{
	       	if (shutdown(s, SHUT_RDWR) == -1) 
			{
				perror(argv);
				fprintf(stderr, "%s: unable to shutdown socket\n", argv);
				exit(1);
			}
	       	break;
	    }				 
	} //FIN while (len = recv(s, buf, TAM_BUFFER, 0))	
	
	time (&timevar);
	
	char comunicacion_finalizada[500] = "Comunicacion finalizada: ";
	
	fwrite("\n", 1, 1, flog);
	fwrite("\n", 1, 1, flog);
		
	fwrite((char *) ctime(&timevar), strlen(ctime(&timevar)), 1, flog);
	fwrite(argv, strlen(argv), 1, flog);
	fwrite("\n", 1, 1, flog);
	
	strcat(mensaje_log.comunicacion_finalizada, "Comunicacion finalizada: ");
	strcat(mensaje_log.comunicacion_finalizada, hostname);
	
	if (inet_ntop(AF_INET, &(clientaddr_in.sin_addr), hostname, MAXHOST) == NULL) 
	{
		perror(" inet_ntop \n");	
	}
	
	strcat(mensaje_log.comunicacion_finalizada, " ");
	strcat(mensaje_log.comunicacion_finalizada, hostname);
	strcat(mensaje_log.comunicacion_finalizada, " ");
	strcat(mensaje_log.comunicacion_finalizada, "TCP");
	strcat(mensaje_log.comunicacion_finalizada, " ");
	sprintf(mensaje_log.puerto_efimero_cliente, "%u", ntohs(clientaddr_in.sin_port));
	strcat(mensaje_log.comunicacion_finalizada, mensaje_log.puerto_efimero_cliente);
	strcat(mensaje_log.comunicacion_finalizada, "\n");
	fwrite(mensaje_log.comunicacion_finalizada, strlen(mensaje_log.comunicacion_finalizada), 1, flog);	

		/* The loop has terminated, because there are no
		 * more requests to be serviced.  As mentioned above,
		 * this close will block until all of the sent replies
		 * have been received by the remote host.  The reason
		 * for lingering on the close is so that the server will
		 * have a better idea of when the remote has picked up
		 * all of the data.  This will allow the start and finish
		 * times printed in the log file to reflect more accurately
		 * the length of time this connection was used.
		 */
	sleep(1);
	close(s);

		/* Log a finishing message. */
	time (&timevar);
		/* The port number must be converted first to host byte
		 * order before printing.  On most hosts, this is not
		 * necessary, but the ntohs() call is included here so
		 * that this program could easily be ported to a host
		 * that does require it.
		 */
	printf("Completed %s port %u, %d requests, at %s\n", hostname, ntohs(clientaddr_in.sin_port), reqcnt, (char *) ctime(&timevar));
}

/*
 *	This routine aborts the child process attending the client.
 */
void errout(char *hostname)
{
	printf("Connection with %s aborted on error\n", hostname);
	exit(1);     
}


/*
 *				S E R V E R U D P
 *
 *	This is the actual server routine that the daemon forks to
 *	handle each individual connection.  Its purpose is to receive
 *	the request packets from the remote client, process them,
 *	and return the results to the client.  It will also write some
 *	logging information to stdout.
 *
 */
void serverUDP(int s, char * buf, struct sockaddr_in clientaddr_in, char * argv)
{
    struct in_addr reqaddr;	/* for requested host's address */
    struct hostent *hp;		/* pointer to host info for requested host */
    int nc, errcode, status;

    struct addrinfo hints, *res;
    
    char hostname[MAXHOST];
    char hostname_decimal_punto[MAXHOST];
    
    struct estructura_mensajes_log mensaje_log;
    struct estructura_mensaje_servidorUDP mensaje_servidor;

    int addrlen;
    long timevar;
	
    
   	addrlen = sizeof(struct sockaddr_in);

    memset (&hints, 0, sizeof (hints));
      
    status = getnameinfo((struct sockaddr *)&clientaddr_in,sizeof(clientaddr_in), hostname,MAXHOST,NULL,0,0);
    if (status)
	{
           	/* The information is unavailable for the remote
			 * host.  Just format its internet address to be
			 * printed out in the logging information.  The
			 * address will be shown in "internet dot format".
			 */
			 /* inet_ntop para interoperatividad con IPv6 */
		if (inet_ntop(AF_INET, &(clientaddr_in.sin_addr), hostname, MAXHOST) == NULL) 
		{
			perror(" inet_ntop \n");
		}
    }

	time (&timevar);
	
	printf("Startup from %s port %u at %s", hostname, ntohs(clientaddr_in.sin_port), (char *) ctime(&timevar));
               
	FILE * flog;	 
	flog = fopen("peticiones.log", "a");	
	
	char comunicacion_realizada[500] = "\nComunicacion realizada: ";
	char puerto_efimero_cliente[50];	 
	
	fwrite((char *) ctime(&timevar), strlen(ctime(&timevar)), 1, flog);
	fwrite(argv, strlen(argv), 1, flog);
	fwrite("\n", 1, 1, flog);
	
	strcat(mensaje_log.comunicacion_realizada, "Comunicacion realizada: ");
	strcat(mensaje_log.comunicacion_realizada, hostname);
	
	if (inet_ntop(AF_INET, &(clientaddr_in.sin_addr), hostname_decimal_punto, MAXHOST) == NULL) 
	{
		perror(" inet_ntop \n");	
	}
	
	strcat(mensaje_log.comunicacion_realizada, " ");
	strcat(mensaje_log.comunicacion_realizada, hostname_decimal_punto);
	strcat(mensaje_log.comunicacion_realizada, " ");
	strcat(mensaje_log.comunicacion_realizada, "UDP");
	strcat(mensaje_log.comunicacion_realizada, " ");
	sprintf(mensaje_log.puerto_efimero_cliente, "%u", ntohs(clientaddr_in.sin_port));
	strcat(mensaje_log.comunicacion_realizada, mensaje_log.puerto_efimero_cliente);	
	fwrite(mensaje_log.comunicacion_realizada, strlen(mensaje_log.comunicacion_realizada), 1, flog);
	fwrite("\n", 1, 1, flog);
			 
	bzero( (char *)&mensaje_log, sizeof(mensaje_log));
    bzero( (char *)&mensaje_servidor, sizeof(mensaje_servidor));	 			 
			                
    int i,j, indice = 0;
		
	for (i = 0; i < sizeof(buf); i++) 
	{
		if (buf[i] == '/') 
		{
		    indice = i;
		    strncpy(mensaje_servidor.orden_cliente, buf, indice-1);
		    break; 
		}
	}
		
	//extraer el directorio
    indice = indice + 1;
    i = 0;
    while (buf[indice] != ' ') 
	{
        mensaje_servidor.paginaHTML[i] = buf[indice];
        indice++;
        i++;
    }
                   
    //buscar el directorio
    FILE *fp;
                   
	strcat(mensaje_servidor.directoriopaginaHTML, "Paginas_web/");
    strcat(mensaje_servidor.directoriopaginaHTML, mensaje_servidor.paginaHTML);
    fp = fopen(mensaje_servidor.directoriopaginaHTML, "r");
		
	int num_trozos = 1;
	char * todo;
	struct stat st;
	stat(mensaje_servidor.directoriopaginaHTML, &st);
	num_trozos = num_trozos + st.st_size / TAMANNO_MAXIMO_UDP;
	for (int t = 0; t < num_trozos; t++) 
	{
		if (strcmp(mensaje_servidor.orden_cliente, "GET") == 0 || strncmp(&buf[0], "GET", 3) == 0)  
		{                        
            if (fp == NULL) 
			{                  	
                sprintf(mensaje_log.mensaje, "\n404 Not Found. La pagina %s no se ha encontrado", mensaje_servidor.paginaHTML);
                fwrite(mensaje_log.mensaje, strlen(mensaje_log.mensaje), 1, flog); 
                   	
                strcat(mensaje_servidor.contenido_paginaHTML, "<html><body><h1>404 Not Found</h1></body></html>\n");
                sprintf(mensaje_servidor.longitud_contenido, "%ld", strlen(mensaje_servidor.contenido_paginaHTML));
                   	              
                strcat(mensaje_servidor.mensaje_respuesta_servidor, "HTTP/1.1 ");   
                strcat(mensaje_servidor.mensaje_respuesta_servidor, "404 Not Found");
                strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
                strcat(mensaje_servidor.mensaje_respuesta_servidor, "Server: ");
                strcat(mensaje_servidor.mensaje_respuesta_servidor, hostname);
                strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
                   	
            		   		   				   
				strcat(mensaje_servidor.mensaje_respuesta_servidor, "Connecion: close");   
				strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
			
				strcat(mensaje_servidor.mensaje_respuesta_servidor, "Content_Length: ");
				strcat(mensaje_servidor.mensaje_respuesta_servidor, mensaje_servidor.longitud_contenido);
				strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
				strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
				strcat(mensaje_servidor.mensaje_respuesta_servidor, mensaje_servidor.contenido_paginaHTML); 
            }
            else 
			{                   	
                if (t == 0) 
				{
                   	sprintf(mensaje_log.mensaje, "\n200 OK. La pagina %s se ha encontrado correctamente", mensaje_servidor.paginaHTML);
                   	fwrite(mensaje_log.mensaje, strlen(mensaje_log.mensaje), 1, flog);
                   	todo = malloc(st.st_size);
                   	fread(todo, st.st_size, 1, fp);
                }
                   	
				if (num_trozos != t+1) 
				{
					strncpy(mensaje_servidor.contenido_paginaHTML, &todo[TAMANNO_MAXIMO_UDP * t], TAMANNO_MAXIMO_UDP);
				}
				else 
				{
					strncpy(mensaje_servidor.contenido_paginaHTML, &todo[TAMANNO_MAXIMO_UDP * t], st.st_size - (TAMANNO_MAXIMO_UDP * t));
				}
	
                sprintf(mensaje_servidor.longitud_contenido, "%ld", strlen(mensaje_servidor.contenido_paginaHTML));	         
                   
                strcat(mensaje_servidor.mensaje_respuesta_servidor, "HTTP/1.1 "); 
                strcat(mensaje_servidor.mensaje_respuesta_servidor, "200 OK");
                strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
                strcat(mensaje_servidor.mensaje_respuesta_servidor, "Server: ");
                strcat(mensaje_servidor.mensaje_respuesta_servidor, hostname);
                strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
                   	
            	strcat(mensaje_servidor.mensaje_respuesta_servidor, "Connecion: close"); 
		   		strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
		   
				strcat(mensaje_servidor.mensaje_respuesta_servidor, "Content_Length: ");
				strcat(mensaje_servidor.mensaje_respuesta_servidor, mensaje_servidor.longitud_contenido);
				strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
				strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
				strcat(mensaje_servidor.mensaje_respuesta_servidor, mensaje_servidor.contenido_paginaHTML);   		             	
            }
                   
		} //FIN if (strcmp(mensaje_servidor.orden_cliente, "GET") == 0)
		else 
		{
		    num_trozos = 1;		    
                                      
            sprintf(mensaje_log.mensaje, "\n501 Not Implemented. La orden %s no esta implementada", mensaje_servidor.orden_cliente);
            fwrite(mensaje_log.mensaje, strlen(mensaje_log.mensaje), 1, flog); 
                   
            strcat(mensaje_servidor.contenido_paginaHTML, "<html><body><h1>501 Not Implemented</h1></body></html>\n");
            sprintf(mensaje_servidor.longitud_contenido, "%ld", strlen(mensaje_servidor.contenido_paginaHTML));
                   
            strcat(mensaje_servidor.mensaje_respuesta_servidor, "HTTP/1.1 "); 
            strcat(mensaje_servidor.mensaje_respuesta_servidor, "501 Not Implemented");
            strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
            strcat(mensaje_servidor.mensaje_respuesta_servidor, "Server: ");
            strcat(mensaje_servidor.mensaje_respuesta_servidor, hostname);
            strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");

                   
		   	strcat(mensaje_servidor.mensaje_respuesta_servidor, "Connecion: close"); 
		   	strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
		   
		   	strcat(mensaje_servidor.mensaje_respuesta_servidor, "Content_Length: ");
		   	strcat(mensaje_servidor.mensaje_respuesta_servidor, mensaje_servidor.longitud_contenido);
		   	strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
		   	strcat(mensaje_servidor.mensaje_respuesta_servidor, "\r\n");
		   	strcat(mensaje_servidor.mensaje_respuesta_servidor, mensaje_servidor.contenido_paginaHTML);                   
		}
                              
        usleep(10000); //simulamos el procesamiento real de un servidor
               
        nc = sendto (s, mensaje_servidor.mensaje_respuesta_servidor, sizeof(mensaje_servidor.mensaje_respuesta_servidor), 0, (struct sockaddr *)&clientaddr_in, addrlen);
		if ( nc == -1) 
		{
         	perror("serverUDP");
         	printf("%s: sendto error\n", "serverUDP");
         	return;
        } 
         	
		bzero(mensaje_servidor.mensaje_respuesta_servidor, sizeof(mensaje_servidor.mensaje_respuesta_servidor));
        bzero(mensaje_servidor.contenido_paginaHTML, sizeof(mensaje_servidor.contenido_paginaHTML));
    } //FIN for (int t = 0; t < num_trozos; t++)
        
	       
	time (&timevar);
				 	
	char comunicacion_finalizada[500] = "Comunicacion finalizada: ";
	
	fwrite("\n", 1, 1, flog);
	fwrite("\n", 1, 1, flog);
		
	fwrite((char *) ctime(&timevar), strlen(ctime(&timevar)), 1, flog);
	fwrite(argv, strlen(argv), 1, flog);
	fwrite("\n", 1, 1, flog);
	
	strcat(mensaje_log.comunicacion_finalizada, "Comunicacion finalizada: ");
	strcat(mensaje_log.comunicacion_finalizada, hostname);
	
	if (inet_ntop(AF_INET, &(clientaddr_in.sin_addr), hostname, MAXHOST) == NULL) 
	{
		perror(" inet_ntop \n");	
	}
	
	strcat(mensaje_log.comunicacion_finalizada, " ");
	strcat(mensaje_log.comunicacion_finalizada, hostname);
	strcat(mensaje_log.comunicacion_finalizada, " ");
	strcat(mensaje_log.comunicacion_finalizada, "UDP"); 
	strcat(mensaje_log.comunicacion_finalizada, " ");
	sprintf(mensaje_log.puerto_efimero_cliente, "%u", ntohs(clientaddr_in.sin_port));
	strcat(mensaje_log.comunicacion_finalizada, mensaje_log.puerto_efimero_cliente);
	strcat(mensaje_log.comunicacion_finalizada, "\n");
	fwrite(mensaje_log.comunicacion_finalizada, strlen(mensaje_log.comunicacion_finalizada), 1, flog);
					 
	fclose(flog);
	
	//printf("Completed %s port %u, at %s\n", hostname, ntohs(clientaddr_in.sin_port), (char *) ctime(&timevar));
}

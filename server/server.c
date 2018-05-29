
#include "fileshareHeader.h"


int main( int argc, char** argv )
{
	if( argc < 2 )
	{
		printf("\tError : less number of arguments\n");
		printf("\te.g.  : ./runserver serverfile.txt\n");
		return 1;
	}	
	if( access(argv[1],F_OK) != 0 ) 
        {
                printf("\tError : no such file exists ( error = %s)\n", strerror(errno) );
		return 1;
        }

	/* Defining Variables */
	char exitserver = '\0' ;

	int sockfd = -1 ;	// server side socket fd
	int clientsockfd = -1 ; // fd accepts client connection

	struct sockaddr_in client_addr; /* Internet Specific(AF_INET) client socket address stucture */
	struct sockaddr_in server_addr; /* Internet Specific(AF_INET) server socket address stucture */
	memset(&client_addr,0,sizeof(client_addr));
	memset(&server_addr,0,sizeof(server_addr));

	int sin_size = 0 ;	// used to finding size of struct sockaddr_in

	/* socket(), Get the Socket file descriptor */
	if( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
	{
		printf("\tERROR : Failed to obtain Socket Descriptor ( error = %s )\n", strerror(errno) );
		return 1;
	}
	else 
		printf("[Server] Obtaining socket descriptor successfully.\n");


	/* Fill the server socket address struct */
	server_addr.sin_family = AF_INET;		// Protocol Family
	server_addr.sin_port = htons(PORT); 		// Port number
	server_addr.sin_addr.s_addr = INADDR_ANY; 	// AutoFill local address
	bzero(&(server_addr.sin_zero), 8); 		// Flush the rest of struct
		/*      IPv4 AF_INET sockets:
                struct sockaddr_in {
                        short            sin_family;   // e.g. AF_INET, AF_INET6
                        unsigned short   sin_port;     // e.g. htons(3490)
                        struct in_addr   sin_addr;     // see struct in_addr, below
                        char             sin_zero[8];  // zero this if you want to
                        };

                struct in_addr {
                        unsigned long s_addr;          // load with inet_pton()
                        };
 	       */

	/* bind(), Bind a special Port */
	if( bind( sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1 ) // cast server_addr to pointer type (struct sockaddr*)
	{
		printf("\tERROR : Failed to Bind Port ( error = %s )\n", strerror(errno) );
		close(sockfd);
		return 1;
	}
	else 
		printf("[Server] Binded tcp port %d in addr 127.0.0.1 sucessfully.\n",PORT);


	/* listen(), Listen remote connection/calling */
	if( listen( sockfd, BACKLOG ) == -1 )	// BACKLOG : limlit to pending connections in queue 
	{
		printf("\tERROR: Failed to Listen Port ( error = %s )\n", strerror(errno) );
		close(sockfd);
		return 1;
	}
	else
		printf ("[Server] Listening the port %d successfully.\n", PORT);


	sin_size = sizeof(struct sockaddr_in);

	/* accept(), Wait a connection, and obtain a new socket file despriptor for single connection */
	if (( clientsockfd = accept(sockfd, (struct sockaddr*)&client_addr, &sin_size)) == -1) // cast client_addr to pointer type (struct sockaddr*)
	{
		printf("\tERROR : Obtaining new Socket Despcritor ( error = %s )\n", strerror(errno) );
		close(sockfd);
		return 1;
	}
	else
		printf("[Server] Server has got connected from %s.\n", inet_ntoa(client_addr.sin_addr)); // convert Inet no. to ASCII		

	/* server is in continuous running state */
	while(1)
	{
		/* call to receive function */
		printf("[Server] ready to receive file\n");
		if( !recvFile( &clientsockfd ) )
		{
			printf("[Server] file received successfully\n");
		}
		else
		{
			printf("[Server] operation failed\n");
		}

		/* call to send function */
		printf("[Server] ready to send file\n");
        	if( !sendFile( &clientsockfd, argv[1]) )
        	{
                	printf("[Server] File sent\n");
        	}
		else
		{
			printf("[Server] operation failed\n");	
		}

		printf("[Server] : wanted to exit server ( y / n ) : ");
		scanf("%c",&exitserver);
		printf("\n");
		if( exitserver == 'y' )
			break;
	}

	close(clientsockfd);
	close(sockfd);
	return 0;
}

//############################################################################################################


#include "fileshareHeader.h"

int main( int argc, char **argv )
{	
        if( argc < 2 )
        {
                printf("\tError : less number of arguments\n");
                printf("\te.g.  : ./runclient clientfile.txt\n");
                return 1;
        }
        if( access(argv[1],F_OK) != 0 )
        {
                printf("\tError : no such file exists ( error = %s)\n", strerror(errno) );
	            	return 1;
        }


	/* Variable Definition */
	int sockfd = -1 ; 

	struct sockaddr_in server_addr; // Internet Specific(AF_INET) server socket address stucture 
	memset(&server_addr,0,sizeof(server_addr));


	/* socket(), Get the Socket file descriptor */
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
		printf("\tERROR : Failed to obtain Socket Descriptor! ( error = %s )\n", strerror(errno) );
		return 1;
	}

	/* Fill the socket address struct */
	server_addr.sin_family = AF_INET; 
	server_addr.sin_port = htons(PORT); 
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); // converts readable-ip into network form
	bzero(&(server_addr.sin_zero), 8);

	/* connect(), Try to connect the server */
	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
	{
		printf("\tERROR : Failed to connect to the host! ( error = %s )\n", strerror(errno) );
		close(sockfd);
		return 1;
	}
	else 
		printf("[Client] Connected to server at port %d...ok!\n", PORT);	

	/* call to send function */
       	printf("[Client] ready to send file : %s\n",argv[1]);
	if( !sendFile( &sockfd, argv[1]) )
	{
		printf("[Client] File sent\n");
	}
	else
        {
                printf("[Client] operation failed\n");
        }

        /* call to receive function */
        printf("[Client] ready to receive file\n");
        if( !recvFile( &sockfd ) )
        {
                printf("[Client] file received successfully\n");
        }
        else
        {
                printf("[Client] operation failed\n");
        }

	close(sockfd);
	return 0;
}


//##########################################################################################################

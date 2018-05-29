
#include "fileshareHeader.h"

int sendFile( int *socket_desc, const char *argvfile )
{
        FILE *fp = NULL;
        int read = 0 ;          // holds return value from fread 
        int fsize = 0 ;         // file size
        int no_of_bytes = 0 ;   // holds ret_value of send()

        char filename[LENGTH];	// send file name
        memset(filename,0,LENGTH);

        char buff[LENGTH];      // buffer to send data
        memset(buff,0,LENGTH);

	      /* check existance of file */ 
      	if( access( argvfile, F_OK ) != 0 )
        {
                printf("\tError : no such file exists ( error = %s)\n", strerror(errno) );
                return 1;
        }

        /* open file in read mode */
        if( (fp = fopen( argvfile, "r" )) == NULL )
        {
                printf("Error : fopen failed ( error = %s )\n", strerror(errno) );
                return 1;
        }

        /* find size of file */
        fseek( fp, 0 , SEEK_END );      // points at last position
        fsize = ftell(fp);
        fseek( fp, 0 , SEEK_SET );      // points at first position

        /* convert file size to string and send */
        if( sprintf(buff,"%d",fsize) < 1 )
        {
                printf("\tError : Incorrect file size\n");
                fclose(fp);
                return 1;
        }
        no_of_bytes = send( *socket_desc, buff, LENGTH, 0 );
        if( no_of_bytes == -1 )
        {
                printf("\tError in sending data\n");
                fclose(fp);
                return 1;
        }

	      /* send file name */
	      strcpy( filename, argvfile );
	      no_of_bytes = send( *socket_desc, filename, LENGTH, 0 );
        if( no_of_bytes == -1 )
        {
                printf("\tError in sending data\n");
                fclose(fp);
                return 1;
        }

        /* continue sending data */
        memset(buff,0,LENGTH);
        while( ( read = fread( buff, 1, LENGTH, fp)) > 0 ) // fread( location, no_of_time_read, size, fp )      
        {
                no_of_bytes = send( *socket_desc, buff, read, 0 );
                if( no_of_bytes == -1 )
                {
                        printf("\tError in sending data\n");
			                  fclose(fp);
                        return 1;
                }
                memset(buff,0,LENGTH);
        }


        fclose(fp);
        return 0;

}

//########################################################################################################################

int recvFile( int *socket_desc )
{
        FILE *fp = NULL;
        long filesize = 0 , recvbsize = 0 ;      // holds total file size and received buff size
        int write = 0 ;                         // holds ret_value of fwrite 
        int no_of_bytes = 0 ;                   // holds ret_value of recv()

	      char filename[LENGTH];			// recv file name
	      memset(filename,0,LENGTH);
  
        char buff[LENGTH];                      // buffer to catch data
        memset(buff,0,LENGTH);

        /* first step is take file size from sender and convert it to integer */
        no_of_bytes = recv( *socket_desc, buff , LENGTH, 0 );
        if( no_of_bytes == -1 )
        {
                printf("\tError : recv() Failed\n");
                return 1;
        }
        if( !(filesize = atol(buff)) )
        {
                printf("\tError : Incorrect file size\n");
                return 1;
        }

	      /* recv file name */
	      no_of_bytes = recv( *socket_desc, filename , LENGTH, 0 );
        if( no_of_bytes == -1 )
        {
                printf("\tError : recv() Failed\n");
                return 1;
        }

        /* open file */
        if( (fp = fopen( filename, "a" )) == NULL )
        {
                printf("\tError : failed to open file\n");
                return 1;
        }

        memset(buff,0,LENGTH);
        while( (no_of_bytes = recv( *socket_desc, buff, LENGTH, 0 )) > 0 )      //  ( > 0 ) ; failure case ( == -1)
        {
	    	/* write into file  */
                write = fwrite( buff, 1, no_of_bytes, fp);      // fwrite( buff, sizeof(char), buff_size, fp)   
                if( write < no_of_bytes )
                {
                        printf("\tError : Failed to write on server\n");
                        fclose(fp);
                        return 1;
                }
		            memset(buff,0,LENGTH);

                /* validate end of file and break */
                recvbsize += no_of_bytes ;
                //printf("\trecvbsize = %d\n",recvbsize);
                if( recvbsize == filesize )
                {
                        break;
                }

        }
        if( no_of_bytes < 0 )
        {
                printf("\tError : recv() Failed\n");
                return 1;
        }

        fclose(fp);
        return 0;
}





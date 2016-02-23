#include <fcntl.h> // for setting socket fd as nonblocking
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

// ChatServe.cpp

void usage(){
	printf("Usage: chatserve #PORT\n");
}

int main(int argc, char const *argv[])
{
	char const* quit_command = "/quit\n";
	fd_set fds; // will contain stdin and socket to monitor for available data
	char in_buffer[500], out_buffer[500];
	int socket_fd; // socket we will listen on
	int client_fd; // fd for connection
	socklen_t sockaddr_size;
	struct addrinfo hints; // for passing parameters to getaddrinfo 
	struct addrinfo *server_addrinfo; // for storing server address info
	struct sockaddr_storage client_addr; // client address
	int ai_errorcode;

	/* check for correct # args */
	if (argc != 2 )
	{
		usage(); 
		exit(1);
	}

	/* create socket fd, bind to port */
	bzero((char *) &hints, sizeof(hints)); // zero out hints
	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_STREAM; // use tcp
	hints.ai_flags = AI_PASSIVE; // use servers IP
	
	if ( (ai_errorcode = getaddrinfo(NULL,  // NULL because address is this server
		argv[1],  // port
		&hints, // contains parameters
		&server_addrinfo// results
		)) != 0 ){
		fprintf(stderr, "%s\n", gai_strerror(ai_errorcode));
		exit(ai_errorcode);
	}	

	// loop over results linked list until we can create a socket fd and bind to it
	for(;server_addrinfo != NULL; server_addrinfo = server_addrinfo->ai_next){
		printf("Creating socket.\n");

		// try to get socket file descriptor
		if ( (socket_fd = socket(server_addrinfo->ai_family, 
			server_addrinfo->ai_socktype, // SOCK_STREAM
			server_addrinfo->ai_protocol
			)) < 0 )
		{ 
			// error getting socket file descriptor try next one
			perror("socket");
			continue;
		}

		printf("Socket created.\n");

		// try to bind
		if (bind(socket_fd, server_addrinfo->ai_addr, server_addrinfo->ai_addrlen)==-1){
			// error, try again	
			perror("bind");
			continue;
		}

		// success!
		break;
	}

	// exit if unable to bind
	if (server_addrinfo == NULL)
	{
		fprintf(stderr, "Unable to bind to a socket\n");
		exit(1);
	}

	// free up server_addrinfo because done using it
	freeaddrinfo(server_addrinfo);

	// listen in on port
	if(listen(socket_fd, 0) == -1 ){
		fprintf(stderr, "Unable to listen to port\n");
		exit(1);
	}

	while (1)
	{
	printf("Listening for connections on port %s.\n", argv[1]);

	// accept a new connection and create fd_set
	client_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &sockaddr_size );
	printf("Connected.\n");
	fflush(stdout);

	while (1){

		// zero out the buffers
		bzero(in_buffer,500);
		bzero(out_buffer,500);
		
		// print incoming message
		if ( recv(client_fd, in_buffer, 499, 0) < 0) {
			// connection broken
			printf("Connection closed.\n");
			break;
		}
		printf("%s", in_buffer);

		printf("host> ");
		fflush(stdout);

		// get message in stdin
		fgets(out_buffer, 500, stdin);

		//check for /quit command
		if (strcmp(out_buffer, quit_command)==0)
		{
			// shutdown and close socket
			shutdown(client_fd, SHUT_RDWR);
			close(client_fd);
			printf("Connection closed.\n");
			break;
		}

		// send it
		if ( send(client_fd, out_buffer, strlen(out_buffer)+1, 0) <=  0){
			// connection broken
			printf("Connection closed.\n");
			break;
		}
	}
	}
	return 0;
}

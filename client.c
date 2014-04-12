#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define WRITE_BYTES (1)



/* File descriptor for server socket */
static int sockfd = -1;

/* File descriptor for connection socket */
static int connfd = -1;


/**
 * @brief opens the client socket
 * @param server_hostname the name of the server (IP Adress)
 * @param server_service service name or port number
 * @return client socket
 */

static int open_client_socket(char* const server_hostname, const char* server_service);

/* print usage message */
static void usage();

/**
 * @brief calculates the next guess to send to the server
 * @return random guess of the order and parity bit set.
 */
uint16_t next_tipp(void);

static int round = 0;

int main(int argc, char *argv[])
{
    if(argc != 3){ usage(); return(EXIT_FAILURE);}
    connfd = open_client_socket(argv[1],argv[2]);
    if(connfd<0){return(EXIT_FAILURE);}else{printf("Connection established");}

    uint8_t buffer;
	uint16_t next_tipp_send;
    
    
    do {
		round++;
		next_tipp_send = next_tipp();
		send(connfd, &next_tipp_send, 2, 0);
       // DEBUG("Sent 0x%x\n", next_tipp_send);
		printf("Sent 0x%x\n", next_tipp_send);
		recv(connfd, &buffer, 1, 0);
       // DEBUG("Got byte 0x%x\n", buffer);
		printf("Got byte 0x%x\n", buffer);
        
        int red, white;
        
        red = buffer & 7;
        white = (buffer >> 3) & 7;
        printf("red: %i",red);
        printf("white: %i",white);
        
        
		errno = 0;
		switch (buffer >> 6) {
			case 1:
				fprintf(stderr, "\nParity Error\n");
				return 2;
			case 2:
				fprintf(stderr, "\nGame Lost\n");
				return 3;
			case 3:
				fprintf(stderr, "\nParity error, game lost\n");
				return 4;
			default:
				break;
		}
		
		
		if ((buffer & 7) == 5 ) {
            printf("\nRunden: %i", round);
			return 0;
		}
		
	} while (1);

}

static void usage(){
    printf("\nClient: SYNOPSIS");
    printf("\nclient <server-hostname> <server-port>");
    printf("\nEXAMPLE");
    printf("\nclient localhost 1280\n");
}

static int open_client_socket(char* const server_hostname, const char* server_service)
{
    struct addrinfo* ai,* ai_sel = NULL,* ai_head;
	struct addrinfo hints;
	int err;
    
	/* 1.) resolve hostname to IPv4 address */
    
	hints.ai_flags = 0;
	hints.ai_family = AF_INET; /* IPv4 only, IPv6: AF_INET6  */
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_addrlen = 0;
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	/* see getaddrinfo(3), returns addrinfo structures in a linked list of a given
     internet host and service; we provide that information by 'hints' */
	if((err = getaddrinfo(server_hostname, server_service, &hints, &ai)) != 0)
	{
		(void) fprintf(stderr, "ERROR:  %s\n", gai_strerror(err));
		return -1;
	}
	ai_head = ai;
	if(ai == NULL)
	{
		(void) fprintf(stderr, "Could not resolve host %s.\n", server_hostname);
		return -1;
	}
    
	/* usually we would go through the list of address informations and try each
     address until we succeed, but for now, we just try the first one. You could
     implement your own client socket function with that feature!
	 */
	ai_sel = ai;
    
	/* 2.) create socket */
	if((sockfd = socket(ai_sel->ai_family, ai_sel->ai_socktype, ai->ai_protocol)) < 0)
	{
		(void) fprintf(stderr, "Socket creation failed\n");
		return -1;
	}
    
	/* 3.) connect */
	if(connect(sockfd, ai_sel->ai_addr, ai_sel->ai_addrlen) < 0)
	{
		(void) close(sockfd);
		freeaddrinfo(ai);
		(void) fprintf(stderr, "Connection failed.\n");
		return -1;
	}
    
	freeaddrinfo(ai); /* the dynamically created linked list of getaddrinfo must
                       be freed */
    
	return sockfd;
}

uint16_t next_tipp(void)
{
    //not really logic, more random bruteforce
    srand(time(NULL));
    int r = (uint16_t)rand();
	uint16_t selected_colors = r << round;
    selected_colors |= round;
	
    uint16_t parity = 0;
	for (int i = 0; i < 15; i++) {
		parity ^= (selected_colors % (1 << (i+1))) >> i;
	}
	
	// paraty del cheat ...
	selected_colors <<= 1;
	selected_colors >>= 1;
	
	parity <<= 15;
	
	return selected_colors | parity;
}






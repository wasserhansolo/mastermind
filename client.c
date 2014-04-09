#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define WRITE_BYTES (1)

/* Name of the program */
static const char *progname = "client"; /* default name */

/* File descriptor for server socket */
static int sockfd = -1;

/* File descriptor for connection socket */
static int connfd = -1;

static int open_client_socket(char* const server_hostname, const char* server_service);

int main(int argc, char *argv[])
{
 
    connfd = open_client_socket("localhost","12345");
    if(connfd<0){printf("Connection failed");}else{printf("Connection super");}

    uint8_t buffer;
	uint16_t next_tipp=0x0002; //todo: programmlogik usw. zum senden
    
       send(connfd, &next_tipp, 2, 0);

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


static void bail_out(int eval, const char *fmt, ...)
{
    va_list ap;
    
    (void) fprintf(stderr, "%s: ", progname);
    if (fmt != NULL) {
        va_start(ap, fmt);
        (void) vfprintf(stderr, fmt, ap);
        va_end(ap);
    }
    if (errno != 0) {
        (void) fprintf(stderr, ": %s", strerror(errno));
    }
    (void) fprintf(stderr, "\n");
    
    free_resources();
    exit(eval);
}



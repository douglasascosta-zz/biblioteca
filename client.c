/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void menu(){
	int menu;
//imprime o menu para o cliente
	    printf("--------------- Livros--------------\n\n"); 
	    printf("	1 - Lista todos os ISBN com os títulos\n");
	    printf("	2 - Descrição a partir do ISBN\n");
	    printf("	3 - Todas as informações a partir do ISBN\n");
	    printf("	4 - Lista todas as informações de todos os livros\n"); 
	    printf("	5 - Altera o número de exemplares em estoque\n"); 
	    printf("	6 - Número de exemplares em estoque a partir do ISBN\n"); 
	    printf("	7 - Sair\n"); 
	
	    printf("Escolha uma opÃ§ao:");
	    scanf("%s",menu);
		
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);
	
    freeaddrinfo(servinfo); // all done with this structure
	//imprime o menu pro cliente.	
	//menu();
	while (1) {
		buf[0] = '\0';
		scanf("%s", &buf);
		
		while (send(sockfd, buf, strlen(buf), 0) == -1) {
		}
		while (recv(sockfd, buf, MAXDATASIZE-1, 0) == -1);
		printf("%s\n", buf);
	}

   	

    buf[numbytes] = '\0';

    printf("client: received '%s'\n",buf);

    close(sockfd);

    return 0;
}

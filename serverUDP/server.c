#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>

struct timeval *tv1, *tv2;
char mesg[1000];

int main(int argc, char**argv) {
	int sockfd, n;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t len;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY );
	servaddr.sin_port = htons(32000);
	bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	for (;;) {
		len = sizeof(cliaddr);
		n = recvfrom(sockfd, mesg, 1000, 0, (struct sockaddr *) &cliaddr, &len);

		mesg[n] = 0;
		printf("Recebido:\n");
		printf("%s\n\n", mesg);

		tv1 = malloc(sizeof(struct timeval));
		tv2 = malloc(sizeof(struct timeval));

		avaliaOpcao();

		suseconds_t time = (*tv2).tv_usec - (*tv1).tv_usec;
		//printf("i: %d\n", i);
		printf("Tempo proc.: %d\n", time);

		sendto(sockfd, mesg, strlen(mesg), 0, (struct sockaddr *) &cliaddr,
				sizeof(cliaddr));

	}
}

void avaliaOpcao() {

	char opcao[1];
	char *isbn;
	char *qde;
	int qdet;
	int achou = 0;
	int i, j;
	FILE *bd;

	gettimeofday(tv1, NULL );

	opcao[0] = mesg[0];

	int op = strcmp(opcao, "0");

	if (op == 2 || op == 3 || op == 5 || op == 6) {

		isbn = malloc(10 * sizeof(char));

		for (i = 0; i < 10; i++) {
			if (mesg[i + 2] == '-')
				break;
			isbn[i] = mesg[i + 2];
		}
	}

	if (op == 5) {

		qde = malloc(10 * sizeof(char));
		j = 0;
		for (i = 2; i < strlen(mesg); i++) {
			if (achou == 0) {
				if (mesg[i] != '-' && achou == 0)
					continue;
				else {
					achou = 1;
					continue;
				}
			}
			qde[j] = mesg[i];
			j++;
		}

		qdet = atoi(qde);
	}

	strcpy(mesg, "\0");
	strcat(mesg, "Opção: ");
	strcat(mesg, opcao);

	if (isbn != NULL ) {
		strcat(mesg, " ");
		strcat(mesg, "Isbn: ");
		strcat(mesg, isbn);
	}
	if (qde != NULL ) {
		strcat(mesg, " ");
		strcat(mesg, "Qde: ");
		strcat(mesg, qde);
	}

	printf("%s\n",mesg);

	//bd = openBD();
	//responde(mesg, opcao, bd, qde, isbn);

	gettimeofday(tv2, NULL );
}

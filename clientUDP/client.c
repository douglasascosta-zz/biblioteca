#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

char sendline[1000];
char recvline[1000];
char opcao[1];
char isbn[10];
struct timeval *tv1, *tv2;

int main(int argc, char**argv) {
	int sockfd, n;
	struct sockaddr_in servaddr, cliaddr;

	if (argc != 2) {
		printf("Passe o ip como parâmetro\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(32000);

	while (1) {

		menu();

		scanf("%s", &opcao);

		int i;
		for (i = 0; i < 50; ++i) {

			verificaOpcao();

			printf("%s\n", sendline);

			tv1 = malloc(sizeof(struct timeval));
			tv2 = malloc(sizeof(struct timeval));

			gettimeofday(tv1, NULL );

			sendto(sockfd, sendline, strlen(sendline), 0,
					(struct sockaddr *) &servaddr, sizeof(servaddr));
			n = recvfrom(sockfd, recvline, 10000, 0, NULL, NULL );

			gettimeofday(tv2, NULL );

			suseconds_t time = (*tv2).tv_usec - (*tv1).tv_usec;
			//printf("i: %d\n", i);
			printf("Tempo total %d : %d\n", i,time);

			recvline[n] = 0;
			printf("%s\n\n\n", recvline);
		}
	}
}

void menu() {
//imprime o menu para o cliente
	printf("--------------- Livros--------------\n\n");
	printf("	1 - Lista todos os ISBN com os títulos\n");
	printf("	2 - Descrição a partir do ISBN\n");
	printf("	3 - Todas as informações a partir do ISBN\n");
	printf("	4 - Lista todas as informações de todos os livros\n");
	printf("	5 - Altera o número de exemplares em estoque\n");
	printf("	6 - Número de exemplares em estoque a partir do ISBN\n");
	printf("	7 - Sair\n");

	printf("Escolha uma opção: ");

}

void verificaOpcao() {

	strcpy(sendline, "");
	strcat(sendline, opcao);

	int op = strcmp(opcao, "0");

	if (op == 7) {
		printf("Programa encerrado...\n");
		exit(1);
	} else if (op == 2 || op == 3 || op == 5 || op == 6) {

		//printf("Digite ISBN: ");

		//isbn = malloc(10*sizeof(char));
		strcpy(isbn, "01");

		//scanf("%10s", isbn);

		strcat(sendline, "-");
		strcat(sendline, isbn);

		if (op == 5) {
			printf("Digite nova quantidade: ");

			//qde = malloc(10*sizeof(char));
			char qde[4];
			strcpy(qde, "25");
			//scanf("%4s", qde);
			strcat(sendline, "-");
			strcat(sendline, qde);
		}
	} else if ((op == 1) || (op == 4)) {
		//continua
	} else {
		printf("Opção inválida");
	}

}

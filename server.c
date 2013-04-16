/*
** server.c -- a stream socket server demo
*/

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

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold

#define BUFSIZE 20

#define MAXDATASIZE 100 // max number of bytes we can get at once 

#define MAX 100

int initiateServer();
FILE* openBD();
void listaISBN(int tamanho);
void leSocket(int new_fd, char* buf);
void analisaOpcao(int new_fd, char* buf);
void zeraBuffer(char *buf);
void popula_banco();
void responde();
//void lista_titulo(FILE *bd, int quant);
//void lista_descricao(FILE *bd, int quant, char ISBN);
//void imprime(int col, int lin, FILE *bd);

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


typedef struct Livro {
  char isbn[13];
  char titulo[200];
  char descricao[300];
  char autores[100];
  char editora[50];
  char ano[4];
  char quant[3];
} Serv_livro;

Serv_livro livros[MAX];/*vetor contendo os livros*/
int TAM = 0;/*guarda o numero de livros*/
int col;
int main(void)
{
	int opcao;
	
	initiateServer();
	
}

void zerabuffer(char *buffer){
  int i;
  for(i=0;i<BUFSIZE;i++)
    buffer[i] = '\0';
}


int initiateServer() {

	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	char buf[MAXDATASIZE];
	int numbytes,i;
	FILE *fp;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
	for(i=0;i<MAXDATASIZE;i++) 
		buf[i]='\0';
    int rv;
    int menu1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
		//Faz a população do banco
		
  	//fp = fopen("bd.txt","w");
		//popula_banco(fp);
		//fclose(fp);
		//fp = fopen("bd.txt","r");
		//printf("entre com o menu\n");
		//scanf("%d", &menu1);
		//responde(menu1, fp, 2);
		//listaISBN(3);

		if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
	if ((sockfd = socket(p->ai_family, p->ai_socktype,
	        p->ai_protocol)) == -1) {
	    perror("server: socket");
	    continue;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
	        sizeof(int)) == -1) {
	    perror("setsockopt");
	    exit(1);
	}

	if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	    close(sockfd);
	    perror("server: bind");
	    continue;
	}

	break;
    }

    if (p == NULL)  {
	fprintf(stderr, "server: failed to bind\n");
	return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (listen(sockfd, BACKLOG) == -1) {
	perror("listen");
	exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
	perror("sigaction");
	exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
	sin_size = sizeof their_addr;
	new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	if (new_fd == -1) {
	    perror("accept");
	    continue;
	}

	inet_ntop(their_addr.ss_family,
	    get_in_addr((struct sockaddr *)&their_addr),
	    s, sizeof s);
	printf("server: got connection from %s\n", s);

	//continuar fazendo recv

	if (!fork()) { // this is the child process
		printf("entrou no fork");
    		close(sockfd); // child doesn't need the listener
		zeraBuffer(buf);
		leSocket(new_fd, buf);
		printf("%s",buf);
		analisaOpcao(new_fd, buf);
    		if (send(new_fd, "lido", strlen(buf), 0) == -1)
			perror("send");
    		close(new_fd);
    		exit(0);
	}

	printf("não entrou no fork");
	close(new_fd);  // parent doesn't need this
    }

	return 0;
}


/*funcao para insercao de um novo filme no banco*/
void popula_banco(FILE *fp){
int i;
  if (!fp){
  	printf("Erro na abertura do arquivo");
		exit(0);
  }else{
		for(i=0;i<2;i++){
			
				fprintf(stderr, "%s","entre com ISBN\n"); 
				scanf(" %[^\n]", livros[TAM].isbn);
				fprintf(fp, "%s;",livros[TAM].isbn);

				fprintf(stderr, "%s","entre com titulo\n"); 
				scanf(" %[^\n]", livros[TAM].titulo);
				fprintf(fp, "%s;",livros[TAM].titulo);

				fprintf(stderr, "%s","entre com descricao\n"); 
				scanf(" %[^\n]", livros[TAM].descricao);
				fprintf(fp, "%s;",livros[TAM].descricao);  

				fprintf(stderr, "%s","entre com autores\n"); 
				scanf(" %[^\n]", livros[TAM].autores);
				fprintf(fp, "%s;",livros[TAM].autores);

				fprintf(stderr, "%s","entre com editora\n"); 
				scanf(" %[^\n]", livros[TAM].editora);
				fprintf(fp, "%s;",livros[TAM].editora);

				fprintf(stderr, "%s","entre com ano\n"); 
				scanf(" %[^\n]", livros[TAM].ano);
				fprintf(fp, "%s;",livros[TAM].ano);

				fprintf(stderr, "%s","entre com a quantidade em estoque\n"); 
				scanf(" %[^\n]", livros[TAM].quant);
				fprintf(fp, "%s\n",livros[TAM].quant);
				TAM++; 
				}
			}
	  
 return;
}

void responde(int menu, FILE *bd, int quant){
  char parametro_id[2], c;
  int indice;
    //strcpy(parametro_id,buffer+2);
char ISBN[10];
int i = 0;
int files;
int count =1,mod;
  switch(menu){
   case 1:/*listar titulos e ISBN*/
				lista_titulo(bd,quant);
    break;   
  case 2:/*Descrição a partir do ISBN*/
				printf("Entre com o ISBN\n");
				scanf(" 	%s", &ISBN);
				printf("%s", ISBN);
    		lista_descricao(bd,quant,ISBN);
    break;
  case 3:/*Todas as informações a partir do ISBN*/
    		
    break;
  case 4:/*Lista todas as informações de todos os livros*/
   		
    break;
  case 5:/*Altera o número de exemplares em estoque*/
    
    break;
  case 6:/*Número de exemplares em estoque a partir do ISBN*/
    		
    break;
  case 7:/*encerra*/
    //strcpy(saida,"Encerra");
    break;

  }
	return;  

}

void leSocket(int new_fd, char* buf) {

	int numbytes;

	do {
		numbytes = ((int)recv(new_fd, buf, MAXDATASIZE-1, 0));
	} while (numbytes == -1); 
	buf[numbytes] = '\0';
	printf("%s\n", buf);
}

void analisaOpcao(int new_fd, char* buf) {
	
	char* opcoes;
	opcoes = strsep(&buf, " - ");
	printf("%c\n",opcoes[0]);
	printf("%c\n",opcoes[1]);
}

FILE* openBD() {

	FILE *fp;
	fp = fopen("bd.txt", "r");
	if (fp == NULL) {
		printf("Problema ao abrir o arquivo\n");
	} else {
		printf("Arquivo aberto com sucesso\n");
		return fp;
	}
}



void zeraBuffer(char *buf){
  int i;
  for(i=0;i<MAXDATASIZE;i++)
    buf[i] = '\0';
}

void imprime(int col, int lin, FILE *bd){
	int indice;
	char c;
	int i = 0, j=0, valida =0, k;
	int files;
	int count =1;
	char campo[300];
	char linha[500];	
	char *result;	
printf("entrei no imprime");
		for (i = 0; i<=linha; i++) result = fgets(linha, 500, bd);						
		for (j=0;j<500;j++){
				if (linha[j] != ';'){
						campo[k] = linha[j];
				}else{
						count++;
				}
		}
		printf("acabei as linhas");						
		if (count == col){
				if (col == 1) printf("ISBN: %s", campo);
				if (col == 2) printf("Título: %s",campo);
				if (col == 3) printf("Descricao: %s",campo);
				if (col == 4) printf("Autores: %s",campo);
				if (col == 5) printf("Editora: %s",campo);
				if (col == 6) printf("Ano: %s",campo);
				if (col == 7) printf("Quantidade: %s",campo);
		}

}

void lista_titulo(int menu, FILE *bd, int quant){
  int indice;
    //strcpy(parametro_id,buffer+2);
char c;
int i = 0;
int files;
int count =1,mod;
			printf("ISBN:");
			while (!feof(bd)){
				c = getc(bd);
				if (c != ';'){
						if((count % 6 == 1)&&(files<quant)){
							printf("%c", c);
						}
						if ((count % 6 == 2)&&(files<quant)){
							printf("%c", c);
						}
				}else{
						if(count % 6 == 1) printf("\nTitulo:");
						//if(count % 6 == 2) 
					count++;
				}
				if (c =='\n') {
					files++;
					if (files != quant) printf("\nISBN:");
				}
    }
}

void lista_descricao(FILE *bd, int quant, char ISBN[]){
  int indice;
    //strcpy(parametro_id,buffer+2);
	char c;
	int i = 0, j=0, valida =0;
	int files;
	printf("%s",ISBN);
	int count =1,mod;
	char linha[100];
	char *result;	
			while (!feof(bd)){
				result = fgets(linha, 400, bd);		
				if (result){
						for (j=0;j<2;j++){
								if (linha[j] != ';'){
										if (ISBN[j]==linha[j]){
											valida =1;
											count++;
										}else{
											valida = 0;
											break;
										}
								}
						}
						printf("result");
						if (valida == 1) {
							printf("ISBN: %s",ISBN);
							imprime(3,1,bd);
						}					
				}
				
		}
				
    
}



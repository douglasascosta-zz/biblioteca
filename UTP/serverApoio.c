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
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold

#define BUFSIZE 20

#define MAXDATASIZE 100 // max number of bytes we can get at once 

#define MAX 100


/*struct para o tipo filme*/
typedef struct Filme {
  char id[3];
  char titulo[200];
  char sinopse[300];
  char sala[3];
  char horarios[20];
} S_filme;


S_filme filmes[MAX];/*vetor contendo os filmes*/
int TAM = 0;/*guarda o numero de filmes*/

void zerabuffer(char *buffer){
  int i;
  for(i=0;i<BUFSIZE;i++)
    buffer[i] = '\0';
}
/*funcao para insercao de um novo filme no banco*/
void popula_banco()
{
  fprintf(stderr, "%s","entre com id\n"); 
  scanf(" %[^\n]", filmes[TAM].id);
  fprintf(stderr, "%s","entre com titulo\n"); 
  scanf(" %[^\n]", filmes[TAM].titulo);
  fprintf(stderr, "%s","entre com sinopse\n"); 
  scanf(" %[^\n]", filmes[TAM].sinopse);
  fprintf(stderr, "%s","entre com sala\n"); 
  scanf(" %[^\n]", filmes[TAM].sala);
  fprintf(stderr, "%s","entre com horarios"); 
  scanf(" %[^\n]", filmes[TAM].horarios);
  TAM++; 
 return;
}


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
/*monta a resposta a ser enviada para o client*/
void responde(char *buffer,char *saida){
  char parametro_id[2];
  int indice;
    strcpy(parametro_id,buffer+2);
  switch(buffer[0]){
   case '1':/*listar titulos e ids*/
    for (indice = 0;indice < TAM;indice++){
      strcat(saida,filmes[indice].id);
      strcat(saida, "\n");
      strcat(saida, filmes[indice].titulo);
      strcat(saida, "\n");
    }
    break;
  case '2':/*retorna a sinopse de um filme*/
    strcpy(saida, filmes[atoi(parametro_id)-1].sinopse);
    break;
  case '3':/*retorna todas as informacoes de um filme*/
    strcat(saida,filmes[atoi(parametro_id)-1].id);
    strcat(saida, "\n");
    strcat(saida, filmes[atoi(parametro_id)-1].titulo);
    strcat(saida, "\n");
    strcat(saida,filmes[atoi(parametro_id)-1].sinopse);
    strcat(saida, "\n");
    strcat(saida, filmes[atoi(parametro_id)-1].sala);
    strcat(saida, "\n");
    strcat(saida, filmes[atoi(parametro_id)-1].horarios);
    strcat(saida, "\n");
    break;
  case '4':/*listar todas informacoes de todos os filmes*/
    for (indice = 0;indice < TAM;indice++){
      strcat(saida,filmes[indice].id);
      strcat(saida, "\n");
      strcat(saida, filmes[indice].titulo);
      strcat(saida, "\n");
      strcat(saida,filmes[indice].sinopse);
      strcat(saida, "\n");
      strcat(saida, filmes[indice].sala);
      strcat(saida, "\n");
      strcat(saida, filmes[indice].horarios);
      strcat(saida, "\n");
    }
    break;
  case '5':
    strcpy(saida,"Encerra");
    break;
  }
	return;  

}

int main(void)
{
    int sockfd, new_fd ,numbytes,send_tam;  // listen on sock_fd, new connection on new_fd
    int i;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    char buf[MAXDATASIZE];
    int yes=1;
    char s[INET6_ADDRSTRLEN];for(i=0;i<MAXDATASIZE;i++) 
		buf[i]='\0';
    int rv;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    for(i=0;i<21;i++)
	    popula_banco();

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // percorre os resultados e da bind ao primeiro que conseguir
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
				perror("server: socket");
				continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
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

    freeaddrinfo(servinfo); // estrutura nao e mais necessaria

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // termina os processos mortos
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {  /*loop para aceitar conexoes*/
        sin_size = sizeof their_addr;
		//aceita conexoes
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
		//coloca o endereco de forma legivel
        inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
		printf("server: got connection from %s\n", s);

        if (!fork()) { // cria processo filho
           close(sockfd); // processo filho nao precisa ouvir
						//vetores de Structs para medidas de tempo
            //struct timeval tempo_comeco[200];vetor para medida de tempos iniciais
            //struct timeval tempo_fim[200];vetor para medidas de tempo finais
            //double diferenca[200];vetor para guardar a o tempo de processamento
            //int k=0;ineteiro para fazer as 100 interacoes
			while(1){

	            numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0);

				//pega tempo de qando chega a mensagem do cliente	            
				//gettimeofday( &tempo_comeco[k], NULL );    
	            
				if (numbytes  == -1) {
					perror("recv");
					exit(1);	
				}
	
				buf[numbytes] = '\0';
	            
	            char saida[10000];

				printf("server: received '%s'\n",buf);
	

				if(buf[0]=='5'){
					printf("server: Acabou a conex√£o\n",buf);
					//imprime a saida  o valor das 100 iteracoes
					//for(i=0;i<100;i++){
						//printf("%g,",diferenca[i]);        
					//}
            
					close(new_fd);
					exit(0);			
				}
				for(i=0;i<MAXDATASIZE;i++) //zera o vetor de saida
					saida[i]='\0';	

	            responde(buf,saida);
	
				printf("saida responde %s\n",saida);

				strcat(saida,"#");
	
	            send_tam = strlen(saida)+1;/*guarda o tamanho total a ser enviado*/

				//tempo do final da execucao do servidor
				//gettimeofday( &tempo_fim[k], NULL ); 

				//calculando a diferenÁa entre os tempos	
				//diferenca[k] = (double)(tempo_fim[k].tv_sec - tempo_comeco[k].tv_sec);
				//diferenca[k] += (double)(tempo_fim[k].tv_usec - tempo_comeco[k].tv_usec)/(double)1000000;
				//k++;
             
				send(new_fd,saida,send_tam, 0);

			}	



			printf("server: Acabou a conex√£o\n",buf);
			printf("\n");
			//quando acaba a conex„o , imprime o tempo das 100 execucoes
			//for(i=0;i<100;i++){
			//printf("%1.10lf,",diferenca[i]);        
            
			close(new_fd);
			exit(0);
		}
        close(new_fd);  // parent doesn't need this
	}

    return 0;
}




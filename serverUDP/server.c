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

#define ARQ 4

typedef struct Livro {
	char isbn[13];
	char titulo[100];
	char descricao[300];
	char autores[50];
	char editora[30];
	char ano[5];
	char quant[3];
} Serv_livro;

Serv_livro le_banco(FILE* bd);
void lista_tudo(char *buf, Serv_livro bd[], char *quant);
void avaliaOpcao();
void responde(char *buf, int menu, Serv_livro livros[], char *quant,char *isbn);
void closeBD(FILE *bd);
FILE* openBD();
void zeraBuffer(char *buf);
void imprime(int col, int lin, Serv_livro bd[], char *buf);
void lista_titulo(char *buf, Serv_livro bd[], char *quant);
void lista_menu(char *buf, Serv_livro bd[], char *quant, char ISBN[], int ret);
void altera_bd(char *buf, Serv_livro bd[], char *quant, char ISBN[]);
void popula_banco(Serv_livro livros[]);

struct timeval *tv1, *tv2;
char mesg[1000];



Serv_livro livros[MAX];/*vetor contendo os livros*/

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
		printf("Tempo proc.: %ld\n", time);

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
	//pega o menu do cliente
	opcao[0] = mesg[0];

	int op = strcmp(opcao, "0");
	// Se for essas opcoes recebe o ISBN para fazer a consulta
	if (op == 2 || op == 3 || op == 5 || op == 6) {

		isbn = malloc(13 * sizeof(char));

		for (i = 0; i < 13; i++) {
			if (mesg[i + 2] == '-')
				break;
			isbn[i] = mesg[i + 2];
		}
	}
 //Se for 5 recebe também a nova quantidade
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
	Serv_livro livros[ARQ];
	bd = openBD();
	*livros = le_banco(bd);
	int men = opcao[0] - '0';
	responde(mesg, men, livros, qde, isbn);

	gettimeofday(tv2, NULL );
}

//funcao que analisa a entrada do cliente
void responde(char *buf, int menu, Serv_livro *bd, char *quant,char *isbn){
  char parametro_id[2], c;
  int indice;
		int i = 0;
		int files;
		int count =1,mod;
  switch(menu){
   case 1:/*listar titulos e ISBN*/
							lista_titulo(buf, bd,quant);
							//closeBD(bd);
    break;   
  case 2:/*Descrição a partir do ISBN*/
		lista_menu(buf, bd,quant,isbn,3);
		//closeBD(bd);
    break;
  case 3:/*Todas as informações a partir do ISBN*/
		lista_menu(buf, bd,quant,isbn,0);
		//closeBD(bd);
    break;
  case 4:/*Lista todas as informações de todos os livros*/
   		lista_tudo(buf,bd,quant);
   		//closeBD(bd);
    break;
  case 5:/*Altera o número de exemplares em estoque*/
    		altera_bd(buf ,bd,quant,isbn);
    break;
  case 6:/*Número de exemplares em estoque a partir do ISBN*/
		lista_menu(buf, bd,quant,isbn,7);
		//closeBD(bd);
    break;
  case 7:/*encerra*/
    //strcpy(saida,"Encerra");
    break;

  }
	return;  

}
//abre o banco de dados
FILE* openBD() {

	FILE *fp;
	fp = fopen("bd.txt", "r");
	if (fp == NULL) {
		printf("Problema ao abrir o arquivo\n");
	} else {
		//printf("Arquivo aberto com sucesso\n");
		return fp;
	}
}

void closeBD(FILE *bd) {
	fseek(bd, 0, SEEK_SET);
}

void zeraBuffer(char *buf){
  int i;
  for(i=0;i<MAXDATASIZE;i++)
    buf[i] = '\0';
}
	//funcao genérica que imprime o campo desejado
	void imprime(int col, int lin, Serv_livro bd[], char *buf){
	int indice;
	char c;
	int i , j, valida =0, k;
	int files;
	int count =0;
	char *result;

		if (col== 1) {
			strcat(buf, "ISBN -");
			strcat(buf, livros[lin].isbn);
			printf("ISBN: %s\n", livros[lin].isbn);
			}
		if (col == 2) {
			strcat(buf, "Titulo -");
			strcat(buf, livros[lin].titulo);
			printf("Titulo: %s\n", livros[lin].titulo);
			}
		if (col == 3) {
			strcat(buf, "Descricao -");
			strcat(buf, livros[lin].descricao);
			printf("Descricao: %s\n", livros[lin].descricao);	
			}
		if (col == 4) {
			strcat(buf, "Autores -");
			strcat(buf, livros[lin].autores);
			printf("Autores: %s\n", livros[lin].autores);
			}
		if (col == 5) {
			strcat(buf, "Editora -");
			strcat(buf, livros[lin].editora);
			printf("Editora: %s\n", livros[lin].editora);
			}
		if (col == 6) {
			strcat(buf, "Ano -");
			strcat(buf, livros[lin].ano);
			printf("Ano: %s\n", livros[lin].ano);
			}
		if (col == 7) {
			strcat(buf, "Estoque -");
			strcat(buf, livros[lin].quant);
			printf("Estoque: %s\n", livros[lin].quant);
			}

}

//Faz leitura do banco de dados e coloca em um struct para facilitar a leitura
Serv_livro le_banco(FILE *bd){
		
		int campo;
		int i,j,k=0;
		char c = ' ';
		char 	linha[500];
		while (!feof(bd)){//enquanto tiver arquivo
			for (j=0;j<4;j++){//para todas as linhas até o final do vetor de struct
				campo=1;
				
				fgets(linha,500,bd);
				for (i=0;i<500;i++){//percorre a linha inteira
						c = linha[i];//le o caractere 			
					if(c!=';'){
						if (campo == 1){
						livros[j].isbn[k++] = c;
						}
						if(campo == 2){
						livros[j].titulo[k++] = c;
						}
						if(campo == 3){
						livros[j].descricao[k++] = c;
						}
						if(campo == 4){
						livros[j].autores[k++] = c;
						}	
						if(campo == 5){
						livros[j].editora[k++] = c;
						}
						if(campo == 6){
						livros[j].ano[k++] = c;
						}	
						if(campo == 7){
						livros[j].quant[k++] = c;
						}
					}else{
							campo++;
							k=0;
					}
				}
				
			}
		}
	fclose(bd);
	return *livros;
	
	}
	
// imprime todos os titulos do banco
void lista_titulo(char *buf, Serv_livro bd[], char *quant){
  int indice;
    //strcpy(parametro_id,buffer+2);
	char c;
	int i = 0;
	int files;
	int count =1;
	int j;
	for (j=0;j<ARQ;j++){//para todas as linhas
		imprime(1, j, bd, buf);
		imprime(2, j, bd, buf);
	}
}
//dependendo da entrada ele imprime o desejado
void lista_menu(char *buf, Serv_livro bd[], char *quant, char ISBN[], int ret){

 int indice;
    //strcpy(parametro_id,buffer+2);
	char c;
	int i,j,k, valida =0;
	int files, aux;
	int count =0,mod;
	char linha[100];
	char *result;	
		for (j=0;j<ARQ;j++){//para todas as linhas	
				printf("Banco: %s",bd[j].isbn);
				printf("Passado como parametro: %s\n",ISBN);
			if ( !strcmp(ISBN,bd[j].isbn) ){//se for igual ao procurado
		
				if (ret == 3) {
					imprime(1, j, bd, buf);
					imprime(3, j, bd, buf);
					return;
				}
				if (ret == 7) {
					imprime(1,j,bd, buf);
					imprime(7,j,bd, buf);
					return;
				}
				if (ret == 0){
					imprime(1,j,bd, buf);
					imprime(2,j,bd, buf);
					imprime(3,j,bd, buf);
					imprime(4,j,bd, buf);
					imprime(5,j,bd, buf);
					imprime(6,j,bd, buf);
					imprime(7,j,bd, buf);
					return;
				}
			}
		}
		strcat(buf, "Não achou ISBN");
		printf("Não achou ISBN\n");
		return;
}

//lista todos os livros do banco
void lista_tudo(char *buf, Serv_livro bd[], char *quant){
  int indice;
    //strcpy(parametro_id,buffer+2);
	char c;
	int i,j,k, valida =0;
	int files;
	int count =0,mod;
	char linha[100];
	char *result;	
		for (j=0;j<ARQ;j++){//para todas as linhas
				imprime(1,j,bd,buf);
				imprime(2,j,bd,buf);
				imprime(3,j,bd,buf);
				imprime(4,j,bd,buf);
				imprime(5,j,bd,buf);
				imprime(6,j,bd,buf);
				imprime(7,j,bd,buf);
			}
			return;
}
	
	//altera o valor do estoque de um determinado isbn
void altera_bd(char *buf, Serv_livro bd[], char *quant, char ISBN[]){

	int indice;
	char c;
	int i,j,k, valida =0;
	int files;
	int count =0,mod;
	char linha[100];
	char *result;	
		for (j=0;j<ARQ;j++){//para todas as linhas	
					if ( !strcmp(ISBN,bd[j].isbn) ){//se for igual ao procurado
					//troca o valor de estoque
					strcpy(bd[j].quant,quant);
				//grava no arquivo todo o struct
				//popula_banco(bd);
				lista_tudo(buf, bd,quant);
				return;
				}
		}
		printf("Não alterou o banco");
		return;		
		
	}
		// reescreve o banco de dados atualizando o estoque
	void popula_banco(Serv_livro livros[]){
	FILE *fp;
	fp = fopen("bd.txt", "w");
	if (fp == NULL) {
		printf("Problema ao abrir o arquivo\n");
	} else {
		printf("Arquivo aberto com sucesso\n");
	}
	int i;
  if (!fp){
  	printf("Erro na abertura do arquivo");
		exit(0);
  }else{
		for(i=0;i<ARQ;i++){
	
			fprintf(fp, "%s;",livros[i].isbn);
			fprintf(fp, "%s;",livros[i].titulo);
			fprintf(fp, "%s;",livros[i].descricao);  
			fprintf(fp, "%s;",livros[i].autores);
			fprintf(fp, "%s;",livros[i].editora);
			fprintf(fp, "%s;",livros[i].ano);
			fprintf(fp, "%s;",livros[i].quant);
			if (i!=ARQ-1) fprintf(fp, "\n",livros[i].quant);
		}
	}
}
	
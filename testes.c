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

//void responde();

void imprime(int col, int lin, FILE *bd);
FILE* openBD();

typedef struct Livro {
  char isbn[13];
  char titulo[200];
  char descricao[300];
  char autores[100];
  char editora[50];
  char ano[4];
  char quant[3];
} Serv_livro;


int TAM = 0;/*guarda o numero de livros*/
int col;
int main(void){
	//FILE *bd;
int opcao;
	
	int menu1,

		bd = openBD();
		printf("entre com o menu\n");
		scanf("%s", &menu1);
		responde(menu1, bd, 2);
	
	
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
Serv_livro livros[10];

void responde(int menu, FILE *bd, int quant){
  char parametro_id[2], c;
  int indice;
    //strcpy(parametro_id,buffer+2);
char ISBN[10];
int i = 0;
int files;
int count =1,mod;
	printf("entrei no responde\n");	
  switch(menu){
   case '1':/*listar titulos e ISBN*/
				printf("entrei no case1\n");	
				lista_titulo(bd,quant);
    break;   
  case '2':/*Descrição a partir do ISBN*/
				printf("Entre com o ISBN\n");
				scanf("%s", &ISBN);
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

void imprime(int col, int lin, FILE *bd){
printf("entrei no imprime\n");
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
	printf("entrei no lista_titulo\n");	
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
printf("entrei no lista_descricao\n");
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



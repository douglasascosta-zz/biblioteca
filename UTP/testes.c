#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#define ARQ 5

//void responde();

int _IO_puts(const char * __c);
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
		responde(menu1, bd, ARQ);
		closeBD(bd);
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
void closeBD(FILE *bd) {
	fseek(bd, 0, SEEK_SET);
}

Serv_livro livros[10];

void responde(int menu, FILE *bd, int quant){
  char parametro_id[2], c;
  int indice;
    //strcpy(parametro_id,buffer+2);
char ISBN[10];
int i = 0;
int files;
int count =1;
int mod;
		printf("\n");	
  switch(menu){
   case '1':/*listar titulos e ISBN*/
			lista_titulo(bd,quant);
			closeBD(bd);
    break;   
  case '2':/*Descrição a partir do ISBN*/
			printf("Entre com o ISBN\n");
			scanf("%s", &ISBN);
    		lista_menu(bd,quant,ISBN,3);
    break;
  case '3':/*Todas as informações a partir do ISBN*/
    		printf("Entre com o ISBN\n");
			scanf("%s", &ISBN);
    		lista_menu(bd,quant,ISBN,0);
    break;
  case '4':/*Lista todas as informações de todos os livros*/
   		 lista_tudo(bd,quant);
    break;
  case '5':/*Altera o número de exemplares em estoque*/
    
    break;
  case '6':/*Número de exemplares em estoque a partir do ISBN*/
    		printf("Entre com o ISBN\n");
			scanf("%s", &ISBN);
    		lista_menu(bd,quant,ISBN,7);
    break;
  case '7':/*encerra*/
    //strcpy(saida,"Encerra");
    break;

  }
	return;  

}

void imprime(int col, int lin, FILE *bd){
	int indice;
	char c;
	int i , j, valida =0, k;
	int files;
	int count =0;
	char campo[300];
	char linha[500];	
	char *result;
	for(i=0;i<300;i++) {
		campo[i]	=' ';	
		}
	for(i=0;i<500;i++) {
		linha[i]	=' ';
		}
		for (i = 1; i<=lin; i++) 
			fgets(linha, 500, bd);						
		for (j=0,k=0;j<300&&(count != col);j++){
			if (linha[j] != ';'){
				campo[k++] = linha[j];
			}else{
				count++;
				k=0;
			}
		}
		if (col == 1) printf("ISBN: %s\n", campo);
		if (col == 2) printf("Título: %s\n",campo);
		if (col == 3) printf("Descricao: %s\n",campo);
		if (col == 4) printf("Autores: %s\n",campo);
		if (col == 5) printf("Editora: %s\n",campo);
		if (col == 6) printf("Ano: %s\n",campo);
		if (col == 7) printf("Quantidade: %s\n",campo);
closeBD(bd);
}

void lista_titulo(FILE *bd, int quant){
  int indice;
    //strcpy(parametro_id,buffer+2);
char c;
int i = 0;
int files;
int count =1;
int j;
			for (j=0;j<quant;j++){//para todas as linhas
				imprime(1, j, bd);
				imprime(2, j, bd);
			}
}
void lista_menu(FILE *bd, int quant, char ISBN[], int ret){
  int indice;
    //strcpy(parametro_id,buffer+2);
	char c;
	int i,j,k, valida =0;
	int files;
	int count =0,mod;
	char linha[100];
	char *result;	
	//fseek(bd, 0, SEEK_SET);
		//while (!feof(bd)){//enquanto tiver arquivo e nao achar o isbn
			for (j=0;j<quant;j++){//para todas as linhas
				for (i=0;i<100 && c!='\n';i++){//percorre a linha inteira
					c = getc(bd);//le o caractere e coloca em linha[]
					linha[i] = c;
				}
				for (k=0;k<2 && count!=2;k++){//para o tamanho do ISBN
					if (ISBN[k]==linha[k]){//se for igual ao procurado
						valida =1;
						count++;
					}else{
						valida = 0;
						break;
					}
				}
			}
		//}

			if (valida==1){
				//printf("ISBN: %s\n",ISBN);
				if (ret == 3) {
					imprime(1,j,bd);
					imprime(3,j,bd);
					}
				if (ret == 7) 
				{
					imprime(1,j,bd);
					imprime(7,j,bd);
					}
				if (ret == 0){
					imprime(1,j,bd);
					imprime(2,j,bd);
					imprime(3,j,bd);
					imprime(4,j,bd);
					imprime(5,j,bd);
					imprime(6,j,bd);
					imprime(7,j,bd);
				}
			}else{
					printf("Não achou ISBN");
			}
}
void lista_tudo(FILE *bd, int quant){
  int indice;
    //strcpy(parametro_id,buffer+2);
	char c;
	int i,j,k, valida =0;
	int files;
	int count =0,mod;
	char linha[100];
	char *result;	
	printf("entrei no lista_tudo\n");
	fseek(bd, 0, SEEK_SET);
	//while (!feof(bd)){//enquanto tiver arquivo e nao achar o isbn
		for (j=0;j<quant;j++){//para todas as linhas
			for (i=0;i<100 && c!='\n';i++){//percorre a linha inteira
				c = getc(bd);//le o caractere e coloca em linha[]
				linha[i] = c;
			}
				imprime(1,j,bd);
				imprime(2,j,bd);
				imprime(3,j,bd);
				imprime(4,j,bd);
				imprime(5,j,bd);
				imprime(6,j,bd);
				imprime(7,j,bd);
			}
	//}
			printf("sai do lista_menu\n");
			
}
void altera_bd(FILE *bd, int quant){}

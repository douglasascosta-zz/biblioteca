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

#define ARQ 4

typedef struct Livro {
  char isbn[13];
  char titulo[200];
  char descricao[300];
  char autores[100];
  char editora[50];
  char ano[5];
  char quant[3];
} Serv_livro;

//void responde();
Serv_livro le_banco(FILE *bd);
int _IO_puts(const char * __c);
void imprime(int col, int lin, Serv_livro livros[]);
void responde(int menu, Serv_livro bd[], int quant);
void altera_bd(Serv_livro bd[], int quant,char ISBN[],char novo_estoque[]);
void popula_banco();
FILE* openBD();



Serv_livro livros[ARQ];

int TAM = 0;/*guarda o numero de livros*/
int col;
int main(void){
	FILE *bd;
int opcao;
	bd = openBD();
	
	int menu1;
	*livros = le_banco(bd);
	printf("entre com o menu\n");
	scanf("%s", &menu1);
	responde(menu1, livros, ARQ);
		//closeBD(bd);
		//return 0;
}

FILE* openBD() {

	FILE *fp;
	fp = fopen("bd.txt", "rw");
	if (fp == NULL) {
		printf("Problema ao abrir o arquivo\n");
	} else {
		printf("Arquivo aberto com sucesso\n");
		return fp;
	}

}
void closeBD(FILE *bd) {
	fseek(bd, 0, SEEK_SET);
	fclose(bd);
}



void responde(int menu, Serv_livro *bd, int quant){
  char parametro_id[2], c;
  int indice;
    //strcpy(parametro_id,buffer+2);
char ISBN[13];
char novo_estoque[3];
int i = 0;
int files;
int count =1;
int mod;
		printf("\n");	
  switch(menu){
   case '1':/*listar titulos e ISBN*/
			lista_titulo(bd,quant);
			//closeBD(bd);
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
  				printf("Entre com o ISBN\n");
					scanf("%s", &ISBN);
					printf("Entre com o novo estoque\n");
					scanf("%s", &novo_estoque);
    			altera_bd(bd,quant,ISBN,novo_estoque);
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

void imprime(int col, int lin, Serv_livro livros[]){

		if (col == 1) printf("ISBN: %s\n", livros[lin].isbn);
		if (col == 2) printf("Título: %s\n",livros[lin].titulo);
		if (col == 3) printf("Descricao: %s\n",livros[lin].descricao);
		if (col == 4) printf("Autores: %s\n",livros[lin].autores);
		if (col == 5) printf("Editora: %s\n",livros[lin].editora);
		if (col == 6) printf("Ano: %s\n",livros[lin].ano);
		if (col == 7) printf("Quantidade: %s\n",livros[lin].quant);

}

void lista_titulo(Serv_livro livros[], int quant){
  int indice;

char c;
int i = 0;
int files;
int count =1;
int j;
			for (j=0;j<quant;j++){//para todas as linhas
				imprime(1, j, livros);
				imprime(2, j, livros);
			}
}

void lista_menu(Serv_livro bd[], int quant, char ISBN[], int ret){
  int indice;
    //strcpy(parametro_id,buffer+2);
	char c;
	int i,j,k, valida =0;
	int files, aux;
	int count =0,mod;
	char linha[100];
	char *result;	
		for (j=0;j<quant;j++){//para todas as linhas	
			if ( !strcmp(ISBN,bd[j].isbn) ){//se for igual ao procurado

				if (ret == 3) {
					imprime(1, j, bd);
					imprime(3, j, bd);
					return;
				}
				if (ret == 7) {
					imprime(1,j,bd);
					imprime(7,j,bd);
					return;
				}
				if (ret == 0){
					imprime(1,j,bd);
					imprime(2,j,bd);
					imprime(3,j,bd);
					imprime(4,j,bd);
					imprime(5,j,bd);
					imprime(6,j,bd);
					imprime(7,j,bd);
					return;
				}
			}
		}
		printf("Não achou ISBN\n");
}

void lista_tudo(Serv_livro bd[], int quant){
  int indice;
	char c;
	int i,j,k, valida =0;
	int files;
	int count =0,mod;
	char linha[100];
	char *result;	


		for (j=0;j<quant;j++){//para todas as linhas
				imprime(1,j,bd);
				imprime(2,j,bd);
				imprime(3,j,bd);
				imprime(4,j,bd);
				imprime(5,j,bd);
				imprime(6,j,bd);
				imprime(7,j,bd);
			}
		
}

void altera_bd(Serv_livro bd[], int quant,char ISBN[],char novo_estoque[]){
		int indice;
	char c;
	int i,j,k, valida =0;
	int files;
	int count =0,mod;
	char linha[100];
	char *result;	
		for (j=0;j<quant;j++){//para todas as linhas	
					if ( !strcmp(ISBN,bd[j].isbn) ){//se for igual ao procurado
					//troca o valor de estoque
					printf("sai:%s, entra:%s\n",bd[j].quant,novo_estoque);
					strcpy(bd[j].quant,novo_estoque);
					printf("ficou:%s\n",bd[j].quant);
				//grava no arquivo todo o struct
				popula_banco(bd);
				//lista_tudo(bd,quant);
				return;
				}
		}
		printf("Não alterou o banco");
		return;

}

//Faz leitura do banco de dados e coloca em um struct para facilitar a leitura
Serv_livro le_banco(FILE *bd){
		
		int campo;
		int i,j,k=0;
		char c = ' ';
		char 	linha[500];
		while (!feof(bd)){//enquanto tiver arquivo
			for (j=0;j<ARQ;j++){//para todas as linhas até o final do vetor de struct
				campo=1;
				
				fgets(linha,500,bd);
				fprintf(stderr, "%s\n",linha);
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
	
	
	
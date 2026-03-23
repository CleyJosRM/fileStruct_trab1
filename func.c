#include "func.h"
#include<stdlib.h>
#include<stdbool.h>
#include<stdio.h>
#include<string.h>

#define LIXO 0x24 // O conteúdo de bytes não utilizados é 0x24, $ em ascii, apelidado de LIXO nesse código
#define LIXO_INT 0x24242424 // Um campo de tipo inteiro não utilizado teria valor $$$$, ou 0x24242424, apelidado de LIXO_INT nesse código
typedef struct reg_dados_struct REG_DADOS_STRUCT; // Essa struct é uma representação do registro de dados na memória. Não é exatamente a mesma estrutura, pois as strings foram substituídas por ponteiros, e elas terminam em \0, diferentemente da versão em disco.

struct reg_dados_struct {
	unsigned char removido;
	int proximo;
	int codEstacao;
	int codLinha;
	int codProxEstacao;
	int distProxEstacao;
	int codLinhaIntegra;
	int codEstIntegra;
	int tamNomeEstacao;
	char* nomeEstacao; // ponteiro para uma string de tamanho tamNomeEstacao + 1, terminada em \0
	int tamNomeLinha;
	char* nomeLinha; // ponteiro para uma string de tamanho tamNomeLinha + 1, terminada em \0
};

// essa função recebe uma linha do arquivo csv e retorna um struct REG_DADOS_STRUCT contendo as informações naquela linha
REG_DADOS_STRUCT ler_linha_csv(char* linha){
	
	char* inicio_campo = linha;
	char* final_campo = inicio_campo;
	REG_DADOS_STRUCT* registro_lido = (REG_DADOS_STRUCT*) malloc (sizeof(REG_DADOS_STRUCT));
	registro_lido->removido = 0;
	registro_lido->proximo = -1;
	
	for(int i=0; i<8; i++) { // cada linha do csv tem 8 campos para serem lidos
		
		while(*final_campo != ',') final_campo++; // incrementa final_campo até ele apontar para a vírgula
		*final_campo = '\0'; // troca a vírgula por \0
		
		if(*inicio_campo == ','){ // quando o registro é nulo, representado por ",," no arquivo 
			switch(i){
			case 0:
				registro_lido->codEstacao = LIXO_INT;
				break;
			case 1:
				registro_lido->tamNomeEstacao = 0;
				registro_lido->nomeEstacao = NULL;
				break;
			case 2:
				registro_lido->codLinha = LIXO_INT;
				break;
			case 3:
				registro_lido->tamNomeEstacao = 0;
				registro_lido->nomeEstacao = NULL;
			case 4:
				registro_lido->codProxEstacao = LIXO_INT;
				break;
			case 5:
				registro_lido->distProxEstacao = LIXO_INT;
				break;
			case 6:
				registro_lido->codLinhaIntegra = LIXO_INT;
				break;
			case 7:
				registro_lido->codEstIntegra = LIXO_INT;
				break;
			}
		}else{
			switch(i){
				case 0:
					registro_lido->codEstacao = atoi(inicio_campo);
					break;
				case 1:
					char* nomeEstacao = strdup(inicio_campo);
					registro_lido->tamNomeEstacao = strlen(nomeEstacao);
					registro_lido->nomeEstacao = nomeEstacao;
					break;
				case 2:
					registro_lido->codLinha = atoi(inicio_campo);
					break;
				case 3:
					char* nomeLinha = strdup(inicio_campo);
					registro_lido->tamNomeLinha = strlen(nomeLinha);
					registro_lido->nomeLinha = nomeLinha;
				case 4:
					registro_lido->codProxEstacao = atoi(inicio_campo);
					break;
				case 5:
					registro_lido->distProxEstacao = atoi(inicio_campo);
					break;
				case 6:
					registro_lido->codLinhaIntegra = atoi(inicio_campo);
					break;
				case 7:
					registro_lido->codEstIntegra = atoi(inicio_campo);
					break;
			}
		}
		
		inicio_campo = final_campo + 1;
		final_campo = inicio_campo;
	}
	
}

void func_1(char* arquivoSaida, char* arquivoEntrada){ // a ordem dos argumentos é o contrário da ordem digitada pelo usuário, devido à ordem de empilhamento dos argumentos na memória, lembrando que cada argumento é o retorno de uma chamada de strtok
	
	// ABRIR CSV EM MODO LEITURA
	FILE* filestream_csv = fopen(arquivoEntrada, "r"); // abre o arquivo csv em modo leitura e como texto
	if(filestream_csv == NULL){ // se falhou
		printf("Falha no processamento do arquivo.\n");
		printf("DEBUG: ERRO AO ABRIR CSV\n");
		exit(1);
	}
	
	// CRIAR ARQUIVO BINÁRIO EM MODO ESCRITA
	FILE* filestream_bin = fopen(arquivoSaida, "wb"); // abre o arquivo de saída em modo escrita e como binário
	if(filestream_csv == NULL){ // se falhou
		printf("Falha no processamento do arquivo.\n");
		printf("DEBUG: ERRO AO ABRIR BIN\n");
		exit(1);
	}
	
	// ESCREVER REGISTRO DE CABEÇALHO
	
	unsigned char cabecalho[] = { // a variável cabecalho é o endereço de memória de uma sequência de bytes, especificados abaixo. Esses são valores iniciais para o registro de cabeçalho, que deverá ser atualizado quando terminarmos a leitura.
		0x00,                   // status 
		0xff, 0xff, 0xff, 0xff, // topo
		0x00, 0x00, 0x00, 0x00, // proxRRN
		0x24, 0x24, 0x24, 0x24, // nroEstacoes, será atualizado depois
		0x24, 0x24, 0x24, 0x24  // nroParesEstacao 
	};
	
	fwrite(cabecalho, 1, sizeof(cabecalho), filestream_bin);
	
	// ESCREVER REGISTRO DE DADOS
	
	// FECHAR ARQUIVOS
	
	if(fclose(filestream_csv) != 0){
		printf("Falha no processamento do arquivo.\n");
		printf("DEBUG: ERRO AO FECHAR CSV\n");
		exit(1);
	}
	if(fclose(filestream_bin) != 0){
		printf("Falha no processamento do arquivo.\n");
		printf("DEBUG: ERRO AO FECHAR BIN\n");
		exit(1);
	}
}

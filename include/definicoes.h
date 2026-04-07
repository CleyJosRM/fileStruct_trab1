/*
Inclusão de bibliotecas, definição de constantes, structs
e funções auxiliares que podem ser usadas por todo o código.
*/

#ifndef DEFINICOES_H
	#define DEFINICOES_H

	#include<stdlib.h>
	#include<stdbool.h>
	#include<stdio.h>
	#include<string.h>
	#include<ctype.h>
	

	#ifdef PRINT_ERROS // se esse símbolo estiver definido
	#	define DEBUG(...) printf(__VA_ARGS__); // substitua DEBUG por printf e copie os argumentos
	#else // se não estiver definido
	# 	define DEBUG(...) // substitua DEBUG por nada, i.e, apague
	#endif

	#define HEADER_S 17 // O tamanho do registro de cabeçalho
	#define REG_DADOS_S 80 // O tamanho de cada registro de dados
	#define BYTES_FIXOS_S 37 // Quantidade de bytes usada para campos fixos no registro de dados, incluindo campos que armazenam tamanhos de nomes
	#define CAMPOS_STRINGS 2 // Quantidade de campos strings em um registro
	#define CAMPOS_INT 7 // Quantidade de campos inteiros em um registro (sem contar indicadores de tamanho)

	#define LIXO 0x24 // O conteúdo de bytes não utilizados é 0x24, $ em ascii, apelidado de LIXO nesse código
	#define LIXO_INT 0x24242424
	typedef struct reg_dados_struct REG_DADOS_STRUCT; // Representação do registro de dados na memória para ajudar na organização do código. Não é a mesma estrutura byte a byte.

	// Todos os scripts devem ver a organização interna do struct para que ele ajude a organizar as variáveis
	struct reg_dados_struct { // ESSE STRUCT SERVE APENAS PARA ORGANIZAR O CÓDIGO E ELE NÃO É ESCRITO DIRETAMENTE NO ARQUIVO!
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

	void ScanQuoteString(char *str);
	void BinarioNaTela(char *arquivo);

	// Função auxiliar para processar strings
	// Lê uma string campo e a duplica na memória, e coloca o endereço dessa cópia em uma variável fora do escopo cujo endereço foi passado
	char* processar_string(char* campo, int* tam);

	// Função auxiliar para processar inteiros
	// Lê uma string e retorna o número correspondente. Se for nula ou sem comprimento, retorna erro.
	int processar_int(char* campo);

	// Dado uma stream com cursor no início de um registro de dados, imprime o registro de dados
	void print_registro(FILE* filestream_bin);

	// lê e adiciona o campo lido ao registro chave de busca
	void ler_campos(REG_DADOS_STRUCT* registro_busca, int* mask);

	bool check_registro(REG_DADOS_STRUCT* busca, int mask, int RRN, FILE* bin);

	void ExibirBinario(char *arquivo);

#endif
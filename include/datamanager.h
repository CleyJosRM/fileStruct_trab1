/*
Operações que alteram o estado do binário. 
Gerencia as estruturas de dados relacionadas.
*/

#ifndef DATA_MANAGER_H
	#define DATA_MANAGER_H

	#include "definicoes.h"
	
	FILE* abre_binario(char* arquivoBin, char* modo);
	
	int fecha_binario(FILE* filestream_bin);

	// Atualiza o cabecalho
	void atualizar_cabecalho(char* arquivoBin, int topo, int proxRRN);

	// A partir de um binário com cursor no início de um registro de dados, escreve ele no struct passado por referência
	bool load_registro(FILE* filestream_bin, REG_DADOS_STRUCT* mem_destino);


	bool check_registro(REG_DADOS_STRUCT* chave, int mask, int RRN, FILE* bin);

	// Escreve o registro de dados no arquivo binário fornecido e nas estruturas de dados
	void escreve_registro(REG_DADOS_STRUCT* registro_lido, FILE* filestream_bin);

	void atualiza_registro(REG_DADOS_STRUCT* campos_novos, int mask, int RRN, FILE* filestream_bin);

#endif
#include "definicoes.h"

// GETTERS E SETTERS

static inline int get_RRNraiz(byteNoB* cabecalho){
	return *((int*)&(cabecalho[BYTEOFFSET_RRNraiz]));
}

static inline int get_topo(byteNoB* cabecalho){
	return *((int*)&(cabecalho[BYTEOFFSET_TOPO]));	
}

static inline int get_proxRRN(byteNoB* cabecalho){
	return *((int*)&(cabecalho[BYTEOFFSET_PROXRRN]));	
}

static inline int get_nroNos(byteNoB* cabecalho){
	return *((int*)&(cabecalho[BYTEOFFSET_NRONOS]));
}

// FUNÇÃO AUXILIAR

ENTRADA_INDICE get_entrada_nula(){
	ENTRADA_INDICE entradaNula;
	entradaNula.chave = -1;
	entradaNula.RRNdados = -1;
	entradaNula.RRNdescendente = -1;

	return entradaNula;
}

bool check_entrada_nula(ENTRADA_INDICE entrada){
	ENTRADA_INDICE nula = get_entrada_nula();
	if(entrada.chave == nula.chave && entrada.RRNdados == nula.RRNdados && entrada.RRNdescendente == nula.RRNdescendente){
		return true;
	}
	return false;
}

void imprimir_no_bytes(byteNoB* no){
	for(int i=0; i<TAM_NO_BTREE; i++){
		printf("%2x ", no[i]);
		if(i%10 == 9){
			printf("\n");
		}
	}
	printf("\n");
}

void imprimir_no(byteNoB* noB, int RRN){
	printf("\n--- NÓ RRN = %d ---\n", RRN);
	imprimir_no_bytes(noB);
    printf("removido: %c | próximo: %d | tipoNo: %d | nroChaves: %d\n",
			noB[0],
            *(int*)&noB[1],
            *(int*)&noB[5],
            *(int*)&noB[9]);

    printf("  Chaves: C1: %d, C2: %d, C3: %d\n",
			*(int*)&noB[13],
			*(int*)&noB[21],
            *(int*)&noB[29]);

    printf("  RRN dados: PR1: %d, PR2: %d, PR3: %d\n",
            *(int*)&noB[17],
            *(int*)&noB[25],
            *(int*)&noB[33]);

    printf("  RRN descendentes: P1: %d, P2: %d, P3: %d, P4: %d\n",
            *(int*)&noB[37],
            *(int*)&noB[41],
            *(int*)&noB[45],
            *(int*)&noB[49]);
}

void imprimir_cabecalho(byteNoB* cabecalho){
	printf("status: %c\nnoRaiz: %d\ntopo: %d\nproxRRN: %d\nnroNos: %d\n", 
           cabecalho[0], 
           *((int*)&cabecalho[1]), 
           *((int*)&cabecalho[5]), 
           *((int*)&cabecalho[9]), 
           *((int*)&cabecalho[13]));
}

// FUNÇÕES QUE LIDAM COM O DISCO, DIVIDIDAS EM LOAD & STORE

void armazenar_no(FILE* arvoreB, int RRN, byteNoB* no){
	fseek(arvoreB, TAM_CABECALHO_BTREE+TAM_NO_BTREE*RRN, SEEK_SET);
	fwrite(no, TAM_NO_BTREE, 1, arvoreB);
}

void carregar_no(FILE* arvoreB, int RRN, byteNoB* buffer){
	fseek(arvoreB, TAM_CABECALHO_BTREE+TAM_NO_BTREE*RRN, SEEK_SET);
	fread(buffer, TAM_NO_BTREE, 1, arvoreB);
}


void armazenar_cabecalho(FILE* arvoreB, byteNoB* buffer){
	fseek(arvoreB, 0, SEEK_SET);
	fwrite(buffer, TAM_CABECALHO_BTREE, 1, arvoreB);
}

void carregar_cabecalho(FILE* arvoreB, byteNoB* buffer, bool statusInconsistente){
	if(arvoreB == NULL || buffer == NULL){
		printf("Erro em carregar_cabecalho!\n");
		exit(1);
	}

	if(statusInconsistente){
		fseek(arvoreB, 0, SEEK_SET);
		char status = '1';
		fwrite(&status, 1, 1, arvoreB);
		fread(buffer+1, TAM_CABECALHO_BTREE-1, 1, arvoreB);
		buffer[0] = '1';
		armazenar_cabecalho(arvoreB, buffer);
	}else{
		fseek(arvoreB, 0, SEEK_SET);
		fread(buffer, TAM_CABECALHO_BTREE, 1, arvoreB);
	}
}

// FUNÇÕES QUE PROCESSAM NÓS DA ÁRVORE B EM MEMÓRIA:

// Lê um nó com n entradas e as coloca no vetorEntradas
void get_entradas(ENTRADA_INDICE* vetorEntradas, int n, byteNoB* no){
	int* C_i = (int*)&no[BYTEOFFSET_C1]; // C1
	int* PR_i = (int*)&no[BYTEOFFSET_C1+4]; // PR1
	int* P_i = (int*)&no[BYTEOFFSET_P1+4]; // P2

	for(int i=0; i<n; i++){
			ENTRADA_INDICE temp = {*C_i, *PR_i, *P_i};
			vetorEntradas[i] = temp;
			C_i+=2;
			PR_i+=2;
			P_i++;
	}
}

// Lê um vetor com n entradas e as coloca no nó, ajustando nroChaves
void set_entradas(ENTRADA_INDICE* vetorEntradas, int n, byteNoB* no){
	
	ENTRADA_INDICE nulo = get_entrada_nula();
	
	for(int i=0; i<n; i++){
		*(int*)&no[BYTEOFFSET_C1+8*i] = vetorEntradas[i].chave;
		*(int*)&no[BYTEOFFSET_C1+8*i+4] = vetorEntradas[i].RRNdados;
		*(int*)&no[BYTEOFFSET_P1+4*i+4] = vetorEntradas[i].RRNdescendente;
	}
	for(int i=n; i<ORDEM_BTREE - 1; i++){
		*(int*)&no[BYTEOFFSET_C1+8*i] = nulo.chave;
		*(int*)&no[BYTEOFFSET_C1+8*i+4] = nulo.RRNdados;
		*(int*)&no[BYTEOFFSET_P1+4*i+4] = nulo.RRNdescendente;
	}
	*(int*)&no[BYTEOFFSET_NROCHAVES] = n;

	printf("Nó depois das entradas setadas:\n");
	imprimir_no(no, 999);
}

/*
retorna se a chave foi encontrada ou não, e um inteiro
Se a chave foi encontrada, esse inteiro é o byte offset de dados
Se a chave não foi encontrada, esse inteiro é o RRN descendente
*/
bool percorrer_no(byteNoB* no, int chaveBusca, int* retRRN){

		int* chaveAtual = (int*)&(no[BYTEOFFSET_C1]); // C1
		int* RRNnoBtemp = (int*)&(no[BYTEOFFSET_P1]); // P1
		bool achou = false;

		// Se chave < C_i, vá para P_i
		
		int chavesRestantes = *(int*)&(no[BYTEOFFSET_NROCHAVES]);
		while(*chaveAtual <= chaveBusca && chavesRestantes >= 0){ // enquanto a chave atual for menor do que a chave de busca
			// vai para o próximo C_i, e próximo P_i
			if(chavesRestantes == 0) break; // Se chegou no último P válido, saia
			if(*chaveAtual == chaveBusca){
				RRNnoBtemp = chaveAtual + 1; // PR1, PR2, PR3
				achou = true;
				break;
			}
			chaveAtual+=2; // C2, C3, dontcare
			RRNnoBtemp++; // P2, P3, P4
			chavesRestantes--;
		}

		*retRRN = *RRNnoBtemp;
		return achou;
}

/*
Parâmetros: ponteiro para cabeçalho da árvore, o tipo do nó inserido, e ponteiro para buffer para guardar o novo nó
Faz: Preenche o buffer com as informações do novo nó, e atualiza o cabeçalho
Retorno: RRN do novo nó
*/
int criar_no(byteNoB* cabecalho, int tipoNo, byteNoB* novoNo){
	*(int*)&novoNo[BYTEOFFSET_REMOVIDO] = '0';
	*(int*)&novoNo[BYTEOFFSET_PROXPILHA] = -1;
	*(int*)&novoNo[BYTEOFFSET_TIPONO] = tipoNo; // !!! COMO SABER O TIPO DO NÓ?
	*(int*)&novoNo[BYTEOFFSET_NROCHAVES] = 0;
	
	ENTRADA_INDICE aux[1]; // apenas para chamar a função
	set_entradas(aux, 0, novoNo); // vai inicializar com entradas nulas
	*(int*)&novoNo[BYTEOFFSET_P1] = -1;

	// Obtendo RRN do nó e atualizando cabeçalho:
	int RRNnovoNo = *((int*)&cabecalho[BYTEOFFSET_PROXRRN]);
	*(int*)&cabecalho[BYTEOFFSET_PROXRRN] += 1;
	*(int*)&cabecalho[BYTEOFFSET_NRONOS] += 1;
	return RRNnovoNo;
}










int buscar_chave_rec(FILE* arvoreB, int chaveBusca, byteNoB* no){

	int BO_RRN; // BO de dados, se encontrou, ou RRN do descendente, se ainda há de procurar mais
	if ( percorrer_no(no, chaveBusca, &BO_RRN) == true || BO_RRN == -1){ // se encontrou a chave, ou não tem mais onde procurar
		return BO_RRN;
	}
	else{
		byteNoB proximoNo[TAM_NO_BTREE];
		carregar_no(arvoreB, BO_RRN, proximoNo);
		imprimir_no(proximoNo, BO_RRN);
		return buscar_chave_rec(arvoreB, chaveBusca, proximoNo);
	}

}

int buscar_chave(FILE* arvoreB, int chaveBusca){ //
	
	byteNoB cabecalho[TAM_CABECALHO_BTREE];
	carregar_cabecalho(arvoreB, cabecalho, false);
	imprimir_cabecalho(cabecalho);

	if(cabecalho[BYTEOFFSET_STATUS] == '1'){
		printf("ATENÇÃO, STATUS INCONSISTENTE!\n");
	}

	byteNoB raiz[TAM_NO_BTREE];
	int RRNraiz = get_RRNraiz(cabecalho);
	carregar_no(arvoreB, RRNraiz, raiz);
	imprimir_no(raiz, RRNraiz);

	return buscar_chave_rec(arvoreB, chaveBusca, raiz);
}










int comparar_entradas(const void* entrada1, const void* entrada2){
	ENTRADA_INDICE e1 = *(ENTRADA_INDICE*)entrada1;
	ENTRADA_INDICE e2 = *(ENTRADA_INDICE*)entrada2;
	return e1.chave - e2.chave;
}

static void inserir_entrada_em_no_shiftada(ENTRADA_INDICE novaEntrada, byteNoB* no){
	
	int nroChaves = *(int*)&(no[BYTEOFFSET_NROCHAVES]);
	if(nroChaves == ORDEM_BTREE - 1){
		printf("O nó está lotado! Não há como inserir.\n");
		exit(1);
	}

	printf("Vamos inserir a chave %d nesse nó:\n", novaEntrada.chave);
	imprimir_no(no, 999);

	ENTRADA_INDICE entradas[nroChaves+1]; // vetor com uma entrada a mais do que o nó tem
	get_entradas(entradas, nroChaves, no); // deixa a última posição sem definir
	bool inseriu = false;
	int i = nroChaves-1; // penúltima posição
	for(; i>=0; i--){
		if(entradas[i].chave > novaEntrada.chave){
			entradas[i+1] = entradas[i];
		}else{
			entradas[i+1] = novaEntrada;
			inseriu = true;
			break;
		}
	}

	if(!inseriu){ // se shiftou todas as entradas sem inserir
		entradas[0] = novaEntrada; // insere no começo
	}

	set_entradas(entradas, nroChaves+1, no);
}


ENTRADA_INDICE inserir_entrada_em_no(FILE* arvoreB, byteNoB* cabecalho, byteNoB* noAtual, int RRNatual, ENTRADA_INDICE entradaInserir, int* tipoNoCriar){
	
	imprimir_no(noAtual, RRNatual);

	if(check_entrada_nula(entradaInserir)){
		return entradaInserir; // caso a entradaInserida seja inválida, não faz nada, pois não ocorreu promoção na chamada anterior
	}

	// Lembrando que m == ORDEM_BTREE

	int nroChaves = *(int*)&(noAtual[BYTEOFFSET_NROCHAVES]);
	if(nroChaves < ORDEM_BTREE - 1){ // PRONTO! exceto armazenar_no
		// Se existe espaço no nó, insere ordenadamente (shiftada em memória principal, sobrescreve no arquivo)
		inserir_entrada_em_no_shiftada(entradaInserir, noAtual);
		armazenar_no(arvoreB, RRNatual, noAtual);
		ENTRADA_INDICE retorno = get_entrada_nula();
		return retorno; // retorna uma entrada inválida, para indicar que não há promoção
	}else{ // PRONTO! 

		// Ordenando entradas
		ENTRADA_INDICE vetorEntradas[m];
		get_entradas(vetorEntradas, m-1, noAtual);
		vetorEntradas[m-1] = entradaInserir;
		qsort(vetorEntradas, m, sizeof(ENTRADA_INDICE), comparar_entradas);
		// Escolhendo a promovida
		ENTRADA_INDICE entradaPromovida = vetorEntradas[m/2];
		// Criando novo nó (se é o primeiro split, tipo folha, senão tipo intermediário)
		byteNoB novoNo[TAM_NO_BTREE];
		int RRNnovoNo = criar_no(cabecalho, *tipoNoCriar, novoNo);
		*tipoNoCriar = TIPOINTERMEDIARIO;
		// Troca de "ponteiros"
		novoNo[BYTEOFFSET_P1] = entradaPromovida.RRNdescendente;
		entradaPromovida.RRNdescendente = RRNnovoNo;
		// Refazendo os nós com as entradas e armazenando em disco
		set_entradas(vetorEntradas, m/2, noAtual);
		set_entradas(vetorEntradas+m/2+1, m-m/2-1, novoNo);
		armazenar_no(arvoreB, RRNatual, noAtual);
		armazenar_no(arvoreB, RRNnovoNo, novoNo);
		// Retornando a entradaPromovida, que mantém a referência para o nó criado
		return entradaPromovida;
	}
	
}


// PRONTA!
static ENTRADA_INDICE inserir_entrada_na_arvore_rec(FILE* arvoreB, byteNoB* cabecalho, byteNoB* noAtual, int RRNatual, ENTRADA_INDICE entradaInserir, int* tipoNoCriado) {

	imprimir_no(noAtual, RRNatual);

	int RRNdescendente;
	bool achou = percorrer_no(noAtual, entradaInserir.chave, &RRNdescendente);
	if(achou){
		printf("Chave %d encontrada! Byteoffset: %d", entradaInserir.chave, RRNdescendente);
	}else{
		printf("Chave %d não encontrada. Procure no RRN %d da BTree.", entradaInserir.chave, RRNdescendente);
	}

	if(RRNdescendente == -1){ // se achou o lugar certo da inserção
		return inserir_entrada_em_no(arvoreB, cabecalho, noAtual, RRNatual, entradaInserir, tipoNoCriado); // retorna entrada promovida
	}else{
		byteNoB descendente[TAM_NO_BTREE];
		carregar_no(arvoreB, RRNdescendente, descendente);
		entradaInserir = inserir_entrada_na_arvore_rec(arvoreB, cabecalho, descendente, RRNdescendente, entradaInserir, tipoNoCriado);
		
		return inserir_entrada_em_no(arvoreB, cabecalho, noAtual, RRNatual, entradaInserir, tipoNoCriado);
	}
}

void criar_raiz_e_inserir(FILE* arvoreB, byteNoB* cabecalho, ENTRADA_INDICE entradaRaiz){

	int tipo = TIPORAIZ;
	if(get_nroNos(cabecalho) == 0){ // se a árvore não tem nós
		tipo = TIPOFOLHA; // o tipo do nó raiz é folha
	}

	byteNoB novaRaiz[TAM_NO_BTREE];
	int RRNnovaRaiz = criar_no(cabecalho, tipo, novaRaiz);
	*(int*)&cabecalho[BYTEOFFSET_RRNraiz] = RRNnovaRaiz; // criar_no retorna RRN do nó criado.

	// Insere a entrada na raiz e armazena ela no disco
	inserir_entrada_em_no_shiftada(entradaRaiz, novaRaiz);
	armazenar_no(arvoreB, RRNnovaRaiz, novaRaiz);
	printf("Raiz que acabou de ser criada!:\n");
	imprimir_no(novaRaiz, 999);
}

/// PRONTA!
void inserir_entrada_na_arvore(FILE* arvoreB, int chave, int RRNdados){
	
	ENTRADA_INDICE inserirNaRaiz = {chave, RRNdados, -1};
	byteNoB cabecalho[TAM_CABECALHO_BTREE];
	carregar_cabecalho(arvoreB, cabecalho, true);

	imprimir_cabecalho(cabecalho);

	if(get_RRNraiz(cabecalho) == -1){ // Se não há raiz
		criar_raiz_e_inserir(arvoreB, cabecalho, inserirNaRaiz); // Cria a raiz e insere
		armazenar_cabecalho(arvoreB, cabecalho);
		return;
	}

	byteNoB raiz[TAM_NO_BTREE];
	int RRNraiz = get_RRNraiz(cabecalho);
	carregar_no(arvoreB, RRNraiz, raiz);
	int tipoNoCriar = TIPOFOLHA;

	ENTRADA_INDICE entradaRaiz = inserir_entrada_na_arvore_rec(arvoreB, cabecalho, raiz, RRNraiz, inserirNaRaiz, &tipoNoCriar);
	
	if(!check_entrada_nula(entradaRaiz)){ // Se alguma entrada foi promovida, precisamos criar uma nova raiz
		criar_raiz_e_inserir(arvoreB, cabecalho, entradaRaiz);
	}

	armazenar_cabecalho(arvoreB, cabecalho);
}

void criar_BTree(FILE* arquivo){

	byteNoB cabecalhoInicial[TAM_CABECALHO_BTREE];
	cabecalhoInicial[BYTEOFFSET_STATUS] = '0';
	*(int*)&cabecalhoInicial[BYTEOFFSET_RRNraiz] = -1;
	*(int*)&cabecalhoInicial[BYTEOFFSET_TOPO] = -1;
	*(int*)&cabecalhoInicial[BYTEOFFSET_PROXRRN] = 0;
	*(int*)&cabecalhoInicial[BYTEOFFSET_NRONOS] = 0;	

	fseek(arquivo, 0, SEEK_SET);
	fwrite(cabecalhoInicial, TAM_CABECALHO_BTREE, 1, arquivo);
}

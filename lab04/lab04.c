 	
// Lab. 4 - O jogo do Bacon	


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define INF INT_MAX

// Lista de filmes de cada ator
typedef struct Filmes{

	char *filme;
	struct Filmes *prox;
} listaFilmes;

// Estrutura que armazena o nomes dos atores com seus respectivos filmes
typedef struct AtorFilmes{

	char *nome;
	listaFilmes* filmes;
	int quantFilmes;
	struct AtorFilmes *prox;
} atorFilmes;

// Estrutura da tabela de hash
typedef struct Hash{

	int quant, tam_tab;
	atorFilmes **ator;
} hash;

// Estrutura da lista de adjacencia, valor do vertice e da aresta
typedef struct AdjLista{

	int vertice;
	listaFilmes *aresta;
	struct AdjLista *prox;
} adjLista;

// Estutura do grafo, quantidade, informacoes e lista de adjacencia de cada vertice
typedef struct Grafo{

	int quantVert;
	atorFilmes *info;
	adjLista **adj;
} grafo;

// Estruturda da pilha
typedef struct infoPILHA{
	int valor;
	struct infoPILHA *prox;
}infoPilha;

// Topo da pilha
typedef struct PILHA{
	infoPilha *topo;
}Pilha;
	
// Funcoes que gerenciam a tabela de hash
hash* criaHash(int tam_tab);
void liberaHash(hash* ha);
int valorNome(char* nome);
int hashDiv(int chave, int tam_tab);
int posicao(hash* ha, char* nome);
int busca(hash* ha, char* nome);
void insereHash(hash* ha, char* nome, char* filme);
void insereFilmes(hash* ha, char* nome, char* filme);
listaFilmes* iniciaFilmes();

// Funcoes que gerenciam o grafo
grafo* iniciaGrafo(int nVert);
void iniciaVert(grafo* G, int i, atorFilmes* ator);
void insereAresta(grafo* G, int v, int w, listaFilmes* filme); 
void insereDados(hash *ha, grafo* G);
void encontraAresta(grafo* G);
void destroiGrafo(grafo* G);
void auxCaminho(grafo* G, int v, int visitados[], Pilha* pilha);
void caminhoMinimo(grafo* G, int s, int* anterior, int* distancia);
void percurso(Pilha* caminho, int* anterior, int ini, int fim);

// Funcoes que gerenciam a pilha
void iniciaPilha(Pilha* pilha);
void push(Pilha* pilha, int n);
int pop(Pilha* pilha);
void liberaPilha(Pilha* pilha);

int main(int argc, char **argv){

	char *baseDados = argv[1];
	char *consulta = argv[2];
	char linha[300], ator1[100], ator2[100];
	char *nomeAtor, *nomeFilme;
	int repetido, numL;
	char c;
	int i, j, k;
	adjLista* p;

	// Abre o arquivo da base de dados
	FILE *arquivo;
	arquivo = fopen(baseDados, "r");
	
	// Cria a tabela de hash com i posicoes
	hash* tabela = criaHash(1000);

	// Armazena os dados do aquivo na tabela de hash
	rewind(arquivo);
	while(!feof(arquivo)){
		fgets(linha, 300, arquivo);

		// Copia da base de dados o nome do ator com seu respectivo filme
		nomeAtor = strtok(linha, "\t");
		nomeFilme = strtok(NULL, "\t");

		if(nomeFilme != NULL)
			nomeFilme[strcspn(nomeFilme, "\n")] = 0;
	
		// Verfica se o ator copiado ja esta presente na tabela
		repetido = 0;
		repetido = busca(tabela, nomeAtor);

		// Insere o ator na tabela, se ele ja estive, apenas seu filme e inserido
		if(nomeFilme != NULL && nomeAtor != NULL){
			if(repetido == 0)
				insereHash(tabela, nomeAtor, nomeFilme);			
			else
				insereFilmes(tabela, nomeAtor, nomeFilme);
		}				
	}

	// Fecha o aquivo da dase de dados
	fclose(arquivo);

	// Inicia o grafo e insere os vertices
	grafo* G = iniciaGrafo(tabela->quant);
	insereDados(tabela, G);

	encontraAresta(G);

	// Libera a memoria da tabela que nao e mais necessaria
	liberaHash(tabela);

	// Vetores que irao armazenar os caminhos e as distancias entre os vertices
	int anterior[G->quantVert];
	int distancia[G->quantVert];
	Pilha caminho;

	// Abre o arquivo com os dados a serem consultados
	arquivo = fopen(consulta, "r");
	
	// Le a quantidade de linhas do aquivo de consulta
	numL=0;
	while(!feof(arquivo)){
		c = fgetc(arquivo);
		if(c == '\n')
			numL++;
	}

	// Le os atores do aquivo de consulta e imprime oa caminha de um ate outro
	rewind(arquivo);
	while(numL > 0){
		numL -= 2;
		
		fgets(ator1, ' ', arquivo);
		fgets(ator2, ' ', arquivo);
		
		// Remove o carectere de quebra de linha do nome
		ator1[strcspn(ator1, "\n")] = 0;
		ator2[strcspn(ator2, "\n")] = 0;
	
		// Procura o indice do vertice do ator1 e do ator2 no grafo
		for(i=0; i<G->quantVert; i++){
			if(strcmp(G->info[i].nome, ator1) == 0)
				j = i;
			if(strcmp(G->info[i].nome, ator2) == 0)
				k = i;	
		}
		// Encontra o caminho minimo entre os vertices
		caminhoMinimo(G, j, anterior, distancia);

		if(distancia[k] != INF){

			// Inializa e monta a pilha com percurso em ordem
			iniciaPilha(&caminho);
			percurso(&caminho, anterior, j, k);

			i = pop(&caminho);
			printf("%s\n", G->info[i].nome);
		
			while(caminho.topo != NULL){
				j = pop(&caminho);

				// Procura a aresta corespondente
				p = G->adj[i];
				while(p->vertice != j)
					p = p->prox;

				printf("trabalhou em \"%s\" com\n", p->aresta->filme);
				printf("%s\n", G->info[j].nome);

				i = j;
			}
			liberaPilha(&caminho);
		}else
			printf("CAMINHO NAO ENCONTRADO\n");	
	}

	// Fecha o arquivo de consulta e libera a memoria do grafo
	fclose(arquivo);
	destroiGrafo(G);
	
	return 0;
}

// Função que cria e inicia uma tabela hash
hash* criaHash(int tam_tab){

	int i;

	hash* ha = (hash*) malloc(sizeof(hash));
	if(ha != NULL){
		ha->tam_tab = tam_tab;
		ha->ator = (atorFilmes**) malloc(tam_tab * sizeof(atorFilmes*));

		if(ha->ator == NULL){
			free(ha);
			return NULL;
		}

		ha->quant= 0;
		for(i=0; i<ha->tam_tab; i++)
			ha->ator[i] = NULL;
	}

	return ha;	
}

// Função que destroi e libera a memoria de uma tabela hash
void liberaHash(hash* ha){

	int i;
	atorFilmes *p, *aux;

	// Desaloca o vetor de atores
	for(i=0; i<ha->tam_tab; i++){
		if(ha->ator[i] != NULL){
			p = ha->ator[i];
			while(p != NULL){
				aux = p;
				free(aux);
				p = p->prox;
			}
		}
	}

	// Desaloca hash
	free(ha->ator);
	free(ha);	
}

// Atribui um valor para o nome de cada ator
int valorNome(char* nome){
	
	int i, tam, valor = 7;

	tam = strlen(nome);
	for(i=0; i<tam; i++)
		valor = 31 * valor + (int) nome[i];

	return valor;
}

// Encontra um posicao na tabela para um determinada chave
int hashDiv(int chave, int tam_tab){

	return (chave & 0x7FFFFFFF) % tam_tab;
}	

// Encontra a posicao em que um item e inserido na tabela
int posicao(hash* ha, char* nome){
	
	int chave, pos;

	chave = valorNome(nome);
	pos = hashDiv(chave, ha->tam_tab);

	return pos;
}

// Insere os atores com seus respectivo filmes na tabela de hash
void insereHash(hash* ha, char* nome, char* filme){

	int pos;
	
	pos = posicao(ha, nome);

	atorFilmes* novo = malloc(sizeof(atorFilmes));

	// Guardo o nome	
	novo->nome = strdup(nome);

	// Guarda o primeiro filme
	novo->filmes = iniciaFilmes();
	listaFilmes *f = malloc(sizeof(listaFilmes));
	f->filme = strdup(filme);
	f->prox = novo->filmes->prox;
	novo->filmes->prox = f;
	
	// Posiciona na tabela
	if(ha->ator[pos] == NULL){
		ha->ator[pos] = novo;
	}else{
		novo->prox = ha->ator[pos];
		ha->ator[pos] = novo;		
	}

	ha->quant++;
}

// Insere os filmes de um ator que ja esta presente na tabela
void insereFilmes(hash* ha, char* nome, char* filme){

	int pos;
	
	pos = posicao(ha, nome);
	
	// Procura a posoicao do ator na tabela e insere o filme
	atorFilmes* p = ha->ator[pos];
	while(p != NULL){
		if(strcmp(ha->ator[pos]->nome, nome) == 0){
			listaFilmes *f = malloc(sizeof(listaFilmes));
			f->filme = strdup(filme);
			f->prox = p->filmes->prox;
			p->filmes->prox = f;

			p->quantFilmes++;
			break;
		}
		p = p->prox;
	}
}	

// Funcao que busca um ator na tabela, retorna 1 se encotrar e 0 caso contrário
int busca(hash* ha, char* nome){

	int pos;

	pos = posicao(ha, nome);

	if(ha->ator[pos] == NULL)
		return 0;
	else{
		atorFilmes* p = ha->ator[pos];
		while(p != NULL){
			if(strcmp(ha->ator[pos]->nome, nome) == 0)
				return 1;

			p = p->prox;
		}
	}

	return 0;
}

// Inicia  a lista de filmes
listaFilmes* iniciaFilmes(){
	
	listaFilmes* filme = malloc(sizeof(listaFilmes));
	filme->prox = NULL;

	return filme;
}

// Recebe o numero de vertices e inicia o grafo
grafo* iniciaGrafo(int nVert){

	int i;

	grafo *G = (grafo*) malloc(sizeof(grafo));
	G->adj = (adjLista**) malloc(nVert * sizeof(adjLista*));

	for(i=0; i<nVert; i++)
		G->adj[i] = NULL;

	G->info = (atorFilmes*) malloc(nVert * sizeof(atorFilmes));
	G->quantVert = nVert;
	
	return G;
}

// Pega os dados da tabela de hash e insere nos vertices do grafo
void insereDados(hash *ha, grafo* G){

	int i, k=0;
	atorFilmes *p;

	// Percorre a tabela e insere cada item em um vertice
	for(i=0; i<ha->tam_tab; i++){
		if(ha->ator[i] != NULL){
			p = ha->ator[i];
			while(p != NULL){
				iniciaVert(G, k, p);
				k++;

				p = p->prox;
			}
		}
	}	
}

// Insere as informacoes de cada vertice
void iniciaVert(grafo* G, int i, atorFilmes* ator){

	G->info[i] = *ator;
}

// Encontra arestas a partir da vetor de vertices
void encontraAresta(grafo* G){

	int i, j;
	listaFilmes *p, *q;
	
	// Encontra o filme que liga dois atores e cria a aresta
	for(i=0; i<G->quantVert; i++){
		for(j=0; j<G->quantVert; j++){
			if(strcmp(G->info[i].nome, G->info[j].nome) != 0){
				if(strcmp(G->info[i].nome, G->info[j].nome) != 0){
					p = G->info[i].filmes->prox;
					q = G->info[j].filmes->prox;
					while(p != NULL){
						while(q != NULL){
							if(strcmp(p->filme, q->filme) == 0)
								insereAresta(G, i, j, q);

							q = q->prox;
						}
						q = G->info[j].filmes->prox;
						p = p->prox;
					}
				}
			}
		}
	}
}

// Insere as arestas no grafo
void insereAresta(grafo* G, int v, int w, listaFilmes* filme){

	adjLista *L;

	// O vertice recebe o indice vetor de vertices
	// A aresta recebe um ponteiro para o nome do filme
	L = (adjLista*) malloc(sizeof(adjLista));
	L->vertice = w;
	L->aresta = filme;
	L->prox = G->adj[v];
	G->adj[v] = L;
}

// Libera a memoria utilizada pelo grafo
void destroiGrafo(grafo* G){

	int i;
	listaFilmes *q, *aux2;

	// Desaloca a lista de filmes filmes
	for(i=0; i<G->quantVert; i++){
			q = G->info[i].filmes;
			while(q != NULL){
				aux2 = q;
				free(aux2->filme);
				free(aux2);
				q = q->prox;	
			}
		
	}

	// Desaloca nomes
	for(i=0; i<G->quantVert; i++){		
		free(G->info[i].filmes);
		free(G->info[i].nome);	
	}	
		
	for(i=0; i<G->quantVert; i++)
		free(G->adj[i]);

	free(G);
}

// Funcao que auxila encotrar o menor caminho entre os vertices
void auxCaminho(grafo* G, int v, int visitados[], Pilha* pilha){

	visitados[v] = 1;

	adjLista* L = G->adj[v];
	while(L != NULL){
		if(!visitados[L->vertice])
			auxCaminho(G, L->vertice, visitados, pilha);

		L = L->prox;
	}

	push(pilha, v);
}
	
// Encontra o menor caminho entre dois atores
void caminhoMinimo(grafo* G, int s, int* anterior, int* distancia){

	Pilha pilha;
	int i, u;
	int j = G->quantVert * G->quantVert;
	adjLista* L;
	int visitados[j];

	iniciaPilha(&pilha);

	// Marca todos o vertices como nao visitados
	for(i=0; i<j; i++)
		visitados[i] = 0;

	for(i=s; i<G->quantVert; i++)
		auxCaminho(G, i, visitados, &pilha);
	
	// Atribui uma distancia infinita entre todos os vertices
	for(i=0; i<G->quantVert; i++)
		distancia[i] = INF;

	distancia[s] = 0;
	anterior[s] = -1;

	// Calcula as distancia e guarda o vertice o anterior
	i=0;
	while(pilha.topo != NULL){
		u = pop(&pilha);
		L = G->adj[u];
		
		if(distancia[u] != INF){
			while(L != NULL){
				if(distancia[L->vertice] > distancia[u] + 1){
					distancia[L->vertice] = distancia[u] + 1;
					anterior[L->vertice] = u;	
				}
				L = L->prox;		
			}
		}
	}

	liberaPilha(&pilha);
}

// A partir de todos os caminhos minimos do grafo, determina o caminho entre dois vertices
void percurso(Pilha* caminho, int* anterior, int ini, int fim){

	push(caminho, fim);
	do{
		push(caminho, anterior[fim]);
		fim =  anterior[fim];	
	}while(ini != fim);	
}

// Incilializa a pilha
void iniciaPilha(Pilha* pilha){

	pilha->topo = NULL;
}

// Insere um elemento da pilha
void push(Pilha* pilha, int n){

	infoPilha* novo = (infoPilha*) malloc(sizeof(infoPilha));
	novo->valor = n;
	novo->prox = pilha->topo;
	pilha->topo = novo;
}

// Remove um elemento da pilha
int pop(Pilha* pilha){

	int i;

	if(pilha->topo == NULL)
		return -1;

	infoPilha *aux;
	aux = pilha->topo;
	pilha->topo = aux->prox;
	i = aux->valor;
	free(aux);

	return i;
}

// Libera a memoria da pilha
void liberaPilha(Pilha* pilha){
	
	infoPilha* aux;
	infoPilha* p = pilha->topo;
	while(p != NULL){
		aux = p;
		p = p->prox;
		free(aux);
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <limits.h>

// elemento das listas de adjacencia e de resposta
typedef struct estr 
{
  int adj; // elemento
	int peso; // custo
  struct estr *prox;
} NO;

// vertices do grafo (salas) - use este tipo ao criar o grafo 
typedef struct 
{
  int flag; // para uso na busca em largura e profundidade, se necessario
  bool aberto; // vale true se a sala em questao esta aberta
  int via; // use este campo se precisar computar algum caminho etc.
	int dist; // use este campo se precisar computar alguma distancia etc.
  NO* inicio;
} VERTICE;
// ------------------------------------------------------------------

// funcoes auxiliares

// criação e consulta de estruturas
void exibirRes(NO* p){
  while (p) {
    printf(" -- peso: %i --> (%i) -> ", p->peso, p->adj);
    p = p->prox;
  }
  if (p == NULL) printf(" NULL");
  printf("\n");
}

void exibirLista(NO* p){
  while (p) {
    printf("adj: %i(peso: %i) -> ", p->adj, p->peso);
    p = p->prox;
  }
}

void exibirGrafo(VERTICE* grafo, int N) {  
  for(int i = 1; i <= N; i++){
    printf("Flag: %i | Via: %i | Dist: %i | V: %i; ", grafo[i].flag, grafo[i].via, grafo[i].dist, i);
    exibirLista(grafo[i].inicio);
    printf("\n");
  }
}

void insereArestaPeso(VERTICE *grafo, int origem, int destino, int peso) {
  NO *novo = (NO*)malloc(sizeof(NO));

  novo->adj = destino;
  novo->peso = peso;
  novo->prox = grafo[origem].inicio;

  grafo[origem].inicio = novo;
}

void populaGrafo(VERTICE *grafo, int *ijpeso, int arestas) {
  for (int i = 0; i < arestas * 3; i += 3) {
    insereArestaPeso(grafo, ijpeso[i + 1], ijpeso[i], ijpeso [i + 2]);
    insereArestaPeso(grafo, ijpeso[i], ijpeso[i + 1], ijpeso [i + 2]);
  }
}

void inicializaGrafo(VERTICE *grafo, int *aberto, int N) {
  for (int i = 1; i <= N; i++) {
    grafo[i].inicio = NULL;
    grafo[i].flag = 1;
    grafo[i].dist = INT_MAX/2;
    grafo[i].via = -1;

    if (aberto[i - 1] == 1) {
      grafo[i].aberto = true;
    } else {
      grafo[i].aberto = false;
    }
  }
}

void resetaGrafo(VERTICE *grafo, int N) {
  for (int i = 1; i <= N; i++) {
    grafo[i].flag = 1;
    grafo[i].dist = INT_MAX/2;
    grafo[i].via = -1;
  }
}

VERTICE *criaGrafo(int N, int A, int *ijpeso, int *aberto) {
  VERTICE* grafo = (VERTICE*)malloc(sizeof(VERTICE) * (N + 1)); // alocar espaço o bastante ter N vertices de 1 a N, desconsiderando o 0
  
  inicializaGrafo(grafo, aberto, N);

  populaGrafo(grafo, ijpeso, A);

  return grafo;
}
// ------------------------------------------------------------

// Funcoes para Dijkastra
void relaxaAresta(VERTICE *grafo, int origem, int destino) {
  NO *p = grafo[origem].inicio;

  // percorre até achar o vertice destino
  while (p && p->adj != destino) {
    p = p->prox;
  }

  if (p) {
    // compara distâncias e relaxa | coloca o antecessor
    if (grafo[destino].dist > (grafo[origem].dist + p->peso)) {
      grafo[destino].dist = grafo[origem].dist + p->peso;
      grafo[destino].via = origem;
    }
  }
}

bool existeNaoExplorado(VERTICE *grafo, int N, bool possuiChave) {
  // Verifica se há alguma flag == 1 | true
  if (possuiChave == true) {
    for (int i = 1; i < N; i++) {
      if (grafo[i].flag == 1) return true;
    }
  } else {
    for (int i = 1; i < N; i++) {
      if (grafo[i].flag == 1 && grafo[i].aberto == 1) return true;
    }
  }

  return false;
}

int menorDist(VERTICE *grafo, int N, bool possuiChave) {
  int i;

  // Percorre até achar o primeiro aberto
  if (possuiChave == true) {
    for (i = 1; i < N; i++) {
      if (grafo[i].flag == 1) break;
    }
  } else {
    for (i = 1; i < N; i++) {
      if (grafo[i].flag == 1 && grafo[i].aberto == 1) break;
    }
  }

  // Se n encontrou nenhum aberto retorne -1
  if (i == N) return -1;

  // Se encontrar, assume-se que é menor dist
  int menor = i;

  // percorre comparando até achar o menor
  if (possuiChave == true) {
    for (i = menor + 1; i <= N; i++){
      if(grafo[i].flag == 1 && grafo[menor].dist > grafo[i].dist) {
        menor = i;
      }
    }
  } else {
    for (i = menor + 1; i <= N; i++){
      if(grafo[i].flag == 1 && grafo[i].aberto == 1 && grafo[menor].dist > grafo[i].dist) {
        menor = i;
      }
    }
  }

  return menor;
}

void dijkstra(VERTICE *grafo, int N, int origem, bool possuiChave) {
  grafo[origem].dist = 0;

  while (existeNaoExplorado(grafo, N, possuiChave)) {
    int u = menorDist(grafo, N, possuiChave);
    grafo[u].flag = 0;

    NO* p = grafo[u].inicio;
    while (p) {
      relaxaAresta(grafo, u, p->adj);
      p = p->prox;
    }
  }
}
// --------------------------------------------------------

int retornaPeso(VERTICE *grafo, int origem, int destino) {
  NO* p = grafo[origem].inicio;
  while (p) {
    if(p->adj == destino)return p->peso;
    p = p->prox;
  }
  return INT_MAX; // Inalcançavel
}

int calculaPeso(NO *caminho) {
  int peso = 0;
  NO *p = caminho;

  while (p) {
    peso += p->peso;
    p = p->prox;
  }

  return peso; 
}

void concatenaListas(NO *lista1, NO *lista2) {
  NO *p = lista1;

  while(true) {
    if (p->prox->prox == NULL) {
      p->prox = lista2;
      break;
    }

    p = p->prox;
  }
}

NO *gravaCaminho(VERTICE *grafo, int origem, int destino) {
  VERTICE *head = (VERTICE*)malloc(sizeof(VERTICE));
  head->inicio = NULL;

  int atual = destino;
  while (true) {
    NO* novo = (NO*)malloc(sizeof(NO));

    if (atual == origem) {
      novo->adj = atual;
      novo->prox = head->inicio;

      head->inicio = novo;
    
      break;
    }

    novo->adj = atual;
    novo->peso = retornaPeso(grafo, atual, grafo[atual].via);
    novo->prox = head->inicio;

    head->inicio = novo;
    
    atual = grafo[atual].via;
  }

  return head->inicio;
}

NO *caminho(int N, int A, int *ijpeso, int *aberto, int inicio, int fim, int chave) {
	NO* resp;
	resp = NULL;

	// Código principal da funcao
  // Primeiro criamos um grafo com as infos passadas nos params
  VERTICE *grafo = criaGrafo(N, A, ijpeso, aberto);

  // Calculamos o menor caminho possivel sem a chave
  NO *respSemChave = NULL;

  dijkstra(grafo, N, inicio, false);

  if (grafo[fim].via != -1) {
    respSemChave = gravaCaminho(grafo, inicio, fim);
  }

  // Calculamos o menor caminho possível, porém 
  // passando obrigatoriamente na chave
  NO *respComChave = NULL;
  NO *listaAux = NULL;

  // Menor caminho do inicio até a chave
  if (grafo[chave].via != -1) {
    respComChave = gravaCaminho(grafo, inicio, chave);
  }

  // Menor caminho da chave até o fim
  resetaGrafo(grafo, N);

  dijkstra(grafo, N, chave, true);

  if (grafo[fim].via != -1) {
    listaAux = gravaCaminho(grafo, chave, fim);

    if (respComChave != NULL) {
      concatenaListas(respComChave, listaAux);
    }
  }

  // Caso não haja caminho possível, retornamos resp com NULL antecipadamente
  if (respSemChave == NULL && respComChave == NULL) return resp;

  // Comparamos o peso de ambas para determinar menor caminho
  int pesoSemChave = INT_MAX;
  int pesoComChave = INT_MAX;

  if (respSemChave != NULL) pesoSemChave = calculaPeso(respSemChave);
  if (respComChave != NULL) pesoComChave = calculaPeso(respComChave);

  if (pesoSemChave < pesoComChave) {
    resp = respSemChave;
  } else {
    resp = respComChave;
  }

  // -------------------------------------------------------------

	return resp;
}

//---------------------------------------------------------
// Utilizar main() para fazer chamadas de teste
//---------------------------------------------------------
int main() {
  int N = 9;
  int A = 10;
  int aberto[] = {1, 1, 0, 1, 1, 1, 1, 1, 1};
  int inicio = 9;
  int fim = 2;
  int chave = 5;
  int ijpeso[] = {
    1, 2, 30,
    1, 6, 15,
    2, 6, 5,
    2, 3, 15,
    3, 4, 10,
    3, 5, 30,
    4, 9, 30,
    5, 8, 10,
    7, 8, 5,
    7, 9, 20
  };

  // int N = 9;
  // int A = 10;
  // int aberto[] = {0, 0, 0, 1, 1, 1, 1, 1, 0};
  // int inicio = 7;
  // int fim = 4;
  // int chave = 1;
  // int ijpeso[] = {
  //   1, 2, 30,
  //   1, 3, 20,
  //   2, 6, 20,
  //   2, 7, 30,
  //   3, 4, 20,
  //   3, 7, 80,
  //   4, 9, 80,
  //   5, 8, 10,
  //   6, 7, 10,
  //   7, 9, 80
  // };

  NO* teste = NULL;
  teste = caminho(N, A, ijpeso, aberto, inicio, fim, chave);
  printf("RESPOSTA: ");
  exibirRes(teste);
}
// EOF
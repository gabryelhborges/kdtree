#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define K 2 //dimensoes
#define n 25 //numero de pontos

/*
	ALUNO: Gabryel Henrique Borges
	RA: 102215626
*/

struct kdtree{
	int ponto[K];
	struct kdtree *esq, *dir;
};
typedef struct kdtree KdTree;

KdTree *criaNo(int ponto[K]){
	KdTree *no = (KdTree*)malloc(sizeof(KdTree));
	for(int i= 0; i < K; i++){
		no->ponto[i]= ponto[i];
	}
	no->esq = no->dir = NULL;
	return no;
}



void insereR(KdTree **raiz, int ponto[K], int nivel){
	int d;
	if (*raiz == NULL){
		//*raiz= Cria_no(ponto);
	}
	else {
		d= n%K;
		
		if (ponto[d] < (*raiz)->ponto[d]){
			insereR(&(*raiz)->esq, ponto, nivel+1);
		}
		else{
			insereR(&(*raiz)->dir, ponto, nivel+1);
		}
	}
}

void exibeCoordenadas(int pontos [][K]){
	printf("\n\n\n");
	for(int i= 0; i < n; i++){
		printf("(%d, %d)  ", pontos[i][0], pontos[i][1]);
	}
	printf("\n\n\n");
}


void ordenarPontosPorNivel(int pontos[][K], int inicio, int fim, int nivel){
	//nivel eh o discriminador
	int posmenor, aux;
	fim= fim + 1;//fim deve ser a quantidade de elementos(TL)
	for(int i= inicio; i < fim-1; i++){
		posmenor= i;
		for(int j= i+1; j < fim; j++){
			if(pontos[j][nivel] < pontos[posmenor][nivel]){
				posmenor= j;
			}
		}
		for(int l= 0; l < K; l++){
			aux= pontos[i][l];
			pontos[i][l]= pontos[posmenor][l];
			pontos[posmenor][l]= aux;
		}
	}
}


void constroiKdTreeBalanceada(KdTree **raiz, int pontos[][K], int inicio, int fim, int nivel){
	int d, meio;
	if(inicio <= fim){
		d = nivel % K;//discriminador ou eixo
		ordenarPontosPorNivel(pontos, inicio, fim, d);
		
		meio = (inicio+fim)/2;//mediana
		
		*raiz = criaNo(pontos[meio]);
		
		constroiKdTreeBalanceada(&(*raiz)->esq, pontos, inicio, meio-1, nivel+1);
		constroiKdTreeBalanceada(&(*raiz)->dir, pontos, meio+1, fim, nivel+1);
	}
}

//Para inserir balanceado, teremos um conjunto de pontos e vamos ordenar ele pelo d
//Random de no max 100

//Rand entre 10 e 80  num % 70 + 10

int numeroAleatorio(){
	return rand() % 70 + 10;
}

void gerarCoordenadas(int pontos[n][K]){
	for(int i= 0; i < n; i++)
		for(int j= 0; j < K; j++)
			pontos[i][j]= numeroAleatorio();
}

void exibeTree(KdTree* raiz) {
	static int i = -1;

	if(raiz != NULL){
		i++;
		exibeTree(raiz->dir);
		for(int j= 0; j < 5 * i; j++){
			printf(" ");
		}
		printf("(%d, %d)\n", raiz->ponto[0], raiz->ponto[1]);
		exibeTree(raiz->esq);
		i--;
	}
}

void exemploTrabalho(int pontos[][K]){
	pontos[0][0]= 30; pontos[0][1]= 40;
	pontos[1][0]= 5; pontos[1][1]= 25;
	pontos[2][0]= 10; pontos[2][1]= 12;
	pontos[3][0]= 70; pontos[3][1]= 70;
	pontos[4][0]= 50; pontos[4][1]= 30;
	pontos[5][0]= 35; pontos[5][1]= 45;
	//(30,40), (5,25), (10,12), (70,70), (50,30), (35,45)
}

//Fila
struct fila{
	KdTree *no;
	struct fila *prox;
};
typedef struct fila Fila;

void init(Fila **f){
	*f= NULL;
}

char isEmpty(Fila *f){
	return f == NULL;
}

void enqueue(Fila **f, KdTree *no){
	Fila *novo = (Fila*)malloc(sizeof(Fila));
	novo->no= no;
	novo->prox= NULL;
	if(isEmpty(*f)){
		*f= novo;
	}
	else{
		Fila *aux= *f;
		while(aux->prox != NULL){
			aux= aux->prox;
		}
		aux->prox= novo;
	}
}

void dequeue(Fila **f, KdTree **no){
	Fila *aux;
	if(!isEmpty(*f)){
		aux= *f;
		*no= aux->no;
		*f= (*f)->prox;
		aux->prox= NULL;
		free(aux);
	}
	else{
		*no= NULL;
	}
}

float distanciaEuclidiana(int ponto1[K], int ponto2[K]){
	double dist= 0.0;
	for(int i= 0; i < K; i++){
		dist += pow(ponto1[i] - ponto2[i], 2);
	}
	return sqrt(dist);
}

//Distancia euclidiana: ((x1 - x2)^2 + (y1 - y2)^2)^0.5
/*
Andar na KdTree ate encontrar um ponto que esteja dentro do raio
A partir dai devemos comecar a andar em fila, testando se os filhos
pertencem ao raio, se pertencerem devemos coloca-los na fila
*/

void buscaKdTree(KdTree *raiz, int ponto[K], int raio, Fila **filaExibir){
	double distEuc;
	char encontrou= 0;
	Fila *F;

	init(&F);
	init(&*filaExibir);
	enqueue(&F, raiz);
	while(!isEmpty(F)){
		dequeue(&F, &raiz);
		distEuc = distanciaEuclidiana(raiz->ponto, ponto);
		if(distEuc <= raio){
			encontrou= 1;
			enqueue(&*filaExibir, raiz);
			if(raiz->esq != NULL){
				enqueue(&F, raiz->esq);
			}
			if(raiz->dir != NULL){
				enqueue(&F, raiz->dir);
			}
		}
		else{
			if(!encontrou){
				if(raiz->esq != NULL){
					enqueue(&F, raiz->esq);
				}
				if(raiz->dir != NULL){
					enqueue(&F, raiz->dir);
				}
			}
		}
	}
}

void exibeFila(Fila **F){
	KdTree *no;
	printf("\n\n\nResultado: ");
	if(isEmpty(*F)){
		printf("Nao foram encontrados pontos!\n\n");
	}
	else{
		while(!isEmpty(*F)){
			dequeue(&*F, &no);
			printf("(%d, %d)   ", no->ponto[0], no->ponto[1]);
		}
	}
	printf("\n\n\n");
}

int main(void){
	int pontos[n][K], pontoBusca[K], raio;//n linhas, k colunas
	KdTree *raiz= NULL;
	Fila *pontosEncontrados;
	srand(time(NULL));
	
	gerarCoordenadas(pontos);
	printf("\n\n n: %d  K: %d  \n\nCoordenadas geradas: ", n, K);
	exibeCoordenadas(pontos);
	
	constroiKdTreeBalanceada(&raiz, pontos, 0, n-1, 0);
	printf("KDTREE: \n\n");
	exibeTree(raiz);
	
	pontoBusca[0]= numeroAleatorio();
	pontoBusca[1]= numeroAleatorio();
	raio = rand() % 20 + 10;
	printf("\n\n\nPonto para buscar: (%d, %d)     Raio: %d\n", pontoBusca[0], pontoBusca[1], raio);
	buscaKdTree(raiz, pontoBusca, raio, &pontosEncontrados);
	exibeFila(&pontosEncontrados);
	return 0;
}

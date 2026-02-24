#include <stdio.h>
#include <string.h>

#define MAX_CIDADES 200
#define MAX_CONEXOES 200

typedef struct {
    int destino;
    int tempo;
}Aresta;

Aresta grafo[MAX_CIDADES][MAX_CONEXOES];
int qtdConexoes[MAX_CIDADES];


int indiceCidade(char nome[], char cidades[][21], int n){
    for(int i = 0; i < n; i++){
        if(strcmp(cidades[i], nome) == 0){
            return i;
        }
    }
    return -1;
}

int main(){
    int n;
    char cidades[200][21];

    scanf("%d", &n);

    for(int i = 0; i < n; i++){
        scanf("%d", qtdConexoes[i]);

        for(int j = 0; j < qtdConexoes[i]; j++){
            char nomeDestino[21];
            int tempo;

            scanf("%s %d", nomeDestino, &tempo);

            int indice = indiceCidade(nomeDestino, cidades, n);

            grafo[i][j].destino = indice;
            grafo[i][j].tempo = tempo;
        }
    }
}


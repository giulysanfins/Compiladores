#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
struct fila_element{
  char *lexema;
  char *simbolo;
  char *tipo;
};
typedef struct fila_element fila_element;
*/

struct item_fila{
    stack_element info;
    item_fila *prox;
};

struct Lista{
    item_fila *inicio;
};

void init_fila(Lista *L){
    L->inicio = NULL;
}

item_fila* novo_item(){
    item_fila* novo;
    novo = malloc(sizeof(item_fila));
    if (!novo){
        printf("alocação nova na fila falhou\n");
        exit(1);
    }
    return novo;
}

void inserir_fila(Lista* L,  stack_element a){ //em qual fila fila_element a o que vai inserir fila_element
    item_fila *p,*x,*s=NULL;
    p = novo_item();
    p->info = a;
    x = L->inicio;
    while (x!=NULL){
        s=x;
        x=x->prox;
    }
    if(x==L->inicio){
        L->inicio = p;
    }else{
        s->prox = p;
    }
    p->prox = x;
}

void imprimir_fila(Lista*L){
    item_fila *x;
    x=L->inicio;
    int i = 0;
    while(x!=NULL){
        //printf("[%d]Lexema: %s - Tipo: %s \n",i,x->info.lexema,x->info.tipo);
        //printf("elemento: %s\n",x->info.part_cod);
        x=x->prox;
        i++;
    }
}

void eliminar(Lista *L,int i){
    //stack_element V;
    item_fila *x,*s=NULL;
    x = L->inicio;
    int cont = 1;

    while(x!=NULL && cont<i){
        s=x;
        x=x->prox;
        cont++;
    }
    //V = x->info;
    if(x == L->inicio){
        L->inicio = x->prox;
    }else{
        s->prox = x->prox;
    }
    free(x);
}

void alterar(Lista * L, int i, stack_element change){
    item_fila *x;
    x = L->inicio;
    int cont = 1;

    while(x!=NULL && cont<i){
        x=x->prox;
        cont++;
    }
    //printf("%s - %s\n",change.lexema,change.tipo);
    x->info = change;
}

stack_element ret_elemento_fila(Lista * L, int i){
    item_fila *x;
    x = L->inicio;
    int cont = 1;

    while(x!=NULL && cont<i){
        x=x->prox;
        cont++;
    }
    
    return x->info;
}

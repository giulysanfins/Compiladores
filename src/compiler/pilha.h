#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct no {
    stack_element info;
    no * link;
};

struct Stack {
    no * top;
};
//ver o que significa o & no paramentro de funçao se é endereço (*) ou endereço do endereço(**)
void initStack     (Stack*);
void push          (Stack* , stack_element);
stack_element pop  (Stack*);
stack_element peek (Stack*);
int isEmpty        (Stack);
void error          (int);

void initStack(Stack *p){ 
    p->top= NULL;
}

no* novo_no(){
    no* novo;
    novo = malloc(sizeof(no));
    if (!novo) error(1);
    return novo;
}

void push(Stack *p, stack_element a){
    no * novo = novo_no();
    novo->info = a;
    novo->link = p->top;
    p->top = novo;
}

stack_element  pop (Stack *p){
    if(p->top == NULL) error (2);
    stack_element v = (p->top) ->info;
    no * x = p->top;
    p->top = (p->top)->link;
    free(x);
    return v;
}

stack_element  peek (Stack *p){
    if(p->top == NULL) error(2);
    return  (p->top)->info;
}

int isEmpty(Stack p){
    if (p.top == NULL){
        //printf("pilha vazia\n");
        return 1;
    }
    return 0;
}

void error(int i){
    switch(i){
        case 1:
            printf("OVERFLOW\n"); 
        break;
        case 2:
            printf("UNDERFLOW\n");  
        break;
    }
    exit(1);
}
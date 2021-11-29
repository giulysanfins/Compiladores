#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//A tabela de simbolos cuida dos identificadores, funçoes procedimentos variaveis
struct stack_element{
  char *lexema;     //Nome do identificador (lexema)(é a palavra em si não s...)
  char *escopo;     //Escopo (nível de declaração) - marcação com X
  char *tipo;		//Tipo (padrão do identificador)
  char *memoria;	//Memória (endereço de memória alocado)
};
typedef struct stack_element stack_element;
typedef struct no no;
typedef struct Stack Stack;
#include "pilha.h"

//funçoes referente a tabela de simbolos
void insere_tabela(Stack *tab_simbolo, char*lexema, char*tipo, char *escopo, int memoria);
void imprimirLista(Stack *);
stack_element consultar_tab(Stack*,char*);
void colocar_tipo(Stack*,char*);

void insere_tabela(Stack *tab_simbolo, char*lexema, char*tipo, char *escopo, int memoria){
	//pelo que parece na chamada dessa funcao seja necessario inserir o tipo
	stack_element element_aux;
	element_aux.lexema = lexema;    
	element_aux.escopo = escopo; 
	element_aux.tipo = tipo;       
	element_aux.memoria = memoria; 
	// insere na pilha o elemento criado
	push(tab_simbolo, element_aux);
}

void imprimirLista(Stack *lista){
   Stack p;
   p = *lista; //p tem endereço do no do topo
   printf("\nPilha\n\n");
   //to bugado e nao to conesguindo entender o print rt
   while(p.top != NULL){
    printf("Lexema %s\n",p.top->info.lexema);
	printf("Tipo %s\n",p.top->info.tipo);
    p.top = p.top->link;
   }
   printf("\n\n");
}

//Essa funcao deveria retornar os dados quando encontrado, mas e caso nao seja encontrado
stack_element consultar_tab(Stack *tab_simbolo,char *lexema){
	Stack tab_aux;
	tab_aux = *tab_simbolo;
	stack_element element_aux;
	
	while(tab_aux.top != NULL && tab_aux.top->info.lexema != lexema){
		tab_aux.top = tab_aux.top->link;
	}

	if(tab_aux.top->info.lexema == lexema){
		return tab_aux.top->info;
	}
	
	//Erro caso nao tenha encontrado
  	printf("Nao encontrou");
	exit(1);

}

void colocar_tipo(Stack *tab_simbolo,char*tipo){
	/*tirar da pilha original trocar o tipo salvar em uma pilha auxiliar
	quando nao for mais elemento do tipo variavel ir tirando da pilha auxiliar
	e recolocar na pilha original */
	Stack tab_aux;
	initStack(&tab_aux);
	stack_element element_aux;

	//tire o elemento e troca o seu campo tipo
	element_aux = pop(tab_simbolo);
  	//enquanto nao for o final da pilha e tipo diferente de variavel
	while(!isEmpty(*tab_simbolo) && element_aux.tipo == "variavel"){
		element_aux.tipo = tipo;
		//atribuição de endereço de memoria
		element_aux.memoria = malloc(sizeof(int));
		
		push(&tab_aux,element_aux);
		element_aux = pop(tab_simbolo);
	}
	push(tab_simbolo,element_aux);

	while(!isEmpty(tab_aux)){
		element_aux = pop(&tab_aux);
		push(tab_simbolo,element_aux);
	}
	
}
/*
void main(){
    Stack tab_simbolo;			//pilha - tabela de simbolos
	initStack(&tab_simbolo);
  
	insere_tabela(&tab_simbolo,"prog1","nomedeprograma","","");
	insere_tabela(&tab_simbolo,"a","variavel","","");
	insere_tabela(&tab_simbolo,"b","variavel","",""); 
	
	imprimirLista(&tab_simbolo); // AQUI ESTA DANDO PROBLEMA 
	colocar_tipo(&tab_simbolo,"inteiro");
	imprimirLista(&tab_simbolo);

  	//EXEMPLO DE CONSULTAR
	stack_element b;
	b = consultar_tab(&tab_simbolo,"a");
	printf("lex (%s)\nescopo (%s)\ntipo (%s)\nmemoria (%p)\n",b.lexema, b.escopo, b.tipo, b.memoria);
	imprimirLista(&tab_simbolo);
}
*/

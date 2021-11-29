//---------------------------------------------------------------------------------
//									DECLARACOES
//---------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//---------------------------------------------------------------------------------
//									TABELA DE SIBOLOS
//---------------------------------------------------------------------------------

struct stack_element{
  char *lexema;     //Nome do identificador (lexema)(é a palavra em si não s...)
  char *escopo;     //Escopo (nível de declaração) - marcação com X
  char *tipo;		    //Tipo (padrão do identificador)
  int  *memoria;  	//Memória (endereço de memória alocado) no caso é o rotulo
};
typedef struct stack_element stack_element;
typedef struct no no;
typedef struct Stack Stack;
#include "pilha.h"

void insere_tabela(Stack *, char*, char*, char *, int *);
void imprimirTabela(Stack *);
stack_element consultar_tab(Stack*,char*);
void colocar_tipo(Stack*,char*);
int pesquisa_duplicvar_tabela(Stack *, char *);
int pesquisa_duplicfunc_tabela(Stack *, char *);
int pesquisa_existencia(Stack*,char*);
void desempilha_nivel(Stack *);                 //CHECK DESALLOC
int pesquisa_tabela(Stack *, char *, int *);

//---------------------------------------------------------------------------------
//									LEXICAL
//---------------------------------------------------------------------------------

struct tokens{
  char *lexema;
  char *simbolo;
//   struct tokens *prox;
};
typedef struct tokens tokens;  

tokens lexico(FILE *);
void pega_token(FILE *, char, tokens *);
int eh_digito(char);
int eh_letra(char);
void trata_digito(FILE *, char, tokens *);
void trata_identPalav(FILE *, char, tokens *);
void trata_atribuicao(FILE *, char, tokens *);
void trata_opArit(FILE *, char, tokens *);
void trata_opRela(FILE *, char, tokens *);
void trata_pontuacao(FILE *, char, tokens *);
void preencher_token(tokens *, char *, char *);

//---------------------------------------------------------------------------------
//									SEMANTICO
//---------------------------------------------------------------------------------

typedef struct item_fila item_fila;
typedef struct Lista Lista;
#include "fila.h"

void infix_posfix(Lista *);
int precedencia(char*);
void analise_semantica(Stack*,Lista *,char *);
int tipo_operador(char *);
void gera_cod_expressao(Stack *,Lista *);

//---------------------------------------------------------------------------------
//									SINTATICO
//---------------------------------------------------------------------------------

void analisa_bloco(FILE *, tokens *, Stack *);

void analisa_et_variaveis(FILE *, tokens *, Stack *); //CHECK  
void analisa_variaveis(FILE *, tokens *, Stack *);    //CHECK
void analisa_tipo(FILE *, tokens *, Stack *);         //CHECK

void analisa_subrotinas(FILE *, tokens *, Stack *);                //CHECK
void analisa_declaracao_procedimento(FILE *, tokens *, Stack *);   //CHECK
void analisa_declaracao_funcao(FILE *, tokens *, Stack *);         

void analisa_comandos(FILE *, tokens *, Stack *);                       //
void analisa_comando_simples(FILE *, tokens *, Stack *);                //
void analisa_atrib_chprocedimento(FILE *, tokens *, Stack *, Lista *);  //CHECK
void analisa_leia(FILE *, tokens *, Stack *);                           //CHECK
void analisa_escreva(FILE *, tokens *, Stack *);                        //CHECK
void analisa_enquanto(FILE *, tokens *, Stack *, Lista *);              //CHECK
void analisa_se(FILE *, tokens *, Stack *, Lista *);                    //CHECK (JMP e JMPF)

void analisa_expressao(FILE *, tokens *, Stack *, Lista *);             //ON GOING
void analisa_expressao_simples(FILE *, tokens *, Stack *, Lista *);
void analisa_termo(FILE *, tokens *, Stack *, Lista *);
void analisa_fator(FILE *, tokens *, Stack *, Lista *);

void analisa_atribuicao(FILE *, tokens *, Stack *, Lista *);            //CHECK
void chamada_procedimento(FILE *, stack_element, Stack *);              //CHECK
void analisa_chamada_funcao(FILE *, tokens *, Stack *);                 //CHECK


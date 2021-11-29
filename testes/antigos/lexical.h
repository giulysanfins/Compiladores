#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//---------------------------------------------------------------------------------
//									DECLARACOES
//---------------------------------------------------------------------------------

//----------------------------------------------
//	Estrutura do tipo token
//----------------------------------------------

struct tokens{
   char *lexema;
   char *simbolo;
//   struct tokens *prox;
};
typedef struct tokens tokens;  

//----------------------------------------------
//	Funcoes criadas
//----------------------------------------------

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
//void inserirFim(tokens **lista, char lexema[], char simbolo[]);
//void imprimirLista(tokens *lista);

//---------------------------------------------------------------------------------
//									MAIN
//---------------------------------------------------------------------------------

tokens lexico(FILE *p){
//---------------------- 
// DECLARACAO VARIAVEIS
//----------------------  
 
//   FILE *p;
//   char str[80]; 
   char ch;
//   tokens *token;
//   token = NULL;
   tokens token;
   
//---------------------- 
// ABERTURA DE ARQUIVO
//----------------------
   
   // Le um nome do arquivo a ser aberto: 
//   printf("\n\n Entre com um nome para o arquivo:\n");
//   gets(str);

   // Abertura do arquivo em mode de leitura (read)
   // Verifica erro na abertura do arquivo 
//   if ((p = fopen(str, "r")) == NULL)
//   {
//      printf("Erro! Impossivel abrir o arquivo!\n");
//      exit(1);
//   }

//---------------------- 
// ANALISE LEXICAL
//----------------------

   ch = fgetc(p);
//   while (ch != EOF)
//   {
      //Tratamento de comentario
      if (ch == '{')
      {
         // Fazendo isso pra jogar fora os comentarios
         while ((ch != '}') && ch != EOF)
         {
            ch = fgetc(p);
         }
         ch = fgetc(p);
      }

      // Fazendo isso para consumir os espaços
      if((ch == ' ') || (ch == '\n') || (ch == '\t') || (ch=='\r')){
         while ((ch == ' ') || (ch == '\n') || (ch == '\t') || (ch=='\r') && ch != EOF)
         {
            ch = fgetc(p);
         }
         ungetc(ch,p);
      }
      else
      {
         if (ch != EOF)
         {
			pega_token(p, ch, &token);
         }
      }
      
//      ch = fgetc(p);
//   }
//   imprimirLista(token);
//   fclose(p);

//---------------------- 
// RETURN
//----------------------

	return token;
}


//----------------------------------------------
//	Funcao de identificar e pegar o token
//----------------------------------------------
void pega_token(FILE *p, char ch, tokens *token)
{
   if (eh_digito(ch))
   {
      trata_digito(p, ch, token);
   }
   else if (eh_letra(ch))
   {
      trata_identPalav(p, ch, token);
   }
   else if (ch == ':')
   {
      trata_atribuicao(p, ch, token); 
   }
   else if (ch == '+' || ch == '-' || ch == '*')
   {
      trata_opArit(p, ch, token);
   }
   else if (ch == '<' || ch == '>' || ch == '=' || ch == '!')
   {
      trata_opRela(p, ch, token);
   }
   else if (ch == ';' || ch == ',' || ch == '(' || ch == ')' || ch == '.')
   {
      trata_pontuacao(p, ch, token);
   }
   else
   {
      //ERRO
      char *oto = (char *)malloc(sizeof(char)+1);
      oto[0] = ch;
      strcat(oto, "\0");
	  preencher_token(token, oto, "serro");
//      inserirFim(token, oto, "serro");
      free(oto);
   }
   return;
}

//----------------------------------------------
//	Funcao verificar se o char eh digito
//----------------------------------------------

int eh_digito(char ch)
{
   //considerando que a leitura do arquivo eh sempre um char, verifico se o char lido tem o aschii entre 48 e 57 se eh [0-9]
   if (ch >= 48 && ch <= 57)
      return 1; //qualquer coisa diferente de 0 é true
   else
      return 0; //0 ehh considera falso em c
}

//----------------------------------------------
//	Funcao verificar se o char eh letra
//----------------------------------------------

int eh_letra(char ch)
{
   //para diminuir um if podemos deixer com que qualquer caracter se torne maiusculo e o if verifica so do 65-90
   //ch = toupper(ch); mas precisariamos adicionar uma biblioteca ctype.h
	if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122))
		return 1;
	else
		return 0;
}

//----------------------------------------------
//	Funcao de tratamento caso digito
//----------------------------------------------

void trata_digito(FILE *p, char ch, tokens *token)
{
   char *num = malloc(sizeof(char));
   *num = ch;
   ch = fgetc(p);

   while(eh_digito(ch)){
      size_t tam = strlen(num);
      num = realloc(num,(tam+1+1));
      num[tam] = ch;
      num[tam + 1] = '\0';
      ch = fgetc(p);
   }
   
   preencher_token(token, num, "snumero");
//   inserirFim(token, num, "snumero");
   ungetc(ch,p);
   free(num);
}

//----------------------------------------------
//	Funcao de tratamento caso palavra 
//----------------------------------------------

void trata_identPalav(FILE *p, char ch,tokens *token)
{
   char *id = malloc(sizeof(char));
   *id = ch;
   ch = fgetc(p);

   while(eh_letra(ch) || eh_digito(ch) || ch == '_'){
      size_t tam = strlen(id);
      id = realloc(id,(tam+1+1));
      id[tam] = ch;
      id[tam + 1] = '\0';
      ch = fgetc(p);
   }

   if(strcmp(id,"programa") == 0) 
   {
		preencher_token(token, id, "sprograma");
//      inserirFim(token, id, "sprograma");
   }
   else if(strcmp(id,"se") == 0)
   {
		preencher_token(token, id, "sse");
//      inserirFim(token, id, "sse");
   }
   else if(strcmp(id,"entao") == 0)
   {
	   preencher_token(token, id, "sentao");
//      inserirFim(token, id, "sentao");
   }
   else if(strcmp(id,"senao") == 0)
   {
	   preencher_token(token, id, "ssenao");
//      inserirFim(token, id, "ssenao");
   }
   else if(strcmp(id,"enquanto") == 0)
   {
	   preencher_token(token, id, "senquanto");	   
//      inserirFim(token, id, "senquanto");
   }
   else if(strcmp(id,"faca") == 0)
   {
	preencher_token(token, id, "sfaca");   
//      inserirFim(token, id, "sfaca");
   }
   else if(strcmp(id,"inicio") == 0)
   {
	   preencher_token(token, id, "sinicio");
//      inserirFim(token, id, "sinicio");
   }
   else if(strcmp(id,"fim") == 0)
   {
	   preencher_token(token, id, "sfim");
//      inserirFim(token, id, "sfim");
   }
   else if(strcmp(id,"escreva") == 0)
   {
	   preencher_token(token, id, "sescreva");
//      inserirFim(token, id, "sescreva");
   }
   else if(strcmp(id,"leia") == 0)
   {
	   preencher_token(token, id, "sleia");
//      inserirFim(token, id, "sleia");
   }
   else if(strcmp(id,"var") == 0)
   {
	   preencher_token(token, id, "svar");
//      inserirFim(token, id, "svar"); 
   }
   else if(strcmp(id,"inteiro") == 0)
   {
	   preencher_token(token, id, "sinteiro");
//      inserirFim(token, id, "sinteiro"); 
   }
   else if(strcmp(id,"booleano") == 0)
   {
	   preencher_token(token, id, "sbooleano");
//      inserirFim(token, id, "sbooleano");
   }
   else if(strcmp(id,"verdadeiro") == 0)
   {
	   preencher_token(token, id, "sverdadeiro");
//      inserirFim(token, id, "sverdadeiro");
   }
   else if(strcmp(id,"falso") == 0)
   {
	   preencher_token(token, id, "sfalso");
//      inserirFim(token, id, "sfalso");
   }
   else if(strcmp(id,"procedimento") == 0)
   {
	   preencher_token(token, id, "sprocedimento");
//      inserirFim(token, id, "sprocedimento");
   }
   else if(strcmp(id,"funcao") == 0)
   {
	   preencher_token(token, id, "sfuncao");
//      inserirFim(token, id, "sfuncao");
   }
   else if(strcmp(id,"div") == 0)
   {
	   preencher_token(token, id, "sdiv");
//      inserirFim(token, id, "sdiv");
   }
   else if(strcmp(id,"e") == 0)
   {
	   preencher_token(token, id, "se");
//      inserirFim(token, id, "se");
   }
   else if(strcmp(id,"ou") == 0)
   {
	   preencher_token(token, id, "sou");
//      inserirFim(token, id, "sou");
   }
   else if(strcmp(id,"nao") == 0)
   {
	   preencher_token(token, id, "snao");
//      inserirFim(token, id, "snao");
   }
   else
   {
	   preencher_token(token, id, "sidentificador");
//      inserirFim(token, id, "sidentificador");
   }
   ungetc(ch,p);
   free(id);
}

//----------------------------------------------
//	Funcao de tratamento caso atribuicao 
//----------------------------------------------

void trata_atribuicao(FILE *p, char ch,tokens *token)
{  
   ch = fgetc(p);
   if(ch == '=')
   {
	   preencher_token(token,":=","sdoispontos");
//      inserirFim(token, ":=", "satribuicao");
   }
   else
   {
	   preencher_token(token,":","sdoispontos");
//      inserirFim(token, ":", "sdoispontos");
      ungetc(ch,p); //joga o caracter ch de volta no arquivo
   }
}

//----------------------------------------------
//	Funcao de tratamento de operador aritmetico 
//----------------------------------------------

//Trata operador aritmetico + - * 
void trata_opArit(FILE *p, char ch,tokens *token)
{
   if(ch == '+'){
	   preencher_token(token, "+", "smais");
//      inserirFim(token, "+", "smais");
   }else if(ch == '-'){
	   preencher_token(token, "-", "smenos");
//      inserirFim(token, "-", "smenos");
   }else if(ch == '*'){
	   preencher_token(token, "*", "smult");
//      inserirFim(token, "*", "smult");
   }
}

//----------------------------------------------
//	Funcao de tratamento de operador relacional 
//----------------------------------------------

//Trata operador relacional < > =
void trata_opRela(FILE *p, char ch,tokens *token)
{  
   if(ch == '>'){
      ch = fgetc(p);
      if(ch == '='){
		  preencher_token(token, ">=", "smaiorig");
//         inserirFim(token, ">=", "smaiorig");  
      }else{
		  preencher_token(token, ">", "smaior");
//         inserirFim(token, ">", "smaior");
         ungetc(ch,p);
      }
   }else if(ch == '<'){
      ch = fgetc(p);
      if(ch == '='){
		  preencher_token(token, "<=", "smenorig");
//         inserirFim(token, "<=", "smenorig");  
      }else{
		  preencher_token(token, "<", "smenor");
//         inserirFim(token, "<", "smenor");
         ungetc(ch,p);
      }
   }else if(ch == '!'){
      char aux = ch;
      ch = fgetc(p);
      if(ch == '='){
		  preencher_token(token, "!=", "sdif");
//         inserirFim(token, "!=", "sdif");  
      }else{
         ungetc(ch,p);
         //ERRO
         char *oto = (char *)malloc(sizeof(char)+1);
         oto[0] = aux;
         strcat(oto, "\0");
		 preencher_token(token, oto, "serro");
//         inserirFim(token, oto, "serro");
         free(oto);
      }
   }else if(ch == '='){
	   preencher_token(token, "=", "sig");
//      inserirFim(token, "=", "sig");
   }  
}

void trata_pontuacao(FILE *p, char ch,tokens *token)
{
   if(ch == '.'){
	   preencher_token(token, ".", "sponto");
//      inserirFim(token, ".", "sponto");
   }else if(ch == ';'){
	   preencher_token(token, ";", "sponto_virgula");
//      inserirFim(token, ";", "sponto_virgula");
   }else if(ch == ','){
	   preencher_token(token, ",", "svirgula");
//      inserirFim(token, ",", "svirgula");
   }else if(ch == '('){
	   preencher_token(token, "(", "sabre_parenteses");
//      inserirFim(token, "(", "sabre_parenteses");
   }else if(ch == ')'){
	   preencher_token(token, ")", "sfecha_parenteses");
//      inserirFim(token, ")", "sfecha_parenteses");
   }
}

void preencher_token(tokens *token, char lexema[], char simbolo[]){
	int lex_tam = strlen(lexema);
	int sim_tam = strlen(simbolo);
	token->lexema = (char *)malloc(sizeof(lex_tam));
   sprintf(token->lexema,"%s",lexema);
	token->simbolo = simbolo;
}
/*
//o retorno deve ser um endereço e o parametro deve ser um endereço
// inserir no final da lista
void inserirFim(tokens **lista, char lexema[], char simbolo[]){//primeiro no da lista, lexema, simbolo
   //lista é o primeiro nó da lista ligada
   //chegou o conteudo correto nos argumentos
   tokens *p;
   p = *lista;
   //pegar o tamanho das entradas para poder formar o tamanho a ser separado para o novo nó
   int lex_tam = strlen(lexema);
   int sim_tam = strlen(simbolo);
   //os tamanhos estao corretos
   
   tokens *novo = (tokens *)malloc((sizeof(tokens *)) + lex_tam + sim_tam); // cria um novo nó
   novo->lexema = (char *)malloc(sizeof(lex_tam));
   sprintf(novo->lexema,"%s",lexema);
   //novo->lexema = lexema;
   novo->simbolo = simbolo;
   novo->prox = NULL;
   //Colando na lista
   //caso lista vazia

   if(p == NULL){ // lista vazia
      *lista = novo;  
      //o endereço do primeiro nao esta sendo substituido
   } 
   else // lista não vazia
   { 
   //encontrar o fim
      while(p->prox != NULL){
         p = p->prox;
      }
      p->prox = novo;
   }
    
}
*/
// imprimir a lista
/*
void imprimirLista(tokens *lista){
   tokens *p;
   p = lista;
   printf("\nTabela de Tokens\n\n");
   while(p != NULL){
      printf("%s - %s\n", p->lexema, p->simbolo);
      p = p->prox;
   }
   printf("\n\n");
}
*/
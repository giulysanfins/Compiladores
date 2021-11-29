#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "declaracoes.h"

int linha_atual = 1;
int rotulo,cont,end_disponivel;

//--Arquivo com o Codigo que sera gerado 
FILE *codigo;
//--Arquivo com o outuput se deu certo ou erro
FILE *output_arquivo;

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------



//---------------------------------------------------------------------------------
//									TABELA DE SIMBOLOS
//---------------------------------------------------------------------------------

//----------------------------------------------
//	Funcao de inserir elemento na pilha
//----------------------------------------------

void insere_tabela(Stack *tab_simbolo, char *lexema, char *tipo, char *escopo, int *memoria)
{
    //pelo que parece na chamada dessa funcao seja necessario inserir o tipo
    stack_element element_aux;
    element_aux.lexema = lexema;
    element_aux.escopo = escopo;
    element_aux.tipo = tipo;
    if(memoria != NULL){
        element_aux.memoria = *memoria;
    }
    // insere na pilha o elemento criado
    push(tab_simbolo, element_aux);
    
}

//----------------------------------------------
//	Funcao auxliar para imprimir a pilha
//----------------------------------------------

void imprimirTabela(Stack *lista)
{
    Stack p;
    p = *lista; //p tem endereço do no do topo
    
    while (p.top != NULL)
    {
        printf("--------------------\n");
        printf("Lexema: %s\n", p.top->info.lexema);
        printf("Tipo:   %s\n", p.top->info.tipo);
        printf("Escopo: %s\n", p.top->info.escopo);
        printf("Memoria:   %d\n", p.top->info.memoria);
        printf("--------------------\n");
        p.top = p.top->link;
    }
    printf("\n\n");
}

//----------------------------------------------
//	Funcao de identificar e pegar o token
//----------------------------------------------

//Essa funcao deveria retornar os dados quando encontrado, mas e caso nao seja encontrado
stack_element consultar_tab(Stack *tab_simbolo, char *lexema)
{

    Stack tab_aux;
    tab_aux = *tab_simbolo;
    stack_element element_aux;

    while (tab_aux.top != NULL && !strcmp(tab_aux.top->info.lexema,lexema) == 0)
    {
        tab_aux.top = tab_aux.top->link;
    }

    if(tab_aux.top != NULL){
        if (strcmp(tab_aux.top->info.lexema,lexema) == 0)
        {
            return tab_aux.top->info;
        }
    }
    
    //Erro caso nao tenha encontrado
    printf("Identificador procurado não consta na tabela de simbolo\n");
    fprintf(output_arquivo,"Linha:%d\nIdentificador procurado não consta na tabela de simbolo.\n",linha_atual);
    exit(1);
}

//----------------------------------------------
//	Funcao para colocar tipo nas variaveis
//----------------------------------------------

void colocar_tipo(Stack *tab_simbolo, char *tipo)
{
    /*tirar da pilha original trocar o tipo salvar em uma pilha auxiliar
	quando nao for mais elemento do tipo variavel ir tirando da pilha auxiliar
	e recolocar na pilha original */
    Stack tab_aux;
    initStack(&tab_aux);
    stack_element element_aux;

    //tire o elemento e troca o seu campo tipo
    element_aux = pop(tab_simbolo);
    //enquanto nao for o final da pilha e tipo diferente de variavel
    while (!isEmpty(*tab_simbolo) && element_aux.tipo == "variavel")
    {
        element_aux.tipo = tipo;

        push(&tab_aux, element_aux);
        element_aux = pop(tab_simbolo);
    }
    push(tab_simbolo, element_aux);

    while (!isEmpty(tab_aux))
    {
        element_aux = pop(&tab_aux);
        push(tab_simbolo, element_aux);
    }
}

//----------------------------------------------
//	Funcao Analise de variavel duplicada na Tabela
//----------------------------------------------

int pesquisa_duplicvar_tabela(Stack *tab_simbolo, char *lexema){
    printf("\n-- Entrou em duplicvar --\n");

    /*
    a) se ele for uma variável verificar se já não existe outra variável visível 
    (Por visível considera-se como aquele que pode ser encontrado na Tabela de Símbolos atual.)
    de mesmo nome no mesmo nível de declaração e verificar se já não existe outro identificador de
    mesmo nome (que não seja uma variável) em qualquer nível inferior ou igual ao da variável agora analisada.
    */

    Stack tab_aux;
    tab_aux = *tab_simbolo;
    //stack_element element_aux;
    //int flag_aux = 0;
    
    //Verificar no mesmo nivel se existe alguma VARIAVEL com o mesmo nome.
    //a pilha auxiliar tem os mesmos conteudos da tabela original
    //printf("\nTabela original \n");
    //imprimirTabela(tab_simbolo);
    //printf("Tabela auxiliar \n");
    //imprimirTabela(&tab_aux);

    while(tab_aux.top != NULL && tab_aux.top->info.escopo != "x"){
        //printf("Tipo (tabela): %s\n",tab_aux.top->info.tipo);
        //printf("Lexema (tabela): %s\nLexema (analisado): %s\n",tab_aux.top->info.lexema, lexema);
        //printf("Nivel: %s\n--------------------\n",tab_aux.top->info.escopo);
        //acho que um esta como z\0 e outro só z 
        if((strcmp(tab_aux.top->info.tipo,"variavel") == 0 || strcmp(tab_aux.top->info.tipo,"inteiro") == 0 || strcmp(tab_aux.top->info.tipo,"booleano") == 0) && strcmp(tab_aux.top->info.lexema,lexema) == 0){
            //entao se o elemento analisado da tabela de simbolo for do tipo variavel e de mesmo nome, dentro do mesmo nivel
            //significa que eh uma duplicata
            //printf("entrou variavel de mesmo lexema\n");
            return 1; //achou duplicidade
        }
        tab_aux.top = tab_aux.top->link;
    }

    //Verificar no nivel restante se existe algo que nao seja uma variavel com o mesmo lexema
    while(tab_aux.top != NULL){
        //printf("Tipo (tabela): %s\n",tab_aux.top->info.tipo);
        //printf("Lexema (tabela): %s\nLexema (analisado): %s\n",tab_aux.top->info.lexema, lexema);
        //printf("Nivel: %s\n",tab_aux.top->info.escopo);
        
        if(!(strcmp(tab_aux.top->info.tipo,"variavel") == 0 || strcmp(tab_aux.top->info.tipo,"inteiro") == 0 || strcmp(tab_aux.top->info.tipo,"booleano") == 0)){
            //printf("nao é uma variavel\n--------------------\n");
            if(strcmp(tab_aux.top->info.lexema,lexema) == 0){
                //entao se o elemento analisado da tabela de simbolo nao for do tipo variavel e de mesmo nome
                //significa que o nome é uma duplicata
                //printf("Não é uma variavel e tem o mesmo lexema\n");
                return 1; //achou duplicidade
            }
        }
        tab_aux.top = tab_aux.top->link;
    }

    //Caso nao tenha encontrado duplicidade
    if (tab_aux.top == NULL){
        return 0;   //nao tem duplicata
    }
}

//----------------------------------------------
//	Funcao Analise de func/proc duplicada na Tabela
//----------------------------------------------

int pesquisa_duplicfunc_tabela(Stack *tab_simbolo, char *lexema){
    printf("\n-- Entrou em duplicfunc --\n");

    /*
    b) Se for o nome de um procedimento ou função verificar se já não existe um outro
    identificador visível de qualquer tipo em nível igual ao inferior ao agora analisado.
    */

    Stack tab_aux;
    tab_aux = *tab_simbolo;
    //stack_element element_aux;
    //int flag_aux = 0;

    //printf("Tabela original \n");
    //imprimirTabela(tab_simbolo);
    //printf("Tabela auxiliar \n");
    //imprimirTabela(&tab_aux);

    while(tab_aux.top != NULL){
        //printf("Tipo (tabela): %s\n",tab_aux.top->info.tipo);
        //printf("Lexema (tabela): %s\nLexema (analisado): %s\n",tab_aux.top->info.lexema, lexema);
        //printf("Nivel: %s\n--------------------\n",tab_aux.top->info.escopo);
        if(strcmp(tab_aux.top->info.lexema,lexema) == 0){
            //printf("encontrou mesmo lexema\n");
            return 1; //achou duplicidade
        }
        tab_aux.top = tab_aux.top->link;  
    }

    //Caso nao tenha encontrado duplicidade
    if (tab_aux.top == NULL){
        return 0;   //nao tem duplicidade
    }
}

//----------------------------------------------
//	Funcao Analise existencia de identificador
//----------------------------------------------

int pesquisa_existencia(Stack *tab_simbolo,char *lexema){
    printf("\n-- Entrou em pesquisa existencia --\n");
    /*
    Sempre que for detectado um identificador, verificar se ele foi declarado (está visível na tabela de símbolos) 
    e é compatível com o uso (exemplo: variável usada que existe como nome de programa ou de procedimento na tabela 
    de símbolos deve dar erro).
    */
    Stack tab_aux;
    tab_aux = *tab_simbolo;

    //printf("Tabela original \n");
    //imprimirTabela(tab_simbolo);

    while(tab_aux.top != NULL){
        //printf("Tipo (tabela): %s\n",tab_aux.top->info.tipo);
        //printf("Lexema (tabela): %s\nLexema (analisado): %s\n",tab_aux.top->info.lexema,lexema);

        if(strcmp(tab_aux.top->info.lexema,lexema) == 0){
            //printf("encontrou mesmo lexema\n");
            //se for nome do programa deve dar erro
            if(strcmp(tab_aux.top->info.tipo,"nomedeprograma")==0){
                //printf("nome de programa nao deve ser chamado para uso");
                return 0; //achou mas nao pode usar
            }
            return 1; //achou e pode usar
        }

        tab_aux.top = tab_aux.top->link;
    }

    //Caso nao tenha encontrado a variavel
    if (tab_aux.top == NULL){
        return 0;   //nao achou
    }

}

//----------------------------------------------
//	Funcao Desempilha
//----------------------------------------------

void desempilha_nivel(Stack *tab_simbolo){
    printf("\n-- Entrou em desempilha --\n");
    while(tab_simbolo->top != NULL && tab_simbolo->top->info.escopo != "x"){
       tab_simbolo->top = tab_simbolo->top->link;
    }
    //tirar a marca x do token
    //talvez testar esse tirar marca
    if(tab_simbolo->top->info.escopo == "x"){
        tab_simbolo->top->info.escopo = "";
    }

    //PARTE DO DALLOC
    //printf("%d %d",end_disponivel, cont);
    if(cont != 0){
        end_disponivel = end_disponivel-cont;
        fprintf(codigo," ,DALLOC,%d,%d\n",end_disponivel,cont);
    }
    
}

//----------------------------------------------
//	Funcao Pesquisa tabela
//----------------------------------------------

int pesquisa_tabela(Stack *tab_simbolo, char *lexema, int *ind){
    printf("\n-- Entrou em pesquisa tabela --\n");
    Stack tab_aux;
    tab_aux = *tab_simbolo;
    while(tab_aux.top != NULL){ //&& tab_aux.top->info.escopo != "x"
        //se existir o lexema dentro do escopo
        if(strcmp(tab_aux.top->info.lexema,lexema) == 0){
            //se for funcao
            if(strcmp(tab_aux.top->info.tipo,"funcao inteiro") == 0 || strcmp(tab_aux.top->info.tipo,"funcao booleano") == 0){
                *ind=1;
            }else{
            //se for variavel
                *ind=2;
            }
            return 1;
        }
        tab_aux.top = tab_aux.top->link;
    }
    //se nao existir
    return 0;
}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------



//---------------------------------------------------------------------------------
//									LEXICAL
//---------------------------------------------------------------------------------

tokens lexico(FILE *p)
{
    char ch;
    tokens token;
    ch = fgetc(p);

    while(ch == '{' ||(ch == ' ') || (ch == '\n') || (ch == '\t') || (ch == '\r') ){
    
    //Tratamento de comentario
    if (ch == '{')
    {
        while ((ch != '}') && ch != EOF)
        {
            ch = fgetc(p);
        }
        ch = fgetc(p); 
    }

    if ((ch == ' ') || (ch == '\n') || (ch == '\t') || (ch == '\r'))
    {
        //Tratamento de espaços
        while ((ch == ' ') || (ch == '\n') || (ch == '\t') || (ch == '\r') && ch != EOF)
        {
            if (ch == '\n')
            {
                linha_atual++;
            }
            ch = fgetc(p);
        }
    }

    }

    if (ch != EOF)
    {
        // Apos consumir todos comentarios e espaços
        pega_token(p, ch, &token);
    }
    else
    {
        //Caso seja necessario verificar fim de arquivo noutra função
        preencher_token(&token, "eof", "seof");
    }

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
        char *oto = (char *)malloc(sizeof(char) + 1);
        oto[0] = ch;
        strcat(oto, "\0");
        preencher_token(token, oto, "serro");
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

    while (eh_digito(ch))
    {
        size_t tam = strlen(num);
        num = realloc(num, (tam + 1 + 1));
        num[tam] = ch;
        num[tam + 1] = '\0';
        ch = fgetc(p);
    }

    preencher_token(token, num, "snumero");
    ungetc(ch, p);
    free(num);
}

//----------------------------------------------
//	Funcao de tratamento caso palavra
//----------------------------------------------

void trata_identPalav(FILE *p, char ch, tokens *token)
{
    char *id = malloc(sizeof(char));
    *id = ch;
    ch = fgetc(p);

    while (eh_letra(ch) || eh_digito(ch) || ch == '_')
    {
        size_t tam = strlen(id);
        id = realloc(id, (tam + 1 + 1));
        id[tam] = ch;
        id[tam + 1] = '\0';
        ch = fgetc(p);
    }

    if (strcmp(id, "programa") == 0)
    {
        preencher_token(token, id, "sprograma");
    }
    else if (strcmp(id, "se") == 0)
    {
        preencher_token(token, id, "sse");
    }
    else if (strcmp(id, "entao") == 0)
    {
        preencher_token(token, id, "sentao");
    }
    else if (strcmp(id, "senao") == 0)
    {
        preencher_token(token, id, "ssenao");
    }
    else if (strcmp(id, "enquanto") == 0)
    {
        preencher_token(token, id, "senquanto");
    }
    else if (strcmp(id, "faca") == 0)
    {
        preencher_token(token, id, "sfaca");
    }
    else if (strcmp(id, "inicio") == 0)
    {
        preencher_token(token, id, "sinicio");
    }
    else if (strcmp(id, "fim") == 0)
    {
        preencher_token(token, id, "sfim");
    }
    else if (strcmp(id, "escreva") == 0)
    {
        preencher_token(token, id, "sescreva");
    }
    else if (strcmp(id, "leia") == 0)
    {
        preencher_token(token, id, "sleia");
    }
    else if (strcmp(id, "var") == 0)
    {
        preencher_token(token, id, "svar");
    }
    else if (strcmp(id, "inteiro") == 0)
    {
        preencher_token(token, id, "sinteiro");
    }
    else if (strcmp(id, "booleano") == 0)
    {
        preencher_token(token, id, "sbooleano");
    }
    else if (strcmp(id, "verdadeiro") == 0)
    {
        preencher_token(token, id, "sverdadeiro");
    }
    else if (strcmp(id, "falso") == 0)
    {
        preencher_token(token, id, "sfalso");
    }
    else if (strcmp(id, "procedimento") == 0)
    {
        preencher_token(token, id, "sprocedimento");
    }
    else if (strcmp(id, "funcao") == 0)
    {
        preencher_token(token, id, "sfuncao");
    }
    else if (strcmp(id, "div") == 0)
    {
        preencher_token(token, id, "sdiv");
    }
    else if (strcmp(id, "e") == 0)
    {
        preencher_token(token, id, "se");
    }
    else if (strcmp(id, "ou") == 0)
    {
        preencher_token(token, id, "sou");
    }
    else if (strcmp(id, "nao") == 0)
    {
        preencher_token(token, id, "snao");
    }
    else
    {
        preencher_token(token, id, "sidentificador");
    }
    ungetc(ch, p);
    free(id);
}

//----------------------------------------------
//	Funcao de tratamento caso atribuicao
//----------------------------------------------

void trata_atribuicao(FILE *p, char ch, tokens *token)
{
    ch = fgetc(p);
    if (ch == '=')
    {
        preencher_token(token, ":=", "satribuicao");
    }
    else
    {
        preencher_token(token, ":", "sdoispontos");
        ungetc(ch, p); //joga o caracter ch de volta no arquivo
    }
}

//----------------------------------------------
//	Funcao de tratamento de operador aritmetico
//----------------------------------------------

//Trata operador aritmetico + - *
void trata_opArit(FILE *p, char ch, tokens *token)
{
    if (ch == '+')
    {
        preencher_token(token, "+", "smais");
    }
    else if (ch == '-')
    {
        preencher_token(token, "-", "smenos");
    }
    else if (ch == '*')
    {
        preencher_token(token, "*", "smult");
    }
}

//----------------------------------------------
//	Funcao de tratamento de operador relacional
//----------------------------------------------

void trata_opRela(FILE *p, char ch, tokens *token)
{
    //Trata operador relacional < > =
    if (ch == '>')
    {
        ch = fgetc(p);
        if (ch == '=')
        {
            preencher_token(token, ">=", "smaiorig");
        }
        else
        {
            preencher_token(token, ">", "smaior");
            ungetc(ch, p);
        }
    }
    else if (ch == '<')
    {
        ch = fgetc(p);
        if (ch == '=')
        {
            preencher_token(token, "<=", "smenorig");
        }
        else
        {
            preencher_token(token, "<", "smenor");
            ungetc(ch, p);
        }
    }
    else if (ch == '!')
    {
        char aux = ch;
        ch = fgetc(p);
        if (ch == '=')
        {
            preencher_token(token, "!=", "sdif");
        }
        else
        {
            ungetc(ch, p);
            //ERRO
            char *oto = (char *)malloc(sizeof(char) + 1);
            oto[0] = aux;
            strcat(oto, "\0");
            preencher_token(token, oto, "serro");
            free(oto);
        }
    }
    else if (ch == '=')
    {
        preencher_token(token, "=", "sig");
    }
}

//----------------------------------------------
//	Funcao de tratamento de pontuacao
//----------------------------------------------

void trata_pontuacao(FILE *p, char ch, tokens *token)
{
    if (ch == '.')
    {
        preencher_token(token, ".", "sponto");
    }
    else if (ch == ';')
    {
        preencher_token(token, ";", "sponto_virgula");
    }
    else if (ch == ',')
    {
        preencher_token(token, ",", "svirgula");
    }
    else if (ch == '(')
    {
        preencher_token(token, "(", "sabre_parenteses");
    }
    else if (ch == ')')
    {
        preencher_token(token, ")", "sfecha_parenteses");
    }
}

//----------------------------------------------
//	Funcao de preenchimento do token
//----------------------------------------------

void preencher_token(tokens *token, char lexema[], char simbolo[])
{
    int lex_tam = strlen(lexema);
    int sim_tam = strlen(simbolo);
    token->lexema = (char *)malloc(sizeof(lex_tam));
    sprintf(token->lexema, "%s", lexema);
    token->simbolo = simbolo;
}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------



//---------------------------------------------------------------------------------
//									SEMANTICO
//---------------------------------------------------------------------------------

//----------------------------------------------
//	Funcao infix to posfix
//----------------------------------------------

void infix_posfix(Lista *fila_aux){
    printf("\n-- Entrou em infix to posfix --\n");
    //se precisa desalocar as Listas e como faze-lo
    //pilha para posfix
    Stack pilha_aux;
    initStack(&pilha_aux);

    //expressao posfix
    Lista saida;
    init_fila(&saida);

    //andar na fila
    item_fila *x;
    x=fila_aux->inicio;

    //auxiliar elemento 
    stack_element elemento_aux;

    //enquanto nao chega ao fim da expressao 
    while(x != NULL){
       
        if(strcmp(x->info.lexema,"(") == 0){ //se eh abre parentesses
            //insere na pilha
            push(&pilha_aux,x->info);
        }
        else if(strcmp(x->info.tipo,"identificador") == 0 || strcmp(x->info.tipo,"numero") == 0 || strcmp(x->info.lexema,"verdadeiro") == 0 || strcmp(x->info.lexema,"falso") == 0){  //se eh variavel ou numero
            //insere na saida
            inserir_fila(&saida,x->info);
        }
        else if(strcmp(x->info.tipo,"operador") == 0){
            //pop da pilha
            if(isEmpty(pilha_aux)){
                push(&pilha_aux,x->info);
            }else{
                elemento_aux = peek(&pilha_aux);
                while(!isEmpty(pilha_aux) && strcmp(elemento_aux.tipo,"operador") == 0 && precedencia(elemento_aux.lexema) >= precedencia(x->info.lexema)){
                    elemento_aux = pop(&pilha_aux);
                    inserir_fila(&saida,elemento_aux);
                    if(!isEmpty(pilha_aux)){
                        elemento_aux = peek(&pilha_aux);
                    }               
                }
                //push(&pilha_aux,elemento_aux);
                push(&pilha_aux,x->info);
            }
            
        }
        else if(strcmp(x->info.lexema,")") == 0){
            elemento_aux = pop(&pilha_aux);
            while(!strcmp(elemento_aux.lexema,"(") == 0){
                inserir_fila(&saida,elemento_aux);
                elemento_aux = pop(&pilha_aux);
            } 
        }
        else{
            printf("\nExpressao infixa invalida.\n");
            fprintf(output_arquivo,"Expressao infixa invalida");
            
            exit(1);
        }
        x = x->prox;
    }
    while(!isEmpty(pilha_aux)){
        elemento_aux = pop(&pilha_aux);
        inserir_fila(&saida,elemento_aux);
    }
    
    *fila_aux = saida;
}

//----------------------------------------------
//	Funcao Precedencia
//----------------------------------------------

int precedencia(char* operador){
    if(strcmp(operador,"-u") == 0 || strcmp(operador,"+u") == 0){ //(+ positivo, - negativo)
        return 8;
    }else if(strcmp(operador,"*") == 0 || strcmp(operador,"div") == 0){ // (*,div)
        return 7;
    }else if(strcmp(operador,"+") == 0 || strcmp(operador,"-") == 0){ //(+,-)
        return 6;
    }else if(strcmp(operador,">") == 0 || strcmp(operador,">=")== 0 || strcmp(operador,"=") == 0 || strcmp(operador,"<") == 0 || strcmp(operador,"<=")==0 || strcmp(operador,"!=")==0){
        return 5;
    }else if(strcmp(operador,"nao") == 0){
        return 4;
    }else if(strcmp(operador,"e") == 0){
        return 3;
    }else if(strcmp(operador,"ou") == 0){
        return 2;
    }else{
        return 1;
    }
}

//----------------------------------------------
//	Funcao Analise Semantica
//----------------------------------------------

void analise_semantica(Stack *tab_simbolo,Lista *fila_aux,char *cmp_tipo){
    printf("\n-- Entrou em analise semantica --\n");

    //andar na fila
    item_fila *x;
    x=fila_aux->inicio;

    Lista aux;
    init_fila(&aux);

    //auxiliar elemento 
    stack_element anterior_old, anterior_new, aux_elem;

    int i=1;
    int tipo;

    //antes de fazer a analise, mudar o campo tipo dos identificadores e numeros
    while(x != NULL){
        inserir_fila(&aux,x->info);
        if(strcmp(x->info.tipo,"identificador") == 0){
            x->info.tipo = consultar_tab(tab_simbolo,x->info.lexema).tipo;

            // fizemos essa medida paleativa para aceitar funcao no meio da expressao
            // Se mexer o titanic afunda
            if(strcmp(x->info.tipo,"funcao inteiro") == 0){
                x->info.tipo = "inteiro";
            }
            if(strcmp(x->info.tipo,"funcao booleano") == 0){
                x->info.tipo = "booleano";
            }

        }else if(strcmp(x->info.tipo,"numero") == 0){
            x->info.tipo = "inteiro";
        }/*else if(strcmp(x->info.tipo,"verdadeiro") == 0 || strcmp(x->info.tipo,"falso") == 0){
            x->info.tipo = "booleano";
        }*/
        x = x->prox;
    }
    //---------
    x=fila_aux->inicio;
    
    while(x != NULL){
        //imprimir_fila(fila_aux);
        //printf("\n\n");
        //deve percorrer ate o 1 operador
        if(strcmp(x->info.tipo,"operador") == 0){ 
            //ver qual operador eh e ver o que ele espera (1-2 args) e o tipo q espera
            tipo = tipo_operador(x->info.lexema);

            if(tipo==1){ // * div + -
                // I e I => I
                if(strcmp(anterior_old.tipo,"inteiro") == 0){
                    if(strcmp(anterior_new.tipo,"inteiro") == 0){
                        //eliminar os itens anterior_new e operador e mudar lexema de anterior_old para R.
                        eliminar(fila_aux, i); //tira operador
                        eliminar(fila_aux, --i); //tira anterior_new
                        aux_elem.lexema = "R";
                        aux_elem.tipo = anterior_old.tipo;
                        alterar(fila_aux, --i,aux_elem);
                    }else{
                        //erro
                        printf("Incompatibilidade de tipo, identificador (%s) e operacao (%s) Linha: %d\n",anterior_new.lexema, x->info.lexema,linha_atual);
                        fprintf(output_arquivo,"Linha: %d\nIncompatibilidade de tipo, identificador (%s) e operacao (%s) ",anterior_new.lexema, x->info.lexema,linha_atual);
                        exit(1);
                    }
                }else{
                    //erro
                    printf("Incompatibilidade de tipo entre identificador (%s) e operacao (%s) Linha: %d\n",anterior_old.lexema, x->info.lexema,linha_atual);
                    fprintf(output_arquivo,"Linha: %d\nIncompatibilidade de tipo entre identificador (%s) e operacao (%s) ",anterior_old.lexema, x->info.lexema,linha_atual);
                    exit(1);
                }
            }else if(tipo==2){ //-u +u
                // I => I
                if(strcmp(anterior_old.tipo,"inteiro") == 0){
                    eliminar(fila_aux, i);
                    aux_elem.lexema = "R";
                    aux_elem.tipo = anterior_old.tipo;
                    alterar(fila_aux, --i,aux_elem);
                }else{
                    //erro
                    printf("Incompatibilidade de tipo entre identificador (%s) e operacao (%s) Linha: %d\n",anterior_old.lexema, x->info.lexema,linha_atual);
                    fprintf(output_arquivo,"Linha: %d\nIncompatibilidade de tipo entre identificador (%s) e operacao (%s) ",anterior_old.lexema, x->info.lexema,linha_atual);
                    exit(1);
                }
            }else if(tipo==3){ // > >= < <= =
                // I+I => B
                if(strcmp(anterior_old.tipo,"inteiro") == 0){
                    if(strcmp(anterior_new.tipo,"inteiro") == 0){
                        //eliminar os itens anterior_new e operador e mudar lexema de anterior_old para R.
                        eliminar(fila_aux, i); //tira operador
                        eliminar(fila_aux, --i); //tira anterior_new
                        aux_elem.lexema = "R";
                        aux_elem.tipo = "booleano";
                        alterar(fila_aux, --i,aux_elem);
                    }else{
                        //erro
                        printf("Incompatibilidade de tipo, identificador (%s) e operacao (%s) Linha: %d\n",anterior_new.lexema, x->info.lexema,linha_atual);
                        fprintf(output_arquivo,"Linha: %d\nIncompatibilidade de tipo, identificador (%s) e operacao (%s) ",anterior_new.lexema, x->info.lexema,linha_atual);
                        exit(1);
                    }
                }else{
                    //erro
                    printf("Incompatibilidade de tipo entre identificador (%s) e operacao (%s) Linha: %d\n",anterior_old.lexema, x->info.lexema,linha_atual);
                    fprintf(output_arquivo,"Linha: %d\nIncompatibilidade de tipo entre identificador (%s) e operacao (%s) ",anterior_old.lexema, x->info.lexema,linha_atual);
                    exit(1);
                }
            }else if(tipo==4){ // e ou
                // B+B => B
                if(strcmp(anterior_old.tipo,"booleano") == 0){
                    if(strcmp(anterior_new.tipo,"booleano") == 0){
                        //eliminar os itens anterior_new e operador e mudar lexema de anterior_old para R.
                        eliminar(fila_aux, i); //tira operador
                        eliminar(fila_aux, --i); //tira anterior_new
                        aux_elem.lexema = "R";
                        aux_elem.tipo = anterior_old.tipo;
                        alterar(fila_aux, --i,aux_elem);
                    }else{
                        //erro
                        printf("Incompatibilidade de tipo, identificador (%s) e operacao (%s) Linha: %d\n",anterior_new.lexema, x->info.lexema,linha_atual);
                        fprintf(output_arquivo,"Linha: %d\nIncompatibilidade de tipo, identificador (%s) e operacao (%s) ",anterior_new.lexema, x->info.lexema,linha_atual);
                        exit(1);
                    }
                }else{
                    //erro
                    printf("Incompatibilidade de tipo entre identificador (%s) e operacao (%s) Linha: %d\n",anterior_old.lexema, x->info.lexema,linha_atual);
                    fprintf(output_arquivo,"Linha: %d\nIncompatibilidade de tipo entre identificador (%s) e operacao (%s)",anterior_old.lexema, x->info.lexema,linha_atual);
                    exit(1);
                }
            }else if(tipo==5){ // nao
                // B => B
                if(strcmp(anterior_old.tipo,"booleano") == 0){
                    eliminar(fila_aux, i);
                    aux_elem.lexema = "R";
                    aux_elem.tipo = anterior_old.tipo;
                    alterar(fila_aux, --i,aux_elem);
                }else{
                    //erro
                    printf("Incompatibilidade de tipo entre identificador (%s) e operacao (%s) Linha: %d\n",anterior_old.lexema, x->info.lexema,linha_atual);
                    fprintf(output_arquivo,"Linha: %d\nIncompatibilidade de tipo entre identificador (%s) e operacao (%s)",anterior_old.lexema, x->info.lexema,linha_atual);
                    exit(1);
                }
            }

            //voltar ao comeco da lista
            i=1;
            x=fila_aux->inicio;
        }else{
            if(i==1){
                anterior_old = x->info;
                i++;
            }else if(i==2){
                anterior_new = x->info;
                i++;
            }else{
                anterior_old = anterior_new;
                anterior_new = x->info;
                i++;
            }
            x=x->prox;
        }
    }
    
    //verificar se o tipo da expressao é correspondente a funcao q chamou
    //se mexer o titanic afunda
    if(strcmp(cmp_tipo,"funcao inteiro")==0||strcmp(cmp_tipo,"funcao booleano")==0||strcmp(fila_aux->inicio->info.tipo,"funcao inteiro")==0||strcmp(fila_aux->inicio->info.tipo,"funcao booleano")==0){
        if(strcmp(cmp_tipo,"funcao inteiro")==0||strcmp(fila_aux->inicio->info.tipo,"funcao inteiro")==0){
            fila_aux->inicio->info.tipo = "inteiro";
            cmp_tipo = "inteiro";
        }else if(strcmp(cmp_tipo,"funcao booleano")==0||strcmp(fila_aux->inicio->info.tipo,"funcao booleano")==0){
            fila_aux->inicio->info.tipo = "booleano";
            cmp_tipo = "booleano";
        }
    }
    //printf("tipo: %s %s\n %s\n",fila_aux->inicio->info.lexema,fila_aux->inicio->info.tipo, cmp_tipo);
    if(!strcmp(fila_aux->inicio->info.tipo,cmp_tipo) == 0){
        //erro
        printf("Incompatibilidade de tipo da expressao com o comando desejado. Linha: %d\n",linha_atual);
        fprintf(output_arquivo,"Linha: %d\nIncompatibilidade de tipo da expressao com o comando desejado.",linha_atual);
        
        exit(1);
    }
    printf("Nenhuma Incompatibilidade de tipo encontrada\n");

    //Gera codigo da expressao
    gera_cod_expressao(tab_simbolo,&aux);
    
}

//----------------------------------------------
//	Funcao tipo
//----------------------------------------------

int tipo_operador(char* operador){
    if(strcmp(operador,"*") == 0 || strcmp(operador,"div") == 0 || strcmp(operador,"+") == 0 || strcmp(operador,"-") == 0){
        return 1;
    }else if(strcmp(operador,"-u") == 0 || strcmp(operador,"+u") == 0){ 
        return 2;
    }else if(strcmp(operador,">") == 0 || strcmp(operador,">=") == 0 || strcmp(operador,"=") == 0 || strcmp(operador,"<") == 0 || strcmp(operador,"<=")==0 || strcmp(operador,"!=")==0){
        return 3;
    }else if(strcmp(operador,"ou") == 0 || strcmp(operador,"e") == 0){
        return 4;
    }else if(strcmp(operador,"nao") == 0){
        return 5;
    }else{
        printf("Erro em visualizar tipo do operador\n");
        fprintf(output_arquivo,"Erro em visualizar tipo do operador\n");
        exit(1);
    }
}


//----------------------------------------------
//	Funcao tipo
//----------------------------------------------

void gera_cod_expressao(Stack *tab_simbolo, Lista *aux){
    printf("-- Entrou em gera codigo da expressao --\n");
    //imprimir_fila(aux);
    //printf("\n\n");
    //imprimirTabela(tab_simbolo);

    //andar na fila
    item_fila *x;
    x=aux->inicio;

    stack_element elem_aux;

    while(x != NULL){
        if(strcmp(x->info.tipo,"identificador")==0){
            //ver qual identificador que é variavel ou funcao se tem diferenca
            elem_aux = consultar_tab(tab_simbolo,x->info.lexema);

            if(strcmp(elem_aux.tipo,"funcao inteiro")==0|| strcmp(elem_aux.tipo,"funcao booleano") == 0){
                fprintf(codigo," ,CALL,%d, \n",elem_aux.memoria);
                fprintf(codigo," ,LDV,%d, \n",0);
            }else{
                fprintf(codigo," ,LDV,%d, \n",elem_aux.memoria);
            }
            //ver como uma funcao pode ser chamada no meio de uma expressao e se ja nao estamos trando isso no chamada de funcao
        
        }else if(strcmp(x->info.tipo,"numero")==0){
            //basta gerar o LDC com o lexema do numero
            fprintf(codigo," ,LDC,%s, \n",x->info.lexema);

        }else if(strcmp(x->info.lexema,"verdadeiro")==0){
            fprintf(codigo," ,LDC,%d, \n",1);

        }else if(strcmp(x->info.lexema,"falso")==0){
            fprintf(codigo," ,LDC,%d, \n",0);

        }else if(strcmp(x->info.tipo,"operador")==0){
            //aqui verificar cada lexema
            if(strcmp(x->info.lexema,"*") == 0){
                fprintf(codigo," ,MULT, , \n");

            }else if(strcmp(x->info.lexema,"div") == 0){
                fprintf(codigo," ,DIVI, , \n");

            }else if(strcmp(x->info.lexema,"+") == 0){
                fprintf(codigo," ,ADD, , \n");

            }else if(strcmp(x->info.lexema,"-") == 0){
                fprintf(codigo," ,SUB, , \n");

            }else if(strcmp(x->info.lexema,"-u") == 0){ //perguntar a respeito do +u
                fprintf(codigo," ,INV, , \n");

            }else if(strcmp(x->info.lexema,">") == 0){
                fprintf(codigo," ,CMA, , \n");

            }else if(strcmp(x->info.lexema,"<") == 0){
                fprintf(codigo," ,CME, , \n");

            }else if(strcmp(x->info.lexema,">=") == 0){
                fprintf(codigo," ,CMAQ, , \n");

            }else if(strcmp(x->info.lexema,"<=") == 0){
                fprintf(codigo," ,CMEQ, , \n");

            }else if(strcmp(x->info.lexema,"=") == 0){
                fprintf(codigo," ,CEQ, , \n");

            }else if(strcmp(x->info.lexema,"!=") == 0){
                fprintf(codigo," ,CDIF, , \n");

            }else if(strcmp(x->info.lexema,"ou") == 0){
                fprintf(codigo," ,OR, , \n");

            }else if(strcmp(x->info.lexema,"e") == 0){
                fprintf(codigo," ,AND, , \n");

            }else if(strcmp(x->info.lexema,"nao") == 0){
                fprintf(codigo," ,NEG, , \n");

            }
        }
        x=x->prox;
    }
}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------



//---------------------------------------------------------------------------------
//									SINTATICO
//---------------------------------------------------------------------------------

//----------------------------------------------
//	Funcao Analisa Bloco
//----------------------------------------------

void analisa_bloco(FILE *p, tokens *token, Stack *tab_simbolo ){
    printf("\n-- Entrou em analisa bloco --\n");
    printf("(Token) Simbolo: %s (Token) Lexico: %s Linha: %d\n",token->simbolo,token->lexema,linha_atual);

	*token = lexico(p);
	analisa_et_variaveis(p,token, tab_simbolo); //talvez seja necessario salvar o valor de contador ao sair dessa func
	analisa_subrotinas(p,token,tab_simbolo);
	analisa_comandos(p,token,tab_simbolo);
}

//----------------------------------------------
//	Funcao Etapa de declaracao de variaveis
//----------------------------------------------

void analisa_et_variaveis(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa_et_variaveis --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

	if(token->simbolo == "svar")
	{        
		*token = lexico(p);        
		if(token->simbolo == "sidentificador")
		{            
			while(token->simbolo == "sidentificador")
			{                
				analisa_variaveis(p, token, tab_simbolo);
				if(token->simbolo == "sponto_virgula")
				{
					//Lexico(token);
                    *token = lexico(p);

                    //Caso aqui tenha mais uma linha de declaracao de variavel {<declaração de variáveis>;}
                    if(token->simbolo == "sidentificador"){
                        analisa_variaveis(p, token, tab_simbolo);
                        if(token->simbolo == "sponto_virgula"){
                            *token = lexico(p);
                        }
                    }
                    
                    //pega o proximo token e volta o analisa bloco
                    return;
					
				}else{
					//erro
					printf("esperado ; AO FIM \n");
                    fprintf(output_arquivo,"esperado ; AO FIM \n");
					exit(1);
				}
			}
		}else{
			//erro
			printf("esperado ao menos um identificador depois de var\n");
            fprintf(output_arquivo,"esperado ao menos um identificador depois de var\n");
			exit(1);
		}
	}
    return;
}

//----------------------------------------------
//	Funcao Analisa variaveis
//----------------------------------------------

void analisa_variaveis(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa variaveis --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);
    
    int aux_cont_var = 0, aux_end_disp = end_disponivel;
    //cont = 0;
	while(token->simbolo != "sdoispontos"){
		if(token->simbolo == "sidentificador")
		{

            //Pesquisa_duplicvar_tabela(token.lexema)
            if(!pesquisa_duplicvar_tabela(tab_simbolo, token->lexema))//1- duplicacao 0- sem duplicacao
            {
                //insere_tabela(token.lexema,"variavel")
                aux_cont_var++;
                insere_tabela(tab_simbolo, token->lexema, "variavel","", &end_disponivel); //talvez colocar o end disponivel utilizado
                end_disponivel++;

                *token = lexico(p);
                
                if(token->simbolo == "svirgula" || token->simbolo == "sdoispontos")
                {
                    if(token->simbolo == "svirgula")
                    {
                        *token = lexico(p);
                        if(token->simbolo == "sdoispontos") // ,:
                        {
                            //erro
                            printf("esperado identificador apos virgula\n");
                            fprintf(output_arquivo,"esperado identificador apos virgula\n");
                            exit(1);
                        }//fim
                    }//fim
                }else{
                    //erro
                    printf("esperado uma , ou : na definicção da variavel\n");
                    fprintf(output_arquivo,"esperado uma , ou : na definicção da variavel\n");
                    exit(1);
                }
            }else{
                //erro
                printf("variavel duplicada na tabela de simbolo\n");
                fprintf(output_arquivo,"variavel duplicada na tabela de simbolo\n");
                exit(1);
            }
		}else{
			//erro
			printf("durante a declaracao de variaveis até : esperado identificadores");
            fprintf(output_arquivo,"durante a declaracao de variaveis até : esperado identificadores");
			exit(1);
		}
	}
    *token = lexico(p);
    analisa_tipo(p, token, tab_simbolo);

    //Gera
    fprintf(codigo," ,ALLOC,%d,%d\n",aux_end_disp,aux_cont_var);
    cont += aux_cont_var;
}

//----------------------------------------------
//	Funcao Analisa tipo das variaveis
//----------------------------------------------

void analisa_tipo(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa tipo --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

    if(token->simbolo != "sinteiro" && token->simbolo != "sbooleano")
    {
        //erro
        printf("tipo da(s) variavel(is) incorreto, esperado inteiro ou booleano\n");
        fprintf(output_arquivo,"tipo da(s) variavel(is) incorreto, esperado inteiro ou booleano\n");
        exit(1);
    }
    else
    {
        //coloca_tipo_tabela(token.lexema)
        colocar_tipo(tab_simbolo, token->lexema);
        *token = lexico(p);
    }
}

//----------------------------------------------
//	Funcao Analisa subrotinas <etapa de declaração de sub-rotinas>
//----------------------------------------------
		
void analisa_subrotinas(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa_subrotinas --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);
   
    int aux_cont;

    int auxrot, flag;   //Def auxrot, flag inteiro
    flag = 0;           //flag = 0
    
    if((token->simbolo == "sprocedimento") || (token->simbolo == "sfuncao")){   //if (token.simbolo = sprocedimento) ou (token.simbolo = sfunção)
        auxrot = rotulo;                                                        //auxrot := rotulo
        fprintf(codigo," ,JMP,%d, \n",rotulo);                                    //GERA('		',JMP,rotulo,'		')		{Salta sub-rotinas}
        rotulo = rotulo + 1;                                                    //rotulo:= rotulo + 1
        flag = 1;                                                               //flag = 1
    }
    while((token->simbolo == "sprocedimento")||(token->simbolo == "sfuncao")){
        aux_cont = cont;
        cont = 0;
        if(token->simbolo == "sprocedimento"){
            analisa_declaracao_procedimento(p,token,tab_simbolo);
        }else{
            analisa_declaracao_funcao(p,token,tab_simbolo);
        }
        cont = aux_cont;
        if(token->simbolo == "sponto_virgula"){
            *token = lexico(p);
        }else{
            //erro
            printf("Falta de ';' ao fim de funcao ou procedimento. Linha: %d\n",linha_atual);
            fprintf(output_arquivo,"Linha: %d\nFalta de ';' ao fim de funcao ou procedimento.\n",linha_atual);  
            exit(1);
        }
    }
    
    if(flag == 1){                              //if flag = 1
        fprintf(codigo,"%d,NULL, , \n",auxrot);    //entao Gera(auxrot,NULL,´ ´,´ ´) {início do principal}
    }
}

//----------------------------------------------
//	Funcao Analisa declaracao de procedimento
//----------------------------------------------
    
void analisa_declaracao_procedimento(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa_declaracao_procedimento --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

    *token = lexico(p);
    //nivel:="L"(marca ou novo galho)
    if(token->simbolo == "sidentificador"){
        //pesquisa_declproc_tabela(token.lexema)
        if(!pesquisa_duplicfunc_tabela(tab_simbolo,token->lexema)){
            insere_tabela(tab_simbolo,token->lexema,"procedimento","x", &rotulo); //{guarda na TabSimb},
                      
            fprintf(codigo,"%d,NULL, , \n",rotulo);   //Gera(rotulo,NULL,´ ´,´ ´) {CALL irá buscar este rótulo na TabSimb}           
            rotulo = rotulo +1;                     //rotulo:= rotulo+1
            
            *token = lexico(p);
            
            if(token->simbolo == "sponto_virgula"){
                //cont = 0;
                analisa_bloco(p,token,tab_simbolo);
            }else{
                //erro
                printf("Falta ; ao fim da declaracao de procedimento. Linha:%d \n", linha_atual);
                fprintf(output_arquivo,"Linha: %d\nFalta de ';' ao fim de funcao ou procedimento.\n",linha_atual);
                exit(1);
            }
        }else{
            //erro
            printf("Nome de procedimento duplicado. Linha:%d \n", linha_atual);
            fprintf(output_arquivo,"Linha: %d\nNome de procedimento duplicado.\n",linha_atual);
            exit(1);
        }
    }else{
        //erro
        printf("Depois de procedimento esperado nome do procedimento. Linha:%d \n", linha_atual);
        fprintf(output_arquivo,"Linha: %d\nDepois de procedimento esperado nome do procedimento.\n",linha_atual);
        exit(1);
    }
    //DESEMPILHA OU VOLTA NÍVEL
    //printf("\nAntes de desempilhar\n");
    //imprimirTabela(tab_simbolo);
    desempilha_nivel(tab_simbolo); //Provavelmente tera o dalloc
    fprintf(codigo," ,RETURN, , \n");
    //printf("\nDepois de desempilhar\n");
    //imprimirTabela(tab_simbolo);
}

//----------------------------------------------
//	Funcao Analisa declaracao de funcao
//----------------------------------------------

void analisa_declaracao_funcao(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa declaracao de funcao --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

	*token = lexico(p);
	
	if(token->simbolo == "sidentificador"){
        //pesquisa_declfunc_tabela(token.lexema)
		if(!pesquisa_duplicfunc_tabela(tab_simbolo,token->lexema)){  //pesquisa_declfunc_tabela(token.lexema)
			//insere_tabela(token.lexema,”procedimento”,nível, rótulo) {guarda na TabSimb}
            insere_tabela(tab_simbolo,token->lexema,"funcao","x",&rotulo);

            fprintf(codigo,"%d,NULL, , \n",rotulo);   //Gera(rotulo,NULL,´ ´,´ ´) {CALL irá buscar este rótulo na TabSimb}           
            rotulo = rotulo +1;                     //rotulo:= rotulo+1

			*token = lexico(p);
			if(token->simbolo == "sdoispontos"){
				*token = lexico(p);
				if(token->simbolo == "sinteiro" || token->simbolo == "sbooleano"){
					if(token->simbolo == "sinteiro"){
						//TABSIMB[pc].tipo:= “função inteiro”		//trocar o tipo da funcao na tabela de simbolo
                        tab_simbolo->top->info.tipo = "funcao inteiro";
					}else{
						//TABSIMB[pc].tipo:= “função boolean”		//acho que da para trocar o conteudo do elemento do topo da pilha
                        tab_simbolo->top->info.tipo = "funcao booleano";
                    }
					*token = lexico(p);
					if(token->simbolo == "sponto_virgula"){
                        //cont = 0;
						analisa_bloco(p,token,tab_simbolo);
					}
				}else{
					//erro
					printf("As funcoes precisam ser do tipo inteiro ou booleano. Linha:%d \n",linha_atual);
                    fprintf(output_arquivo,"Linha: %d\nAs funcoes precisam ser do tipo inteiro ou booleano.\n",linha_atual);
					exit(1);
				}
			}else{
				//erro
				printf("Esperado dois pontos depois do nome da funcao. Linha:%d \n",linha_atual);
                fprintf(output_arquivo,"Linha: %d\nEsperado dois pontos depois do nome da funcao.\n",linha_atual);
				exit(1);
			}
		}else{
			//erro
			printf("Nome de funcao duplicado na tabela de simbolo. Linha:%d \n",linha_atual);
            fprintf(output_arquivo,"Linha: %d\nNome de funcao duplicado na tabela de simbolo.\n",linha_atual);
			exit(1);
		}
	}else{
        //erro
        printf("Depois de funcao esperado nome da funcao. Linha:%d \n",linha_atual);
        fprintf(output_arquivo,"Linha: %d\nDepois de procedimento esperado nome do procedimento.\n",linha_atual);
        exit(1);
    }
	//DESEMPILHA OU VOLTA AO NIVEL		Entendo que tenha que apgar da tabela de simbolos o que foi colocado até aqui da func
    //printf("\nAntes de desempilhar\n");
    //imprimirTabela(tab_simbolo);
    desempilha_nivel(tab_simbolo);
    fprintf(codigo," ,RETURN, , \n");
    //printf("\nDepois de desempilhar\n");
    //imprimirTabela(tab_simbolo);
}

//----------------------------------------------
//	Funcao Analisa comandos
//----------------------------------------------

void analisa_comandos(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa comandos --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);
    
	if(token->simbolo == "sinicio"){
		*token = lexico(p);
		analisa_comando_simples(p,token,tab_simbolo);
		while(token->simbolo != "sfim"){
			if(token->simbolo == "sponto_virgula"){
				*token = lexico(p);
				if(token->simbolo != "sfim"){
					analisa_comando_simples(p,token,tab_simbolo);
                    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);
				}
				//fim
			}else{
				//erro
				printf("Falta de ; ao fim do comando. Linha:%d\n",linha_atual);
                fprintf(output_arquivo,"Linha: %d\nFalta de ; ao fim do comando.\n",linha_atual);
				exit(1);				
			}
			//fim
		}
		*token = lexico(p);
        printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);
	}else{
		//erro
        fprintf(output_arquivo,"Linha: %d\nPara o bloco de comandos falta identificador de inicio.\n",linha_atual);
		printf("Para o bloco de comandos falta identificador de inicio. Linha:%d\n",linha_atual);
		exit(1);
	}
}

//----------------------------------------------
//	Funcao Analisa comando simples
//----------------------------------------------

void analisa_comando_simples(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa comando simples --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

    Lista fila_aux;
    init_fila(&fila_aux);

	if(token->simbolo == "sidentificador"){
		analisa_atrib_chprocedimento(p,token,tab_simbolo,&fila_aux);
		
	}else if(token->simbolo == "sse"){
		analisa_se(p,token,tab_simbolo,&fila_aux);
		
	}else if(token->simbolo == "senquanto"){
		analisa_enquanto(p,token,tab_simbolo,&fila_aux);
		
	}else if(token->simbolo == "sleia"){
		analisa_leia(p,token,tab_simbolo);
		
	}else if(token->simbolo == "sescreva"){
		analisa_escreva(p,token,tab_simbolo);
		
	}else{
		analisa_comandos(p,token,tab_simbolo);
	}
}

//----------------------------------------------
//	Funcao Analisa atribuicao ch procedimento
//----------------------------------------------

void analisa_atrib_chprocedimento(FILE *p, tokens *token, Stack *tab_simbolo, Lista *fila_aux){
    printf("\n-- Entrou em atribuicao chamada de procedimento --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

    //Parte pesquisar na tabela a primeira ocorrencia: aula: 13/10 1h07min
    stack_element var_bkp;
    var_bkp = consultar_tab(tab_simbolo,token->lexema); //pesquisa na tabela de simbolo o identificador

	*token = lexico(p);
    printf("Token simbolo: %s Token lexico: %s\n",token->simbolo,token->lexema); // simbolo sdoispontos, lexico :=

	if(token->simbolo == "satribuicao"){ //satribuicao
        inserir_fila(fila_aux,var_bkp); //inseriu a variavel da atribuicao na fila

        *token = lexico(p);
		analisa_atribuicao(p,token,tab_simbolo,fila_aux); //variavel var_bkp tem a variavel onde sera feito atribuicao
	}else{
        //se chega aqui significa que exite na tab de simbolo
		chamada_procedimento(p, var_bkp, tab_simbolo); //se for so chamada é direto ; ai nao faz nada na funcao
	}
}

//----------------------------------------------
//	Funcao Analisa Leia
//----------------------------------------------

void analisa_leia(FILE *p, tokens *token, Stack *tab_simbolo){ 
    printf("\n-- Entrou em analisa leia --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

    stack_element aux_var;

    //Gera RD
    fprintf(codigo," ,RD, , \n");

	*token = lexico(p);
	if(token->simbolo == "sabre_parenteses"){
		*token = lexico(p);
		if(token->simbolo == "sidentificador"){
			//if(pesquisa_declvar_tabela(token.lexema)){  //(pesquisa em toda a tabela) ele nao para no x
            if(pesquisa_existencia(tab_simbolo,token->lexema)){ //ve se ja foi declarado em algum lugar
                aux_var = consultar_tab(tab_simbolo,token->lexema);
				*token = lexico(p);
				if(token->simbolo == "sfecha_parenteses"){
                    if(strcmp(aux_var.tipo,"inteiro")==0){
                        //Gera STR de em qual variavel deve guardar o read
                        fprintf(codigo," ,STR,%d, \n",aux_var.memoria);
                    }else{
                        printf("Nao eh possivel fazer um leia de variavel booleana\n");
                        fprintf(output_arquivo,"Linha: %d\nNao eh possivel fazer um leia de variavel booleana.\n",linha_atual);
                        exit(1);
                    }

                    
					*token = lexico(p);
				}else{
					//erro
					printf("Necessario fechar parentes no fim do comando leia. Linha:%d\n",linha_atual);
                    fprintf(output_arquivo,"Linha: %d\nNecessario fechar parentes no fim do comando leia.\n",linha_atual);
					exit(1);		
				}
			}else{
				//erro
				printf("Variavel nao declarada. Linha:%d\n",linha_atual);
                fprintf(output_arquivo,"Linha: %d\nVariavel nao declarada.\n",linha_atual);
				exit(1);
			}
		}else{
			//erro
			printf("Necessario uma variavel dentro do parenteses. Linha:%d\n",linha_atual);
            fprintf(output_arquivo,"Linha: %d\nNecessario uma variavel dentro do parenteses.\n",linha_atual);
			exit(1);
		}
	}else{
		//erro
		printf("Necessario abre parentes no comando leia. Linha:%d\n",linha_atual);
        fprintf(output_arquivo,"Linha: %d\nNecessario abre parentes no comando leia.\n",linha_atual);
		exit(1);
	}
}

//----------------------------------------------
//	Funcao Analisa escreva
//----------------------------------------------

void analisa_escreva(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa escreva --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

    stack_element var_aux;

	*token = lexico(p);
	if(token->simbolo == "sabre_parenteses"){
		*token = lexico(p);
		if(token->simbolo == "sidentificador"){
			//if(pesquisa_ declvarfunc_tabela(token.lexema))
            if(pesquisa_existencia(tab_simbolo,token->lexema)){
                var_aux = consultar_tab(tab_simbolo,token->lexema);

				*token = lexico(p);
				if(token->simbolo == "sfecha_parenteses"){
                    if(strcmp(var_aux.tipo,"inteiro")==0){
                        //Gera carregamento da variavel
                        fprintf(codigo," ,LDV,%d, \n",var_aux.memoria);
                        //Gera do print
                        fprintf(codigo," ,PRN, , \n");  
                    }else{
                        printf("Não é possivel imprimir variaveis do tipo booleano\n");
                        fprintf(output_arquivo,"Linha: %d\nNão é possivel imprimir variaveis do tipo booleano.\n",linha_atual);
                        exit(1);
                    }
                    

					*token = lexico(p);
				}else{
					//erro
					printf("Necessario fechar parentes no fim do comando escreva. Linha:%d\n", linha_atual);
                    fprintf(output_arquivo,"Linha: %d\nNecessario fechar parentes no fim do comando escreva.\n",linha_atual);
					exit(1);
				}
			}else{
				//erro
				printf("Varfunc nao foi declarada. Linha:%d\n",linha_atual);
                fprintf(output_arquivo,"Linha: %d\nVarfunc nao foi declarada.\n",linha_atual);
				exit(1);
			}
		}else{
			//erro
			printf("Necessario uma variavel dentro do parenteses. Linha:%d\n", linha_atual);
            fprintf(output_arquivo,"Linha: %d\nNecessario uma variavel dentro do parenteses.\n",linha_atual);
			exit(1);
		}
	}else{
		//erro
		printf("Necessario abre parentes no comando escreva. Linha:%d\n", linha_atual);
        fprintf(output_arquivo,"Linha: %d\nNecessario abre parentes no comando escreva.\n",linha_atual);
		exit(1);
	}
}

//----------------------------------------------
//	Funcao Analisa Enquanto <comando repeticao>
//----------------------------------------------

void analisa_enquanto(FILE *p, tokens *token, Stack *tab_simbolo, Lista *fila_aux){
    printf("\n-- Entrou em analisa enquanto --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

	//DEF auxrot1,auxrot2 inteiro
    int auxrot1,auxrot2;
	//auxrot1:=rotulo
    auxrot1 = rotulo;
	//GERA(rotulo,NULL,´ ´,´ ´) {inicio do while}
    fprintf(codigo,"%d,NULL, , \n",rotulo);
	//rotulo:=rotulo+1
    rotulo = rotulo+1;

	*token = lexico(p);
	analisa_expressao(p,token,tab_simbolo,fila_aux);
    //funcao lexico infix->posfix
    infix_posfix(fila_aux);
    //analise lexical (tipos variaveis, etc)
    analise_semantica(tab_simbolo, fila_aux,"booleano");

	if(token->simbolo == "sfaca"){
		//auxrot2:=rotulo
        auxrot2 = rotulo;
		//GERA(´ ´,JMPF,rotulo,´ ´) {salta se falso}
        fprintf(codigo," ,JMPF,%d, \n",rotulo);
		//rotulo:=rotulo+1
        rotulo = rotulo +1;

		*token = lexico(p);
		analisa_comando_simples(p,token,tab_simbolo);
		//GERA(´ ´,JMP,auxrot1,´ ´) {retorna inicio loop}
        fprintf(codigo," ,JMP,%d, \n",auxrot1);
		//GERA(auxrot2,NULL,´ ´,´ ´) {fim do while}
        fprintf(codigo,"%d,NULL, , \n",auxrot2);
	}else{
		//erro
		printf("Necessario indicar faça apos o comando while. Linha:%d\n", linha_atual);
        fprintf(output_arquivo,"Linha: %d\nNecessario indicar faça apos o comando while.\n",linha_atual);
		exit(1);
	}
}

//----------------------------------------------
//	Funcao Analisa Se <comando condicional>
//----------------------------------------------

void analisa_se(FILE *p, tokens *token, Stack *tab_simbolo, Lista *fila_aux){
    printf("\n-- Entrou em analisa se --\n");
    printf("Token simbolo: %s Token lexico: %s Linha:%d\n",token->simbolo,token->lexema,linha_atual);

    int auxrot,auxrot2,flag;

	*token = lexico(p);
    
	analisa_expressao(p,token,tab_simbolo,fila_aux);        //formacao da expressao na fila aux
    infix_posfix(fila_aux);                                 //funcao lexico infix->posfix
    analise_semantica(tab_simbolo, fila_aux,"booleano");    //analise lexical (tipos variaveis, etc)

	if(token->simbolo == "sentao"){
        //geracao para pular para pular ao senao
        auxrot = rotulo;
        fprintf(codigo," ,JMPF,%d, \n",rotulo);
        rotulo = rotulo + 1;

		*token = lexico(p);
		analisa_comando_simples(p,token,tab_simbolo);

		if(token->simbolo == "ssenao"){
            flag = 1;
            //geracao para pular de volta ao fluxo fora do if
            auxrot2 = rotulo;
            fprintf(codigo," ,JMP,%d, \n",rotulo);
            rotulo = rotulo + 1;

            //geracao que marca inicio do senao
            fprintf(codigo,"%d,NULL, , \n",auxrot);

			*token = lexico(p);
			analisa_comando_simples(p,token,tab_simbolo);
		} //aqui nao tem else pois senão nao é obrigatorio em um comando condicional
        else{
            //geracao que marca inicio do senao
            fprintf(codigo,"%d,NULL, , \n",auxrot);
        }
	}else{
		//erro
		printf("Necessario indicar enquanto do comando condicional. Linha:%d\n", linha_atual);
        fprintf(output_arquivo,"Linha: %d\nNecessario indicar enquanto do comando condicional.\n",linha_atual);
		exit(1);
	}
    if(flag == 1){
        //geracao que marca o fluxo fora do if
        fprintf(codigo,"%d,NULL, , \n",auxrot2);
    }
    
}

//----------------------------------------------
//	Funcao Analisa expressão <expressão>
//----------------------------------------------

void analisa_expressao(FILE *p, tokens *token, Stack *tab_simbolo, Lista *fila_aux){
    printf("\n-- Entrou em analisa expressao --\n");
    printf("Token simbolo: %s Token lexico: %s Linha:%d\n",token->simbolo,token->lexema,linha_atual);

	analisa_expressao_simples(p,token,tab_simbolo,fila_aux);
    
    stack_element auxiliar;
    
	if(token->simbolo == "smaior" || token->simbolo == "smaiorig" || token->simbolo == "sig" || token->simbolo == "smenor" || token->simbolo == "smenorig" || token->simbolo == "sdif"){
		auxiliar.lexema = token->lexema;
        auxiliar.tipo = "operador";
        inserir_fila(fila_aux,auxiliar);

        *token = lexico(p);
		analisa_expressao_simples(p,token,tab_simbolo, fila_aux);
	}
}

//----------------------------------------------
//	Funcao Analisa expressao simples
//----------------------------------------------

void analisa_expressao_simples(FILE *p, tokens *token, Stack *tab_simbolo, Lista *fila_aux){
    printf("\n-- Entrou em analisa expressao simples --\n");
    printf("Token simbolo: %s Token lexico: %s Linha:%d\n",token->simbolo,token->lexema,linha_atual);

    stack_element auxiliar;
	if(token->simbolo == "smais" || token->simbolo == "smenos"){  //smais de positivo
        auxiliar.tipo = "operador";
        //+u
        if(token->simbolo == "smais"){
            auxiliar.lexema = "+u";
            inserir_fila(fila_aux,auxiliar);
        }else{
           //-u
            auxiliar.lexema = "-u";
            inserir_fila(fila_aux,auxiliar);
        }
        
		*token = lexico(p);
    }
    analisa_termo(p,token,tab_simbolo,fila_aux);
    
    while(token->simbolo == "smais" || token->simbolo == "smenos" || token->simbolo == "sou"){
        auxiliar.lexema = token->lexema;
        auxiliar.tipo = "operador";
        inserir_fila(fila_aux,auxiliar);

        *token = lexico(p);
        analisa_termo(p,token,tab_simbolo,fila_aux);
    }
	
}

//----------------------------------------------
//	Funcao Analisa termo 
//----------------------------------------------

void analisa_termo(FILE *p, tokens *token, Stack *tab_simbolo,Lista *fila_aux){
    printf("\n-- Entrou em analisa termo --\n");
    printf("Token simbolo: %s Token lexico: %s Linha:%d\n",token->simbolo,token->lexema,linha_atual);

	analisa_fator(p,token,tab_simbolo,fila_aux);
    
    stack_element auxiliar;
    
	while(token->simbolo == "smult" || token->simbolo == "sdiv" || token->simbolo == "se"){
        auxiliar.lexema = token->lexema;
        auxiliar.tipo = "operador";
        inserir_fila(fila_aux,auxiliar);

		*token = lexico(p);
        analisa_fator(p,token,tab_simbolo,fila_aux);
        //analisa_expressao(p,token,tab_simbolo,fila_aux);
	}
}

//----------------------------------------------
//	Funcao Analisa fator
//----------------------------------------------

void analisa_fator(FILE *p, tokens *token, Stack *tab_simbolo, Lista *fila_aux){
    printf("\n-- Entrou em analisa fator --\n");
    printf("(Token) Simbolo: %s\t(Token) Lexico: %s - Linha:%d\n",token->simbolo,token->lexema,linha_atual);

    stack_element auxiliar;
    
    int ind=0;

	if(token->simbolo == "sidentificador"){	//variavel ou funcao
		//Se pesquisa_tabela(token.lexema,nível,ind)
        //imprimirTabela(tab_simbolo);
        if(pesquisa_tabela(tab_simbolo,token->lexema, &ind)){ 
            //Então Se (TabSimb[ind].tipo = “função inteiro”) ou (TabSimb[ind].tipo = “função booleano”)
            if(ind == 1){ //funcao
                //Inserir elemento na fila
                auxiliar.lexema = token->lexema;
                auxiliar.tipo = "identificador";
                inserir_fila(fila_aux,auxiliar);

                //Então Analisa_chamada_função
                analisa_chamada_funcao(p,token,tab_simbolo);
            }else if(ind == 2){ //variavel
                auxiliar.lexema = token->lexema;
                auxiliar.tipo = "identificador";
                inserir_fila(fila_aux,auxiliar);

                //Senão Léxico(token)
                *token = lexico(p);
                //printf("Token simbolo: %s Token lexico: %s\n",token->simbolo,token->lexema);
            }
		}else{
            //Senão ERRO
            printf("Variavel ou funcao nao declaradas. Linha: %d\n",linha_atual);
            fprintf(output_arquivo,"Linha: %d\nVariavel ou funcao nao declaradas.\n",linha_atual);
            exit(1);
        }
	}else if(token->simbolo == "snumero"){	//numero
        auxiliar.lexema = token->lexema;
        auxiliar.tipo = "numero";
        inserir_fila(fila_aux,auxiliar);

		*token = lexico(p);
		
        }else if(token->simbolo == "snao"){	//nao
            auxiliar.lexema = token->lexema;
            auxiliar.tipo = "operador";
            inserir_fila(fila_aux,auxiliar);

            *token = lexico(p);
            analisa_fator(p,token,tab_simbolo,fila_aux);
            
            }else if(token->simbolo == "sabre_parenteses"){	//expressao entre parenteses
                auxiliar.lexema = token->lexema;
                auxiliar.tipo = token->simbolo;
                inserir_fila(fila_aux,auxiliar);

                *token = lexico(p);
                analisa_expressao(p,token,tab_simbolo,fila_aux);
                if(token->simbolo == "sfecha_parenteses"){
                    auxiliar.lexema = token->lexema;
                    auxiliar.tipo = token->simbolo;
                    inserir_fila(fila_aux,auxiliar);

                    *token = lexico(p);
                }else{
                    // erro
                    printf("Falta fechar o parenteses da expressao. Linha:%d\n",linha_atual);
                    fprintf(output_arquivo,"Linha: %d\nFalta fechar o parenteses da expressao.\n",linha_atual);
                    exit(1);
                }
            }else if(strcmp(token->lexema,"verdadeiro") == 0 || strcmp(token->lexema,"falso")==0){
                auxiliar.lexema = token->lexema;
                auxiliar.tipo = "booleano";
                inserir_fila(fila_aux,auxiliar);

                *token = lexico(p);
            }else{
                //erro
                printf("Erro com operador relacional. Linha:%d\n",linha_atual);
                fprintf(output_arquivo,"Linha: %d\nErro com operador relacional.\n",linha_atual);
                exit(1);
            }
}

//----------------------------------------------
//	Funcao analisa atribuicao
//----------------------------------------------

void analisa_atribuicao(FILE *p, tokens *token, Stack *tab_simbolo,Lista *fila_aux){
    printf("\n-- Entrou em analisa atribuicao --\n");
    //testar chamada de procedimento

    stack_element var_atribuida = ret_elemento_fila(fila_aux,1);
    eliminar(fila_aux,1);
    
    analisa_expressao(p,token,tab_simbolo,fila_aux);
    //imprimir_fila(fila_aux);
    infix_posfix(fila_aux);
    //imprimir_fila(fila_aux);
    analise_semantica(tab_simbolo,fila_aux,var_atribuida.tipo);


    var_atribuida = consultar_tab(tab_simbolo,var_atribuida.lexema);
    //entendido que é para parte de geração de codigo
    if(strcmp(var_atribuida.tipo,"funcao inteiro")==0||strcmp(var_atribuida.tipo,"funcao booleano")==0){
        fprintf(codigo," ,STR,%d, \n",0);
    }else{
        fprintf(codigo," ,STR,%d, \n",var_atribuida.memoria);
    }
    
}

//----------------------------------------------
//	Funcao analisa chamada de procedimento
//----------------------------------------------

void chamada_procedimento(FILE *p, stack_element var_bkp, Stack *tab_simbolo){
    printf("\n-- Entrou em chamada de procedimento --\n");
    //Se entrou aqui significa que o identificador é um procedimento
    fprintf(codigo," ,CALL,%d, \n",consultar_tab(tab_simbolo,var_bkp.lexema).memoria);
    
}

//----------------------------------------------
//	Funcao analisa chamada funcao
//----------------------------------------------

void analisa_chamada_funcao(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa chamada funcao --\n");
    
    //Perguntar se a chamada da funcao tem que ser antes dos codigos da expressao
    //fprintf(codigo,"\tCALL %d\n",consultar_tab(tab_simbolo,token->lexema).memoria);

    //Nao sei se é aqui mas tem que ser feito o carregamento da variavel caso usado entao funcao tbm deve ser
    //funcao sempre no 0
    //fprintf(codigo,"\tLDV %d\n",0);
    
    *token = lexico(p);
}

//----------------------------------------------
//	Funcao Sintatico
//----------------------------------------------

void main(int argc, char **argv){
    // verificando se foi passado os argumentos
    if(argc != 2){
        printf("Compilou errado, por favor passe o caminho do arquivo\n");
        fprintf(output_arquivo,"Compilou errado, por favor passe o caminho do arquivo\n");
        exit(1);
    }
    FILE *p;
    char str[80];
    tokens token;

    Stack tab_simbolo;
    initStack(&tab_simbolo);

    //----------------------
    // ABRIR ARQUIVO
    //----------------------

    // Le um nome do arquivo a ser aberto
    //printf("\n\n Entre com um nome para o arquivo:\n");
    //gets(str);
    // Abre o arquivo e verifica erro na abertura
    if ((p = fopen(argv[1], "r")) == NULL){
        printf("Erro! Impossivel abrir o arquivo!\n");
        exit(1);
    }

    // Abre para escrever no arquivo
    if((codigo = fopen("gera.obj","w")) == NULL){
        printf("Erro! Impossivel abrir o arquivo da geração de codigo!\n");
        exit(1);
    }

    // Abre para escrever no arquivo
    if((output_arquivo = fopen("output.txt","w")) == NULL){
        printf("Erro! Impossivel abrir o arquivo do output!\n");
        exit(1);
    }

    //----------------------
    // ANALISE SINTATICA
    //----------------------

    end_disponivel = 0;

    //Def rotulo inteiro
    rotulo = 1; //rotulo:=1

    token = lexico(p);
    printf("%s - %s\n", token.lexema, token.simbolo);

    if(token.simbolo == "sprograma")
    {
        token = lexico(p);
        printf("%s - %s\n", token.lexema, token.simbolo);
        if(token.simbolo == "sidentificador")
        {
            //insere_tabela(token.lexema,"nomedeprograma","","")
            insere_tabela(&tab_simbolo, token.lexema,"nomedeprograma", "x", NULL); //se pah coloca "X"
            
            token = lexico(p);
            printf("%s - %s\n", token.lexema, token.simbolo);
            if (token.simbolo == "sponto_virgula")
            {
                //gerar codigo
                //Gera(´ ´,NULL,´ ´,´ ´) 
                fprintf(codigo," ,START, , \n");
                //Alocar para loop
                fprintf(codigo," ,ALLOC,%d,%d\n",end_disponivel,1);
                cont = 1;
                end_disponivel += 1;

                analisa_bloco(p,&token, &tab_simbolo); //analisa bloco atualiza o conteudo de token
                
                if(token.simbolo == "sponto")
                {
                    token = lexico(p);
                    printf("%s - %s\n", token.lexema, token.simbolo);
                    if(token.simbolo == "seof") //se acabou arquivo ou só tem comentario ao fijm
                    {
                        //sucesso
                        printf("SUCESSO\n");
                        fprintf(output_arquivo,"SUCESSO\n");
                    }
                    else
                    {
                        //erro
                        printf("Alem do ponto é permitido apenas comentarios\nLinha:%d\n", linha_atual);
                        fprintf(output_arquivo,"Linha: %d\nAlem do ponto é permitido apenas comentarios.\n",linha_atual);
                        exit(1);
                    }
                }
                else
                {
                    //erro
                    printf("Esperado um . (ponto final) ao fim do programa\nLinha:%d\n", linha_atual);
                    fprintf(output_arquivo,"Linha: %d\nEsperado um . (ponto final) ao fim do programa.\n",linha_atual);
                    exit(1);
                }
            }
            else
            {
                //erro
                printf("Esperado ; (ponto e virgula) ao fim do comando de programa\nLinha:%d\n", linha_atual);
                fprintf(output_arquivo,"Linha: %d\nEsperado ; (ponto e virgula) ao fim do comando de programa.\n",linha_atual);
                exit(1);
            }
        }
        else
        {
            //erro
            printf("Esperado nome do programa\nLinha:%d\n", linha_atual);
            fprintf(output_arquivo,"Linha: %d\nEsperado nome do programa.\n",linha_atual);
            exit(1);
        }
    }
    else
    {
        //erro
        printf("Esperado palavra reservada 'programa'\nLinha:%d\n", linha_atual);
        fprintf(output_arquivo,"Linha: %d\nEsperado palavra reservada 'programa'.\n",linha_atual);
        exit(1);
    }

    desempilha_nivel(&tab_simbolo);
    fprintf(codigo," ,HLT, , \n");
    fclose(p);
}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
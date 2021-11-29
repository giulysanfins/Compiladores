#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "declaracoes.h"

//---------------------------------------------------------------------------------
//									VARIAVEIS GLOBAIS
//---------------------------------------------------------------------------------

// Variavel que aponta a linha atual em analise
int linha_atual = 1;

// Variavel que controla o rotulo disponivel para os comando de mudança de fluxo de execuçao
int rotulo;

// Variavel contadora que controla a quantidade de variaveis criadas em cada sub programa
int cont;

// Variavel que indica qual o eh o endereço disponivel para memoria
int end_disponivel;

// Arquivo que contera o codigo de maquina que sera gerado 
FILE *codigo;

// Arquivo com o outuput para interface, se deu certo ou errado
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

// Funcionalidade: Insere um novo item na tabela de simbolos.              
// Comunicaçao:    Recebe pelos parametros, uma palavra para lexema, para  
//                 tipo, para escopo e um inteiro para memoria, e o endereço
//                 base da tabe_simbolo.  
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo são feitas por referencia.
void insere_tabela(Stack *tab_simbolo, char *lexema, char *tipo, char *escopo, int *memoria)
{
    /* Variavel de mesmo tipo da tabela de simbolo usada para formar o proximo elemento a
    ser inserido.*/
    stack_element element_aux;

    /* A variavel de elemento auxiliar, da tabela de simbolo, recebe as informaçoes dos parametros
    respectivos, lexema, tipo, escopo e memoria, então é inserido na pilha da tabela de simbolos.   
    */
    element_aux.lexema = lexema;
    element_aux.escopo = escopo;
    element_aux.tipo = tipo;
    if(memoria != NULL){
        element_aux.memoria = *memoria;
    }
    push(tab_simbolo, element_aux);
    
}

//----------------------------------------------
//	Funcao auxliar para imprimir a pilha
//----------------------------------------------

// Funcionalidade: Imprimir os conteudos da pilha tabela de simbolos.              
// Comunicaçao:    Recebe o endereço base da tabela de simbolos.
//                 Não devolve, ou altera, qualquer informação.
void imprimirTabela(Stack *lista)
{
    /* Variavel do mesmo tipo da tabela de simbolos, usada para se movimentar
    pelos nós da pilha e não perder o endereço base da tabela de simbolos*/
    Stack p;
    p = *lista; 
    
    /* Enquanto não chegar ao fim da tabela de simbolos eh impresso o conteudo
    de cada nó.
    */
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
//	Funcao de consultar e pegar o token
//----------------------------------------------

//Essa funcao deveria retornar os dados quando encontrado
// Funcionalidade: Retornar o primeiro elemento da tabela de simbolos
//                 em que o lexema desejado eh encontrado.
// Comunicaçao:    Recebe o endereço base da tabela de simbolos e o 
//                 lexema desejado.
//                 Devolve o elemento da tabela de simbolos encontrado.
stack_element consultar_tab(Stack *tab_simbolo, char *lexema)
{
    /* Variavel do mesmo tipo da tabela de simbolos, usada para se movimentar
    pelos nós da pilha e não perder o endereço base da tabela de simbolos*/
    Stack tab_aux;
    tab_aux = *tab_simbolo;

    /* Variavel de mesmo tipo da tabela de simbolo usada para formar o proximo elemento a
    ser inserido.*/
    stack_element element_aux;

    /* Pesquisa na tabela de simbolo o nó que possui o mesmo lexema que o lexema 
    passado pelo parametro, caso encontre esse elemento é retornado, caso nao o 
    fluxo é interrompido, e a mensagem de erro é apresentada.
    */
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
    
    printf("Identificador procurado não consta na tabela de simbolo\n");
    fprintf(output_arquivo,"Linha:%d\nIdentificador procurado não consta na tabela de simbolo.\n",linha_atual);
    exit(1);
}

//----------------------------------------------
//	Funcao para colocar tipo nas variaveis
//----------------------------------------------

// Funcionalidade: Troca o tipo das variaveis para seu tipo definido 
//                 arquivo original.
// Comunicaçao:    Recebe o endereço inicial da tabela de simbolos e  
//                 a string com o tipo da(s) variavel(is).
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo são feitas por referencia.
void colocar_tipo(Stack *tab_simbolo, char *tipo)
{
    // Pilha auxiliar
    Stack tab_aux;
    initStack(&tab_aux);

    /* Variavel de mesmo tipo da tabela de simbolo usada para formar o proximo elemento a
    ser inserido.*/
    stack_element element_aux;

    /* Tira um elemento do topo da pilha original(tabela de simbolo), verifica o tipo 
    desse elemento se for "variavel" ele recebe o tipo passado pelo parametro, entao 
    esse elemento é inserido na pilha auxiliar. Quando o elemento em analise nao for
    mais uma variavel o ultimo elemento retirado é inserido novamente na pilha original
    e um loop inicia retirando os elementos da pilha auxiliar e colocando na original.    
    */
    element_aux = pop(tab_simbolo);

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

// Funcionalidade: Pesquisa na tabela de simbolos se um determinado lexema
//                 de uma variavel ja existe na tabela.
// Comunicaçao:    Recebe o endereço inicial da tabela de simbolos e  
//                 a string com o lexema desejado.
//                 Devolve um inteiro que indica se existe variavel com o mesmo lexema na tabela.
int pesquisa_duplicvar_tabela(Stack *tab_simbolo, char *lexema){
    printf("\n-- Entrou em duplicvar --\n");

    /* Variavel do mesmo tipo da tabela de simbolos, usada para se movimentar
    pelos nós da pilha e não perder o endereço base da tabela de simbolos*/
    Stack tab_aux;
    tab_aux = *tab_simbolo;
    
    /* Verifica na tabela de simbolos se no mesmo nivel de declaração existe uma variavel com o mesmo 
    lexema da variavel em analise, caso encontre retorna 1 indicando que existe duplicata.  
    */
    while(tab_aux.top != NULL && tab_aux.top->info.escopo != "x"){ 
        if((strcmp(tab_aux.top->info.tipo,"variavel") == 0 || strcmp(tab_aux.top->info.tipo,"inteiro") == 0 || strcmp(tab_aux.top->info.tipo,"booleano") == 0) && strcmp(tab_aux.top->info.lexema,lexema) == 0){
            return 1; 
        }
        tab_aux.top = tab_aux.top->link;
    }

    /* Verifica no restante da tabela de simbolos, alem do nivel de declaração anterior, se existe algo
    diferente de uma variavel com o mesmo lexema, se existir retorna 1 indicando que o lexema ja está em uso.
    Caso em nenhuma das verificaçoes seja encontrado o mesmo lexema na tabela de simbolo retorna 0 indicando
    que o lexema analisado nao esta em uso.
    */
    while(tab_aux.top != NULL){
        if(!(strcmp(tab_aux.top->info.tipo,"variavel") == 0 || strcmp(tab_aux.top->info.tipo,"inteiro") == 0 || strcmp(tab_aux.top->info.tipo,"booleano") == 0)){
            if(strcmp(tab_aux.top->info.lexema,lexema) == 0){
                return 1; 
            }
        }
        tab_aux.top = tab_aux.top->link;
    }

    if (tab_aux.top == NULL){
        return 0;   
    }
}

//----------------------------------------------
//	Funcao Analise de func/proc duplicada na Tabela
//----------------------------------------------

// Funcionalidade: Pesquisa na tabela de simbolos se um determinado lexema
//                 de uma funcao ja existe na tabela.
// Comunicaçao:    Recebe o endereço inicial da tabela de simbolos e  
//                 a string com o lexema desejado.
//                 Devolve um inteiro que indica se existe funcao com o mesmo lexema na tabela.
int pesquisa_duplicfunc_tabela(Stack *tab_simbolo, char *lexema){
    printf("\n-- Entrou em duplicfunc --\n");

    /* Variavel do mesmo tipo da tabela de simbolos, usada para se movimentar
    pelos nós da pilha e não perder o endereço base da tabela de simbolos*/
    Stack tab_aux;
    tab_aux = *tab_simbolo;

    /* Verifica em toda tabela de simbolos se o lexema em analise ja foi declarado na tabela, 
    se existir retorna 1 indicando que o lexema ja está em uso.
    Caso nao seja retorna 0 indicando que o lexema analisado nao esta em uso.
    */
    while(tab_aux.top != NULL){
        if(strcmp(tab_aux.top->info.lexema,lexema) == 0){
            return 1;
        }
        tab_aux.top = tab_aux.top->link;  
    }

    if (tab_aux.top == NULL){
        return 0;   
    }
}

//----------------------------------------------
//	Funcao Analise existencia de identificador
//----------------------------------------------

// Funcionalidade: Pesquisa na tabela de simbolos se um determinado lexema
//                 ja existe na tabela e verifica se é compativel com uso.
// Comunicaçao:    Recebe o endereço inicial da tabela de simbolos e  
//                 a string com o lexema desejado.
//                 Devolve um inteiro que indica se o lexema ja existe na tabela.
int pesquisa_existencia(Stack *tab_simbolo,char *lexema){
    printf("\n-- Entrou em pesquisa existencia --\n");

    /* Variavel do mesmo tipo da tabela de simbolos, usada para se movimentar
    pelos nós da pilha e não perder o endereço base da tabela de simbolos*/
    Stack tab_aux;
    tab_aux = *tab_simbolo;

    /* Sempre que um identificador for detectado, verificar se ele foi declarado na tabela de simbolos, 
    e é compatível com o uso. Se o lexema for encontrado e nao for o mesmo do nome de programa retorna 1 indicando 
    que existe na tabelacaso seja encontrado, mas é o mesmo do nome do programa, ou caso nao seja encontrado 
    retorna 0 indicando erro.
    */
    while(tab_aux.top != NULL){
        if(strcmp(tab_aux.top->info.lexema,lexema) == 0){
            if(strcmp(tab_aux.top->info.tipo,"nomedeprograma")==0){
                return 0; 
            }
            return 1; 
        }
        tab_aux.top = tab_aux.top->link;
    }

    if (tab_aux.top == NULL){
        return 0;   
    }

}

//----------------------------------------------
//	Funcao Desempilha
//----------------------------------------------

// Funcionalidade:  Retira os elementos da tabela de simbolo até a primeira marcaçao
//                  de escopo, e escrever o DALLOC no arquivo do codigo de maquina. 
// Comunicaçao:     Recebe o endereço inicial da tabela de simbolo.
//                  Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                  a tabela de simbolo são feitas por referencia.
void desempilha_nivel(Stack *tab_simbolo){
    printf("\n-- Entrou em desempilha --\n");
    /* Altera o indicador do topo da pilha(o ultimo elemento) da tabela de simbolos
    até que o elemento analisado esteja marcado com x no campo do escopo, então retira
    essa marca, e escreve no arquivo de codigo de maquina gerado o DALLOC com o numero 
    de variaveis a serem desalocadas, atraves da variavel global cont.
    */
    while(tab_simbolo->top != NULL && tab_simbolo->top->info.escopo != "x"){
       tab_simbolo->top = tab_simbolo->top->link;
    }
    
    if(tab_simbolo->top->info.escopo == "x"){
        tab_simbolo->top->info.escopo = "";
    }

    if(cont != 0){
        end_disponivel = end_disponivel-cont;
        fprintf(codigo," ,DALLOC,%d,%d\n",end_disponivel,cont);
    }
    
}

//----------------------------------------------
//	Funcao Pesquisa tabela
//----------------------------------------------

// Funcionalidade: Pesquisar na tabela de simbolo se um determinado lexema é uma variavel ou 
//                 funcao de acordo com seu tipo.
// Comunicaçao:    Recebe da funcao analisa fator o endereço inicial da tabela de simbolo 
//                 recebe um lexema e uma variavel para identificação do tipo do elemento 
//                 em analise.
int pesquisa_tabela(Stack *tab_simbolo, char *lexema, int *ind){
    printf("\n-- Entrou em pesquisa tabela --\n");
    /* Variavel do mesmo tipo da tabela de simbolos, usada para se movimentar
    pelos nós da pilha e não perder o endereço base da tabela de simbolos*/
    Stack tab_aux;
    tab_aux = *tab_simbolo;

    /* Pesquisa na tabela de simbolos o lexema passado pelo parametro, caso encontre verifica seu tipo, se for
    uma funcao booleano ou inteira troca o valor da variavel ind para 1 indicando ser uma funcao, senao altera
    o valor da variavel ind para 2 indicando ser uma variavel, entao retorna 1 indicando que foi encontrado; 
    Caso nada seja encontrado na tabela de simbolos retorna 0.   
    */
    while(tab_aux.top != NULL){
        if(strcmp(tab_aux.top->info.lexema,lexema) == 0){
            //se for funcao
            if(strcmp(tab_aux.top->info.tipo,"funcao inteiro") == 0 || strcmp(tab_aux.top->info.tipo,"funcao booleano") == 0){
                *ind=1;
            }else{
                *ind=2;
            }
            return 1;
        }
        tab_aux.top = tab_aux.top->link;
    }
    return 0;
}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------



//---------------------------------------------------------------------------------
//									LEXICAL
//---------------------------------------------------------------------------------

// Funcionalidade: Ler o arquivo do codigo de entrada e retornar o proximo token 
//                 desse arquivo.
// Comunicaçao:    Recebe da funcao main() o endereço atual do arquivo do codigo 
//                 de entrada. Retorna o proximo token formado.
tokens lexico(FILE *p)
{
    // Variavel que contem o caracter em analise do arquivo de entrada.
    char ch;

    // Variavel auxiliar do mesmo tipo da funcao que recebera as informacoes e será retornada para main() 
    tokens token;

    /* Verifica se o caracter em analise eh comentario(o que estiver entre chaves) ou algum tipo de espaço
    e o consome, caso nao seja comentario ou espaço, verifica se chegou ao fim do arquivo, caso nao seja, chama
    a funcao para formar o token e preenche-lo, entao o retorna para a funcao main()  
    */
    ch = fgetc(p);
    while(ch == '{' ||(ch == ' ') || (ch == '\n') || (ch == '\t') || (ch == '\r') ){
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
        pega_token(p, ch, &token);
    }
    else
    {
        preencher_token(&token, "eof", "seof");
    }

    return token;  

}

//----------------------------------------------
//	Funcao de identificar e pegar o token
//----------------------------------------------

// Funcionalidade:  Verifica qual o caracter em analise e chama a funcao
//                  que ira trata-lo.
// Comunicaçao:     Recebe da funcao token() o endereço do arquivo de entrada,
//                  o caracter em analise e o endereço do token que sera formado.
void pega_token(FILE *p, char ch, tokens *token)
{
    /* Verifica se o caracter eh um digito, uma letra, dois pontos, operadores
    aritmeticos ou relacionais, ou se eh alguma pontuaçao, então chama a funcao
    que ira fazer o tratamento especifico em cada caso, e se nao for nenhuma
    das opcao indica erro.
    */
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

// Funcionalidade:  Verifica se o caracter em analise eh um digito.
// Comunicaçao:     Recebe da funcao pega_token() o caracter em 
//                  analise. Retorna um inteiro que indica se eh
//                  um digito ou nao.
int eh_digito(char ch)
{
    /* Verifica o valor na tabela aschii do caracter em analise se eh igual ou esta entre 48 e 57
    (valor da tabela aschii dos digitos [0-9])
    */
    if (ch >= 48 && ch <= 57)
        return 1; 
    else
        return 0; 
}

//----------------------------------------------
//	Funcao verificar se o char eh letra
//----------------------------------------------

// Funcionalidade:  Verifica se o caracter em analise eh uma letra.
// Comunicaçao:     Recebe da funcao pega_token() o caracter em 
//                  analise. Retorna um inteiro que indica se eh
//                  uma letra ou nao.
int eh_letra(char ch)
{
    /* Verifica o valor na tabela aschii do caracter em analise se eh igual
    ou esta entre 65 e 90 ou 97 e 122(valor da tabela aschii das letras [a-z] ou [A-Z]).
    */
    if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122))
        return 1;
    else
        return 0;
}

//----------------------------------------------
//	Funcao de tratamento caso digito
//----------------------------------------------

// Funcionalidade:  Verificar os caracteres subsequente para formar
//                  o numero completo, e preenche o token.
// Comunicaçao:     Recebe da funcao token() o endereço do arquivo de entrada,
//                  o caracter em analise e o endereço do token que sera formado.
//                  Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                  no token são feitas por referencia.
void trata_digito(FILE *p, char ch, tokens *token)
{
    // Variavel que contera a string do numero formado.
    char *num = malloc(sizeof(char));
    *num = ch;
    ch = fgetc(p);

    /* Enquanto o caracter em analise for um digito adiciona na variavel que 
    contem a string do numero formado, quando nao for mais digito o token eh
    preenchido.
    */
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

// Funcionalidade:  Verificar os caracteres subsequente para formar
//                  o numero completo, e preenche o token.
// Comunicaçao:     Recebe da funcao token() o endereço do arquivo de entrada,
//                  o caracter em analise e o endereço do token que sera formado.
//                  Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                  no token são feitas por referencia.
void trata_identPalav(FILE *p, char ch, tokens *token)
{
    // Variavel que contera a string da palavra formada.
    char *id = malloc(sizeof(char));
    *id = ch;
    ch = fgetc(p);

    /* Enquanto o caracter em analise for uma letra o adiciona na variavel que 
    contem a string da palavra formada, quando nao for mais letra  verifica se
    a palavra formada eh uma palavra reservada e entao preenche o token com o 
    simbolo correto.
    */
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

// Funcionalidade:  Verificar o caractere subsequente para tratar
//                  atribuiçao
// Comunicaçao:     Recebe da funcao token() o endereço do arquivo de entrada,
//                  o caracter em analise e o endereço do token que sera formado.
//                  Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                  no token são feitas por referencia.
void trata_atribuicao(FILE *p, char ch, tokens *token)
{
    /* Verifica se o caracter depois dos dois pontos eh um igual, se for preenche o
    token com o simbolo satribuicao, senao sdoispontos.
    */
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

// Funcionalidade:  Verifica o caracter com um operador aritmetico e
//                  preenche o token. 
// Comunicaçao:     Recebe da funcao token() o endereço do arquivo de entrada,
//                  o caracter em analise e o endereço do token que sera formado.
//                  Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                  no token são feitas por referencia.
void trata_opArit(FILE *p, char ch, tokens *token)
{
    /* Se o caracter for um '+' preenche o token com smais, '-' com smenos
    '*' com smult.
    */
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

// Funcionalidade:  Verifica o caracter com um operador relacional e  
//                  preenche o token. 
// Comunicaçao:     Recebe da funcao token() o endereço do arquivo de entrada,
//                  o caracter em analise e o endereço do token que sera formado.
//                  Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                  no token são feitas por referencia.
void trata_opRela(FILE *p, char ch, tokens *token)
{
    /* Caso o carater seja o operador de maior, verifica se o proximo caracter
    eh um igual e entao faz o preenchimento correto, faz o mesmo para o operador 
    de menor e esclamaçao (para o diferente)
    */
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

// Funcionalidade:  Verifica o caracter com uma pontuacao e  
//                  preenche o token.
// Comunicaçao:     Recebe da funcao token() o endereço do arquivo de entrada,
//                  o caracter em analise e o endereço do token que sera formado.
//                  Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                  no token são feitas por referencia.
void trata_pontuacao(FILE *p, char ch, tokens *token)
{
    /* Verifica o caracter com uma pontuacao e preenche o token(lexema e simbolo) de 
    acordo com o caracter, '.' recebe sponto no simbolo, ';' recebe sponto_virgula, 
    ',' recebe svirgula, '(' recebe sabre_parenteses, e ')' recebe sfecha_parenteses.
    */
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

// Funcionalidade:  Inserir nos campos lexema e simbolo da variavel token as 
//                  respectivas informaçoes dos paramentros.
// Comunicaçao:     Recebe o endereço do token, a string do lexema e do simbolo.
//                  Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                  no token são feitas por referencia.
void preencher_token(tokens *token, char lexema[], char simbolo[])
{
    /* Eh alocado para campo lexema da variavel token espaço para receber o lexema
    do parametro, entao os campos lexema e simbolo recebem as informaçoes respectivas.  
    */
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

// Funcionalidade:  Funcao que transforma expressao infixa para
//                  posfixa.
// Comunicaçao:     Recebe uma Lista ligada com a expressao infixa.
//                  Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                  na Lista ligada são feitas por referencia.
void infix_posfix(Lista *fila_aux){
    printf("\n-- Entrou em infix to posfix --\n");

    // Pilha com operadores para auxiliar a realizaçao do posfix
    Stack pilha_aux;
    initStack(&pilha_aux);

    // Lista ligada que tera a expressao posfix
    Lista saida;
    init_fila(&saida);

    /* Variavel do mesmo tipo da Lista ligada, usada para se movimentar
    pelos nós da Lista e não perder seu endereço base*/
    item_fila *x;
    x=fila_aux->inicio;

    /* Variavel de mesmo tipo do item da Lista ligada usada para receber o elemente em
    analise da expressao*/
    stack_element elemento_aux;

    /* Verifica cada elemento da expressao: 
        Caso ele seja um abre parentesses o insere na pilha auxiliar.
        Caso seja uma variavel, um numero, verdadeiro ou falso insere na Lista de saida (da expressao posfix).
        Caso seja um operador insere na pilha auxiliar caso ela esteja vazia, caso nao esteja verifica a 
    precedencia do operador em analise e operador no topo da pilha auxiliar e enquanto a precedencia do 
    elemento da pilha for maior ele eh inserido na Lista de saida, quando deixar de ser maior insere na pilha 
    auxiliar o elemento em analise.
        Caso seja um fecha parentes retira elementos da pilha auxiliar inserindo-os na Lista de saida, ate 
        encontrar um abre parentes.
    */
    while(x != NULL){
       
        if(strcmp(x->info.lexema,"(") == 0){ 
            push(&pilha_aux,x->info);
        }
        else if(strcmp(x->info.tipo,"identificador") == 0 || strcmp(x->info.tipo,"numero") == 0 || strcmp(x->info.lexema,"verdadeiro") == 0 || strcmp(x->info.lexema,"falso") == 0){  
            inserir_fila(&saida,x->info);
        }
        else if(strcmp(x->info.tipo,"operador") == 0){
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

// Funcionalidade:  Analisa a precedencia de um determinado operador.
// Comunicaçao:     Recebe o operador.
//                  Retorna um inteiro que indicara o valor de precedencia.
int precedencia(char* operador){
    /* Caso seja o operador de numero positivo ou negativo retorna 8, caso seja os operadores de
    multiplicacao e divisao retorna 7, caso seja soma ou subtraçao retorna 6, caso seja qualquer 
    operador relacional retorna 5, caso seja de negaçao retorna 4, caso seja de and retorna 3,
    caso seja ou retorna 2, se nao for nenhuma das opcoes retorna 1.
    */
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

// Funcionalidade:  Analisa se o tipo do resultado da expressao corresponde
//                  ao tipo da funcao ou variavel.
// Comunicaçao:     Recebe o endereço base da tabela de simbolo, o endereço
//                  da Lista ligada da expressão analisada, e uma string do
//                  o tipo a ser comparado.
void analise_semantica(Stack *tab_simbolo,Lista *fila_aux,char *cmp_tipo){
    printf("\n-- Entrou em analise semantica --\n");

    /* Variavel do mesmo tipo da Lista ligada, usada para se movimentar
    pelos nós da Lista e não perder seu endereço base*/
    item_fila *x;
    x=fila_aux->inicio;

    // Lista auxiliar com copia da Lista ligada da expressao
    Lista aux;
    init_fila(&aux);

    // Variaveis do mesmo tipo dos elementos da Lista ligada para guardar elementos para comparaçoes dos tipos 
    stack_element anterior_old, anterior_new, aux_elem;

    int i=1;
    int tipo;

    /* Verifica os elementos da Lista da expressao trocando o tipo dos elementos que são numeros e funçoes, para
    simplificar os if's de comparaçao de tipo, e copia os elementos da expressao para a Lista auxiliar.
    */
    while(x != NULL){
        inserir_fila(&aux,x->info);
        if(strcmp(x->info.tipo,"identificador") == 0){
            x->info.tipo = consultar_tab(tab_simbolo,x->info.lexema).tipo;

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
    
    /* Verifica cada elemento da funcao enquanto nao é um operador sao salvos os dois ultimos elementos, quando
    um operador é encontrado de acordo com a operação são analisados o primeira elemento anterior a operacao ou
    os dois elementos anteriores, entao o tipo do(s) elemento(s) é analisado e uma resposta colocando o tipo 
    (inteiro ou booleano) dependendo da operacao pode ser gerada e alterada na Lista Ligada da expressao.
    Entao volta do inicio da lista ligada da expressao procurando o proximo operador, e fazer novamente a analise
    dos tipos dependendo da operação encontrada.
    */
    while(x != NULL){
        if(strcmp(x->info.tipo,"operador") == 0){ 
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
    
    /* Com a expressao analisada resta um ultimo elemento com a resposta e o tipo resultante, entao ele sera analisado
    com o tipo passado pelo parametro (variavel cmp_tipo), caso os tipos nao sejam correspondentes o fluxo de execucao
    eh interrompido, caso esteja correto eh chamada uma funcao para gerar o codigo de maquina desse expressao.
    */
    if(strcmp(cmp_tipo,"funcao inteiro")==0||strcmp(cmp_tipo,"funcao booleano")==0||strcmp(fila_aux->inicio->info.tipo,"funcao inteiro")==0||strcmp(fila_aux->inicio->info.tipo,"funcao booleano")==0){
        if(strcmp(cmp_tipo,"funcao inteiro")==0||strcmp(fila_aux->inicio->info.tipo,"funcao inteiro")==0){
            fila_aux->inicio->info.tipo = "inteiro";
            cmp_tipo = "inteiro";
        }else if(strcmp(cmp_tipo,"funcao booleano")==0||strcmp(fila_aux->inicio->info.tipo,"funcao booleano")==0){
            fila_aux->inicio->info.tipo = "booleano";
            cmp_tipo = "booleano";
        }
    }
    
    if(!strcmp(fila_aux->inicio->info.tipo,cmp_tipo) == 0){
        //erro
        printf("Incompatibilidade de tipo da expressao com o comando desejado. Linha: %d\n",linha_atual);
        fprintf(output_arquivo,"Linha: %d\nIncompatibilidade de tipo da expressao com o comando desejado.",linha_atual);
        
        exit(1);
    }
    printf("Nenhuma Incompatibilidade de tipo encontrada\n");

    gera_cod_expressao(tab_simbolo,&aux);
    
}

//----------------------------------------------
//	Funcao tipo
//----------------------------------------------

// Funcionalidade: Indicar qual a classe do operador em analise, para informar qual fluxo
//                 deve ser seguido de acordo com a operação na funcao analise_semantica().
// Comunicaçao:    Recebe o operador em analise da funcao analise_semantica().
//                 Retorna um inteiro que indica a classe do operador.
int tipo_operador(char* operador){
    /* Verifica qual o operador, caso seja aritmetico retorna 1, caso seja operador unitario (positivo e negativo)
    retorna 2, caso operador relacional retorna 3, caso "ou" ou "e" retorna 4, e caso negação retorna 5, senao erro.
    */
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
//	Funcao gerar codigos da expressao
//----------------------------------------------

// Funcionalidade: Gerar o codigo de maquina da expressao posfixa
// Comunicaçao:    Recebe a tabela de simbolos e a Lista ligada da expressao.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na Lista ligada são feitas por referencia.
void gera_cod_expressao(Stack *tab_simbolo, Lista *aux){
    printf("-- Entrou em gera codigo da expressao --\n");
    /* Variavel do mesmo tipo da Lista ligada, usada para se movimentar
    pelos nós da Lista e não perder seu endereço base*/
    item_fila *x;
    x=aux->inicio;

    // Variaveis do mesmo tipo dos elementos da Lista ligada
    stack_element elem_aux;

    /* Verifica cada elemento da expressao e de acordo com o que ele eh sao gerados os comandos
    em codigo de maquina e adicionado ao arquivo de saida.  
    */
    while(x != NULL){
        if(strcmp(x->info.tipo,"identificador")==0){
            elem_aux = consultar_tab(tab_simbolo,x->info.lexema);

            if(strcmp(elem_aux.tipo,"funcao inteiro")==0|| strcmp(elem_aux.tipo,"funcao booleano") == 0){
                fprintf(codigo," ,CALL,%d, \n",elem_aux.memoria);
                fprintf(codigo," ,LDV,%d, \n",0);
            }else{
                fprintf(codigo," ,LDV,%d, \n",elem_aux.memoria);
            }
        }else if(strcmp(x->info.tipo,"numero")==0){
            fprintf(codigo," ,LDC,%s, \n",x->info.lexema);

        }else if(strcmp(x->info.lexema,"verdadeiro")==0){
            fprintf(codigo," ,LDC,%d, \n",1);

        }else if(strcmp(x->info.lexema,"falso")==0){
            fprintf(codigo," ,LDC,%d, \n",0);

        }else if(strcmp(x->info.tipo,"operador")==0){
            if(strcmp(x->info.lexema,"*") == 0){
                fprintf(codigo," ,MULT, , \n");

            }else if(strcmp(x->info.lexema,"div") == 0){
                fprintf(codigo," ,DIVI, , \n");

            }else if(strcmp(x->info.lexema,"+") == 0){
                fprintf(codigo," ,ADD, , \n");

            }else if(strcmp(x->info.lexema,"-") == 0){
                fprintf(codigo," ,SUB, , \n");

            }else if(strcmp(x->info.lexema,"-u") == 0){ 
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

// Funcionalidade: Chama as funcoes de analise das variaveis, subrotinas e comandos.
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_bloco(FILE *p, tokens *token, Stack *tab_simbolo ){
    printf("\n-- Entrou em analisa bloco --\n");

    /* Pega a proxima palavra e simbolo(token) do arquivo de entrada e chama as funcoes para
    analise das partes declaradas na bloco de comando, variaveis, subrotinas e comandos.
    */
	*token = lexico(p);
	analisa_et_variaveis(p,token, tab_simbolo); //talvez seja necessario salvar o valor de contador ao sair dessa func
	analisa_subrotinas(p,token,tab_simbolo);
	analisa_comandos(p,token,tab_simbolo);
}

//----------------------------------------------
//	Funcao Etapa de declaracao de variaveis
//----------------------------------------------

// Funcionalidade: Analise sintatica da etapa de declaracao de variaveis
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_et_variaveis(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa_et_variaveis --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

    /* Verifica se o token em analiseeh o simbolo de inicio da etapa de declaraçao de variaveis
    entao verifica se o proximo eh um identificador e enquanto for chama a funcao de analise da
    variavel, se depois do ponto e virgula houver outro identificador analisa novamente as proximas
    variaveis.
    */
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
                    *token = lexico(p);

                    if(token->simbolo == "sidentificador"){
                        analisa_variaveis(p, token, tab_simbolo);
                        if(token->simbolo == "sponto_virgula"){
                            *token = lexico(p);
                        }
                    }
                    
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

// Funcionalidade: Analise sintatica da variaveis da etapa de declaracao de variaveis
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_variaveis(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa variaveis --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);
    
    int aux_cont_var = 0, aux_end_disp = end_disponivel;
    //cont = 0;

    /* Verifica se a variavel ja havia sido declarada, se nao for insere essa variavel na tabela de simbolo,
    isso é feito até que seja encontrado o dois pontos, para entao adicionar o tipo da(s) variavel(is), e 
    então gera o codigo de maquina de allocaçao.  
    */
	while(token->simbolo != "sdoispontos"){
		if(token->simbolo == "sidentificador")
		{

            if(!pesquisa_duplicvar_tabela(tab_simbolo, token->lexema))
            {
                aux_cont_var++;
                insere_tabela(tab_simbolo, token->lexema, "variavel","", &end_disponivel); 
                end_disponivel++;

                *token = lexico(p);
                
                if(token->simbolo == "svirgula" || token->simbolo == "sdoispontos")
                {
                    if(token->simbolo == "svirgula")
                    {
                        *token = lexico(p);
                        if(token->simbolo == "sdoispontos") 
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

// Funcionalidade: Verificar se o tipo definido na declaraçao das variaveis eh 
//                 permitido ou nao.
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_tipo(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa tipo --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

    /* Verifica se o token depois dos dois pontos na declaraçao das variaveis
    eh diferente de inteiro ou booleano, senao for entao chama a funcao que
    colocará os tipo correto nas variaveis declaradas.
    */
    if(token->simbolo != "sinteiro" && token->simbolo != "sbooleano")
    {
        //erro
        printf("tipo da(s) variavel(is) incorreto, esperado inteiro ou booleano\n");
        fprintf(output_arquivo,"tipo da(s) variavel(is) incorreto, esperado inteiro ou booleano\n");
        exit(1);
    }
    else
    {
        colocar_tipo(tab_simbolo, token->lexema);
        *token = lexico(p);
    }
}

//----------------------------------------------
//	Funcao Analisa subrotinas <etapa de declaração de sub-rotinas>
//----------------------------------------------
		
// Funcionalidade: Analise sintatica da etapa de declaracao de subrotinas
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_subrotinas(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa_subrotinas --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);
   
    int aux_cont;

    int auxrot, flag;   
    flag = 0;           
    
    /* Verifica se o token em analise eh um identificador de inicio de procedimento 
    ou funcao, caso seja, a funcao de analise sintatica respectiva eh chamada, e quando 
    voltar caso o proximo token seja outro identificador de inicio de procedimento ou
    funcao faz o mesmo processo novamente. Alem disso gera os comandos em codigo de 
    maquina responsaveis por indicar para qual instruçao deve ir para começar o 
    fluxo de execuçao dos comandos daquela determinada funcao, procedimento ou main.
    */
    if((token->simbolo == "sprocedimento") || (token->simbolo == "sfuncao")){   
        auxrot = rotulo;                                                        
        fprintf(codigo," ,JMP,%d, \n",rotulo);                                  
        rotulo = rotulo + 1;                                                    
        flag = 1;                                                               
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
    
    if(flag == 1){                              
        fprintf(codigo,"%d,NULL, , \n",auxrot);    
    }
}

//----------------------------------------------
//	Funcao Analisa declaracao de procedimento
//----------------------------------------------

// Funcionalidade: Analise sintatica da declaracao de procedimentos
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia. 
void analisa_declaracao_procedimento(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa_declaracao_procedimento --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

    *token = lexico(p);
    
    /* Apos identificado inicio da declaracao do procedimento eh verificado o nome dado ao procedimento
    se ja foi declarado anteriormente, se nao foi eh entao inserido na tabela de simbolos, gerado o codigo
    de maquina para saltar para o inicio desse procedimento, então a funcao de analisar bloco com declaracao
    de variaveis, subrotinas e comandos desse procedimento, e depois de fazer a analise total do procedimento
    é retirado da tabela de simbolos todas as declaraçoes feitas dentro do procedimento e gerado o comando de 
    retorno ao fim.
    */
    if(token->simbolo == "sidentificador"){
        if(!pesquisa_duplicfunc_tabela(tab_simbolo,token->lexema)){
            insere_tabela(tab_simbolo,token->lexema,"procedimento","x", &rotulo); 
                      
            fprintf(codigo,"%d,NULL, , \n",rotulo); 
            rotulo = rotulo +1;                     
            
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
    
    desempilha_nivel(tab_simbolo); 
    fprintf(codigo," ,RETURN, , \n");
    
}

//----------------------------------------------
//	Funcao Analisa declaracao de funcao
//----------------------------------------------

// Funcionalidade: Analise sintatica da declaracao de funçoes
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_declaracao_funcao(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa declaracao de funcao --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

	*token = lexico(p);
	
    /* Apos identificado inicio da declaracao de funcao eh verificado o nome dado ao procedimento
    se ja foi declarado anteriormente, se nao foi eh entao inserido na tabela de simbolos, gerado o codigo
    de maquina para saltar para o inicio desse procedimento, entao eh verificado o tipo declarado da funcao 
    e atualizado seu tipo na tabela de simbolos, então a funcao de analisar bloco com declaracao
    de variaveis, subrotinas e comandos desse procedimento, e depois de fazer a analise total do procedimento
    é retirado da tabela de simbolos todas as declaraçoes feitas dentro do procedimento e gerado o comando de 
    retorno ao fim.
    */
	if(token->simbolo == "sidentificador"){
		if(!pesquisa_duplicfunc_tabela(tab_simbolo,token->lexema)){  
            insere_tabela(tab_simbolo,token->lexema,"funcao","x",&rotulo);

            fprintf(codigo,"%d,NULL, , \n",rotulo);            
            rotulo = rotulo +1;                     

			*token = lexico(p);
			if(token->simbolo == "sdoispontos"){
				*token = lexico(p);
				if(token->simbolo == "sinteiro" || token->simbolo == "sbooleano"){
					if(token->simbolo == "sinteiro"){
                        tab_simbolo->top->info.tipo = "funcao inteiro";
					}else{
                        tab_simbolo->top->info.tipo = "funcao booleano";
                    }
					*token = lexico(p);
					if(token->simbolo == "sponto_virgula"){
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
	
    desempilha_nivel(tab_simbolo);
    fprintf(codigo," ,RETURN, , \n");
    
}

//----------------------------------------------
//	Funcao Analisa comandos
//----------------------------------------------

// Funcionalidade: Analise sintatica dos comandos 
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_comandos(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa comandos --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);
    
    /* Verifica se o token eh o identificador de inicio do bloco de comandos, chama a funcao que verificara
    qual(is) eh(sao) o(s) comando(s) escrito(s), enquanto nao eh o identificador de fim do bloco de comandos.
    */
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

// Funcionalidade: Verifica qual comando deve ser tratato e chama sua funcao 
//                 respectiva.
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_comando_simples(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa comando simples --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

    Lista fila_aux;
    init_fila(&fila_aux);

    /* Se o token do comando em analise for um identificador chama a analise de atribuicao 
    e chamada de procedimentos, se for um se chama analise de comando se, enquanto chama a
    analise do enquanto, de igual forma para leia e o escreva, e caso nao seja nenhuma das
    opcoes chama a analise de comandos.
    */
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

// Funcionalidade: Analise sintatica da atribuicao ou chamada de procedimento
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_atrib_chprocedimento(FILE *p, tokens *token, Stack *tab_simbolo, Lista *fila_aux){
    printf("\n-- Entrou em atribuicao chamada de procedimento --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

    /* Verifica se o token em analise, variavel ou nome de procedimento, ja foi declarado para poder
    ser utilizado, depois verifica se o proximo token eh o simbolo de atribuiçao, se sim chama a 
    funcao para analise de atribuiçoes, senao chama funcao para analise de chamada de procedimentos. 
    */
    stack_element var_bkp;
    var_bkp = consultar_tab(tab_simbolo,token->lexema); 

	*token = lexico(p);
    printf("Token simbolo: %s Token lexico: %s\n",token->simbolo,token->lexema); 

	if(token->simbolo == "satribuicao"){ 
        inserir_fila(fila_aux,var_bkp); 

        *token = lexico(p);
		analisa_atribuicao(p,token,tab_simbolo,fila_aux); 
	}else{
		chamada_procedimento(p, var_bkp, tab_simbolo); 
	}
}

//----------------------------------------------
//	Funcao Analisa Leia
//----------------------------------------------

// Funcionalidade: Analise sintatica do comando de leitura(entrada de dados pelo usuario)
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_leia(FILE *p, tokens *token, Stack *tab_simbolo){ 
    printf("\n-- Entrou em analisa leia --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

    stack_element aux_var;

    /* Gera o comando na linguagem de maquina para leitura de dado do usuario, verifica
    se ha abertura de partesses e se ha uma variavel, se ela foi declarada e se eh do 
    tipo inteiro, e entao gera o codigo de maquina store, para guardar valor na variavel. 
    */
    fprintf(codigo," ,RD, , \n");

	*token = lexico(p);
	if(token->simbolo == "sabre_parenteses"){
		*token = lexico(p);
		if(token->simbolo == "sidentificador"){
            if(pesquisa_existencia(tab_simbolo,token->lexema)){ 
                aux_var = consultar_tab(tab_simbolo,token->lexema);
				*token = lexico(p);
				if(token->simbolo == "sfecha_parenteses"){
                    if(strcmp(aux_var.tipo,"inteiro")==0){
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

// Funcionalidade: Analise sintatica do comando de escrita(saida de dados)                
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_escreva(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa escreva --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

    stack_element var_aux;

    /* Apos identificar o comando escreva verifica se ha abertura de parentesses e se ha 
    uma variavel, se ela foi declarada e se eh do tipo inteiro e entao gera o codigo de 
    maquina para ler o dado da memoria e de apresentar na tela.
    */
	*token = lexico(p);
	if(token->simbolo == "sabre_parenteses"){
		*token = lexico(p);
		if(token->simbolo == "sidentificador"){
            if(pesquisa_existencia(tab_simbolo,token->lexema)){
                var_aux = consultar_tab(tab_simbolo,token->lexema);

				*token = lexico(p);
				if(token->simbolo == "sfecha_parenteses"){
                    if(strcmp(var_aux.tipo,"inteiro")==0){
                        fprintf(codigo," ,LDV,%d, \n",var_aux.memoria);
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

// Funcionalidade: Analise sintatica do comando de repetiçao enquanto
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_enquanto(FILE *p, tokens *token, Stack *tab_simbolo, Lista *fila_aux){
    printf("\n-- Entrou em analisa enquanto --\n");
    printf("Token simbolo: %s Token lexico: %s\nLinha:%d\n",token->simbolo,token->lexema,linha_atual);

	/* gera o codigo de maquina que indica o começo do loop, chama a funcao que analisa e forma a 
    expressao da condiçao para a repeticao, entao transforma a expressao de infixa para posfixa
    e entao chama a funcao para analisar semanticamente a expressao, entao se ha o identificador de
    inicio do bloco de comando de repetiçao, entao gerar o comando na linguagem de maquina de 
    jump para quando a expressao for falsa, analisa os comandos de dentro do enquanto, e por fim
    gera os codigos de maquina do jump que volta ao começo do loop e que indica a saida do loop.
    */
    int auxrot1,auxrot2;
    auxrot1 = rotulo;
    fprintf(codigo,"%d,NULL, , \n",rotulo);
    rotulo = rotulo+1;

	*token = lexico(p);
	analisa_expressao(p,token,tab_simbolo,fila_aux);
    infix_posfix(fila_aux);
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

// Funcionalidade: Analisa o sintatico do comando condicional(comando se)
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_se(FILE *p, tokens *token, Stack *tab_simbolo, Lista *fila_aux){
    printf("\n-- Entrou em analisa se --\n");
    printf("Token simbolo: %s Token lexico: %s Linha:%d\n",token->simbolo,token->lexema,linha_atual);

    int auxrot,auxrot2,flag;

	*token = lexico(p);
    
    /* Chama a funcao que analisa e forma a expressao da condiçao, entao transforma a expressao de 
    infixa para posfixa entao chama a funcao para analisar semanticamente a expressao, em seguida 
    se ha o identificador de inicio do bloco de comando da condicional, eh gerado o comando na 
    linguagem de maquina de jump para quando a expressao for falsa, analisa os comandos desse
    bloco, e faz o mesmo para o bloco de comando do senao caso haja.
    */
	analisa_expressao(p,token,tab_simbolo,fila_aux);        
    infix_posfix(fila_aux);                                 
    analise_semantica(tab_simbolo, fila_aux,"booleano");    

	if(token->simbolo == "sentao"){
        auxrot = rotulo;
        fprintf(codigo," ,JMPF,%d, \n",rotulo);
        rotulo = rotulo + 1;

		*token = lexico(p);
		analisa_comando_simples(p,token,tab_simbolo);

		if(token->simbolo == "ssenao"){
            flag = 1;
            auxrot2 = rotulo;
            fprintf(codigo," ,JMP,%d, \n",rotulo);
            rotulo = rotulo + 1;

            fprintf(codigo,"%d,NULL, , \n",auxrot);

			*token = lexico(p);
			analisa_comando_simples(p,token,tab_simbolo);
		}
        else{
            fprintf(codigo,"%d,NULL, , \n",auxrot);
        }
	}else{
		//erro
		printf("Necessario indicar enquanto do comando condicional. Linha:%d\n", linha_atual);
        fprintf(output_arquivo,"Linha: %d\nNecessario indicar enquanto do comando condicional.\n",linha_atual);
		exit(1);
	}
    if(flag == 1){
        fprintf(codigo,"%d,NULL, , \n",auxrot2);
    }
    
}

//----------------------------------------------
//	Funcao Analisa expressão <expressão>
//----------------------------------------------

// Funcionalidade: Analise sintatica das expressoes e controle de precedencia dos elementos da expressao
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_expressao(FILE *p, tokens *token, Stack *tab_simbolo, Lista *fila_aux){
    printf("\n-- Entrou em analisa expressao --\n");
    printf("Token simbolo: %s Token lexico: %s Linha:%d\n",token->simbolo,token->lexema,linha_atual);

    /* Chama a analise de expresao simples onde fara outra verificaçao no token, e entao quando voltar das
    chamadas, verifica se o token eh um operador de maior, menor, menor igual, maior igual, igual e caso 
    seja analisa a expressao simples. A medida que sao identificados os elementos da expressao eles sao 
    inseridos em uma Lista auxiliar que contem apenas a expressao.
    */
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

// Funcionalidade: Verificar se ha um sinal de negativo ou positivo, soma, subtraçao e "ou"
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_expressao_simples(FILE *p, tokens *token, Stack *tab_simbolo, Lista *fila_aux){
    printf("\n-- Entrou em analisa expressao simples --\n");
    printf("Token simbolo: %s Token lexico: %s Linha:%d\n",token->simbolo,token->lexema,linha_atual);

    /* Verifica se o token eh um operador de positivo ou negativo, entao chama analise de termo, e 
    depois de analisar o elemento ve se o proximo elemento eh uma operacao de soma, subtraçao, ou,
    caso seja eh chamado a funcao para analisar o proximo termo da expressao. A medida que sao
    identificados os elementos da expressao eles sao inseridos em uma Lista auxiliar que contem 
    apenas a expressao.
    */
    stack_element auxiliar;
	if(token->simbolo == "smais" || token->simbolo == "smenos"){
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

// Funcionalidade: Verifica se eh um operador de multiplicacao, divisao ou "e"
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_termo(FILE *p, tokens *token, Stack *tab_simbolo,Lista *fila_aux){
    printf("\n-- Entrou em analisa termo --\n");
    printf("Token simbolo: %s Token lexico: %s Linha:%d\n",token->simbolo,token->lexema,linha_atual);

    /* Chama funcao de analise do fator entao verifica se o token eh um operador de 
    multiplicacao, divisao ou "e", caso seja chama a funcao de analisa fator. A medida que sao
    identificados os elementos da expressao eles sao inseridos em uma Lista auxiliar que contem 
    apenas a expressao.
    */
	analisa_fator(p,token,tab_simbolo,fila_aux);
    
    stack_element auxiliar;
    
	while(token->simbolo == "smult" || token->simbolo == "sdiv" || token->simbolo == "se"){
        auxiliar.lexema = token->lexema;
        auxiliar.tipo = "operador";
        inserir_fila(fila_aux,auxiliar);

		*token = lexico(p);
        analisa_fator(p,token,tab_simbolo,fila_aux);
	}
}

//----------------------------------------------
//	Funcao Analisa fator
//----------------------------------------------

// Funcionalidade: Identifica qual o fator da expressao e faz seu tratamento na expressao.
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_fator(FILE *p, tokens *token, Stack *tab_simbolo, Lista *fila_aux){
    printf("\n-- Entrou em analisa fator --\n");
    printf("(Token) Simbolo: %s\t(Token) Lexico: %s - Linha:%d\n",token->simbolo,token->lexema,linha_atual);

    stack_element auxiliar;
    
    int ind=0;

    /* Verifica se o fator eh um identificador(uma variavel ou funcao),caso seja,
    verifica se ja foi declarada, se o fator eh um numero, uma negaçao,
    um abre parentesses ou um booleano. Entao esse fator eh inseridos na
    Lista auxiliar que contem a expressao.
    */
	if(token->simbolo == "sidentificador"){	
        if(pesquisa_tabela(tab_simbolo,token->lexema, &ind)){ 
            if(ind == 1){ 
                auxiliar.lexema = token->lexema;
                auxiliar.tipo = "identificador";
                inserir_fila(fila_aux,auxiliar);

                analisa_chamada_funcao(p,token,tab_simbolo);
            }else if(ind == 2){ 
                auxiliar.lexema = token->lexema;
                auxiliar.tipo = "identificador";
                inserir_fila(fila_aux,auxiliar);

                *token = lexico(p);
            }
		}else{
            //Senão ERRO
            printf("Variavel ou funcao nao declaradas. Linha: %d\n",linha_atual);
            fprintf(output_arquivo,"Linha: %d\nVariavel ou funcao nao declaradas.\n",linha_atual);
            exit(1);
        }
	}else if(token->simbolo == "snumero"){	
        auxiliar.lexema = token->lexema;
        auxiliar.tipo = "numero";
        inserir_fila(fila_aux,auxiliar);

		*token = lexico(p);
		
        }else if(token->simbolo == "snao"){	
            auxiliar.lexema = token->lexema;
            auxiliar.tipo = "operador";
            inserir_fila(fila_aux,auxiliar);

            *token = lexico(p);
            analisa_fator(p,token,tab_simbolo,fila_aux);
            
            }else if(token->simbolo == "sabre_parenteses"){	
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

// Funcionalidade: Analise sintatica do comando de atribuiçao.
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_atribuicao(FILE *p, tokens *token, Stack *tab_simbolo,Lista *fila_aux){
    printf("\n-- Entrou em analisa atribuicao --\n");
    
    /* Quando identificado um comando de atribuição e feita a analise da expressao
    que sera atribuida, se a expressao esta correta em formato e tipo, e entao
    gera o codigo de maquina STR(store) que indicara em qual endereço da memoria
    tal valor devera ser guardado. 
    */
    stack_element var_atribuida = ret_elemento_fila(fila_aux,1);
    eliminar(fila_aux,1);
    
    analisa_expressao(p,token,tab_simbolo,fila_aux);
    infix_posfix(fila_aux);
    analise_semantica(tab_simbolo,fila_aux,var_atribuida.tipo);

    var_atribuida = consultar_tab(tab_simbolo,var_atribuida.lexema);
    if(strcmp(var_atribuida.tipo,"funcao inteiro")==0||strcmp(var_atribuida.tipo,"funcao booleano")==0){
        fprintf(codigo," ,STR,%d, \n",0);
    }else{
        fprintf(codigo," ,STR,%d, \n",var_atribuida.memoria);
    }
    
}

//----------------------------------------------
//	Funcao analisa chamada de procedimento
//----------------------------------------------

// Funcionalidade: Gerar o codigo de maquina da chamada
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void chamada_procedimento(FILE *p, stack_element var_bkp, Stack *tab_simbolo){
    printf("\n-- Entrou em chamada de procedimento --\n");
    fprintf(codigo," ,CALL,%d, \n",consultar_tab(tab_simbolo,var_bkp.lexema).memoria);
}

//----------------------------------------------
//	Funcao analisa chamada funcao
//----------------------------------------------

// Funcionalidade: Pegar o proximo token quando na chamada de funcao
// Comunicaçao:    Recebe o endereço base do arquivo de entrada, o endereço do token, e o 
//                 endereço base da tabela de simbolos.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 na tabela de simbolo, token e arquivo são feitas por referencia.
void analisa_chamada_funcao(FILE *p, tokens *token, Stack *tab_simbolo){
    printf("\n-- Entrou em analisa chamada funcao --\n");
    *token = lexico(p);
}

//----------------------------------------------
//	Funcao Sintatico
//----------------------------------------------

// Funcionalidade: Analise sintatica de inicio de programa, e chama funcao 
//                 que analisa bloco de programa(declaracao de variaveis, procedimentos e comandos) 
// Comunicaçao:    Recebe da parte grafica o arquivo que deseja compilar 
//                 Não retorna nada diretamente.
void main(int argc, char **argv){
    // Verificando se foi passado os argumentos na inicialização
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

    // Abre o arquivo e verifica erro na abertura
    if ((p = fopen(argv[1], "r")) == NULL){
        printf("Erro! Impossivel abrir o arquivo!\n");
        exit(1);
    }

    if((codigo = fopen("gera.obj","w")) == NULL){
        printf("Erro! Impossivel abrir o arquivo da geração de codigo!\n");
        exit(1);
    }

    if((output_arquivo = fopen("output.txt","w")) == NULL){
        printf("Erro! Impossivel abrir o arquivo do output!\n");
        exit(1);
    }

    end_disponivel = 0;

    rotulo = 1; 

    token = lexico(p);
    printf("%s - %s\n", token.lexema, token.simbolo);

    /* Verifica se a primeira palavra do arquivo é o identificador de programa
    e se foi definido um nome a ele, entao o insere na tabela de simbolos, gera os 
    codigos de maquina responsaveis por indicar inicio e allocar o primeiro endereço
    da memoria, entao faz a chamada para a analise do bloco do programa, quando toda 
    a analise estiver sido feita, verifica se ha o identificador de fim de programa,
    desempilha toda a tabela de simbolos restante, e encerra o programa.  
    */
    if(token.simbolo == "sprograma")
    {
        token = lexico(p);
        printf("%s - %s\n", token.lexema, token.simbolo);
        if(token.simbolo == "sidentificador")
        {
            insere_tabela(&tab_simbolo, token.lexema,"nomedeprograma", "x", NULL); //se pah coloca "X"
            
            token = lexico(p);
            printf("%s - %s\n", token.lexema, token.simbolo);
            if (token.simbolo == "sponto_virgula")
            {
                fprintf(codigo," ,START, , \n");
                fprintf(codigo," ,ALLOC,%d,%d\n",end_disponivel,1);
                cont = 1;
                end_disponivel += 1;

                analisa_bloco(p,&token, &tab_simbolo); 
                
                if(token.simbolo == "sponto")
                {
                    token = lexico(p);
                    printf("%s - %s\n", token.lexema, token.simbolo);
                    if(token.simbolo == "seof") 
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
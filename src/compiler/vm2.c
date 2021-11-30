#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/* Server port  */
#define PORT 5000


/* Buffer length */
#define BUFFER_LENGTH 4096


struct stack_element{
    int linha;
    char* rot_command;
    char* command;
    char* arg1_command;
    char* arg2_command;
};
typedef struct stack_element stack_element;
typedef struct item_fila item_fila;
typedef struct Lista Lista;
#include "fila.h"

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//									DECLARACOES
//---------------------------------------------------------------------------------

void imprime_fila(Lista *);
char* pega_palavra(FILE *, char *);
stack_element ret_elem_pos(Lista*, int);
stack_element ret_elem_rot(Lista*, char*);
void forma_area_prog(FILE*,Lista*);
stack_element prox_comando(Lista*, int, int*, int);
int maquina_virtual();

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------



//---------------------------------------------------------------------------------
//									VARIAVEIS GLOBAIS
//---------------------------------------------------------------------------------

// Variavel que indica qual a linha do proximo comando
int i; 

// Variavel que indica qual opcao de execucao foi escolhida
int flag;

// Variavel do input do usuario na parte grafica
int var_read;

// Arquivo que guarda o estado atual da memoria depois de cada comando
FILE *area_mem;

// Arquivo que guarda as linhas dos comandos e indica seus respectivos numeros
FILE *area_cod;

// Variavel da memoria e apontador do topo da memoria
int *mem, s=0;

Lista comandos;
char response2[] = "200,";
//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------



//---------------------------------------------------------------------------------
//									FUNCOES
//---------------------------------------------------------------------------------

//----------------------------------------------
//	Funcao formar area programa
//----------------------------------------------

// Funcionalidade: Ler o arquivo objeto de entrada, e montar a lista ligada cujos 
//                 nós conterão o numero da linha e o comando respectivel.
// Comunicaçao:    Recebe da main() o endereço inicial do arquivo objeto, gerado 
//                 na compilacao, e o endereço inicial da lista ligada.
//                 Nao devolve diretamente nenhuma informaçao, as modificaçoes
//                 ao arquivo objeto e lista ligada são feitos por referencia. 
void forma_area_prog(FILE* p, Lista* comandos){
    //printf("        entrou em forma area de programa\n");
    // Variavel contadora que indica qual o numero da linha do comando
    int linha_cont = 0;

    // Variavel contadora que indica em qual campo da variavel <element_aux> 
    // a palavra encontrada deve ser guardada
    int cont_aux = 0;

    // Variavel que contem qual caracter do arquivo objeto esta sendo analisado
    char ch;

    // Variavel que recebera a proxima palavra do comando
    char *vet_aux;

    // Variavel de mesmo tipo da Lista ligada usada para formar o proximo elemente a
    // ser inserido na lista ligada que contem os comandos.
    stack_element element_aux;

    ch = fgetc(p);

    /* Enquanto nao for o fim do arquivo objeto, analisa-se a linha atual
    e enquanto nao chegar ao fim da linha busca a proxima palavra e vai montando
    o proximo elemento lista ligada dos comandos, quando chegar ao fim da linha
    esse novo elemento eh inserido na lista dos comandos.
    */ 
    while(ch != EOF){
        element_aux.linha = linha_cont;
        while(ch != '\n'){
            if(ch == ','){
                //printf("    montando element aux [%d]\n",cont_aux);
                if(cont_aux == 1){
                    element_aux.rot_command = vet_aux;
    
                }else if(cont_aux == 2){
                    element_aux.command = vet_aux;

                }else if(cont_aux == 3){
                    element_aux.arg1_command = vet_aux;

                }
                ch = fgetc(p);
            }else{
                vet_aux = pega_palavra(p,&ch);
                //printf("    saiu de pega palavra\n");
                cont_aux++;
            }
        }
        element_aux.arg2_command = vet_aux;
        //printf("    inserindo item na fila");
        inserir_fila(comandos,element_aux);
        //printf("    inserido com sucesso");
        ch = fgetc(p);
        cont_aux = 0;
        linha_cont++;
    }
    imprime_fila(comandos);
    
}

//----------------------------------------------
//	Funcao retorna elemento pela linha
//----------------------------------------------

// Funcionalidade: Buscar e retornar o comando de uma determinada linha contido na  
//                 lista ligada dos comandos.
// Comunicaçao:    Recebe da main() ou prox_comando() o endereço inicial da lista  
//                 ligada dos comandos e o numero da linha desejada.
//                 Devolve para a main() ou prox_comando() o comando encontrado. 
stack_element ret_elem_pos(Lista* comando, int linha){
    //printf("    entrou em retorna elemento\n");
    // Variavel para se movimentar pela Lista ligada sem perder o endereço de inicio
    item_fila *x;
    x=comando->inicio;
    
    /* Enquanto nao chegar ao final da lista ligada e a linha do comando em analise
    nao for a linha desejada ele vai ao proximo, e quando encontrar retorna o comando.
    */
    while(x!=NULL && x->info.linha != linha){ 
        x = x->prox;
    }
    return x->info;
}

//----------------------------------------------
//	Funcao retorna elemento pelo rotulo
//----------------------------------------------

// Funcionalidade: Buscar e retornar o comando que possui um determinado rotulo contido  
//                 na lista ligada dos comandos.
// Comunicaçao:    Recebe da main() o endereço inicial da lista ligada dos comandos 
//                 e o rotulo desejado.
//                 Devolve para a main() o comando encontrado. 
stack_element ret_elem_rot(Lista* comando, char* rotulo){
    // Variavel para se movimentar pela Lista ligada sem perder o endereço de inicio
    item_fila *x;
    x=comando->inicio;

    /* Enquanto nao chegar ao final da lista ligada e a linha do comando em analise
    nao for o rotulo desejado ele vai ao proximo, e quando encontrar retorna o comando.
    */
    while(x!=NULL && strcmp(x->info.rot_command,rotulo) != 0){
        x = x->prox;
    }
    return x->info;
}

//----------------------------------------------
//	Funcao imprimir a fila dos comandos
//----------------------------------------------

// Funcionalidade: Imprimir no terminal e escrever no arquivo da area de codigo os   
//                 comandos contidos na lista ligada dos comandos.
// Comunicaçao:    Recebe da forma_area_prog() o endereço inicial da lista ligada dos comandos. 
//                 Retorna a palavra encontrada.
void imprime_fila(Lista *comando){
    //printf("        entrou em imprimir fila\n");

    // Abre ou cria o arquivo da area de codigo no mdo de escrita.
    if((area_cod = fopen("area_cod.txt","w")) == NULL){
        printf("Erro! Impossivel abrir o arquivo!\n");
        exit(1);
    }

    // Variavel para se movimentar pela Lista ligada sem perder o endereço de inicio
    item_fila *x;
    x=comando->inicio;
    
    // Variavel contadora que indica qual o numero da linha do comando
    int linha_cont = 0;
    
    /* Imprime e grava no arquivo da area de codigo todos os comandos e o numero de suas respectivas linhas.
    Então o arquivo de texto da area de codigo eh fechado. 
    */
    while(x!=NULL){
        printf("[%d]Linha  : [%d]\n",linha_cont,x->info.linha);
        printf("[%d]Rotulo : [%s]\n",linha_cont,x->info.rot_command);
        printf("[%d]Comando: [%s]\n",linha_cont,x->info.command);
        printf("[%d]Arg_1  : [%s]\n",linha_cont,x->info.arg1_command);
        printf("[%d]Arg_2  : [%s]\n\n",linha_cont,x->info.arg2_command);
        fprintf(area_cod,"%d,%s,%s,%s,%s\n",x->info.linha,x->info.rot_command,x->info.command,x->info.arg1_command,x->info.arg2_command);
        x=x->prox;
        linha_cont++;
    }

    fclose(area_cod);
}

//----------------------------------------------
//	Funcao pega a proxima palavra em uma linha
//----------------------------------------------

// Funcionalidade: Pegar a proxima palavra contida antes de uma virgula dentro de uma    
//                 linha do arquivo gerado pelo compilador.
// Comunicaçao:    Recebe da main() o endereço atual do arquivo gerado pelo compilador 
//                 e o endereço da variavel que contem o caracter em analise.
char* pega_palavra(FILE*p, char *ch){
    //printf("        entrou em pega palavra\n");
    // Variavel para guardar o tamanho atual da palavra que está sendo lida. 
    size_t tam;

    // Vetor que conterá a palavra lida
    char *vet_aux = malloc(sizeof(char));;
    
    /* pega o proximo caractere do arquivo original gerado pelo compilador e
    o adiciona a palavra até que o caractere seja uma virgula ou indicador de
    quebra de linha, e entao retorna a palavra formada.
    */
    while(*ch != ',' && *ch!='\n'){
        tam = strlen(vet_aux);
        vet_aux = realloc(vet_aux, (tam + 1 + 1));
        vet_aux[tam] = *ch;
        vet_aux[tam + 1] = '\0';
        *ch = fgetc(p);
    }
    return vet_aux;
}

//----------------------------------------------
//	Funcao imprimir os dados da memoria
//----------------------------------------------

// Funcionalidade: Imprimir no terminal e escrever no arquivo da area de memoria os   
//                 valores dos endereços e conteudos da memoria criada na maquina virtual.
// Comunicaçao:    Recebe da prox_comando() o endereço base da memoria e o até qual posicao  
//                 da memoria ela deve ir.
void printa_memoria(int* mem, int s){
    // Abre ou cria o arquivo da area de codigo no mdo de escrita.
    if((area_mem = fopen("area_mem.txt","w")) == NULL){
        printf("Erro! Impossivel abrir o arquivo!\n");
        exit(1);
    }

    /* Imprime e grava no arquivo da area de memoria, cada endereço relativo da memoria
    e seu conteudo. Então o arquivo de texto da area de memoria eh fechado. 
    */
    if(mem == NULL){
        printf("    Memoria Vazia\n");
    }else{
        for(int cont_aux =0; cont_aux<=s ;cont_aux++){
        printf("    mem[%d]: %d\n",cont_aux,*(mem + cont_aux));
        fprintf(area_mem,"%d,%d\n",cont_aux,*(mem + cont_aux));
        } 
    }  

    fclose(area_mem);
    printf("\n");
}

//----------------------------------------------
//	Funcao pega proximo comando
//----------------------------------------------

// Funcionalidade: Controla a opçao de execuçao e chama as funcoes de retorno 
//                 do comando da linha desejada e da funcao que atualiza o 
//                 arquivo da memoria.
// Comunicaçao:    Recebe da main() o endereço base da lista ligada de comandos,
//                 a linha do comando desejado, o endereço base da memoria de   
//                 dadose o até qual posicao da memoria ela deve ir.
//                 Retorna para main() o comando a ser executado.
stack_element prox_comando(Lista* comandos, int i, int* mem, int s){
    // Variavel dummy para fazer a execução parar até que seja carregado aolgo a variavel
    char controle;

    /* Eh chamada a funcao <printa_memoria> para atualização do arquivo da area da memoria
    caso tenha sido escolhido a opçao de execucao passo a passo, a execucao é interrompida
    ate que algo seja atribuida a variavel, e então a funcao <ret_elem_pos> é chamada para 
    pegar o proximo comando que deve ser executado.
    */
    printa_memoria(mem,s);
   /* if(flag == 1){ 
        gets(&controle);
        fflush(stdin);
    }*/
    return ret_elem_pos(comandos,i);
}

//----------------------------------------------
//	Funcao VM
//----------------------------------------------

// Funcionalidade: Verifica qual o comando do codigo de maquina, e realiza
//                 os comandos, modificando caso necessario memoria e fluxo 
//                 de execução 
// Comunicaçao:    Chamado pela main(), mas não recebe nenhum parametro
//                 as variaveis de memoria, apontador de topo e arquivos são
//                 globais.
//                 Não há retorno na funçao, as modificaçoes necessaria sao 
//                 feitas nas variaveis globais.
int maquina_virtual(){

    /* if(argc == 2){
        //Setar as variaveis primeira vez
    }*/

    stack_element comando_atual;
    int aux;
    
    /* Pega o priximo comando, verifica qual o comando e o executa, depois
    atualiza os arquivos, que indica a linha do comando, que apresenta os 
    dados da memoria e a saida de dados, para comunicação com a parte grafica.
    */
    comando_atual = prox_comando(&comandos,i,mem,s);

    if(strcmp(comando_atual.command,"START") == 0){
        s=-1;
        i++;   
    }else if(strcmp(comando_atual.command,"LDC") == 0){           
        //LDC k
        printf("LDC\n");
        s = s+1;    //incrimenta o apontador da memoria
        mem = (int *) realloc(mem,((s+1)*sizeof(int))); //realloca a memoria para receber um novo item
        *(mem + s) = atoi(comando_atual.arg1_command);    //pega o argumento k do comando transforma em inteiro e salva na mem
        i++;

    }else if(strcmp(comando_atual.command,"LDV") == 0){     //S:=s+1 ; M[s]:=M[n]
        //LDV n
        printf("LDV\n");
        s = s+1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int)));
        aux = atoi(comando_atual.arg1_command);
        *(mem + s) = *(mem + aux);
        i++;

    }else if(strcmp(comando_atual.command,"ADD") == 0){     //M[s-1]:=M[s-1]+M[s]; s:=s-1
        printf("ADD\n");
        *(mem + (s-1)) = *(mem + (s-1)) + *(mem + s);
        s = s-1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int))); 
        i++;

    }else if(strcmp(comando_atual.command,"SUB") == 0){     //M[s-1]:=M[s-1]-M[s]; s:=s-1
        printf("SUB\n");
        *(mem + (s-1)) = *(mem + (s-1)) - *(mem + s);
        s = s-1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int)));
        i++;

    }else if(strcmp(comando_atual.command,"MULT") == 0){    //M[s-1]:=M[s-1]*M[s]; s:=s-1
        printf("MULT\n");
        *(mem + (s-1)) = *(mem + (s-1)) * *(mem + s);
        s = s-1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int)));
        i++;

    }else if(strcmp(comando_atual.command,"DIVI") == 0){    //M[s-1]:=M[s-1]div M[s]; s:=s-1
        printf("DIVI\n");
        *(mem + (s-1)) = *(mem + (s-1)) / *(mem + s);
        s = s-1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int)));
        i++;

    }else if(strcmp(comando_atual.command,"INV") == 0){     //M[s]:=-M[s]
        printf("INV\n");
        *(mem + s) = -(*(mem + s));
        i++;

    }else if(strcmp(comando_atual.command,"AND") == 0){     //Se M [s-1]=1 e M[s]=1 então M[s-1]:=1 senão M[s-1]:=0; S:=s-1
        printf("AND\n");
        if(*(mem + (s-1)) && *(mem + s)){
            *(mem + (s-1)) = 1;
        }else{
            *(mem + (s-1)) = 0;
        }
        s = s-1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int)));
        i++;

    }else if(strcmp(comando_atual.command,"OR") == 0){      //Se M[s-1]=1 ou M[s]=1 então M[s-1]:=1 senão M[s-1]:=0; s:=s-1
        printf("OR\n");
        if(*(mem + (s-1)) || *(mem + s)){
            *(mem + (s-1)) = 1;
        }else{
            *(mem + (s-1)) = 0;
        }
        s = s-1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int)));
        i++;

    }else if(strcmp(comando_atual.command,"NEG") == 0){     //M[s]:=1-M[s]
        printf("NEG\n");
        *(mem + s) = 1-(*(mem + s));
        i++;

    }else if(strcmp(comando_atual.command,"CME") == 0){     //Se M[s-1]<M[s] então M[s-1]:=1 senão M[s-1]:=0; s:=s-1
        printf("CME\n");
        if(*(mem + (s-1)) < *(mem + s)){
            *(mem + (s-1)) = 1;
        }else{
            *(mem + (s-1)) = 0;
        }
        s = s-1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int)));
        i++;

    }else if(strcmp(comando_atual.command,"CMA") == 0){     //Se M[s-1] >M[s] então M[s-1]:=1 senão M[s-1]:=0;s:=s-1
        printf("CMA\n");
        if(*(mem + (s-1)) > *(mem + s)){
            *(mem + (s-1)) = 1;
        }else{
            *(mem + (s-1)) = 0;
        }
        s = s-1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int))); 
        i++;

    }else if(strcmp(comando_atual.command,"CEQ") == 0){     //Se M[s-1] == M[s] então M[s-1]:=1 senão M[s-1]:=0;s:=s-1
        printf("CEQ\n");
        if(*(mem + (s-1)) == *(mem + s)){
            *(mem + (s-1)) = 1;
        }else{
            *(mem + (s-1)) = 0;
        }
        s = s-1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int)));
        i++;

    }else if(strcmp(comando_atual.command,"CDIF") == 0){    //Se M[s-1] != M[s] então M[s-1]:=1 senão M[s-1]:=0; s:=s-1
        printf("CDIF\n");
        if(*(mem + (s-1)) != *(mem + s)){
            *(mem + (s-1)) = 1;
        }else{
            *(mem + (s-1)) = 0;
        }
        s = s-1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int)));
        i++;

    }else if(strcmp(comando_atual.command,"CMEQ") == 0){    //Se M[s-1] <= M[s] então M[s-1]:=1 senão M[s-1]:=0;s:=s-1
        printf("CMEQ\n");
        if(*(mem + (s-1)) <= *(mem + s)){
            *(mem + (s-1)) = 1;
        }else{
            *(mem + (s-1)) = 0;
        }
        s = s-1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int)));
        i++;

    }else if(strcmp(comando_atual.command,"CMAQ") == 0){    //Se M[s-1] >= M[s] então M[s-1]:=1 senão M[s-1]:=0; s:=s-1
        printf("CMAQ\n");
        if(*(mem + (s-1)) >= *(mem + s)){
            *(mem + (s-1)) = 1;
        }else{
            *(mem + (s-1)) = 0;
        }
        s = s-1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int)));
        i++;

    }else if(strcmp(comando_atual.command,"STR") == 0){     //M[n]:=M[s]; s:=s-1
        //STR n
        printf("STR\n");
        aux = atoi(comando_atual.arg1_command);
        *(mem + aux) = *(mem + s);
        s = s-1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int)));
        i++;

    }else if(strcmp(comando_atual.command,"JMP") == 0){     //i:=p
        // JMP p
        printf("JMP\n");
        printf("entrou no JMP\n");
        char* rot_aux;
        rot_aux = comando_atual.arg1_command; //pega o rotulo 
        i = ret_elem_rot(&comandos,rot_aux).linha; //procura a linha que tem o rotulo de cima

    }else if(strcmp(comando_atual.command,"JMPF") == 0){    //Se M[s]=0 então i:=p senão i:=i+1; S:=s-1
        //JMPF p
        printf("JMPF\n");
        char* rot_aux;
        if(*(mem + s) == 0){
            rot_aux = comando_atual.arg1_command;
            i = ret_elem_rot(&comandos,rot_aux).linha;
        }else{
            i = i+1;
        }
        s=s-1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int)));
        
    }else if(strcmp(comando_atual.command,"NULL") == 0){    //vai para a proxima instrucao
        printf("NULL\n");
        i = i+1;

    }else if(strcmp(comando_atual.command,"RD") == 0){    //S:=s+1; M[s]:= “próximo valor de entrada”
        printf("RD\n");
        s = s+1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int)));
        //printf("Entre o valor: ");
        //scanf("%d",&aux);
        aux = var_read;
        *(mem + s) = aux;
        i++;

    }else if(strcmp(comando_atual.command,"PRN") == 0){     //“Imprimir M[s]”; s:=s-1
        printf("PRN\nconteudo da variavel[%d]: %d\n",aux, *(mem + s));
        s = s-1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int)));
        i++;

    }else if(strcmp(comando_atual.command,"ALLOC") == 0){   //ALLOC m,n
        printf("ALLOC\n");
        int k;
        int arg_1,arg_2;
        arg_1 = atoi(comando_atual.arg1_command); //m
        arg_2 = atoi(comando_atual.arg2_command); //n

        for(k=0;k<=(arg_2-1);k++){ //Para k:=0 até n-1 faça
            s=s+1;    //{s:=s+1;M[s]:=M[m+k]}
            mem = (int *) realloc(mem,((s+1)*sizeof(int)));
            *(mem + s) = *(mem + (arg_1+k));
        }
        i++;

    }else if(strcmp(comando_atual.command,"DALLOC") == 0){  //DALLOC m,n
        printf("DALLOC\n");
        int k,i_aux=0;
        int arg_1,arg_2;
        arg_1 = atoi(comando_atual.arg1_command); //m
        arg_2 = atoi(comando_atual.arg2_command); //n

        for(k=(arg_2-1);k>=0;k--){
            printf("    Desalocando memoria [%d]\n",s);
            *(mem + (arg_1+k)) = *(mem + s);
            s = s-1;
            mem = (int *) realloc(mem,((s+1)*sizeof(int)));
            //printa_memoria(mem,s);
            //printf("\n");                
        }
        i++;

    }else if(strcmp(comando_atual.command,"CALL") == 0){
        // CALL p
        printf("CALL\n");
        char* rot_aux;
        rot_aux = comando_atual.arg1_command; //pega o rotulo 

        s=s+1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int)));
        *(mem + s) = i+1;                                   //mem[s]
        i = ret_elem_rot(&comandos,rot_aux).linha;    //i=p;

    }else if(strcmp(comando_atual.command,"RETURN") == 0){
        printf("RETURN\n");
        i = *(mem + s);
        s=s-1;
        mem = (int *) realloc(mem,((s+1)*sizeof(int)));
    }else if(strcmp(comando_atual.command,"HLT") == 0){
        printf("HLT\n");
    }
    
    printa_memoria(mem,s);
    return i;
}

/* Search string start with */
bool StartsWith(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------


//---------------------------------------------------------------------------------
//									MAIN
//---------------------------------------------------------------------------------

// Funcionalidade: 
//                 
//                 
// Comunicaçao:    
//                 
//                 
//                                   
int main(void) {
    // Arquivo principal 
	FILE *p;
    // delimitador para split
	char str[BUFFER_LENGTH] = "";
	int linha_aux = 0;
    // zerar a resposta
    char response[] = "200,";
	
    /* Client and Server socket structures */
    struct sockaddr_in client, server;

    /* File descriptors of client and server */
    int serverfd, clientfd;
	
	/* Max size buffer recept */
    char buffer[BUFFER_LENGTH];


    /* Abrindo socket para passar informacoes
    */

    fprintf(stdout, "Starting server\n");

    /* Creates a IPv4 socket */
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverfd == -1) {
        perror("Can't create the server socket:");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Server socket created with fd: %d\n", serverfd);


    /* Defines the server socket properties */
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    memset(server.sin_zero, 0x0, 8);


    /* Handle the error of the port already in use */
    int yes = 1;
    if(setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR,
                  &yes, sizeof(int)) == -1) {
        perror("Socket options error:");
        return EXIT_FAILURE;
    }


    /* bind the socket to a port */
    if(bind(serverfd, (struct sockaddr*)&server, sizeof(server)) == -1 ) {
        perror("Socket bind error:");
        return EXIT_FAILURE;
    }


    /* Starts to wait connections from clients */
    if(listen(serverfd, 1) == -1) {
        perror("Listen error:");
        return EXIT_FAILURE;
    }
    // debug
    fprintf(stdout, "Listening on port %d\n", PORT);
    //se der erro 
    socklen_t client_len = sizeof(client);
    if ((clientfd=accept(serverfd,
        (struct sockaddr *) &client, &client_len )) == -1) {
        perror("Accept error:");
        return EXIT_FAILURE;
    }
	
    /* Copies into buffer our welcome messaage */
    strcpy(buffer, "200\n\0");

    /* Sends the message to the client */
    if (send(clientfd, buffer, strlen(buffer), 0)) {
        fprintf(stdout, "Client connected.\nWaiting for client message ...\n");
        

        /* Communicates with the client until bye message come */
        do {

            /* Zeroing buffers */
            memset(buffer, 0x0, BUFFER_LENGTH);

            /* Receives client message */
            int message_len;
            if((message_len = recv(clientfd, buffer, BUFFER_LENGTH, 0)) > 0) 
            {
				//se comecar com init_app, passar a instrucao de main()
                /* if buffer start with 'init_app' alloc memory */
                if(StartsWith(buffer, "init_app")) 
                {

                    char delim[] = ",";
                    // passar prox do buffer
                    char *ptr = strtok(buffer, delim);

					mem = (int *) malloc(sizeof(int));
				
					//Lista que recebera o comando do arquivo gera.obj
					
					init_fila(&comandos); 
					
					i = 0;
					//-- Abertura do arquivo .obj criado pelo compilador --
				
					// Le um nome do arquivo a ser aberto
					//printf("\n\n Entre com um nome para o arquivo:\n");
					//gets(str);
                    ptr = strtok(NULL, delim);
					if((p = fopen(ptr, "r")) == NULL)
                    {
						printf("Erro! Impossivel abrir o arquivo!\n");
                        return EXIT_FAILURE;
					}
                    // passar prox do buffer
                    ptr = strtok(NULL, delim);
				
					//--- Forma area de programa
					forma_area_prog(p,&comandos); //forma uma lista ligada com os comandos onde cada no tem a linha que pertence e seu codigo
                    //ptr = strtok(NULL, delim);

                } 
				// quando o comando com virtual_machine comeca a chamar o programa principal maquina_virtual
                if (StartsWith(buffer, "virtual_machine"))
				{
                    printf("buffer ta %s\n",buffer);
                    char delim[] = ",";

                    char *ptr = strtok(buffer, delim);
                    ptr = strtok(NULL, delim);
                    // se chegar prox parametro significa que tem um numero adicional para o read, entra no else e atribui na variavel global do read
                    if(ptr == NULL){
                    }
                    else{
                        var_read = atoi(ptr);
                    }
					linha_aux = maquina_virtual();
                    sprintf(str, "%i", linha_aux);
                    strcat(response, str);
                    printf("MACHINE LINE: %i\n", linha_aux);
                    printf("RESPONSE VIRTUAL NACHINE: %s\n", response);

                    
				}
                
            }

            /* 'bye' message finishes the connection */
            if(strcmp(buffer, "bye") == 0)
            {
                send(clientfd, "bye", 3, 0);
            }

            else 
            {
                // passando a informacao para o cliente
                int size_response;
                size_response = strlen(response);
                printf("RESPONSE: %s\n", response);
                send(clientfd, response, size_response, 0);
                strcpy(response,response2);
                int linha_aux = 0;
                char str[BUFFER_LENGTH] = "";
            }

        } while(strcmp(buffer, "bye"));
    }

    /* Client connection Close */
    close(clientfd);

    /* Close the local socket */
    close(serverfd);

    printf("Connection closed\n\n");

    return EXIT_SUCCESS;
}

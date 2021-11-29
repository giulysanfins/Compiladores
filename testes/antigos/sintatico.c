#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexical.h"
#include "simbolos.h"

void main()
{
   FILE *p;
   char str[80], ch;
   tokens token;

   Stack tab_simbolo; //pilha - tabela de simbolos
   initStack(&tab_simbolo);

   /* Le um nome do arquivo a ser aberto: */
   printf("\n\n Entre com um nome para o arquivo:\n");
   gets(str);

   // Caso ocorra algum erro na abertura do arquivo
   if ((p = fopen(str, "r")) == NULL)
   {
      // o programa aborta automaticamente
      printf("Erro! Impossivel abrir o arquivo!\n");
      exit(1);
   }
   /* Se nao houve erro, imprime no arquivo, fecha ...*/
   fprintf(p, "Este e um arquivo chamado:\n%s\n", str);

   //----------------------
   // ANALISE SINTATICA
   //----------------------
   //printf("\nlexema: %s simbolo: %s\n",token.lexema,token.simbolo);

   //Def rotulo inteiro
   //inicio
   //rotulo:=1

   //ele nao esta andando com o ponteiro do arquivo, so esta pegando a primeira palavra do arquivo
   //teremos de transformar o simbolos.c em um .h para as funçoes de manipulaçao da tabela de simbolo

   token = lexico(p);
   if (token.simbolo == "sprograma")
   {
      token = lexico(p);
      if (token.simbolo == "sidentificador")
      {
         insere_tabela(&tab_simbolo, token.lexema, "nomedeprograma", "", "");
         token = lexico(p);
         if (token.simbolo == "spontovirgula")
         {
            //analisa_bloco(); //analisa bloco atualiza o conteudo de token
            if (token.simbolo == "sponto")
            {
               //se acabou arquivo ou é comentario
               //sucesso
               //senao
               //erro
               //Alem do ponto é permitido apenas comentarios
            }
            else
            {
               //erro
               //esperado um . (ponto final) ao fim do programa
            }
         }
         else
         {
            //erro
            //esperado ; ao fim do comando de programa
         }
      }
      else
      {
         //erro
         //esperado um identificador apos "programa" o giu eh gayy quem eh giu o giubrisqueudo kkkkkkkkkkkkkkk
      }
   }
   else
   {
      //erro
      //esperado palavra reservada "programa"
   }
   imprimirLista(&tab_simbolo);
   fclose(p);
}
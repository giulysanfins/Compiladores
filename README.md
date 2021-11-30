# Transfêrencia de Arquivos

![GitHub repo size](https://img.shields.io/github/repo-size/joaohp2000/Redes_b)
![GitHub commit activity](https://img.shields.io/github/commit-activity/w/joaohp2000/Redes_b)
![Forks](https://img.shields.io/github/forks/joaohp2000/Redes_b?style=social)
![Likes](https://img.shields.io/github/stars/joaohp2000/Redes_b?style=social)

<img src="https://github.com/giulysanfins/Compiladores/blob/master/compilador_04.png" alt="exemplo imagem">

> Programa desenvolvido na linguagem C com objetivo de construir um Compilador e uma Máquina Virtual. Além disso, utilizou-se pyhton para construção da parte gráfica doo projeto.

## 💻 Pré-requisitos

Antes de começar, verifique se você atendeu aos seguintes requisitos:
* Você instalou a versão mais recente do compilador [GCC](https://gcc.gnu.org/).
* Você possui a versão mais recente do [GTK](https://www.gtk.org).
* Você tem uma máquina Linux ou Windows (usando o WSL). 
* Você possui Python 3 instalado em sua máquina.

## 🚀 Instalando: 

Para instalar o projeto, siga estas etapas:

1. Baixe o código fonte do projeto com o seguinte comando:
```
git clone https://github.com/giulysanfins/Compiladores.git

```
2. Descompacte os arquivos usando algum descompressor a sua escolha.
3. Mude para a pasta baixada e descompactada:
```
cd Compiladores
```
4. Abra um outro terminal nessa mesma pasta para facilitar a compilação.

## ☕ Compilando 

Será necessário dois terminais para fazer a compilação, o primeiro para o Compilador e o segundo para a Máquina Virtual.
Para compilar, siga estas etapas:

1. No Terminal para executar o Compilador execute os seguintes comandos.
```
#### sudo apt install libgirepository1.0-dev gcc libcairo2-dev pkg-config python3-dev gir1.2-gtk-3.0
#### sudo apt install python3-venv
#### cd into root directory 
#### python3 -m venv venv
#### source venv/bin/activate
#### pip3 install pycairo
#### pip3 install PyGObject
#### python src/gtkcompiler.py
#### if gtk error apt-get install gir1.2-gtksource
```
2. No Terminal 2 - compile o cliente juntamente com a biblioteca criada.
```
./compila.sh cliente
```
3. 
```
./servidor
```
4. 
5. 
```
./cliente
```
6. Feito, agora a mensagem de arquivo enviado aparecerá, e você já deve estar vendo o arquivo gerado na pasta.
<img src="https://gyazo.com/296580837e831cb73465d4b27a94d5b7.png" alt="terminal 1">


## 📫 Contribuindo para o nosso projeto
<!---Se o seu README for longo ou se você tiver algum processo ou etapas específicas que deseja que os contribuidores sigam, considere a criação de um arquivo CONTRIBUTING.md separado--->
Para contribuir com o projeto, siga estas etapas:

1. Bifurque este repositório.
2. Crie um branch: `git checkout -b <nome_branch>`.
3. Faça suas alterações e confirme-as: `git commit -m '<mensagem_commit>'`
4. Envie para o branch original: `git push origin <nome_do_projeto> / <local>`
5. Crie a solicitação de pull.

Como alternativa, consulte a documentação do GitHub em [como criar uma solicitação pull](https://help.github.com/en/github/collaborating-with-issues-and-pull-requests/creating-a-pull-request).

## 🤝 Colaboradores

Agradecemos às seguintes pessoas que contribuíram para este projeto:

<table>
  <tr>
    <td align="center">
      <a href="#">
        <img src="https://avatars.githubusercontent.com/u/38138832?v=4.png" width="100px;" alt="Foto do Carlos Henrique"/><br>
        <sub>
          <b>Carlos Henrique</b>
        </sub>
      </a>
    </td>
    <td align="center">
      <a href="#">
        <img src="https://avatars.githubusercontent.com/u/32877842?v=4.png" width="100px;" alt="Foto do Giuliano Sanfins"/><br>
        <sub>
          <b>Giuliano Sanfins</b>
        </sub>
      </a>
    </td>
    <td align="center">
      <a href="#">
        <img src="https://avatars.githubusercontent.com/u/50489803?v=4.png" width="100px;" alt="Foto do Matheus Moretti"/><br>
        <sub>
          <b>Matheus Moretti</b>
        </sub>
      </a>
    </td>
  </tr>
</table>




# Compiladores
#### sudo apt install libgirepository1.0-dev gcc libcairo2-dev pkg-config python3-dev gir1.2-gtk-3.0
#### sudo apt install python3-venv
#### cd into root directory 
#### python3 -m venv venv
#### source venv/bin/activate
#### pip3 install pycairo
#### pip3 install PyGObject
#### python src/gtkcompiler.py
#### if gtk error apt-get install gir1.2-gtksource

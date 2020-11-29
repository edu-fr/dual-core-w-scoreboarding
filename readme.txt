README PROJETO 2

Alunos: 
Edson Matheus Alexandre Cizeski
Gabriel Ossamu Maeda
Eduardo Felipe Ribeiro

------------------------------

Para compilar o codigo "main.c":
	-> gcc main.c -o nome_executavel -lm 

Para executar:
	-> ./nome_executavel -c arq_configuracao.txt -o arq_saida -p arq_programa.txt -m tamanho do programa

Para abrir um menu de ajuda basta digitar a opcao "-h"

Os registradores contidos no arquivo "arq_programa.txt" são lidos conforme padrão MIPS, com os nomes como seguem os exemplos: "$t2" "$s1"

------------------------------

A compilacao e os testes foram feitos em um compilador gcc versao 9.3.0 (Ubuntu 20.04 LTS)

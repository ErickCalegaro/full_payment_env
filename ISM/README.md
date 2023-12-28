# README #

* Este é o core lógico principal utilizado para interpretar mensagens ISO-8583 de pagamentos da Gertec.

### Para que serve este repositorio? ###

* Utilizado em conjunto com a aplicação DEMO-POS permite transações simuladas em ambiente online.

### Como faço para configurar? ###

#### Requisitos:
* Estar executando um SO Linux
* Ter instalado o gcc
* Ter a biblioteca sqlite3 instalada

#### Configuração:
1. Executar o clone do repositorio
2. Executar o shell "compiler.sh" para obter o executavel

### Como faço para executar? ###

1. Executar o shell "runner.sh" para ouvir a porta local 55555
2. Executar o serviço "CM_TCP" para fazer o tunel com os clients

### Como faço para debugar? ###

* Os logs deste serviço estão disponiveis em PUB/log_ism.txt

### Com quem posso falar? ###
[Linkedin](https://www.linkedin.com/in/erick-calegaro/)

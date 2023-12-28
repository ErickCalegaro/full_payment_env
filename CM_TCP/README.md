# README #

* Este é o serviço de comunicação TCP/IP utilizado para trafegar mensagens ISO-8583 entre a os terminais Gertec e o ambiente em nuvem.

### Para que serve este repositorio? ###

* Utilizado em conjunto com as aplicações DEMO-POS e ISO_MODULE permite transações em ambiente online.

### Como faço para configurar? ###

#### Requisitos:
* Estar executando um SO Linux
* Ter instalado o gcc
* Ter a biblioteca pthread instalada

#### Configuração:
1. Executar o clone do repositorio
2. Executar o arquivo "compiler.sh" para obter o executavel

### Como faço para executar? ###

1. Executar o shell "runner.sh" para ouvir a porta local 7072
2. Executar o serviço "ISM" para interpretar as mensagens recebidas

### Com quem posso falar? ###
[Linkedin](https://www.linkedin.com/in/erick-calegaro/)
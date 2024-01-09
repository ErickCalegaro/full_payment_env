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

## Hierarquia de Logs

Para definir o nivel de log que deseja utilizar basta alterar o define "LOG_LEVEL" em header.h:91. 
Os niveis disponiveis são os seguintes, sendo que números maiores contém também os logs dos menores.

0. LOG_NONE - Não retornará nenhum log
1. LOG_FATAL - Usado para erros que encerrem a execução da aplicação.
2. LOG_ERROR - Usado para casos de erro de interpretação, porém recuperaveis.
3. LOG_WARN - Usado para informações sobre memória e desempenho
4. LOG_INFO - Usado para logs de conexão, input e output
5. LOG_DEBUG - Usado para exibir todas as linhas de log na aplicação (parser, maker, etc...)

* X = Presente no nivel da coluna

| NIVEL | LOG_DEBUG | LOG_INFO  | LOG_WARN  | LOG_ERROR | LOG_FATAL | LOG_NONE  |
| ----- | --------- | --------- | --------- | --------- | --------- | --------- |
| NONE  |           |           |           |           |           |           |
| DEBUG |     X     |           |           |           |           |           |
| INFO  |     X     |     X     |           |           |           |           |
| WARN  |     X     |     X     |     X     |           |           |           |
| ERROR |     X     |     X     |     X     |     X     |           |           |
| FATAL |     X     |     X     |     X     |     X     |     X     |           |

### Com quem posso falar? ###
[Linkedin](https://www.linkedin.com/in/erick-calegaro/)

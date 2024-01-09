# README #

Este é um projeto criado para integrar ao host robopay e validar as soluções de criptografia e PIN 

## Para que serve este repositorio? ###

Permite testar a veracidade e fidelidade da Gedi e PPcomp para criptografar blocos com MasterKey e DUKPT.

## Como faço para configurar? ###

### Requisitos:
* Estar executando um SO Linux
* Ter instalado o gcc
* Ter as bibliotecas ssl e crypto instaladas

### Configuração:
1. Executar o clone do repositorio
2. Executar o shell "compiler.sh" para obter o executavel

## 📋  Documentação

### Chaves utilizadas:

#### PIN:
	- Perfil "Gertec"
	- Tipo 	 "DUKPT 3DES"
	- BDK 	 "0123456789ABCDEFFEDCBA9876543210"
	- KSN  	 "FFFFFFFFFF0000000000"
	- Indice "00"

#### DADOS:
	- Perfil	 "Gertec"
	- Tipo 	 	 "MASTER KEY 3DES"
	- MasterKey	 "44415500F960B44EB4FCB80F3CEEA300"
	- WorkingKey "31323334353637383837363534333231"
	- Indice 	 "01"

### Mensagem recebida:
| Posição | Tipo | Tamanho | Descrição |
| ------- | ---- | ------- | --------- |
| 0 | H | 20 | KSN de PIN |
| 20 | H | 16 | PIN Block |
| 26 | H | END | Dados criptografados |

### Mensagem enviada:
| Posição | Tipo | Tamanho | Descrição |
| ------- | ---- | ------- | --------- |
| 0 | AN | 19 | PAN á direita |
| 19 | AN | 76 | Trilha 1 á direita |
| 95 | AN | 37 | Trilha 2 á direita |
| 132 | N | 4 | Data de Validade |
| 136 | AN | 4 | Código de Verificação |
| 140 | AN | 8 | PIN á direita |

## Como faço para executar?

1. Executar o shell "runner.sh" para ouvir a porta local 55556
2. Executar o serviço "ISM" para fazer o tunel com o rebatedor

## Como faço para debugar?

* Os logs deste serviço estão disponiveis em PUB/log_hsm.txt

## Hierarquia de Logs

Para definir o nivel de log que deseja utilizar basta alterar o define "LOG_LEVEL" em header.h:64. 
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

## Com quem posso falar?
[Linkedin](https://www.linkedin.com/in/erick-calegaro/)

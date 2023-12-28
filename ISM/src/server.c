#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include "sqlite3.h"

//"Makefile" XGH
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#include "header.h"
#include "def_types.h"
#include "utl_sqlite.h"
#include "utl_sqlite.c"
#include "utils.c"
#include "isoProcess.c"

int main(void) {

    //Variaveis auxiliares para encontrar o arquivo a ser transferido.
    struct dirent *myfile;
    struct stat mystat;

    int portaServidor = ISM_PORT;

    //variaveis
    int socket_desc, conexao, c, nova_conex;
    struct sockaddr_in servidor, cliente;
    char *mensagem;
    char tempRecvMessage[MAX_MSG] = {0x00};
    char recvMessage[MAX_MSG] = {0x00};
    char sendMessage[MAX_MSG] = {0x00};
    int tamanho, count, sizeRecv, sizeSend, isfirstMsg, sizeTotal;

    // para pegar o IP e porta do cliente  
    char *cliente_ip;
    int cliente_port;

    do{
        //Criando um socket
        socket_desc = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_desc == -1) {
            printf("Nao foi possivel criar o socket, aguarde alguns instantes\n");
            return -1;
        }

        //Preparando a struct do socket
        servidor.sin_family = AF_INET;
        servidor.sin_addr.s_addr = INADDR_ANY; // Obtem IP do S.O.
        servidor.sin_port = htons(portaServidor);

        //Associando o socket a porta e endereco
        if (bind(socket_desc, (struct sockaddr *) &servidor, sizeof (servidor)) < 0) {
            puts("Porta ocupada, aguarde alguns instantes\n");
            return -1;
        }
        puts("Bind efetuado com sucesso\n");

        // Ouvindo por conexoes
        listen(socket_desc, 3);
        /*********************************************************/
        //Aceitando e tratando conexoes

        puts("Aguardando por conexoes...");
        c = sizeof (struct sockaddr_in);

        while ((conexao = accept(socket_desc, (struct sockaddr *) &cliente, (socklen_t*) & c))) {
            if (conexao < 0) {
                perror("Erro ao receber conexao\n");
                continue;
            }
        // pegando IP e porta do cliente
        cliente_ip = inet_ntoa(cliente.sin_addr);
        cliente_port = ntohs(cliente.sin_port);
        printf("cliente conectou: %s : [ %d ]\n", cliente_ip, cliente_port);
        isfirstMsg = TRUE;

        // lendo dados enviados pelo cliente
        //mensagem 1 recebido nome do arquivo   
        memset(tempRecvMessage, 0x00, sizeof(tempRecvMessage));
        if ((sizeRecv = read(conexao, tempRecvMessage, MAX_MSG)) < 0) {
            perror("Erro ao receber dados do cliente: ");
            continue;
        }

        if (tempRecvMessage == NULL){
            perror("Erro ao receber dados do cliente");
            continue;   
        }

        printf("message result = [%s]\n", tempRecvMessage);
        util_String_DumpStringLog(tempRecvMessage, 100);

        do{
            if (strlen(tempRecvMessage) == 4 || strlen(tempRecvMessage) == 5){      //ping
                write(conexao, "pong", 4);
                break;
            }

            if (strlen(tempRecvMessage) == 6 || strlen(tempRecvMessage) == 7){   //paynet
                for (int timeout = 0; timeout < 10; timeout++)
                    usleep(1000*1000);
                write(conexao, "captura", 7);
                break;
            }

            // processamento de ISO
            printf("Size recv [%d]\n", sizeRecv);
            int result = 0;
            do{
                if (isfirstMsg){
                    isfirstMsg = FALSE;
                    tamanho = util_ConvertToDecimal_Header(tempRecvMessage) - 4; //tirando cabeçalho
                    strcpy(recvMessage, tempRecvMessage);
                }else{
                    if (sizeRecv < tamanho){
                        memset(tempRecvMessage, 0x00, sizeof(tempRecvMessage));
                        sizeRecv += read(conexao, tempRecvMessage, MAX_MSG);
                        printf("Passou read[%d] = [%s]\n", sizeRecv, tempRecvMessage);
                        strcat(recvMessage, tempRecvMessage);
                    }else{
                        break;
                    }
                }
            }while(TRUE);

            //!todo: tratar erros de formato ao receber iso
            //Pegar a mensagem, adicionar tamanho ao cabeçalho e enviar de volta
            result = iso_MainProcess(&recvMessage[4], tamanho, sendMessage, &sizeSend);
            printf("sendMessage(%d) = [%s]\n", sizeSend, sendMessage);

            if (!result)
                write(conexao, sendMessage, sizeSend);

        }while(FALSE);
        
        close(conexao);
        printf("Servidor finalizado...\n");
        continue;
            puts("Handler assigned");
        }
        if (nova_conex < 0) {
            perror("accept failed");
            continue;
        }
    }while(1);
    
    return 0;
}

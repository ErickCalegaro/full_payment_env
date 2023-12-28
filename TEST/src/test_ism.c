#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <arpa/inet.h>

#ifdef LOCAL
#define SERVER_IP   "127.0.0.1"
#else
#define SERVER_IP   "54.207.182.103"
#endif

#define PORT        7072
#define BUFFER_SIZE 1024*5
#define EXPECTED    "1025#001#0210B238040002C0820E0000000000000012003000000000010000"

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    char enter[10];

    // Criação do socket do cliente
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Erro ao criar o socket do cliente");
        exit(EXIT_FAILURE);
    }

    // Configuração do endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        perror("Erro ao configurar o endereço do servidor");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    // Conecta-se ao servidor
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Erro ao conectar ao servidor");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    // Solicitação de entrada do usuário
    printf("Aperte enter para enviar a mensagem ao servidor");
    fgets(enter, 10, stdin);

    sprintf(buffer, "0917#001#0200B238060000C0920A240000000000040800300000000001000002221051510000071051510222059003000000006C885262000000766007630001359860000000000000000312820230009F2701809F2608B94D6FC30864445C9F36020027950500000080009F34034103029F37045ACE21589F3303E0F0C85F280200569F10120210A5800F040000000000000000000000FF9A032102229F1A0200769F3501229C01008407A00000000410109F02060000001995015F2A0209869F03060000000000005F24032512319F120A4D617374657243617264500A4D4153544552434152440160010020100200202244001026MTIP23 MCD 12A            002003001003016541333XXXXXX0110012002000130041.08014018v653_01 2019.11.07015018v653_01 2019.11.07016015v553 2019.12.12017018v353_01 2019.12.10018018v250_03 2019.09.24019018v151_01 2019.09.20022016001.41 200617   0100001176572DB8906C5191FC560C959D33B9ABE8B5AAB1C139ECF40045D4E8378134C7D43E9B5C884E7AAD3D3BC1C9BC867C0AA6FB2723C94360E1026E53FB3D9104CDD532B1E8DC7B2769C004D4E85E242D22F8CDC8FA9F6064666A");

    // Envia a mensagem ao servidor
    send(clientSocket, buffer, strlen(buffer), 0);

    // Encerra o cliente se a mensagem for "exit"
    if (strncmp(buffer, "exit", 4) == 0)
        return 0;

    // Limpa o buffer
    memset(buffer, 0, sizeof(buffer));

    // Recebe a resposta do servidor
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead == -1) {
        perror("Erro ao receber dados do servidor");
    } else if (bytesRead == 0) {
        // O servidor fechou a conexão
        printf("O servidor fechou a conexão\n");
    } else {
        // Imprime a resposta recebida
        printf("Resposta do servidor: %s\n", buffer);
    }

    // Fecha o socket do cliente
    close(clientSocket);

    assert(memcmp(buffer, EXPECTED, strlen(EXPECTED)) == 0);

    return 0;
}
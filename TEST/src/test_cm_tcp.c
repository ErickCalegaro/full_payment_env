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
#define EXPECTED 	"pongcm"

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
    printf("Aperte enter para enviar a mensagem ao comunicator");
    fgets(enter, 10, stdin);

    // Envia a mensagem ao servidor
    send(clientSocket, "pingcm", 6, 0);

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <pthread.h>

#define LOCALHOST   "127.0.0.1"
#define ISM_PORT    55555

#define MAX_CLIENTS 100
#define MAX_MSG     1024*5

// Estrutura para armazenar informações do cliente
typedef struct {
    int socket;
    struct sockaddr_in address;
} ClientInfo;

// Função para lidar com a comunicação com um cliente
void *handleClient(void *arg) {
    struct sockaddr_in serverAddrISM;
    ClientInfo *client = (ClientInfo *)arg;
    char        buffer[MAX_MSG];
    ssize_t     bytesRead;
    int         commSocketISM;

    while ((bytesRead = recv(client->socket, buffer, sizeof(buffer), 0)) > 0) {
        printf("Cliente %d mandou: %s\n", ntohs(client->address.sin_port), buffer);

        if (strncmp(buffer, "pingcm", 6) == 0)
        {
            send(client->socket, "pongcm", 6, 0);
            close(client->socket);
            break;
        }

        // Criação do socket do ISM
        commSocketISM = socket(AF_INET, SOCK_STREAM, 0);
        if (commSocketISM == -1) {
            perror("Erro ao criar o socket do cliente");
            exit(EXIT_FAILURE);
        }

        // Configuração do endereço do ISO_MODULE
        serverAddrISM.sin_family = AF_INET;
        serverAddrISM.sin_port = htons(ISM_PORT);
        if (inet_pton(AF_INET, LOCALHOST, &serverAddrISM.sin_addr) <= 0) {
            perror("Erro ao configurar o endereço do ISO_MODULE");
            close(commSocketISM);
            exit(EXIT_FAILURE);
        }

        // Conecta-se ao ISO_MODULE
        if (connect(commSocketISM, (struct sockaddr *)&serverAddrISM, sizeof(serverAddrISM)) == -1) {
            perror("Erro ao conectar ao ISO_MODULE");
            close(commSocketISM);
            exit(EXIT_FAILURE);
        }

        // Envia a mensagem ao ISO_MODULE
        send(commSocketISM, buffer, strlen(buffer), 0);

        // Limpa o buffer
        memset(buffer, 0, sizeof(buffer));

        // Recebe a resposta do ISO_MODULE
        bytesRead = recv(commSocketISM, buffer, sizeof(buffer), 0);

        if (bytesRead == -1) {
            perror("Erro ao receber dados do servidor");
        } else if (bytesRead == 0) {
            // O servidor fechou a conexão
            printf("O servidor fechou a conexão\n");
        } else {
            // Imprime a resposta recebida
            printf("Resposta do servidor: %.*s\n", (int)bytesRead, buffer);
        }

        // Fecha o socket do ISO_MODULE
        close(commSocketISM);

        // Envia dados de volta ao TERMINAL
        send(client->socket, buffer, bytesRead, 0);

        // Libera a estrutura do TERMINAL
        close(client->socket);

        break;
    }

    // Cliente desconectado
    printf("Cliente desconectado: %s:%d\n", inet_ntoa(client->address.sin_addr), ntohs(client->address.sin_port));

    free(client);

    return NULL;
}

int main(int argc, char* argv[]) {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    pthread_t clientThreads[MAX_CLIENTS];

    //verificando se foi executando o comando corretamente
    if (argc != 2) {
        fprintf(stderr, "use:./server [Porta]\n");
        return -1;
    } else if (!isdigit(*argv[1])) {
        fprintf(stderr, "Argumento invalido '%s'\n", argv[1]);
        fprintf(stderr, "use:./server [Porta]\n");
        return -1;
    }

    char* aux1 = argv[1];
    int portaServidor = atoi(aux1);

    // Criação do socket do servidor
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Erro ao criar o socket do servidor");
        exit(EXIT_FAILURE);
    }

    // Configuração do endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(portaServidor);

    // Vincula o socket ao endereço e à porta
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Erro ao vincular o socket ao endereço e à porta");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Habilita o servidor para ouvir conexões
    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        perror("Erro ao configurar o servidor para ouvir conexões");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Servidor aguardando conexões na porta %d...\n", portaServidor);

    while (1) {
        // Aceita uma nova conexão
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("Erro ao aceitar a conexão");
            continue;
        }

        // Cria uma nova estrutura para armazenar informações do cliente
        ClientInfo *client = (ClientInfo *)malloc(sizeof(ClientInfo));
        client->socket = clientSocket;
        client->address = clientAddr;
        
        // Cliente desconectado
        printf("Cliente conectado: %s:%d\n", inet_ntoa(client->address.sin_addr), ntohs(client->address.sin_port));

        // Cria uma nova thread para lidar com o cliente
        pthread_create(&clientThreads[MAX_CLIENTS], NULL, handleClient, (void *)client);
    }

    // Nunca atingiremos este ponto no exemplo, mas para fins de demonstração
    close(serverSocket);
    return 0;
}
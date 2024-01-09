#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <pthread.h>

#define LOCALHOST   "127.0.0.1"
#define ISM_PORT    55555

#define MAX_CLIENTS 100
#define MAX_MSG     1024*5

#define LOG_NONE    0
#define LOG_FATAL   1
#define LOG_ERROR   2    //PROD
#define LOG_WARN    3    //HOMOLOG
#define LOG_INFO    4    //DEV
#define LOG_DEBUG   5    
#define LOG_PATH    "../PUB/log_cm_tcp_" // + gPortaServidor
#define LOG_EXT     ".txt"

#define LOG_LEVEL   LOG_INFO
#define LOG_FILE    1   //LOGA EM ARQUIVO, OU CONSOLE

#if (LOG_LEVEL == LOG_DEBUG)
#define TRACE_DEBUG(fmt, ...) util_logger(LOG_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define TRACE_INFO(fmt, ...) util_logger(LOG_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define TRACE_WARN(fmt, ...) util_logger(LOG_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define TRACE_ERROR(fmt, ...) util_logger(LOG_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define TRACE_FATAL(fmt, ...) util_logger(LOG_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#elif (LOG_LEVEL == LOG_INFO)
#define TRACE_DEBUG(fmt, ...)
#define TRACE_INFO(fmt, ...) util_logger(LOG_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define TRACE_WARN(fmt, ...) util_logger(LOG_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define TRACE_ERROR(fmt, ...) util_logger(LOG_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define TRACE_FATAL(fmt, ...) util_logger(LOG_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#elif (LOG_LEVEL == LOG_WARN)
#define TRACE_DEBUG(fmt, ...)
#define TRACE_INFO(fmt, ...)
#define TRACE_WARN(fmt, ...) util_logger(LOG_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define TRACE_ERROR(fmt, ...) util_logger(LOG_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define TRACE_FATAL(fmt, ...) util_logger(LOG_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#elif (LOG_LEVEL == LOG_ERROR)
#define TRACE_DEBUG(fmt, ...)
#define TRACE_INFO(fmt, ...)
#define TRACE_WARN(fmt, ...)
#define TRACE_ERROR(fmt, ...) util_logger(LOG_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define TRACE_FATAL(fmt, ...) util_logger(LOG_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#elif (LOG_LEVEL == LOG_FATAL)
#define TRACE_DEBUG(fmt, ...)
#define TRACE_INFO(fmt, ...)
#define TRACE_WARN(fmt, ...)
#define TRACE_ERROR(fmt, ...)
#define TRACE_FATAL(fmt, ...) util_logger(LOG_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define TRACE_DEBUG(fmt, ...)
#define TRACE_INFO(fmt, ...)
#define TRACE_WARN(fmt, ...)
#define TRACE_ERROR(fmt, ...)
#define TRACE_FATAL(fmt, ...)
#endif

char buffer[MAX_MSG];
int gPortaServidor;

int util_log_Append(char *data)
{
    FILE *fd;
    char zName[64] = {0x00};
    int  i;

    sprintf(zName, "%s%d%s", LOG_PATH, gPortaServidor, LOG_EXT);
    fd = fopen(zName, "a+");

    if(!fd){
        printf("ERRO CRIACAO DO ARQUIVO [%s]\n", zName);
        return(-1);
    }

    for(i = 0; data[i]; i++)
        fputc(data[i], fd);

    fclose(fd);

    return(0);
}

void util_logger(int level, char * file, int line, const char *fmt, ...)
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char sLevel[16] = {0x00};

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    switch(level){
        case LOG_DEBUG: strcpy(sLevel, "DEBUG");  break;
        case LOG_INFO:  strcpy(sLevel, "INFO ");  break;
        case LOG_WARN:  strcpy(sLevel, "WARN ");  break;
        case LOG_ERROR: strcpy(sLevel, "ERROR");  break;
        default:
        case LOG_FATAL: strcpy(sLevel, "FATAL");  break;
            break;
    }

#if LOG_FILE
    //!TODO: Logica para apagar o log após x dias
    char output[MAX_MSG] = {0x00};
    sprintf(output, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] %s:%d - %s\n", 
            tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
            tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec,
            sLevel, &file[6], line, buffer);
    util_log_Append(output);
#else
    printf("[%04d-%02d-%02d %02d:%02d:%02d] [%s] %s:%d - %s\n", 
            tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
            tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec,
            sLevel, &file[6], line, buffer);
#endif

}

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
        TRACE_DEBUG("Cliente %d mandou: %s", ntohs(client->address.sin_port), buffer);

        if (strncmp(buffer, "pingcm", 6) == 0)
        {
            send(client->socket, "pongcm", 6, 0);
            close(client->socket);
            break;
        }

        // Criação do socket do ISM
        commSocketISM = socket(AF_INET, SOCK_STREAM, 0);
        if (commSocketISM == -1) {
            TRACE_FATAL("Erro ao criar o socket do cliente");
            exit(EXIT_FAILURE);
        }

        // Configuração do endereço do ISO_MODULE
        serverAddrISM.sin_family = AF_INET;
        serverAddrISM.sin_port = htons(ISM_PORT);
        if (inet_pton(AF_INET, LOCALHOST, &serverAddrISM.sin_addr) <= 0) {
            TRACE_FATAL("Erro ao configurar o endereço do ISO_MODULE");
            close(commSocketISM);
            exit(EXIT_FAILURE);
        }

        // Conecta-se ao ISO_MODULE
        if (connect(commSocketISM, (struct sockaddr *)&serverAddrISM, sizeof(serverAddrISM)) == -1) {
            TRACE_FATAL("Erro ao conectar ao ISO_MODULE");
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
            TRACE_ERROR("Erro ao receber dados do servidor");
        } else if (bytesRead == 0) {
            // O servidor fechou a conexão
            TRACE_INFO("O servidor fechou a conexão");
        } else {
            // Imprime a resposta recebida
            TRACE_DEBUG("Resposta do servidor: %.*s", (int)bytesRead, buffer);
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
    TRACE_INFO("Cliente desconectado: %s:%d", inet_ntoa(client->address.sin_addr), ntohs(client->address.sin_port));

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
    gPortaServidor = atoi(aux1);

    // Criação do socket do servidor
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        TRACE_FATAL("Erro ao criar o socket do servidor");
        exit(EXIT_FAILURE);
    }

    // Configuração do endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(gPortaServidor);

    // Vincula o socket ao endereço e à porta
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        TRACE_FATAL("Erro ao vincular o socket ao endereço e à porta");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Habilita o servidor para ouvir conexões
    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        TRACE_FATAL("Erro ao configurar o servidor para ouvir conexões");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    TRACE_INFO("Servidor aguardando conexões na porta %d...", gPortaServidor);

    while (1) {
        // Aceita uma nova conexão
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            TRACE_ERROR("Erro ao aceitar a conexão");
            continue;
        }

        // Cria uma nova estrutura para armazenar informações do cliente
        ClientInfo *client = (ClientInfo *)malloc(sizeof(ClientInfo));
        client->socket = clientSocket;
        client->address = clientAddr;
        
        // Cliente desconectado
        TRACE_INFO("Cliente conectado: %s:%d", inet_ntoa(client->address.sin_addr), ntohs(client->address.sin_port));

        // Cria uma nova thread para lidar com o cliente
        pthread_create(&clientThreads[MAX_CLIENTS], NULL, handleClient, (void *)client);
    }

    // Nunca atingiremos este ponto no exemplo, mas para fins de demonstração
    close(serverSocket);
    return 0;
}
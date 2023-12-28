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
// #define EXPECTED    "1025#001#0210B238040002C0820E0000000000000012003000000000010000"

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

    sprintf(buffer, "0936#001#0200B238040000C0920224000000000004880030000000000600002312281554000002155405122807100000000000000000000000001234567890198646A4EFAFDAF93F40336820219809F2701809F26081D54930CA8BF359D9F36020744950504200480019F34034203009F370452996D919F3303E060C85F280200769F10120014A00003220000000000000000000000FF9A032011209F1A0200769F3501229C01008407A00000000410109F02060000000600005F2A0209869F03060000000000005F24032912319F120E4372656469746F204E7562616E6B500A4D6173746572636172649F1101015F340100232001026                          002010MasterCard003016520048XXXXXX80250080042.12009004214 010004130 011016120.T1 2023.11.0012016110 2023.06.26  013016120 2023.06.26  014016210 2023.06.26  015016                016016000.91 231113   0100001026002020FFFFFFFFFF0000000005176AC1963C350B45393CCEB7A7ADDEF43E6C42847383BBDB0975CDE72AC2CE3AF03D23AFBBDF4EA2D0F1D113DCD36FE7F8567177F86898AA6936FBA7A8D12CFF12A124AA47F69AF37E47C928E1E73DEE15983F775474D46DF6D");

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

    // assert(memcmp(buffer, EXPECTED, strlen(EXPECTED)) == 0);

    return 0;
}

#include "header.h"
#include "private.c"
#include "public.c"

#define BDK_DATA "0123456789ABCDEFFEDCBA9876543210"
#define MK_DATA  "44415500F960B44EB4FCB80F3CEEA300"
#define WK_DATA  "31323334353637383837363534333231"

// int main(int argc, char const *argv[])
// {
	// char *pinblock_dec = doDukptDecryptPinBlock(PIN_BLK, PAN_DATA, strlen(PAN_DATA));
	// printf("doDukptDecryptPinBlock: pinblock_dec = [%s]\n", pinblock_dec);
	// assert(memcmp(pinblock_dec, PIN_DEC, strlen(PIN_DEC)) == 0);

	// char *pinblock_enc = doDukptEncryptPinBlock(PIN_DEC, strlen(PIN_DEC), PAN_DATA, strlen(PAN_DATA));
	// printf("doDukptEncryptPinBlock: pinblock_enc = [%s]\n", pinblock_enc);
	// assert(memcmp(pinblock_enc, PIN_BLK, strlen(PIN_BLK)) == 0);
	
	// char *pin_dec_res = doDukptDecryptPIN(BDK_PIN, KSN_PIN, PIN_ENC, strlen(PIN_ENC));
	// printf("doDukptDecryptPIN: pin_dec_res = [%s]\n", pin_dec_res);
	// assert(memcmp(pin_dec_res, PIN_BLK, strlen(PIN_BLK)) == 0);

	// char *pin_enc_res = doDukptEncryptPIN(BDK_PIN, KSN_PIN, PIN_BLK, strlen(PIN_BLK));
	// printf("doDukptEncryptPIN: pin_enc_res = [%s]\n", pin_enc_res);
	// assert(memcmp(pin_enc_res, PIN_ENC, strlen(PIN_ENC)) == 0);

	// printf("doDukptDecryptData: DATA_DEC = [%s]\n", DATA_DEC);
	// u8   aAux [248] = {0x00};
	// u32  zAux       = Str2Hex(strlen(DATA_DEC), DATA_DEC, aAux);
	// printf("doDukptDecryptData: DATA_DEC = [%s]\n", aAux);

	// char *enc_res = doDukptEncryptData(BDK_DATA, KSN_DATA, DATA_DEC, strlen(DATA_DEC));
	// printf("doDukptEncryptData: enc_res = [%s]\n", enc_res);
	// assert(memcmp(enc_res, DATA_ENC, strlen(DATA_ENC)) == 0);

	// char *dec_res = doDukptDecryptData(BDK_DATA, KSN_DATA, enc_res, strlen(enc_res));
	// printf("doDukptDecryptData: dec_res = [%s]\n", dec_res);
	// u8   bAux [248] = {0x00};
	// u32  iAux       = Str2Hex(strlen(enc_res), dec_res, bAux);
	// printf("doDukptDecryptData: dec_res = [%s]\n", bAux);
	// assert(memcmp(dec_res, DATA_DEC, strlen(DATA_DEC)) == 0);

	// char *dec_res = doMkWkDecryptData(MK_DATA, WK_DATA, DATA_ENC_3DES, strlen(DATA_ENC_3DES));
	// printf("doMkWkDecryptData: dec_res = [%s]\n", dec_res);
	// u8   bAux [248] = {0x00};
	// u32  iAux       = Str2Hex(strlen(DATA_ENC_3DES), dec_res, bAux);
	// printf("doMkWkDecryptData: dec_res = [%s]\n", bAux);
	// assert(memcmp(dec_res, DATA_DEC_3DES, strlen(DATA_DEC_3DES)) == 0);

// 	return 0;
// }

int main(void) {
    //Variaveis auxiliares para encontrar o arquivo a ser transferido.
    struct dirent *myfile;
    struct stat mystat;

    int portaServidor = HSM_PORT;

    //variaveis
    int socket_desc, conexao, c, nova_conex;
    struct sockaddr_in servidor, cliente;
    char *mensagem;
    char tempMessage[MAX_MSG] = {0x00};
    char recvMessage[MAX_MSG] = {0x00};
    char sendMessage[MAX_MSG] = {0x00};
    int tamanho, count, sizeRecv, sizeSend, isfirstMsg, sizeTotal;
	unsigned char bAux [512] = {0x00};

    // para pegar o IP e porta do cliente  
    char *cliente_ip;
    int cliente_port;

    st_CryptoData t_cryptoData;

    do
    {
        //Criando um socket
        socket_desc = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_desc == -1) {
            TRACE_FATAL("Nao foi possivel criar o socket, aguarde alguns instantes");
            return -1;
        }

        //Preparando a struct do socket
        servidor.sin_family = AF_INET;
        servidor.sin_addr.s_addr = INADDR_ANY; // Obtem IP do S.O.
        servidor.sin_port = htons(portaServidor);

        //Associando o socket a porta e endereco
        if (bind(socket_desc, (struct sockaddr *) &servidor, sizeof (servidor)) < 0) {
            TRACE_FATAL("Porta ocupada, aguarde alguns instantes");
            return -1;
        }
        TRACE_INFO("Bind efetuado com sucesso");

        // Ouvindo por conexoes
        listen(socket_desc, 3);
        /*********************************************************/
        //Aceitando e tratando conexoes

        TRACE_INFO("Aguardando por conexoes...");
        c = sizeof (struct sockaddr_in);

        while ((conexao = accept(socket_desc, (struct sockaddr *) &cliente, (socklen_t*) & c))) 
        {
            if (conexao < 0) {
                TRACE_ERROR("Erro ao receber conexao");
                continue;
            }

	        // pegando IP e porta do cliente
	        cliente_ip = inet_ntoa(cliente.sin_addr);
	        cliente_port = ntohs(cliente.sin_port);
	        TRACE_INFO("cliente conectou: %s : [ %d ]", cliente_ip, cliente_port);
	        memset(&t_cryptoData, 0x00, sizeof(t_cryptoData));

	        // lendo dados enviados pelo cliente
	        //mensagem 1 recebido nome do arquivo   
	        memset(tempMessage, 0x00, sizeof(tempMessage));
	        if ((sizeRecv = read(conexao, tempMessage, MAX_MSG)) < 0) {
	            TRACE_ERROR("Erro ao receber dados do cliente: ");
	            continue;
	        }

	        if (tempMessage == NULL){
	            TRACE_ERROR("Erro ao receber dados do cliente");
	            continue;   
	        }

	        strcpy(recvMessage, tempMessage);
			TRACE_INFO("Parse Message: recvMessage    = [%s]", recvMessage);
        	strncpy(t_cryptoData.sKSN     , &recvMessage[0] , 20);
        	strncpy(t_cryptoData.sPINBlock, &recvMessage[20], 16);
        	strcpy (t_cryptoData.sDataEnc , &recvMessage[36]);
        	TRACE_DEBUG("Parse HSM Message: sKSN       = [%s]", t_cryptoData.sKSN);
        	TRACE_DEBUG("Parse HSM Message: sPINBlock  = [%s]", t_cryptoData.sPINBlock);
        	TRACE_DEBUG("Parse HSM Message: sDataEnc   = [%s]", t_cryptoData.sDataEnc);
        	
        	//Descriptografa dados do DE125
        	char *dec_res = doMkWkDecryptData(MK_DATA, WK_DATA, t_cryptoData.sDataEnc, strlen(t_cryptoData.sDataEnc));
			Str2Hex(strlen(t_cryptoData.sDataEnc), dec_res, bAux);
			free(dec_res);
			strcpy(t_cryptoData.sDataDec, bAux);
			TRACE_DEBUG("doMkWkDecryptData: sDataDec   = [%s]", t_cryptoData.sDataDec);

			//Parseia informações do bit
			count = 3;  		// Pula "PAN"
			tamanho = 3;
			memset(tempMessage, 0x00, sizeof(tempMessage));
			strncpy(tempMessage, &t_cryptoData.sDataDec[count], tamanho);
			count += 3;  		// Pula bytes de tamanho
			tamanho = atoi(tempMessage);
			strncpy(t_cryptoData.sPAN, &t_cryptoData.sDataDec[count], tamanho);
			count += tamanho; 	// Pula dados do pan
			count += 3;  		// Pula "TR1"
			tamanho = 3;
			memset(tempMessage, 0x00, sizeof(tempMessage));
			strncpy(tempMessage, &t_cryptoData.sDataDec[count], tamanho);
			count += 3; 		// Pula bytes de tamanho
			tamanho = atoi(tempMessage);
			strncpy(t_cryptoData.sTrack1, &t_cryptoData.sDataDec[count], tamanho);
			count += tamanho; 	// Pula dados da trilha 1
			count += 3;  		// Pula "TR2"
			tamanho = 3;
			memset(tempMessage, 0x00, sizeof(tempMessage));
			strncpy(tempMessage, &t_cryptoData.sDataDec[count], tamanho);
			count += 3; 		// Pula bytes de tamanho
			tamanho = atoi(tempMessage);
			strncpy(t_cryptoData.sTrack2, &t_cryptoData.sDataDec[count], tamanho);
			count += tamanho; 	// Pula dados da trilha 2
			count += 3;  		// Pula "DTV"
			tamanho = 3;
			memset(tempMessage, 0x00, sizeof(tempMessage));
			strncpy(tempMessage, &t_cryptoData.sDataDec[count], tamanho);
			count += 3; 		// Pula bytes de tamanho
			tamanho = atoi(tempMessage);
			strncpy(t_cryptoData.sDTV, &t_cryptoData.sDataDec[count], tamanho);
			count += tamanho; 	// Pula dados da data de validade
			count += 3;  		// Pula "CVV"
			tamanho = 3;
			memset(tempMessage, 0x00, sizeof(tempMessage));
			strncpy(tempMessage, &t_cryptoData.sDataDec[count], tamanho);
			count += 3; 		// Pula bytes de tamanho
			tamanho = atoi(tempMessage);
			strncpy(t_cryptoData.sCVV, &t_cryptoData.sDataDec[count], tamanho);

			TRACE_DEBUG("Parse HSM Message: sPAN       = [%s]", t_cryptoData.sPAN);
			TRACE_DEBUG("Parse HSM Message: sTrack1    = [%s]", t_cryptoData.sTrack1);
			TRACE_DEBUG("Parse HSM Message: sTrack2    = [%s]", t_cryptoData.sTrack2);
			TRACE_DEBUG("Parse HSM Message: sDTV       = [%s]", t_cryptoData.sDTV);
			TRACE_DEBUG("Parse HSM Message: sCVV       = [%s]", t_cryptoData.sCVV);

			char *pin_dec_res = doDukptDecryptPIN(BDK_DATA, t_cryptoData.sKSN, t_cryptoData.sPINBlock, strlen(t_cryptoData.sPINBlock));
			TRACE_DEBUG("DukptDecryptPIN: pin_dec_res  = [%s]", pin_dec_res);

			char *pinblock_dec = doDukptDecryptPinBlock(pin_dec_res, t_cryptoData.sPAN, strlen(t_cryptoData.sPAN));
			TRACE_DEBUG("DukptDecryptPinBlock: pin_dec = [%s]", pinblock_dec);
			strcpy(t_cryptoData.sPINDec, pinblock_dec);
			free(pin_dec_res);
			free(pinblock_dec);

			sprintf(sendMessage, "%19s%76s%37s%4s%4s%8s", 
									t_cryptoData.sPAN,
									t_cryptoData.sTrack1,
									t_cryptoData.sTrack2,
									t_cryptoData.sDTV,
									t_cryptoData.sCVV,
									t_cryptoData.sPINDec);
			TRACE_INFO("Maker Message: sendMessage    = [%s]", sendMessage);

            write(conexao, sendMessage, strlen(sendMessage));

	        close(conexao);
	        TRACE_INFO("Servidor finalizado...");
	        continue;
        }

        if (nova_conex < 0) {
            TRACE_ERROR("Falha no recebimento");
            continue;
        }
    }while(1);
    
    TRACE_FATAL("Aplicacao abortada...");
    return 0;
}

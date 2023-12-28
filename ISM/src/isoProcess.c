#include "utl_sqlite.h"

//Administrativas
int  iso_NetworkProcess	(char * messageRecv, int sizeRecv, char * messageSend, int * sizeSend);
int  iso_NetworkParser(char * message, ISO_NET_MODAL_T * structMsgIn);
void iso_DumpNetStruct(ISO_NET_MODAL_T * structMsg);
int  iso_NetworkMaker(char * message, ISO_NET_MODAL_T * structMsg);
int  iso_CommTest(ISO_NET_MODAL_T * structMsgIn, ISO_NET_MODAL_T * structMsgOut);

//Financeiras
int  iso_FinancialProcess(char * messageRecv, int sizeRecv, char * messageSend, int * sizeSend);
int  iso_ReversalProcess (char * messageRecv, int sizeRecv, char * messageSend, int * sizeSend);
int  iso_FinancialParser (char * message, ISO_FIN_MODAL_T * structMsg);
int  iso_ReversalParser  (char * message, ISO_REV_MODAL_T * structMsg);
void iso_DumpFinStruct   (ISO_FIN_MODAL_T * structMsg);
void iso_DumpRevStruct   (ISO_REV_MODAL_T * structMsg);
int  iso_FinancialMaker  (char * message, ISO_FIN_MODAL_T * structMsg);
int  iso_ReversalMaker   (char * message, ISO_REV_MODAL_T * structMsg);
int  iso_FinancialMockup (ISO_FIN_MODAL_T * structMsgIn, ISO_FIN_MODAL_T * structMsgOut);
int  iso_ReversalMockup  (ISO_REV_MODAL_T * structMsgIn, ISO_REV_MODAL_T * structMsgOut);
int  iso_SaveFinStruct   (ISO_FIN_MODAL_T * structMsg);
int  iso_UpdateFinStruct (ISO_FIN_MODAL_T * structMsg);
int  iso_UpdateRevStruct (ISO_REV_MODAL_T * structMsg);
int  iso_InitProcess     (ISO_NET_MODAL_T * structMsgIn, ISO_NET_MODAL_T * structMsgOut);
int  iso_ChargeProcess   (ISO_NET_MODAL_T * structMsgIn, ISO_NET_MODAL_T * structMsgOut);
int  iso_KeepAliveProcess(ISO_NET_MODAL_T * structMsgIn, ISO_NET_MODAL_T * structMsgOut);
int  iso_TimeGetDateAndTime(char *dateAndTime);

#define CV_MOCKUP_EC     "F              Argotechno               @                MasterCard                @ @CARTAO: 541333XXXXXX0110@2a VIA LOJISTA DATA:22/02/21 10:52:21@AUTO: 000142@CV: 000213000915@COD VENDA: 003680@COD ESTAB: TBD@POS: 80036318@NRO LOGICO: T45931255563865@@VENDA CREDITO A VISTA    @VALOR TOTAL: R$1995,01@@TRANSACAO AUTORIZADA COM SENHA    @@AID: A0000000041010@ARQC: B94D6FC30864445C@LABEL: MasterCard"
#define CV_MOCKUP_HOLDER "F              Argotechno               @                MasterCard                @ @CARTAO: 541333XXXXXX0110@1a VIA CLIENTE DATA:22/02/21 10:52:21@AUTO: 000142@CV: 000213000915@COD VENDA: 003680@COD ESTAB: TBD@POS: 80036318@NRO LOGICO: T45931255563865@@VENDA CREDITO A VISTA    @VALOR TOTAL: R$1995,01@@AID: A0000000041010@ARQC: B94D6FC30864445C@LABEL: MasterCard"

int iso_MainProcess(char * messageRecv, int sizeRecv, char * messageSend, int * sizeSend)
{
	int result = RC_OK;
	char acquirer [5+1] = {0x00};
	char mtiType  [4+1] = {0x00};
    char isoMessage[MAX_MSG] = {0x00};
    int  isoSize = 0;

	strncpy(acquirer, &messageRecv[0], 5);
	strncpy(mtiType , &messageRecv[5], 4);

	do{

		if (strcmp(acquirer, "#001#") != 0){ //define o apontamento do adquirente futuramente
			result = RC_ACQUIRER_INV;
			break;
		}

		if (strcmp(mtiType, "0800") == 0 || strcmp(mtiType, "0810") == 0){ //tirando o codigo do acquirer
			result = iso_NetworkProcess(&messageRecv[5], sizeRecv - 5, isoMessage, &isoSize);
		}else if(strcmp(mtiType, "0400") == 0 || strcmp(mtiType, "0410") == 0){
			result = iso_ReversalProcess(&messageRecv[5], sizeRecv - 5, isoMessage, &isoSize);
		}else if(strcmp(mtiType, "0200") == 0 || strcmp(mtiType, "0210") == 0){
			result = iso_FinancialProcess(&messageRecv[5], sizeRecv - 5, isoMessage, &isoSize);
		}

	}while(0);

	if (result != RC_OK){ //Simula timeout (Retornos possiveis [1,2,3,4,5])
		sizeSend = 0;
        sleep(30);
	}else{
		*sizeSend = isoSize + 9;
		sprintf(messageSend, "%04d%s", isoSize + 9, acquirer);
		strcat(messageSend, isoMessage);
	}

	return result;
}

int iso_NetworkProcess(char * messageRecv, int sizeRecv, char * messageSend, int * sizeSend)
{
	char messageIn  [MAX_MSG] = {0x00};
	char messageOut [MAX_MSG] = {0x00};
	ISO_NET_MODAL_T structMsgIn;
	ISO_NET_MODAL_T structMsgOut;

	memset(&structMsgIn, 0x00, sizeof(structMsgIn));
	memset(&structMsgOut, 0x00, sizeof(structMsgOut));

	strncpy(messageIn, messageRecv, sizeRecv);
	iso_NetworkParser(messageIn, &structMsgIn);
	iso_DumpNetStruct(&structMsgIn);

	if (strcmp(structMsgIn.managementCode, "001") == 0){
		iso_CommTest(&structMsgIn, &structMsgOut);
		iso_NetworkMaker(messageOut, &structMsgOut);
		iso_DumpNetStruct(&structMsgOut);
		strcpy(messageSend, messageOut);
		*sizeSend = strlen(messageOut);
	}else if (strcmp(structMsgIn.managementCode, "002") == 0){
		iso_InitProcess(&structMsgIn, &structMsgOut);
		iso_NetworkMaker(messageOut, &structMsgOut);
		iso_DumpNetStruct(&structMsgOut);
		strcpy(messageSend, messageOut);
		*sizeSend = strlen(messageOut);
	}else if (strcmp(structMsgIn.managementCode, "003") == 0){
		iso_KeepAliveProcess(&structMsgIn, &structMsgOut);
		iso_NetworkMaker(messageOut, &structMsgOut);
		iso_DumpNetStruct(&structMsgOut);
		strcpy(messageSend, messageOut);
		*sizeSend = strlen(messageOut);
	}else if (strcmp(structMsgIn.managementCode, "004") == 0){
		iso_ChargeProcess(&structMsgIn, &structMsgOut);
		iso_NetworkMaker(messageOut, &structMsgOut);
		iso_DumpNetStruct(&structMsgOut);
		strcpy(messageSend, messageOut);
		*sizeSend = strlen(messageOut);
	}

	
	return 0;
}

int iso_FinancialProcess(char * messageRecv, int sizeRecv, char * messageSend, int * sizeSend)
{
	char messageIn  [MAX_MSG] = {0x00};
	char messageOut [MAX_MSG] = {0x00};
	ISO_FIN_MODAL_T structMsgIn;
	ISO_FIN_MODAL_T structMsgOut;
	int result = 0;

	memset(&structMsgIn, 0x00, sizeof(structMsgIn));
	memset(&structMsgOut, 0x00, sizeof(structMsgOut));


	strncpy(messageIn, messageRecv, sizeRecv);
	iso_FinancialParser(messageIn, &structMsgIn);
	iso_DumpFinStruct(&structMsgIn);
	iso_SaveFinStruct(&structMsgIn);
	if (strlen(structMsgIn.pinBlock)){
		result = util_sendDataToHSM(structMsgIn.decryptData, structMsgIn.pinBlock, structMsgIn.cryptedData);
		if (result){
			strcpy(structMsgOut.responseCode, "55");
		}
	}
	result = iso_FinancialMockup(&structMsgIn, &structMsgOut);
	iso_UpdateFinStruct(&structMsgOut);
	iso_DumpFinStruct(&structMsgOut);
	//make CPV

	iso_FinancialMaker(messageOut, &structMsgOut);
	strcpy(messageSend, messageOut);
	*sizeSend = strlen(messageOut);

	return result;
}


int iso_ReversalProcess(char * messageRecv, int sizeRecv, char * messageSend, int * sizeSend)
{
	char messageIn  [MAX_MSG] = {0x00};
	char messageOut [MAX_MSG] = {0x00};
	ISO_REV_MODAL_T structMsgIn;
	ISO_REV_MODAL_T structMsgOut;
	int result = 0;

	memset(&structMsgIn, 0x00, sizeof(structMsgIn));
	memset(&structMsgOut, 0x00, sizeof(structMsgOut));

	strncpy(messageIn, messageRecv, sizeRecv);
	iso_ReversalParser(messageIn, &structMsgIn);
	iso_DumpRevStruct(&structMsgIn);
	// iso_SaveFinStruct(&structMsgIn);
	result = iso_ReversalMockup(&structMsgIn, &structMsgOut);
	iso_UpdateRevStruct(&structMsgOut);
	iso_DumpRevStruct(&structMsgOut);

	iso_ReversalMaker(messageOut, &structMsgOut);
	strcpy(messageSend, messageOut);
	*sizeSend = strlen(messageOut);

	return result;
}

int iso_NetworkParser(char * message, ISO_NET_MODAL_T * structMsg)
{
	char messageAux[MAX_MSG] = {0x00};
	char sizeAux[4] = {0x00};
	int lastPosition = 0;
	strcpy(messageAux, message);

	strncpy(structMsg->mti, &messageAux[lastPosition], 4);
	lastPosition += 4;
	strncpy(structMsg->bitmap, &messageAux[lastPosition], 16);
	lastPosition += 16;

	// POS 0
	if (util_ConvertCharToHex(structMsg->bitmap[0]) & 0x08){ //bit 1
		strncat(structMsg->bitmap, &messageAux[lastPosition], 16);
		lastPosition += 16;
	}else{
		strncat(structMsg->bitmap, "0000000000000000", 16);		
	}

	// POS 1
	if (util_ConvertCharToHex(structMsg->bitmap[1]) & 0x02){ //bit 7
		strncpy(structMsg->gmtDateTime, &messageAux[lastPosition], 10);
		lastPosition += 10;
	}

	// POS 2
	if (util_ConvertCharToHex(structMsg->bitmap[2]) & 0x02){ //bit 11
		strncpy(structMsg->localNSU, &messageAux[lastPosition], 6);
		lastPosition += 6;
	}
	if (util_ConvertCharToHex(structMsg->bitmap[2]) & 0x01){ //bit 12
		strncpy(structMsg->localTime, &messageAux[lastPosition], 6);
		lastPosition += 6;
	}

	// POS 3
	if (util_ConvertCharToHex(structMsg->bitmap[3]) & 0x08){ //bit 13
		strncpy(structMsg->localDate, &messageAux[lastPosition], 4);
		lastPosition += 4;
	}

	// POS 4
	if (util_ConvertCharToHex(structMsg->bitmap[4]) & 0x02){ //bit 19
		strncpy(structMsg->countryCode, &messageAux[lastPosition], 3);
		lastPosition += 3;
	}

	// POS 9
	if (util_ConvertCharToHex(structMsg->bitmap[9]) & 0x02){ //bit 39
		strncpy(structMsg->responseCode, &messageAux[lastPosition], 2);
		lastPosition += 2;
	}

	// POS 10
	if (util_ConvertCharToHex(structMsg->bitmap[10]) & 0x08){ //bit 41
		strncpy(structMsg->serialNumber, &messageAux[lastPosition], 16);
		lastPosition += 16;
	}
	if (util_ConvertCharToHex(structMsg->bitmap[10]) & 0x04){ //bit 42
		strncpy(structMsg->ecDocument, &messageAux[lastPosition], 20);
		lastPosition += 20;
	}

	// POS 15
	if (util_ConvertCharToHex(structMsg->bitmap[15]) & 0x08){ //bit 61
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->terminalInfo, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}
	if (util_ConvertCharToHex(structMsg->bitmap[15]) & 0x04){ //bit 62
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->tableTrade1, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}
	if (util_ConvertCharToHex(structMsg->bitmap[15]) & 0x02){ //bit 63
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->tableTrade2, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	// POS 17
	if (util_ConvertCharToHex(structMsg->bitmap[17]) & 0x04){ //bit 70
		strncpy(structMsg->managementCode, &messageAux[lastPosition], 3);
		lastPosition += 3;
	}

	// POS 29
	if (util_ConvertCharToHex(structMsg->bitmap[29]) & 0x01){ //bit 120
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->versionDetail, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	// POS 30
	if (util_ConvertCharToHex(structMsg->bitmap[30]) & 0x02){ //bit 123
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->simCardInfo, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	// POS 31
	if (util_ConvertCharToHex(structMsg->bitmap[31]) & 0x01){ //bit 123
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->acquirerNSU, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	return 0;
}

int iso_FinancialParser(char * message, ISO_FIN_MODAL_T * structMsg)
{
	char messageAux[MAX_MSG] = {0x00};
	char sizeAux[4] = {0x00};
	int lastPosition = 0;
	strcpy(messageAux, message);

	strncpy(structMsg->mti, &messageAux[lastPosition], 4);
	lastPosition += 4;
	strncpy(structMsg->bitmap, &messageAux[lastPosition], 16);
	lastPosition += 16;

	// POS 0
	if (util_ConvertCharToHex(structMsg->bitmap[0]) & 0x08){ //bit 1
		strncat(structMsg->bitmap, &messageAux[lastPosition], 16);
		lastPosition += 16;
	}else{
		strncat(structMsg->bitmap, "0000000000000000", 16);		
	}
	if (util_ConvertCharToHex(structMsg->bitmap[0]) & 0x02){ //bit 3
		strncpy(structMsg->processCode, &messageAux[lastPosition], 6);
		lastPosition += 6;
	}
	if (util_ConvertCharToHex(structMsg->bitmap[0]) & 0x01){ //bit 4
		strncpy(structMsg->amount, &messageAux[lastPosition], 12);
		lastPosition += 12;
	}

	// POS 1
	if (util_ConvertCharToHex(structMsg->bitmap[1]) & 0x02){ //bit 7
		strncpy(structMsg->gmtDateTime, &messageAux[lastPosition], 10);
		lastPosition += 10;
	}

	// POS 2
	if (util_ConvertCharToHex(structMsg->bitmap[2]) & 0x02){ //bit 11
		strncpy(structMsg->localNSU, &messageAux[lastPosition], 6);
		lastPosition += 6;
	}
	if (util_ConvertCharToHex(structMsg->bitmap[2]) & 0x01){ //bit 12
		strncpy(structMsg->localTime, &messageAux[lastPosition], 6);
		lastPosition += 6;
	}

	// POS 3
	if (util_ConvertCharToHex(structMsg->bitmap[3]) & 0x08){ //bit 13
		strncpy(structMsg->localDate, &messageAux[lastPosition], 4);
		lastPosition += 4;
	}

	// POS 4
	if (util_ConvertCharToHex(structMsg->bitmap[4]) & 0x02){ //bit 19
		strncpy(structMsg->countryCode, &messageAux[lastPosition], 3);
		lastPosition += 3;
	}

	// POS 5
	if (util_ConvertCharToHex(structMsg->bitmap[5]) & 0x04){ //bit 22
		strncpy(structMsg->entryMode, &messageAux[lastPosition], 3);
		lastPosition += 3;
	}
	if (util_ConvertCharToHex(structMsg->bitmap[5]) & 0x02){ //bit 23
		strncpy(structMsg->PanSeqNumber, &messageAux[lastPosition], 3);
		lastPosition += 3;
	}

	// POS 9
	if (util_ConvertCharToHex(structMsg->bitmap[9]) & 0x04){ //bit 38
		strncpy(structMsg->hostNSU, &messageAux[lastPosition], 6);
		lastPosition += 6;
	}
	if (util_ConvertCharToHex(structMsg->bitmap[9]) & 0x02){ //bit 39
		strncpy(structMsg->responseCode, &messageAux[lastPosition], 2);
		lastPosition += 2;
	}

	// POS 10
	if (util_ConvertCharToHex(structMsg->bitmap[10]) & 0x08){ //bit 41
		strncpy(structMsg->serialNumber, &messageAux[lastPosition], 16);
		lastPosition += 16;
	}
	if (util_ConvertCharToHex(structMsg->bitmap[10]) & 0x04){ //bit 42
		strncpy(structMsg->ecDocument, &messageAux[lastPosition], 20);
		lastPosition += 20;
	}

	// POS 12
	if (util_ConvertCharToHex(structMsg->bitmap[12]) & 0x08){ //bit 49
		strncpy(structMsg->currencyCode, &messageAux[lastPosition], 3);
		lastPosition += 3;
	}
	if (util_ConvertCharToHex(structMsg->bitmap[12]) & 0x01){ //bit 52
		strncpy(structMsg->pinBlock, &messageAux[lastPosition], 16);
		lastPosition += 16;
	}

	// POS 13
	if (util_ConvertCharToHex(structMsg->bitmap[13]) & 0x02){ //bit 55
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->emvData, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	// POS 14
	if (util_ConvertCharToHex(structMsg->bitmap[14]) & 0x01){ //bit 60
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->lastTrsData, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	// POS 15
	if (util_ConvertCharToHex(structMsg->bitmap[15]) & 0x08){ //bit 61
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->terminalInfo, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}
	if (util_ConvertCharToHex(structMsg->bitmap[15]) & 0x04){ //bit 62
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->receiptEC, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}
	if (util_ConvertCharToHex(structMsg->bitmap[15]) & 0x02){ //bit 63
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->adtTrsData, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	// POS 16
	if (util_ConvertCharToHex(structMsg->bitmap[16]) & 0x02){ //bit 67
		strncpy(structMsg->installments, &messageAux[lastPosition], 2);
		lastPosition += 2;
	}

	// POS 17
	if (util_ConvertCharToHex(structMsg->bitmap[17]) & 0x04){ //bit 70
		strncpy(structMsg->managementCode, &messageAux[lastPosition], 3);
		lastPosition += 3;
	}

	// POS 22
	if (util_ConvertCharToHex(structMsg->bitmap[22]) & 0x04){ //bit 90
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->primalTrsData, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	// POS 29
	if (util_ConvertCharToHex(structMsg->bitmap[29]) & 0x04){ //bit 118
		strncpy(structMsg->paymentMethod, &messageAux[lastPosition], 2);
		lastPosition += 2;
	}
	if (util_ConvertCharToHex(structMsg->bitmap[29]) & 0x01){ //bit 120
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->versionDetail, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	// POS 30
	if (util_ConvertCharToHex(structMsg->bitmap[30]) & 0x08){ //bit 121
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->decryptData, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}
	if (util_ConvertCharToHex(structMsg->bitmap[30]) & 0x02){ //bit 123
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->simCardInfo, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}
	if (util_ConvertCharToHex(structMsg->bitmap[30]) & 0x01){ //bit 124
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->receiptHolder, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	// POS 31
	if (util_ConvertCharToHex(structMsg->bitmap[31]) & 0x08){ //bit 125
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->cryptedData, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}
	if (util_ConvertCharToHex(structMsg->bitmap[31]) & 0x02){ //bit 127
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->acquirerNSU, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	return 0;
}

int iso_ReversalParser(char * message, ISO_REV_MODAL_T * structMsg)
{
	char messageAux[MAX_MSG] = {0x00};
	char sizeAux[4] = {0x00};
	int lastPosition = 0;
	strcpy(messageAux, message);

	strncpy(structMsg->mti, &messageAux[lastPosition], 4);
	lastPosition += 4;
	strncpy(structMsg->bitmap, &messageAux[lastPosition], 16);
	lastPosition += 16;

	// POS 0
	if (util_ConvertCharToHex(structMsg->bitmap[0]) & 0x08){ //bit 1
		strncat(structMsg->bitmap, &messageAux[lastPosition], 16);
		lastPosition += 16;
	}else{
		strncat(structMsg->bitmap, "0000000000000000", 16);		
	}
	if (util_ConvertCharToHex(structMsg->bitmap[0]) & 0x02){ //bit 3
		strncpy(structMsg->processCode, &messageAux[lastPosition], 6);
		lastPosition += 6;
	}
	if (util_ConvertCharToHex(structMsg->bitmap[0]) & 0x01){ //bit 4
		strncpy(structMsg->amount, &messageAux[lastPosition], 12);
		lastPosition += 12;
	}

	// POS 1
	if (util_ConvertCharToHex(structMsg->bitmap[1]) & 0x02){ //bit 7
		strncpy(structMsg->gmtDateTime, &messageAux[lastPosition], 10);
		lastPosition += 10;
	}

	// POS 2
	if (util_ConvertCharToHex(structMsg->bitmap[2]) & 0x02){ //bit 11
		strncpy(structMsg->localNSU, &messageAux[lastPosition], 6);
		lastPosition += 6;
	}
	if (util_ConvertCharToHex(structMsg->bitmap[2]) & 0x01){ //bit 12
		strncpy(structMsg->localTime, &messageAux[lastPosition], 6);
		lastPosition += 6;
	}

	// POS 3
	if (util_ConvertCharToHex(structMsg->bitmap[3]) & 0x08){ //bit 13
		strncpy(structMsg->localDate, &messageAux[lastPosition], 4);
		lastPosition += 4;
	}

	// POS 5
	if (util_ConvertCharToHex(structMsg->bitmap[5]) & 0x04){ //bit 22
		strncpy(structMsg->entryMode, &messageAux[lastPosition], 3);
		lastPosition += 3;
	}
	if (util_ConvertCharToHex(structMsg->bitmap[5]) & 0x02){ //bit 23
		strncpy(structMsg->PanSeqNumber, &messageAux[lastPosition], 3);
		lastPosition += 3;
	}

	// POS 9
	if (util_ConvertCharToHex(structMsg->bitmap[9]) & 0x04){ //bit 38
		strncpy(structMsg->hostNSU, &messageAux[lastPosition], 6);
		lastPosition += 6;
	}
	if (util_ConvertCharToHex(structMsg->bitmap[9]) & 0x02){ //bit 39
		strncpy(structMsg->responseCode, &messageAux[lastPosition], 2);
		lastPosition += 2;
	}

	// POS 10
	if (util_ConvertCharToHex(structMsg->bitmap[10]) & 0x08){ //bit 41
		strncpy(structMsg->serialNumber, &messageAux[lastPosition], 16);
		lastPosition += 16;
	}
	if (util_ConvertCharToHex(structMsg->bitmap[10]) & 0x04){ //bit 42
		strncpy(structMsg->ecDocument, &messageAux[lastPosition], 20);
		lastPosition += 20;
	}

	// POS 12
	if (util_ConvertCharToHex(structMsg->bitmap[12]) & 0x08){ //bit 49
		strncpy(structMsg->currencyCode, &messageAux[lastPosition], 3);
		lastPosition += 3;
	}

	// POS 13
	if (util_ConvertCharToHex(structMsg->bitmap[13]) & 0x02){ //bit 55
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->emvData, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	// POS 14
	if (util_ConvertCharToHex(structMsg->bitmap[14]) & 0x01){ //bit 60
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->lastTrsData, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	// POS 15
	if (util_ConvertCharToHex(structMsg->bitmap[15]) & 0x08){ //bit 61
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->terminalInfo, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}
	if (util_ConvertCharToHex(structMsg->bitmap[15]) & 0x04){ //bit 62
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->receiptEC, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}
	if (util_ConvertCharToHex(structMsg->bitmap[15]) & 0x02){ //bit 63
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->adtTrsData, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	// POS 17
	if (util_ConvertCharToHex(structMsg->bitmap[17]) & 0x04){ //bit 70
		strncpy(structMsg->managementCode, &messageAux[lastPosition], 3);
		lastPosition += 3;
	}

	// POS 22
	if (util_ConvertCharToHex(structMsg->bitmap[22]) & 0x04){ //bit 90
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->primalTrsData, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	// POS 29
	if (util_ConvertCharToHex(structMsg->bitmap[29]) & 0x01){ //bit 120
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->versionDetail, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	// POS 30
	if (util_ConvertCharToHex(structMsg->bitmap[30]) & 0x08){ //bit 121
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->decryptData, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}
	if (util_ConvertCharToHex(structMsg->bitmap[30]) & 0x02){ //bit 123
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->simCardInfo, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}
	if (util_ConvertCharToHex(structMsg->bitmap[30]) & 0x01){ //bit 124
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->receiptHolder, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	// POS 31
	if (util_ConvertCharToHex(structMsg->bitmap[31]) & 0x08){ //bit 125
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->cryptedData, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}
	if (util_ConvertCharToHex(structMsg->bitmap[31]) & 0x02){ //bit 127
		memset(sizeAux, 0x00, sizeof(sizeAux));
		strncpy(sizeAux, &messageAux[lastPosition], 3);
		lastPosition += 3;
		strncpy(structMsg->acquirerNSU, &messageAux[lastPosition], atoi(sizeAux));
		lastPosition += atoi(sizeAux);
	}

	return 0;
}

int iso_NetworkMaker(char * message, ISO_NET_MODAL_T * structMsg)
{
	char messageAux[MAX_MSG] = {0x00};
	char sizeAux[4] = {0x00};
	int  lastPosition = 0;
	int  secondBitmap = 0;
	int  currentByte  = 0;

	//Injetando bmpDefault
	strcpy(structMsg->bitmap, "80000000000000000000000000000000");
	
	// structMsg->mti
	if (strlen(structMsg->mti) > 0){
		strncpy(messageAux, structMsg->mti, 4);
	}

	// structMsg->bitmap
	if (strlen(structMsg->bitmap) > 0){
		strncat(messageAux, structMsg->bitmap, 32);
	}

	//Byte 1
	if (strlen(structMsg->gmtDateTime) > 0){ //bit 7
		currentByte = 1;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		strncat(messageAux, structMsg->gmtDateTime, 10);
	}

	//Byte 2
	if (strlen(structMsg->localNSU) > 0){ //bit 11
		currentByte = 2;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		strncat(messageAux, structMsg->localNSU, 6);
	}
	if (strlen(structMsg->localTime) > 0){ //bit 12
		currentByte = 2;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x01);
		strncat(messageAux, structMsg->localTime, 6);
	}

	//Byte 3
	if (strlen(structMsg->localDate) > 0){ //bit 13
		currentByte = 3;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x08);
		strncat(messageAux, structMsg->localDate, 4);
	}

	//Byte 4
	if (strlen(structMsg->countryCode) > 0){ //bit 19
		currentByte = 4;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		printf("CountryCode [%s]\n", structMsg->countryCode);
		strncat(messageAux, structMsg->countryCode, 3);
	}

	//Byte 9
	if (strlen(structMsg->responseCode) > 0){ //bit 39
		currentByte = 9;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		strncat(messageAux, structMsg->responseCode, 2);
	}

	//Byte 10
	if (strlen(structMsg->serialNumber) > 0){ //bit 41
		currentByte = 10;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x08);
		strncat(messageAux, structMsg->serialNumber, 16);
	}
	if (strlen(structMsg->ecDocument) > 0){ //bit 42
		currentByte = 10;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x04);
		strncat(messageAux, structMsg->ecDocument, 20);
	}

	//Byte 15
	if (strlen(structMsg->terminalInfo) > 0){ //bit 61
		currentByte = 15;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x08);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->terminalInfo));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->terminalInfo, strlen(structMsg->terminalInfo));
	}
	if (strlen(structMsg->tableTrade1) > 0){ //bit 62
		currentByte = 15;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x04);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->tableTrade1));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->tableTrade1, strlen(structMsg->tableTrade1));
	}
	if (strlen(structMsg->tableTrade2) > 0){ //bit 63
		currentByte = 15;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->tableTrade2));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->tableTrade2, strlen(structMsg->tableTrade2));
	}

	//Byte 17
	if (strlen(structMsg->managementCode) > 0){ //bit 70
		currentByte = 17;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x04);
		strncat(messageAux, structMsg->managementCode, 3);
	}

	//Byte 29
	if (strlen(structMsg->versionDetail) > 0){ //bit 120
		currentByte = 29;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x01);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->versionDetail));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->versionDetail, strlen(structMsg->versionDetail));
	}

	//Byte 30
	if (strlen(structMsg->simCardInfo) > 0){ //bit 123
		currentByte = 30;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->simCardInfo));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->simCardInfo, strlen(structMsg->simCardInfo));
	}

	//Byte 31
	if (strlen(structMsg->acquirerNSU) > 0){ //bit 127
		currentByte = 31;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->acquirerNSU));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->acquirerNSU, strlen(structMsg->acquirerNSU));
	}

	// structMsg->bitmap
	if (strlen(structMsg->bitmap) > 0){
		strncpy(&messageAux[4], structMsg->bitmap, 32);
	}

	strcpy(message, messageAux);

	return 0;
}

int iso_FinancialMaker(char * message, ISO_FIN_MODAL_T * structMsg)
{
	char messageAux[MAX_MSG] = {0x00};
	char sizeAux[4] = {0x00};
	int  lastPosition = 0;
	int  secondBitmap = 0;
	int  currentByte  = 0;

	//Injetando bmpDefault
	strcpy(structMsg->bitmap, "80000000000000000000000000000000");
	
	// structMsg->mti
	if (strlen(structMsg->mti) > 0){
		strncpy(messageAux, structMsg->mti, 4);
	}

	// structMsg->bitmap
	if (strlen(structMsg->bitmap) > 0){
		strncat(messageAux, structMsg->bitmap, 32);
	}

	//Byte 0
	if (strlen(structMsg->processCode) > 0){ //bit 3
		currentByte = 0;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		strncat(messageAux, structMsg->processCode, 6);
	}
	if (strlen(structMsg->amount) > 0){ //bit 4
		currentByte = 0;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x01);
		strncat(messageAux, structMsg->amount, 12);
	}

	//Byte 1
	if (strlen(structMsg->gmtDateTime) > 0){ //bit 7
		currentByte = 1;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		strncat(messageAux, structMsg->gmtDateTime, 10);
	}

	//Byte 2
	if (strlen(structMsg->localNSU) > 0){ //bit 11
		currentByte = 2;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		strncat(messageAux, structMsg->localNSU, 6);
	}
	if (strlen(structMsg->localTime) > 0){ //bit 12
		currentByte = 2;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x01);
		strncat(messageAux, structMsg->localTime, 6);
	}

	//Byte 3
	if (strlen(structMsg->localDate) > 0){ //bit 13
		currentByte = 3;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x08);
		strncat(messageAux, structMsg->localDate, 4);
	}

	//Byte 4
	if (strlen(structMsg->countryCode) > 0){ //bit 19
		currentByte = 4;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		strncat(messageAux, structMsg->countryCode, 3);
	}

	//Byte 5
	if (strlen(structMsg->entryMode) > 0){ //bit 22
		currentByte = 5;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x04);
		strncat(messageAux, structMsg->entryMode, 3);
	}
	if (strlen(structMsg->PanSeqNumber) > 0){ //bit 23
		currentByte = 5;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		strncat(messageAux, structMsg->PanSeqNumber, 3);
	}

	//Byte 9
	if (strlen(structMsg->PanSeqNumber) > 0){ //bit 38
		currentByte = 9;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x04);
		strncat(messageAux, structMsg->PanSeqNumber, 6);
	}
	if (strlen(structMsg->responseCode) > 0){ //bit 39
		currentByte = 9;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		strncat(messageAux, structMsg->responseCode, 2);
	}

	//Byte 10
	if (strlen(structMsg->serialNumber) > 0){ //bit 41
		currentByte = 10;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x08);
		strncat(messageAux, structMsg->serialNumber, 16);
	}
	if (strlen(structMsg->ecDocument) > 0){ //bit 42
		currentByte = 10;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x04);
		strncat(messageAux, structMsg->ecDocument, 20);
	}

	//Byte 12
	if (strlen(structMsg->currencyCode) > 0){ //bit 49
		currentByte = 12;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x08);
		strncat(messageAux, structMsg->currencyCode, 3);
	}
	if (strlen(structMsg->pinBlock) > 0){ //bit 52
		currentByte = 12;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x01);
		strncat(messageAux, structMsg->pinBlock, 16);
	}

	//Byte 13
	if (strlen(structMsg->emvData) > 0){ //bit 55
		currentByte = 13;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->emvData));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->emvData, strlen(structMsg->emvData));
	}

	//Byte 14
	if (strlen(structMsg->lastTrsData) > 0){ //bit 60
		currentByte = 14;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x01);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->lastTrsData));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->lastTrsData, strlen(structMsg->lastTrsData));
	}

	//Byte 15
	if (strlen(structMsg->terminalInfo) > 0){ //bit 61
		currentByte = 15;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x08);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->terminalInfo));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->terminalInfo, strlen(structMsg->terminalInfo));
	}
	if (strlen(structMsg->receiptEC) > 0){ //bit 62
		currentByte = 15;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x04);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->receiptEC));
		// printf("structMsg->receiptEC(%d) = [%s]\n", (int)strlen(structMsg->receiptEC), structMsg->receiptEC);
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->receiptEC, strlen(structMsg->receiptEC));
	}
	if (strlen(structMsg->adtTrsData) > 0){ //bit 63
		currentByte = 15;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->adtTrsData));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->adtTrsData, strlen(structMsg->adtTrsData));
	}

	//Byte 16
	if (strlen(structMsg->installments) > 0){ //bit 67
		currentByte = 16;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		strncat(messageAux, structMsg->installments, 2);
	}

	//Byte 17
	if (strlen(structMsg->managementCode) > 0){ //bit 70
		currentByte = 17;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x04);
		strncat(messageAux, structMsg->managementCode, 3);
	}

	//Byte 22
	if (strlen(structMsg->primalTrsData) > 0){ //bit 90
		currentByte = 22;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x04);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->primalTrsData));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->primalTrsData, strlen(structMsg->primalTrsData));
	}

	//Byte 29
	if (strlen(structMsg->paymentMethod) > 0){ //bit 118
		currentByte = 29;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x04);
		strncat(messageAux, structMsg->paymentMethod, 2);
	}
	if (strlen(structMsg->versionDetail) > 0){ //bit 120
		currentByte = 29;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x01);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->versionDetail));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->versionDetail, strlen(structMsg->versionDetail));
	}

	//Byte 30
	if (strlen(structMsg->decryptData) > 0){ //bit 121
		currentByte = 30;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x08);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->decryptData));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->decryptData, strlen(structMsg->decryptData));
	}
	if (strlen(structMsg->simCardInfo) > 0){ //bit 123
		currentByte = 30;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->simCardInfo));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->simCardInfo, strlen(structMsg->simCardInfo));
	}
	if (strlen(structMsg->receiptHolder) > 0){ //bit 124
		currentByte = 30;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x01);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->receiptHolder));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->receiptHolder, strlen(structMsg->receiptHolder));
	}

	//Byte 31
	if (strlen(structMsg->cryptedData) > 0){ //bit 125
		currentByte = 31;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x08);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->cryptedData));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->cryptedData, strlen(structMsg->cryptedData));
	}
	if (strlen(structMsg->acquirerNSU) > 0){ //bit 127
		currentByte = 31;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->acquirerNSU));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->acquirerNSU, strlen(structMsg->acquirerNSU));
	}

	//Implementar mecanismo para tirar segundo bloco de bitmap quando não for necessário
	// structMsg->bitmap
	if (strlen(structMsg->bitmap) > 0){
		strncpy(&messageAux[4], structMsg->bitmap, 32);
	}

	strcpy(message, messageAux);

	return 0;
}

int iso_ReversalMaker(char * message, ISO_REV_MODAL_T * structMsg)
{
	char messageAux[MAX_MSG] = {0x00};
	char sizeAux[4] = {0x00};
	int  lastPosition = 0;
	int  secondBitmap = 0;
	int  currentByte  = 0;

	//Injetando bmpDefault
	strcpy(structMsg->bitmap, "80000000000000000000000000000000");
	
	// structMsg->mti
	if (strlen(structMsg->mti) > 0){
		strncpy(messageAux, structMsg->mti, 4);
	}

	// structMsg->bitmap
	if (strlen(structMsg->bitmap) > 0){
		strncat(messageAux, structMsg->bitmap, 32);
	}

	//Byte 0
	if (strlen(structMsg->processCode) > 0){ //bit 3
		currentByte = 0;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		strncat(messageAux, structMsg->processCode, 6);
	}
	if (strlen(structMsg->amount) > 0){ //bit 4
		currentByte = 0;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x01);
		strncat(messageAux, structMsg->amount, 12);
	}

	//Byte 1
	if (strlen(structMsg->gmtDateTime) > 0){ //bit 7
		currentByte = 1;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		strncat(messageAux, structMsg->gmtDateTime, 10);
	}

	//Byte 2
	if (strlen(structMsg->localNSU) > 0){ //bit 11
		currentByte = 2;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		strncat(messageAux, structMsg->localNSU, 6);
	}
	if (strlen(structMsg->localTime) > 0){ //bit 12
		currentByte = 2;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x01);
		strncat(messageAux, structMsg->localTime, 6);
	}

	//Byte 3
	if (strlen(structMsg->localDate) > 0){ //bit 13
		currentByte = 3;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x08);
		strncat(messageAux, structMsg->localDate, 4);
	}

	//Byte 5
	if (strlen(structMsg->entryMode) > 0){ //bit 22
		currentByte = 5;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x04);
		strncat(messageAux, structMsg->entryMode, 3);
	}
	if (strlen(structMsg->PanSeqNumber) > 0){ //bit 23
		currentByte = 5;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		strncat(messageAux, structMsg->PanSeqNumber, 3);
	}

	//Byte 9
	if (strlen(structMsg->PanSeqNumber) > 0){ //bit 38
		currentByte = 9;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x04);
		strncat(messageAux, structMsg->PanSeqNumber, 6);
	}
	if (strlen(structMsg->responseCode) > 0){ //bit 39
		currentByte = 9;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		strncat(messageAux, structMsg->responseCode, 2);
	}

	//Byte 10
	if (strlen(structMsg->serialNumber) > 0){ //bit 41
		currentByte = 10;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x08);
		strncat(messageAux, structMsg->serialNumber, 16);
	}
	if (strlen(structMsg->ecDocument) > 0){ //bit 42
		currentByte = 10;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x04);
		strncat(messageAux, structMsg->ecDocument, 20);
	}

	//Byte 12
	if (strlen(structMsg->currencyCode) > 0){ //bit 49
		currentByte = 12;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x08);
		strncat(messageAux, structMsg->currencyCode, 3);
	}

	//Byte 13
	if (strlen(structMsg->emvData) > 0){ //bit 55
		currentByte = 13;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->emvData));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->emvData, strlen(structMsg->emvData));
	}

	//Byte 14
	if (strlen(structMsg->lastTrsData) > 0){ //bit 60
		currentByte = 14;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x01);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->lastTrsData));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->lastTrsData, strlen(structMsg->lastTrsData));
	}

	//Byte 15
	if (strlen(structMsg->terminalInfo) > 0){ //bit 61
		currentByte = 15;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x08);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->terminalInfo));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->terminalInfo, strlen(structMsg->terminalInfo));
	}
	if (strlen(structMsg->receiptEC) > 0){ //bit 62
		currentByte = 15;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x04);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->receiptEC));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->receiptEC, strlen(structMsg->receiptEC));
	}
	if (strlen(structMsg->adtTrsData) > 0){ //bit 63
		currentByte = 15;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->adtTrsData));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->adtTrsData, strlen(structMsg->adtTrsData));
	}

	//Byte 17
	if (strlen(structMsg->managementCode) > 0){ //bit 70
		currentByte = 17;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x04);
		strncat(messageAux, structMsg->managementCode, 3);
	}

	//Byte 22
	if (strlen(structMsg->primalTrsData) > 0){ //bit 90
		currentByte = 22;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x04);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->primalTrsData));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->primalTrsData, strlen(structMsg->primalTrsData));
	}

	//Byte 29
	if (strlen(structMsg->versionDetail) > 0){ //bit 120
		currentByte = 29;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x01);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->versionDetail));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->versionDetail, strlen(structMsg->versionDetail));
	}

	//Byte 30
	if (strlen(structMsg->decryptData) > 0){ //bit 121
		currentByte = 30;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x08);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->decryptData));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->decryptData, strlen(structMsg->decryptData));
	}
	if (strlen(structMsg->simCardInfo) > 0){ //bit 123
		currentByte = 30;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->simCardInfo));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->simCardInfo, strlen(structMsg->simCardInfo));
	}
	if (strlen(structMsg->receiptHolder) > 0){ //bit 124
		currentByte = 30;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x01);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->receiptHolder));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->receiptHolder, strlen(structMsg->receiptHolder));
	}

	//Byte 31
	if (strlen(structMsg->cryptedData) > 0){ //bit 125
		currentByte = 31;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x08);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->cryptedData));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->cryptedData, strlen(structMsg->cryptedData));
	}
	if (strlen(structMsg->acquirerNSU) > 0){ //bit 127
		currentByte = 31;
		structMsg->bitmap[currentByte] = util_AddBitToByte(structMsg->bitmap[currentByte], 0x02);
		memset(sizeAux, 0x00, sizeof(sizeAux));
		sprintf(sizeAux, "%03d", (int)strlen(structMsg->acquirerNSU));
		strncat(messageAux, sizeAux, 3); 
		strncat(messageAux, structMsg->acquirerNSU, strlen(structMsg->acquirerNSU));
	}

	//Implementar mecanismo para tirar segundo bloco de bitmap quando não for necessário
	// structMsg->bitmap
	if (strlen(structMsg->bitmap) > 0){
		strncpy(&messageAux[4], structMsg->bitmap, 32);
	}

	strcpy(message, messageAux);

	return 0;
}

void iso_DumpNetStruct(ISO_NET_MODAL_T * structMsg)
{
	printf("########################\n#       ISO DUMP       #\n########################\n");
	printf("mti            = [%s]\n", structMsg->mti);
	printf("bitmap         = [%s]\n", structMsg->bitmap);
	printf("gmtDateTime    = [%s]\n", structMsg->gmtDateTime);
	printf("localNSU       = [%s]\n", structMsg->localNSU);
	printf("localTime      = [%s]\n", structMsg->localTime);
	printf("localDate      = [%s]\n", structMsg->localDate);
	printf("countryCode    = [%s]\n", structMsg->countryCode);
	printf("responseCode   = [%s]\n", structMsg->responseCode);
	printf("serialNumber   = [%s]\n", structMsg->serialNumber);
	printf("ecDocument     = [%s]\n", structMsg->ecDocument);
	printf("terminalInfo   = [%s]\n", structMsg->terminalInfo);
	printf("tableTrade1    = [%s]\n", structMsg->tableTrade1);
	printf("tableTrade2    = [%s]\n", structMsg->tableTrade2);
	printf("managementCode = [%s]\n", structMsg->managementCode);
	printf("versionDetail  = [%s]\n", structMsg->versionDetail);
	printf("simCardInfo    = [%s]\n", structMsg->simCardInfo);
	printf("acquirerNSU    = [%s]\n", structMsg->acquirerNSU);
	return;
}

void iso_DumpFinStruct(ISO_FIN_MODAL_T * structMsg)
{
	printf("########################\n#       ISO DUMP       #\n########################\n");
	printf("mti            = [%s]\n", structMsg->mti);
	printf("bitmap         = [%s]\n", structMsg->bitmap);
	printf("processCode    = [%s]\n", structMsg->processCode);
	printf("amount         = [%s]\n", structMsg->amount);
	printf("gmtDateTime    = [%s]\n", structMsg->gmtDateTime);
	printf("localNSU       = [%s]\n", structMsg->localNSU);
	printf("localTime      = [%s]\n", structMsg->localTime);
	printf("localDate      = [%s]\n", structMsg->localDate);
	printf("countryCode    = [%s]\n", structMsg->countryCode);
	printf("entryMode      = [%s]\n", structMsg->entryMode);
	printf("PanSeqNumber   = [%s]\n", structMsg->PanSeqNumber);
	printf("hostNSU        = [%s]\n", structMsg->hostNSU);
	printf("responseCode   = [%s]\n", structMsg->responseCode);
	printf("serialNumber   = [%s]\n", structMsg->serialNumber);
	printf("ecDocument     = [%s]\n", structMsg->ecDocument);
	printf("currencyCode   = [%s]\n", structMsg->currencyCode);
	printf("pinBlock       = [%s]\n", structMsg->pinBlock);
	printf("emvData        = [%s]\n", structMsg->emvData);
	printf("lastTrsData    = [%s]\n", structMsg->lastTrsData);
	printf("terminalInfo   = [%s]\n", structMsg->terminalInfo);
	printf("receiptEC      = [%s]\n", structMsg->receiptEC);
	printf("adtTrsData     = [%s]\n", structMsg->adtTrsData);
	printf("installments   = [%s]\n", structMsg->installments);
	printf("managementCode = [%s]\n", structMsg->managementCode);
	printf("primalTrsData  = [%s]\n", structMsg->primalTrsData);
	printf("paymentMethod  = [%s]\n", structMsg->paymentMethod);
	printf("versionDetail  = [%s]\n", structMsg->versionDetail);
	printf("decryptData    = [%s]\n", structMsg->decryptData);
	printf("simCardInfo    = [%s]\n", structMsg->simCardInfo);
	printf("receiptHolder  = [%s]\n", structMsg->receiptHolder);
	printf("cryptedData    = [%s]\n", structMsg->cryptedData);
	printf("acquirerNSU    = [%s]\n", structMsg->acquirerNSU);
	return;
}

void iso_DumpRevStruct(ISO_REV_MODAL_T * structMsg)
{
	printf("########################\n#       ISO DUMP       #\n########################\n");
	printf("mti            = [%s]\n", structMsg->mti);
	printf("bitmap         = [%s]\n", structMsg->bitmap);
	printf("processCode    = [%s]\n", structMsg->processCode);
	printf("amount         = [%s]\n", structMsg->amount);
	printf("gmtDateTime    = [%s]\n", structMsg->gmtDateTime);
	printf("localNSU       = [%s]\n", structMsg->localNSU);
	printf("localTime      = [%s]\n", structMsg->localTime);
	printf("localDate      = [%s]\n", structMsg->localDate);
	printf("entryMode      = [%s]\n", structMsg->entryMode);
	printf("PanSeqNumber   = [%s]\n", structMsg->PanSeqNumber);
	printf("hostNSU        = [%s]\n", structMsg->hostNSU);
	printf("responseCode   = [%s]\n", structMsg->responseCode);
	printf("serialNumber   = [%s]\n", structMsg->serialNumber);
	printf("ecDocument     = [%s]\n", structMsg->ecDocument);
	printf("currencyCode   = [%s]\n", structMsg->currencyCode);
	printf("emvData        = [%s]\n", structMsg->emvData);
	printf("lastTrsData    = [%s]\n", structMsg->lastTrsData);
	printf("terminalInfo   = [%s]\n", structMsg->terminalInfo);
	printf("receiptEC      = [%s]\n", structMsg->receiptEC);
	printf("adtTrsData     = [%s]\n", structMsg->adtTrsData);
	printf("managementCode = [%s]\n", structMsg->managementCode);
	printf("primalTrsData  = [%s]\n", structMsg->primalTrsData);
	printf("versionDetail  = [%s]\n", structMsg->versionDetail);
	printf("decryptData    = [%s]\n", structMsg->decryptData);
	printf("simCardInfo    = [%s]\n", structMsg->simCardInfo);
	printf("receiptHolder  = [%s]\n", structMsg->receiptHolder);
	printf("cryptedData    = [%s]\n", structMsg->cryptedData);
	printf("acquirerNSU    = [%s]\n", structMsg->acquirerNSU);
	return;
}

int iso_SaveFinStruct(ISO_FIN_MODAL_T * structMsg)
{
	char fields [512+1] = {0x00};
	char values[2560+1] = {0x00};
	char nsu_host [8+1] = {0x00};
	int rc              = RC_OK;
	int  nsuAux         = 0;
	if (util_Config_GetTableField(CFG_NSU_HOST, nsu_host) == RC_OK)
		nsuAux = atoi(nsu_host) + 1;

    sprintf (fields, "'%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s'",
		SQLITE_FIELD_MTI           , SQLITE_FIELD_PROCESS_CODE  , SQLITE_FIELD_AMOUNT         ,
		SQLITE_FIELD_DATETIME      , SQLITE_FIELD_NSU_TERMINAL  , SQLITE_FIELD_LOCAL_TIME     ,
		SQLITE_FIELD_LOCAL_DATE    , SQLITE_FIELD_ENTRY_MODE    , SQLITE_FIELD_RESPONSE_CODE  , 
		SQLITE_FIELD_SERIAL_NUMBER , SQLITE_FIELD_EC_DOCUMENT   , SQLITE_FIELD_CURRENCY_CODE  , 
		SQLITE_FIELD_INSTALLMENTS  , SQLITE_FIELD_PAY_METHOD    , SQLITE_FIELD_TABLES_VERSION , 
		SQLITE_FIELD_NSU_ACQUIRER  , SQLITE_FIELD_EMV_DATA      , SQLITE_FIELD_NSU_HOST       );

    sprintf (values, "'%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%06d'",
		structMsg->mti          , structMsg->processCode   , structMsg->amount       ,
		structMsg->gmtDateTime  , structMsg->localNSU      , structMsg->localTime    ,
		structMsg->localDate    , structMsg->entryMode     , structMsg->responseCode , 
		structMsg->serialNumber , structMsg->ecDocument    , structMsg->currencyCode ,
		structMsg->installments , structMsg->paymentMethod , "!TODO"				 , 
		structMsg->acquirerNSU  , structMsg->emvData       , nsuAux                  );
    rc = db_sqLite_insert (SQLITE_DB_DATABASE, SQLITE_TABLE_TRANSACTIONS, fields, values);
	
	return rc;
}

int iso_UpdateFinStruct(ISO_FIN_MODAL_T * structMsg)
{
	char zAux 			[2048] = {0x00};
	char device_type     [2+1] = {0x00};
	char conection_type  [8+1] = {0x00};
	char cardholderName [32+1] = {0x00};
	char brandName      [32+1] = {0x00};
	char truncated_PAN  [19+1] = {0x00};
	char sim_number     [20+1] = {0x00};
	char operator_name  [20+1] = {0x00};
	char modality       [32+1] = {0x00};
	char amount         [32+1] = {0x00};

	char fields       [1536+1] = {0x00};
	char condition     [128+1] = {0x00};
	int  rc                    = RC_OK;

	time_t rawtime;
	struct tm *nowTime;
	time(&rawtime);
	nowTime = localtime( &rawtime );

	//DE61
	memset(zAux, 0x00, sizeof(zAux));
	rc = util_String_GetValueTLV(ID_DEVICE_TYPE, structMsg->terminalInfo, zAux);
	if (rc == RC_OK)
		strcpy(device_type, zAux);
	memset(zAux, 0x00, sizeof(zAux));
	rc = util_String_GetValueTLV(ID_CONECTION_TYPE, structMsg->terminalInfo, zAux);
	if (rc == RC_OK)
		strcpy(conection_type, zAux);

	//DE63
	memset(zAux, 0x00, sizeof(zAux));
	rc = util_String_GetValueTLV(ID_HOLDER_NAME, structMsg->adtTrsData, zAux);
	if (rc == RC_OK)
		util_String_Trim(zAux, cardholderName);
	memset(zAux, 0x00, sizeof(zAux));
	rc = util_String_GetValueTLV(ID_BRAND_NAME, structMsg->adtTrsData, zAux);
	if (rc == RC_OK)
		strcpy(brandName, zAux);
	memset(zAux, 0x00, sizeof(zAux));
	rc = util_String_GetValueTLV(ID_TRUNCATED_PAN, structMsg->adtTrsData, zAux);
	if (rc == RC_OK)
		strcpy(truncated_PAN, zAux);

	//DE123
	memset(zAux, 0x00, sizeof(zAux));
	rc = util_String_GetValueTLV(ID_SIM_NUMBER, structMsg->simCardInfo, zAux);
	if (rc == RC_OK)
		if (atoi(&zAux[15]) > 0)
			strcpy(sim_number, zAux);
		
	memset(zAux, 0x00, sizeof(zAux));
	rc = util_String_GetValueTLV(ID_OPERATOR_NAME, structMsg->simCardInfo, zAux);
	if (rc == RC_OK)
		strcpy(operator_name, zAux);

    sprintf (fields, "'%s' = '%s', '%s' = '%s', '%s' = '%s', '%s' = '%s', '%s' = '%s', '%s' = '%s', '%s' = '%s', '%s' = '%s', '%s' = '%s'",
		SQLITE_FIELD_DEVICE_TYPE  , device_type  , SQLITE_FIELD_CONECTION_TYPE, conection_type         , SQLITE_FIELD_CARDHOLDER_NAME, cardholderName,
		SQLITE_FIELD_BRAND_NAME   , brandName    , SQLITE_FIELD_TRUNCATED_PAN , truncated_PAN          , SQLITE_FIELD_SIM_NUMBER     , sim_number,
		SQLITE_FIELD_OPERATOR_NAME, operator_name, SQLITE_FIELD_RESPONSE_CODE , structMsg->responseCode, SQLITE_FIELD_NSU_ACQUIRER   , structMsg->acquirerNSU);
	sprintf (condition, "%s == '%s'", SQLITE_FIELD_NSU_HOST, structMsg->hostNSU);
	rc = db_sqLite_update (SQLITE_DB_DATABASE, SQLITE_TABLE_TRANSACTIONS, fields, condition);
	printf("db_sqLite_update res = [%d]\n", rc);

	//Comprovante
	// @=MASTERCARD@=CREDITO A VISTA@=1234XXXXXX456789@=VIA ESTABELECIMENTO/POS-12345678@
	// <03.654.119/0003-38@<!GERTEC BRASIL LTDA@<AV JABAQUARA 3060 - SÃO PAULO SP@
	// =ADQ:012345678901 NSU:000002 AUT:123456@
	// <!28/07/2022                       15:30@
	// <VENDA A CRÉDITO@<!VALOR:                        R$ 10,00@@=TRANSAÇÃO AUTORIZADA COM SENHA@=Calegaro/Erick@=A0000000041010@=Credito Nubank@
	if 		(strcmp(structMsg->processCode, "002000") == 0)	strcpy(modality, "DEBITO A VISTA");
	else if (strcmp(structMsg->processCode, "003100") == 0)	strcpy(modality, "CREDITO PARCELADO S/ JUROS");
	else if (strcmp(structMsg->processCode, "003800") == 0)	strcpy(modality, "CREDITO PARCELADO C/ JUROS");
	else													strcpy(modality, "CREDITO A VISTA");
	utl_String_FormatAmount(structMsg->amount, amount);

	memset(structMsg->receiptEC, 0x00, sizeof(structMsg->receiptEC));
	memset(structMsg->receiptHolder, 0x00, sizeof(structMsg->receiptHolder));

	if(strcmp(structMsg->responseCode, "00") == 0){
		for (int i = 0; i < 2; i++){
			memset(zAux, 0x00, sizeof(zAux));
			sprintf(zAux, 	"@" "=%s@" "=%s@" "<03.654.119/0003-38 - POS-%s@" "=!GERTEC BRASIL LTDA@" "<AV JABAQUARA 3060 - SAO PAULO SP@" \
							"=ADQ:%s NSU:%s AUT:%s@" "<!%02d/%02d/%04d              %02d:%02d@" "<VENDA A %s@" "<!VALOR%s@" "@" "%s" "=%s@", 
							(i == 0) ? "VIA ESTABELECIMENTO" : "VIA CLIENTE",
							brandName, 
							&structMsg->serialNumber[8],
							structMsg->acquirerNSU, structMsg->localNSU, structMsg->hostNSU,
							nowTime->tm_mday, nowTime->tm_mon + 1, (nowTime->tm_year+1900), nowTime->tm_hour, nowTime->tm_min,
							modality,
							amount,
							(atoi(structMsg->entryMode) == 21 || atoi(structMsg->entryMode) == 51 || atoi(structMsg->entryMode) == 59) ? "=TRANSACAO AUTORIZADA COM SENHA@" : (atoi(structMsg->entryMode) == 71 || atoi(structMsg->entryMode) == 78) ? "" : "=_________________________________@=ASSINATURA DO PORTADOR@",
							cardholderName);
			if (i == 0){
				strcpy(structMsg->receiptEC, zAux);
			}else{
				strcpy(structMsg->receiptHolder, zAux);
			}
		}
	}else{
		strcpy(structMsg->receiptEC    , "ERRO NO SERVIDOR - CONSULTE AUTH CODE");
		strcpy(structMsg->receiptHolder, "ERRO NO SERVIDOR - CONSULTE AUTH CODE");
	}

	return rc;
}


int iso_UpdateRevStruct(ISO_REV_MODAL_T * structMsg)
{
	char zAux 			[2048] = {0x00};
	char device_type     [2+1] = {0x00};
	char conection_type  [8+1] = {0x00};
	char cardholderName [32+1] = {0x00};
	char brandName      [32+1] = {0x00};
	char truncated_PAN  [19+1] = {0x00};
	char sim_number     [20+1] = {0x00};
	char operator_name  [20+1] = {0x00};
	char amount         [32+1] = {0x00};

	char fields       [1536+1] = {0x00};
	char condition     [128+1] = {0x00};
	int  rc                    = RC_OK;

	time_t rawtime;
	struct tm *nowTime;
	time(&rawtime);
	nowTime = localtime( &rawtime );

	//DE61
	memset(zAux, 0x00, sizeof(zAux));
	rc = util_String_GetValueTLV(ID_DEVICE_TYPE, structMsg->terminalInfo, zAux);
	if (rc == RC_OK)
		strcpy(device_type, zAux);
	memset(zAux, 0x00, sizeof(zAux));
	rc = util_String_GetValueTLV(ID_CONECTION_TYPE, structMsg->terminalInfo, zAux);
	if (rc == RC_OK)
		strcpy(conection_type, zAux);

	//DE63
	memset(zAux, 0x00, sizeof(zAux));
	rc = util_String_GetValueTLV(ID_HOLDER_NAME, structMsg->adtTrsData, zAux);
	if (rc == RC_OK)
		util_String_Trim(zAux, cardholderName);
	memset(zAux, 0x00, sizeof(zAux));
	rc = util_String_GetValueTLV(ID_BRAND_NAME, structMsg->adtTrsData, zAux);
	if (rc == RC_OK)
		strcpy(brandName, zAux);
	memset(zAux, 0x00, sizeof(zAux));
	rc = util_String_GetValueTLV(ID_TRUNCATED_PAN, structMsg->adtTrsData, zAux);
	if (rc == RC_OK)
		strcpy(truncated_PAN, zAux);

	//DE123
	memset(zAux, 0x00, sizeof(zAux));
	rc = util_String_GetValueTLV(ID_SIM_NUMBER, structMsg->simCardInfo, zAux);
	if (rc == RC_OK)
		if (atoi(&zAux[15]) > 0)
			strcpy(sim_number, zAux);
		
	memset(zAux, 0x00, sizeof(zAux));
	rc = util_String_GetValueTLV(ID_OPERATOR_NAME, structMsg->simCardInfo, zAux);
	if (rc == RC_OK)
		strcpy(operator_name, zAux);

    // sprintf (fields, "'%s' = '%s', '%s' = '%s', '%s' = '%s', '%s' = '%s', '%s' = '%s', '%s' = '%s', '%s' = '%s', '%s' = '%s', '%s' = '%s'",
	// 	SQLITE_FIELD_DEVICE_TYPE  , device_type  , SQLITE_FIELD_CONECTION_TYPE, conection_type         , SQLITE_FIELD_CARDHOLDER_NAME, cardholderName,
	// 	SQLITE_FIELD_BRAND_NAME   , brandName    , SQLITE_FIELD_TRUNCATED_PAN , truncated_PAN          , SQLITE_FIELD_SIM_NUMBER     , sim_number,
	// 	SQLITE_FIELD_OPERATOR_NAME, operator_name, SQLITE_FIELD_RESPONSE_CODE , structMsg->responseCode, SQLITE_FIELD_NSU_ACQUIRER   , structMsg->acquirerNSU);
	// sprintf (condition, "%s == '%s'", SQLITE_FIELD_NSU_HOST, structMsg->hostNSU);
	// rc = db_sqLite_update (SQLITE_DB_DATABASE, SQLITE_TABLE_TRANSACTIONS, fields, condition);
	// printf("db_sqLite_update res = [%d]\n", rc);

	//Comprovante
	// @=MASTERCARD@=CREDITO A VISTA@=1234XXXXXX456789@=VIA ESTABELECIMENTO/POS-12345678@
	// <03.654.119/0003-38@<!GERTEC BRASIL LTDA@<AV JABAQUARA 3060 - SÃO PAULO SP@
	// =ADQ:012345678901 NSU:000002 AUT:123456@
	// <!28/07/2022                       15:30@
	// <VENDA A CRÉDITO@<!VALOR:                        R$ 10,00@@=TRANSAÇÃO AUTORIZADA COM SENHA@=Calegaro/Erick@=A0000000041010@=Credito Nubank@
	utl_String_FormatAmount(structMsg->amount, amount);

	memset(structMsg->receiptEC, 0x00, sizeof(structMsg->receiptEC));
	memset(structMsg->receiptHolder, 0x00, sizeof(structMsg->receiptHolder));

	if(strcmp(structMsg->responseCode, "00") == 0){
		for (int i = 0; i < 2; i++){
			memset(zAux, 0x00, sizeof(zAux));
			sprintf(zAux, 	"@" "=%s@" "=%s@" "<03.654.119/0003-38 - POS-%s@" "=!GERTEC BRASIL LTDA@" "<AV JABAQUARA 3060 - SAO PAULO SP@" \
							"=ADQ:%s NSU:%s AUT:%s@" "<!%02d/%02d/%04d              %02d:%02d@" "<CANCELAMENTO DE VENDA@" "<!VALOR%s@" "@" "%s" "=%s@", 
							(i == 0) ? "VIA ESTABELECIMENTO" : "VIA CLIENTE",
							brandName, 
							&structMsg->serialNumber[8],
							structMsg->acquirerNSU, structMsg->localNSU, structMsg->hostNSU,
							nowTime->tm_mday, nowTime->tm_mon + 1, (nowTime->tm_year+1900), nowTime->tm_hour, nowTime->tm_min,
							amount,
							(i == 0) ? "=_________________________________@=ASSINATURA DO PORTADOR@" : "@",
							cardholderName);
			if (i == 0){
				strcpy(structMsg->receiptEC, zAux);
			}else{
				strcpy(structMsg->receiptHolder, zAux);
			}
		}
	}else if(strcmp(structMsg->responseCode, "56") == 0){
		strcpy(structMsg->receiptEC    , "56 - TRANSACAO ORIGINAL NAO ENCONTRADA");
		strcpy(structMsg->receiptHolder, "56 - TRANSACAO ORIGINAL NAO ENCONTRADA");
	}else{
		strcpy(structMsg->receiptEC    , "ERRO NO SERVIDOR - CONSULTE AUTH CODE");
		strcpy(structMsg->receiptHolder, "ERRO NO SERVIDOR - CONSULTE AUTH CODE");
	}

	

	return rc;
}

int iso_ChargeProcess(ISO_NET_MODAL_T * structMsgIn, ISO_NET_MODAL_T * structMsgOut)
{
	char timestamp[12+1] = {0x00};
	int currIndex = atoi(structMsgIn->tableTrade1);

	strncpy(structMsgOut->mti          , "0810"					 	, 4 ); //FIXO
	strncpy(structMsgOut->gmtDateTime  , structMsgIn->gmtDateTime 	, 10); // M
	strncpy(structMsgOut->localNSU     , structMsgIn->localNSU	 	, 6 ); // ME
	strncpy(structMsgOut->localTime    , structMsgIn->localTime	 	, 6 ); // ME
	strncpy(structMsgOut->localDate    , structMsgIn->localDate	 	, 4 ); // ME
	strncpy(structMsgOut->responseCode , "00"					 	, 2 ); // M
	strncpy(structMsgOut->serialNumber , structMsgIn->serialNumber 	, 16); // ME
	strncpy(structMsgOut->ecDocument   , structMsgIn->ecDocument 	, 20); // ME
	
	// primeira tabela a ser enviada nos 3 primeiros bytes de structMsgOut->tableTrade1
	// Tabela a ser baixada está salva em CFG_TIMESTAMP

	if (util_Config_GetTableField(CFG_TIMESTAMP, timestamp) != RC_OK){
		memset(timestamp, 0x00, sizeof(timestamp));
		strncpy(timestamp , "TAB000GERTEC" , 12);
		util_Config_SetTableField(CFG_TIMESTAMP, timestamp); //Corrige possivel bug nas config
	}

	util_tableSerialization(&currIndex, structMsgOut->tableTrade1);
	util_tableSerialization(&currIndex, structMsgOut->tableTrade2);

	// !TODO: Melhorar este campo para ser mais dinamico
	sprintf(structMsgOut->versionDetail, "016012010%10s" , timestamp); // M

	return 0;
}


int iso_InitProcess(ISO_NET_MODAL_T * structMsgIn, ISO_NET_MODAL_T * structMsgOut)
{
	char timestamp[12+1] = {0x00};

	strncpy(structMsgOut->mti          , "0810"					 	, 4 ); //FIXO
	strncpy(structMsgOut->gmtDateTime  , structMsgIn->gmtDateTime 	, 10); // M
	strncpy(structMsgOut->localNSU     , structMsgIn->localNSU	 	, 6 ); // ME
	strncpy(structMsgOut->localTime    , structMsgIn->localTime	 	, 6 ); // ME
	strncpy(structMsgOut->localDate    , structMsgIn->localDate	 	, 4 ); // ME
	strncpy(structMsgOut->countryCode  , "076" 	 					, 3 ); // M
	strncpy(structMsgOut->responseCode , "00"					 	, 2 ); // M
	strncpy(structMsgOut->serialNumber , structMsgIn->serialNumber 	, 16); // ME
	strncpy(structMsgOut->ecDocument   , structMsgIn->ecDocument 	, 20); // ME
	
	if (util_Config_GetTableField(CFG_TIMESTAMP, timestamp) != RC_OK){
		memset(timestamp, 0x00, sizeof(timestamp));
		strncpy(timestamp , "TAB000GERTEC" , 12);
		util_Config_SetTableField(CFG_TIMESTAMP, timestamp); //Corrige possivel bug nas config
	}
	
	// !TODO: Melhorar este campo para ser mais dinamico
	sprintf(structMsgOut->tableTrade1  , "%03d"        	 , util_getNumberOfTables(timestamp)); // M
	sprintf(structMsgOut->versionDetail, "012010%10s" , timestamp); // M
	util_TableReadToList(timestamp);

	return 0;
}

int iso_KeepAliveProcess(ISO_NET_MODAL_T * structMsgIn, ISO_NET_MODAL_T * structMsgOut)
{
	BOOL update = FALSE;
	 
	strncpy(structMsgOut->mti          , "0810"					 	, 4 ); //FIXO
	strncpy(structMsgOut->gmtDateTime  , structMsgIn->gmtDateTime 	, 10); // M
	strncpy(structMsgOut->localNSU     , structMsgIn->localNSU	 	, 6 ); // ME
	strncpy(structMsgOut->localTime    , structMsgIn->localTime	 	, 6 ); // ME
	strncpy(structMsgOut->localDate    , structMsgIn->localDate	 	, 4 ); // ME
	strncpy(structMsgOut->countryCode  , "076" 	 					, 3 ); // M
	strncpy(structMsgOut->serialNumber , structMsgIn->serialNumber 	, 16); // ME
	strncpy(structMsgOut->ecDocument   , structMsgIn->ecDocument 	, 20); // ME

	if (util_String_GetValueTLV(ID_MODEL_POS   , structMsgIn->versionDetail, structMsgIn->versions.modelPOS   )){
		strcpy(structMsgOut->tableTrade1, "ERRO AO OBTER MODELO DO TERMINAL"); // M
		strncpy(structMsgOut->responseCode , "01"					 	, 2 ); // M
		return 0;
	}
	strncpy(structMsgOut->versions.modelPOS , structMsgIn->versions.modelPOS , 4); // ME

	if (util_String_GetValueTLV(ID_APP_VER     , structMsgIn->versionDetail, structMsgIn->versions.appVer     ) != RC_OK ||
		util_String_GetValueTLV(ID_FIRMWARE_VER, structMsgIn->versionDetail, structMsgIn->versions.firmwareVer) != RC_OK ){
		strcpy(structMsgOut->tableTrade1, "ERRO AO OBTER VERSOES DO TERMINAL"); // M
		strncpy(structMsgOut->responseCode , "01"					 	, 2 ); // M
		return 0;
	}

	if (util_Config_GetTableField(CFG_TIMESTAMP, structMsgOut->versions.emvTableVer) != RC_OK){
		strcpy(structMsgOut->tableTrade1, "ERRO AO OBTER TIMESTAMP"); // M
		strncpy(structMsgOut->responseCode , "01"					 	, 2 ); // M
	}

	if ((util_Config_GetTableField(CFG_HOSTNAME, structMsgOut->versions.hostName) != RC_OK) ||
		(util_Config_GetTableField(CFG_HOSTPORT, structMsgOut->versions.hostPort) != RC_OK) ||
		(util_Config_GetTableField(CFG_HOSTUSER, structMsgOut->versions.hostUser) != RC_OK) ||
		(util_Config_GetTableField(CFG_HOSTPASS, structMsgOut->versions.hostPass) != RC_OK) ){
		strcpy(structMsgOut->tableTrade1, "ERRO AO OBTER CONFIGURACOES FTP"); // M
		strncpy(structMsgOut->responseCode , "01"					 	, 2 ); // M
		return 0;
	}

	if (!strncmp(structMsgIn->versions.modelPOS, MODEL_MP35, 4)){
		if ((util_Config_GetTableField(CFG_MP35_VER_APP, structMsgOut->versions.appVer     ) != RC_OK) ||
			(util_Config_GetTableField(CFG_MP35_VER_FW , structMsgOut->versions.firmwareVer) != RC_OK) ){
			strcpy(structMsgOut->tableTrade1, "ERRO AO OBTER CONFIGURACOES PARA MP35"); // M
			strncpy(structMsgOut->responseCode , "01"					 	, 2 ); // M
			return 0;
		}
	}else if (!strncmp(structMsgIn->versions.modelPOS, MODEL_MP30, 4)){
		/* code */
	}
	
	if (strcmp(structMsgIn->versions.appVer, structMsgOut->versions.appVer)){
		printf("structMsgIn->versions.appVer  [%s]\n", structMsgIn->versions.appVer);
		printf("structMsgOut->versions.appVer [%s]\n", structMsgOut->versions.appVer);
		sprintf(structMsgOut->versions.hostPath, "%s/%s/%s/%s.%s", 
			structMsgOut->versions.modelPOS,
			"APP", 
			structMsgOut->versions.appVer,
			"demo_pos",
			(strncmp(structMsgIn->versions.modelPOS, MODEL_MP35, 4)) ? "bin" : "cy");
		update = TRUE;
	}else if (strcmp(structMsgIn->versions.firmwareVer, structMsgOut->versions.firmwareVer)){
		printf("structMsgIn->versions.firmwareVer  [%s]\n", structMsgIn->versions.firmwareVer);
		printf("structMsgOut->versions.firmwareVer [%s]\n", structMsgOut->versions.firmwareVer);
		//./MP35/FW/4.7.1/4.6.8_to_4.7.1.up
		sprintf(structMsgOut->versions.hostPath, "%s/%s/%s/%s_to_%s.%s", 
			structMsgOut->versions.modelPOS,
			"FW", 
			structMsgOut->versions.firmwareVer,
			structMsgIn->versions.firmwareVer,
			structMsgOut->versions.firmwareVer,
			(strncmp(structMsgIn->versions.modelPOS, MODEL_MP35, 4)) ? "bin" : "up");
		update = TRUE;
	}

	util_String_MountTLV(ID_APP_VER		 , structMsgOut->versions.appVer 	 , structMsgOut->versionDetail);
	util_String_MountTLV(ID_FIRMWARE_VER , structMsgOut->versions.firmwareVer, structMsgOut->versionDetail);
	if (update){
		util_String_MountTLV(ID_HOSTNAME	 , structMsgOut->versions.hostName 	 , structMsgOut->versionDetail);
		util_String_MountTLV(ID_HOSTPORT	 , structMsgOut->versions.hostPort 	 , structMsgOut->versionDetail);
		util_String_MountTLV(ID_HOSTUSER	 , structMsgOut->versions.hostUser 	 , structMsgOut->versionDetail);
		util_String_MountTLV(ID_HOSTPATH     , structMsgOut->versions.hostPath   , structMsgOut->versionDetail);
		util_String_MountTLV(ID_HOSTPASS	 , structMsgOut->versions.hostPass 	 , structMsgOut->versionDetail);
	}
	util_String_MountTLV(ID_EMV_TABLE_VER, structMsgOut->versions.emvTableVer, structMsgOut->versionDetail);
	util_String_MountTLV(ID_MODEL_POS    , structMsgOut->versions.modelPOS   , structMsgOut->versionDetail);

	strcpy (structMsgOut->tableTrade1 		, "KEEP ALIVE" 					    ); // M
	strncpy(structMsgOut->responseCode   	, "00"					 		 , 2); // M

	return 0;
}

int iso_CommTest(ISO_NET_MODAL_T * structMsgIn, ISO_NET_MODAL_T * structMsgOut)
{
	strncpy(structMsgOut->mti         , "0810"					 , 4);  //FIXO
	strncpy(structMsgOut->localNSU    , structMsgIn->localNSU	 , 6);  // ME
	strncpy(structMsgOut->localTime   , structMsgIn->localTime	 , 6);  // ME
	strncpy(structMsgOut->localDate   , structMsgIn->localDate	 , 4);  // ME
	strncpy(structMsgOut->responseCode, "00"					 , 2);  // M
	strncpy(structMsgOut->serialNumber, structMsgIn->serialNumber, 16); // ME
	strncpy(structMsgOut->ecDocument  , structMsgIn->ecDocument	 , 20); // ME
	strncpy(structMsgOut->tableTrade1 , "CONECTADO COM SUCESSO!" , 22); // M
	
	char datetime[10+1] = {0x00};
	iso_TimeGetDateAndTime(datetime);
	strncpy(structMsgOut->gmtDateTime , datetime , 10); // M
	
	return 0;
}

int iso_FinancialMockup(ISO_FIN_MODAL_T * structMsgIn, ISO_FIN_MODAL_T * structMsgOut)
{
	char retCode  [2+1]  = {0x00};
	char nsu_host [8+1]  = {0x00};
	char acquirer [12+1] = {0x00};
	unsigned long acquirerAux     = 0;
	int  nsuAux          = 0;
	int  result          = 0;

	strncpy(structMsgOut->mti          , "0210"					  , 4);  	//FIXO
	strncpy(structMsgOut->processCode  , structMsgIn->processCode , 6); 	// ME
	strncpy(structMsgOut->amount       , structMsgIn->amount      , 12);	// ME
	strncpy(structMsgOut->localNSU     , structMsgIn->localNSU	  , 6);  	// ME
	strncpy(structMsgOut->localTime    , structMsgIn->localTime	  , 6);  	// ME
	strncpy(structMsgOut->localDate    , structMsgIn->localDate	  , 4);  	// ME
	strncpy(structMsgOut->countryCode  , structMsgIn->countryCode , 3);  	// ME
	strncpy(structMsgOut->entryMode    , structMsgIn->entryMode   , 3);  	// ME

	char datetime[10+1] = {0x00};
	iso_TimeGetDateAndTime(datetime);
	strncpy(structMsgOut->gmtDateTime , datetime , 10); // M

	result = util_Config_GetTableField(CFG_NSU_HOST, nsu_host);
	if (result == RC_OK){
		nsuAux = atoi(nsu_host) + 1;
		memset(nsu_host, 0x00, sizeof(nsu_host));
		sprintf(nsu_host, "%06d", nsuAux);
		strncpy(structMsgOut->hostNSU      , nsu_host			  , 6);  	// O
		util_Config_SetTableField(CFG_NSU_HOST, nsu_host);
	}else{
		strncpy(structMsgOut->hostNSU      , "999999"			  , 6);  	// O
	}

	if (atoi(structMsgIn->amount) >= 3900 && atoi(structMsgIn->amount) <= 3999){
		retCode[0] = structMsgIn->amount[10];
		retCode[1] = structMsgIn->amount[11];
		strncpy(structMsgOut->responseCode , retCode			  , 2);  	// M
	}else if (memcmp(structMsgOut->responseCode, "55", 2) == 0){
		strcpy(structMsgOut->receiptEC    , "55 - SENHA INCORRETA");
		strcpy(structMsgOut->receiptHolder, "55 - SENHA INCORRETA");
	}else{
		strncpy(structMsgOut->responseCode , "00"				  , 2);  	// M
	}

	strncpy(structMsgOut->serialNumber , structMsgIn->serialNumber, 16  ); 	// ME
	strncpy(structMsgOut->ecDocument   , structMsgIn->ecDocument  , 20  );    // ME
	strncpy(structMsgOut->currencyCode , structMsgIn->currencyCode, 3   );  	// ME
	strcpy(structMsgOut->terminalInfo  , structMsgIn->terminalInfo      );		// M
	strncpy(structMsgOut->emvData      , "9108FFFFFFFFFFFFFFFF"   , 20  ); 	// C9
	// strncpy(structMsgOut->receiptEC    , CV_MOCKUP_EC             , 401 );	// M
	strcpy(structMsgOut->adtTrsData    , structMsgIn->adtTrsData        );	// M
	strcpy(structMsgOut->versionDetail , structMsgIn->versionDetail     );	// M
	strcpy(structMsgOut->simCardInfo   , structMsgIn->simCardInfo       );	// M
	// strncpy(structMsgOut->receiptHolder, CV_MOCKUP_HOLDER         , 365 );	// M

	result = util_Config_GetTableField(CFG_NSU_ACQUIRER, acquirer);
	if (result == RC_OK){
		acquirerAux = atol(acquirer) + 1;
		memset(acquirer, 0x00, sizeof(acquirer));
		sprintf(acquirer, "%012ld", acquirerAux);
		strncpy(structMsgOut->acquirerNSU, acquirer	  			  , 12);  	// O
		util_Config_SetTableField(CFG_NSU_ACQUIRER, acquirer);
	}else{
		strncpy(structMsgOut->acquirerNSU, "999999999999"	 	  , 12);  	// O
	}

	if (atoi(structMsgIn->amount) > 42000 && atoi(structMsgIn->amount) <= 42005){
		return structMsgIn->amount[11] - 48;
	}

	if (atoi(structMsgIn->amount) >= 80000 && atoi(structMsgIn->amount) <= 80005){
		util_needUpdateTimestamp(atoi(structMsgIn->amount) - 80000);
	}

	return 0;
}

int iso_ReversalMockup(ISO_REV_MODAL_T * structMsgIn, ISO_REV_MODAL_T * structMsgOut)
{
	char retCode  [2+1]  = {0x00};
	char nsu_host [8+1]  = {0x00};
	char acquirer [12+1] = {0x00};
	char origData [1024+1] = {0x00};
	unsigned long acquirerAux     = 0;
	int  nsuAux          = 0;
	int  result          = 0;

	strncpy(structMsgOut->mti          , "0410"					  , 4);  	//FIXO
	strncpy(structMsgOut->processCode  , structMsgIn->processCode , 6); 	// ME
	strncpy(structMsgOut->amount       , structMsgIn->amount      , 12);	// ME
	strncpy(structMsgOut->localNSU     , structMsgIn->localNSU	  , 6);  	// ME
	strncpy(structMsgOut->localTime    , structMsgIn->localTime	  , 6);  	// ME
	strncpy(structMsgOut->localDate    , structMsgIn->localDate	  , 4);  	// ME
	strncpy(structMsgOut->entryMode    , structMsgIn->entryMode   , 3);  	// ME
	
	char datetime[10+1] = {0x00};
	iso_TimeGetDateAndTime(datetime);
	strncpy(structMsgOut->gmtDateTime , datetime , 10); // M

	result = util_Config_GetTableField(CFG_NSU_HOST, nsu_host);
	if (result == RC_OK){
		nsuAux = atoi(nsu_host) + 1;
		memset(nsu_host, 0x00, sizeof(nsu_host));
		sprintf(nsu_host, "%06d", nsuAux);
		strncpy(structMsgOut->hostNSU      , nsu_host			  , 6);  	// O
		util_Config_SetTableField(CFG_NSU_HOST, nsu_host);
	}else{
		strncpy(structMsgOut->hostNSU      , "999999"			  , 6);  	// O
	}

	if (atoi(structMsgIn->amount) >= 4900 && atoi(structMsgIn->amount) <= 4999){
		retCode[0] = structMsgIn->amount[10];
		retCode[1] = structMsgIn->amount[11];
		strncpy(structMsgOut->responseCode , retCode			  , 2);  	// M
	}else{
		//PARSEAR BIT 90 e BUSCAR NO BANCO DE DADOS
		result = utl_SQL_select_original_Transaction(structMsgIn->primalTrsData, origData, sizeof(origData));
		if (!result){
			printf("origData = [%s]\n", origData);
			strncpy(structMsgOut->responseCode , "00"				  , 2);  	// M
		}else{
			//SE NÃO ENCONTRAR RETORNAR 39 com "56 - No card record"
			strncpy(structMsgOut->responseCode , "56"				  , 2);  	// M
		}
	}

	strncpy(structMsgOut->serialNumber , structMsgIn->serialNumber, 16  ); 	// ME
	strncpy(structMsgOut->ecDocument   , structMsgIn->ecDocument  , 20  );  // ME
	strncpy(structMsgOut->currencyCode , structMsgIn->currencyCode, 3   );  // ME
	strcpy(structMsgOut->terminalInfo  , structMsgIn->terminalInfo      );	// M
	strncpy(structMsgOut->emvData      , "9108FFFFFFFFFFFFFFFF"   , 20  ); 	// C9
	// strncpy(structMsgOut->receiptEC    , CV_MOCKUP_EC             , 401 );	// M
	strcpy(structMsgOut->adtTrsData    , structMsgIn->adtTrsData        );	// M
	strcpy(structMsgOut->versionDetail , structMsgIn->versionDetail     );	// M
	strcpy(structMsgOut->simCardInfo   , structMsgIn->simCardInfo       );	// M
	// strncpy(structMsgOut->receiptHolder, CV_MOCKUP_HOLDER         , 365 );	// M

	result = util_Config_GetTableField(CFG_NSU_ACQUIRER, acquirer);
	if (result == RC_OK){
		acquirerAux = atol(acquirer) + 1;
		memset(acquirer, 0x00, sizeof(acquirer));
		sprintf(acquirer, "%012ld", acquirerAux);
		strncpy(structMsgOut->acquirerNSU, acquirer	  			  , 12);  	// O
		util_Config_SetTableField(CFG_NSU_ACQUIRER, acquirer);
	}else{
		strncpy(structMsgOut->acquirerNSU, "999999999999"	 	  , 12);  	// O
	}

	if (atoi(structMsgIn->amount) > 42005 && atoi(structMsgIn->amount) <= 42010){
		return structMsgIn->amount[11] - 48;
	}

	// if (atoi(structMsgIn->amount) >= 80000 && atoi(structMsgIn->amount) <= 80005){
	// 	util_needUpdateTimestamp(atoi(structMsgIn->amount) - 80000);
	// }

	return 0;
}

int iso_TimeGetDateAndTime(char *dateAndTime)
{
	time_t t = time(NULL);
  	struct tm tm = *localtime(&t);
	char datetime[10+1] = {0x00};

	//YYMMDDhhmm
	sprintf(datetime, "%02d%02d%02d%02d%02d", ((tm.tm_year + 1900) - 2000), tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
	strncpy(dateAndTime, datetime, 10);

	return 0;
}
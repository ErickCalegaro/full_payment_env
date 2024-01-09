#ifndef HEADER_H
#define HEADER_H

//Constantes
#define ARGV_SERVER_PORT 	1 
#define ARGV_CLIENT_IP 		1 
#define ARGV_CLIENT_PORT 	2 

#define BOOL 				u_char
#define FALSE				0
#define TRUE				1

//Codigos de retorno
#define RC_ERR_GENERIC	   -1
#define RC_OK 				0
#define RC_MANY_ARGS 		1
#define RC_FEW_ARGS 		2
#define RC_LOW_PORT_VALUE	3
#define RC_HIGH_PORT_VALUE	4
#define RC_SOCKET_CREATE	5
#define RC_SOCKET_OPTIONS	6
#define RC_SOCKET_ATACH		7
#define RC_SOCKET_LISTEN	8
#define RC_SOCKET_ACCEPT	9
#define RC_SOCKET_CONNECT	10
#define RC_INET_ADDRESS		11

#define RC_ACQUIRER_INV		1001

#define CFG_NSU_HOST 		1
#define CFG_NSU_ACQUIRER	2
#define CFG_TIMESTAMP		3
#define CFG_MP35_VER_APP	4
#define CFG_MP35_VER_FW		5
#define CFG_HOSTNAME		6
#define CFG_HOSTUSER		7
#define CFG_HOSTPORT		8
#define CFG_HOSTPASS		9

#define ID_DEVICE_TYPE	  	1
#define ID_CONECTION_TYPE  	2

#define ID_HOLDER_NAME		1
#define ID_BRAND_NAME		2
#define ID_TRUNCATED_PAN	3

#define ID_BIN_TABLE_VER	1
#define ID_LABEL_POS		2
#define ID_LANGUAGES		3
#define ID_APP_VER			4
#define ID_FIRMWARE_VER		5
#define ID_HOSTNAME			6
#define ID_HOSTPORT			7
#define ID_HOSTUSER			8
#define ID_HOSTPATH			9
#define ID_HOSTPASS			10
#define ID_CNPJ_SUB_ACQ		11
#define ID_EMV_TABLE_VER	12
#define ID_ESPEC_VER		13
#define ID_NEED_INIT		14
#define ID_MODEL_POS		15

#define ID_SIM_NUMBER		1
#define ID_OPERATOR_NAME	2

#define TABLE_GERTEC		0
#define TABLE_AMEX   		1
#define TABLE_REDE			2
#define TABLE_CIELO			3
#define TABLE_VERO			4
#define TABLE_STONE			5

#define MODEL_MP35      	"MP35"
#define MODEL_MP35P      	"MP35"
#define MODEL_MP30      	"MP30"
#define MODEL_MP30P      	"MP30"

#define MAX_MSG 	1024*5
#define ISM_PORT    55555
#define HSM_PORT    55556
#define LOCALHOST   "127.0.0.1"

#define LOG_NONE    0
#define LOG_FATAL   1
#define LOG_ERROR   2    //PROD
#define LOG_WARN    3    //HOMOLOG
#define LOG_INFO    4    //DEV
#define LOG_DEBUG   5    
#define LOG_PATH    "../PUB/log_ism.txt"

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

//Prototipos Funcoes
int doProcessSocketServer	(char const *sPort);
int doProcessSocketClient	(char const *ip, char const *sPort);
int doProcessGame			(char * sRandNum);

//Prototipos Tratativas
int debugErrorsServer		(int rc);
int debugErrorsClient		(int rc);

//Util
int  util_ConvertToDecimal_Header(char *msg);
int  util_ConvertCharToHex(char caracter);
void util_Convert_HexaToBin(char h[], char *result);
char util_ConvertHexToChar(int hex);
char util_AddBitToByte(char byte, int bit);
int  util_ConvertStrigToHexa(char * string, char * hexa);
int  util_Config_GetTableField(int field, char *output);
int  util_Config_SetTableField(int field, char *input);
int  util_String_GetValueTLV(int targetTag, char *input, char *output);
void util_String_DumpStringLog(char *input, int charPerLine);
int util_String_MountTLV(int inputId, char *inputData, char *output);
void utl_String_FormatAmount(const char *input, char *output);
int  util_String_Trim(char *texto, char *result);
int  util_invertLastName(char *input, char *result);
int  util_getNumberOfTables(char * timestamp);
void util_String_RemoveNewLine(char input[], char output[]);
int  util_TableReadToList(char * timestamp);
int  util_tableSerialization(int * currIndex, char * output);
int  util_sendDataToHSM(char * psKSN, char * psPINBlock, char * psDataEnc);
void util_logger(int level, char * file, int line, const char *fmt, ...);

//Iso Process
int iso_MainProcess(char * messageRecv, int sizeRecv, char * messageSend, int * sizeSend);

#endif
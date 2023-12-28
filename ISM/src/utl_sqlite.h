
#ifndef _UTL_SQLITE_H_
#define _UTL_SQLITE_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//================================ aqui vamos definir os tipos =====================================

typedef sqlite3_stmt *SQLITE_STMT;

#define SQL_STS_OK			(1)		//	SUCESSO
#define SQL_STS_NONE		(0)		//	NENHUM
#define SQL_ERR_GENERIC		(-1)	//	ERROS ORGANIZADOS PARA ALGUM GERENCIAMENTO
#define SQL_ERR_TIMEOUT		(-2)	//	..
#define SQL_ERR_OPEN		(-3)	//	..
#define SQL_ERR_EXEC		(-4)	//	..
#define SQL_ERR_NOTEXISTS	(-5)	//	..
#define SQL_ERR_INVALIDPARM	(-6)	//	..
#define	SQL_ERR_SQLERR		(-102)	//	ERRO DO MYSQL ESPECIFICO

//================================ status de transações ============================================

#define STATUS_TRANSACAO_PENDENTE                            -1
#define STATUS_TRANSACAO_DESCONHECIDA                         0
#define STATUS_TRANSACAO_APROVADA                             1
#define STATUS_TRANSACAO_RECUSADA                             2
#define STATUS_TRANSACAO_DESFEITA                             3
#define STATUS_TRANSACAO_CANCELADA                            4
#define STATUS_TRANSACAO_ADMINISTRATIVA                       5
#define STATUS_TRANSACAO_DESFEITA_ADM                         6

//====================== aqui vamos definir os banco e tabelas =====================================

#define SQLITE_DB_DATABASE                      "../PUB/database.db"

#define SQLITE_TABLE_TRANSACTIONS               "transactions"
#define SQLITE_TABLE_CONFIG               		"config"

//====================== aqui vamos definir os campos =====================================

#define SQLITE_FIELD_MTI     					"mti"
#define SQLITE_FIELD_PROCESS_CODE     			"process_code"
#define SQLITE_FIELD_AMOUNT     				"amount"
#define SQLITE_FIELD_DATETIME     				"datetime"
#define SQLITE_FIELD_NSU_TERMINAL     			"nsu_terminal"
#define SQLITE_FIELD_LOCAL_TIME     			"local_time"
#define SQLITE_FIELD_LOCAL_DATE     			"local_date"
#define SQLITE_FIELD_ENTRY_MODE     			"entry_mode"
#define SQLITE_FIELD_NSU_HOST     				"nsu_host"
#define SQLITE_FIELD_RESPONSE_CODE     			"response_code"
#define SQLITE_FIELD_SERIAL_NUMBER     			"serial_number"
#define SQLITE_FIELD_EC_DOCUMENT     			"ec_document"
#define SQLITE_FIELD_CURRENCY_CODE     			"currency_code"
#define SQLITE_FIELD_DEVICE_TYPE     			"device_type"
#define SQLITE_FIELD_CONECTION_TYPE     		"conection_type"
#define SQLITE_FIELD_CARDHOLDER_NAME     		"cardholder_name"
#define SQLITE_FIELD_BRAND_NAME     			"brand_name"
#define SQLITE_FIELD_TRUNCATED_PAN     			"truncated_pan"
#define SQLITE_FIELD_INSTALLMENTS     			"installments"
#define SQLITE_FIELD_PAY_METHOD     			"pay_method"
#define SQLITE_FIELD_TABLES_VERSION     		"tables_version"
#define SQLITE_FIELD_SIM_NUMBER     			"sim_number"
#define SQLITE_FIELD_OPERATOR_NAME     			"operator_name"
#define SQLITE_FIELD_NSU_ACQUIRER     			"nsu_acquirer"
#define SQLITE_FIELD_EMV_DATA     				"emv_data"

//================ aqui vamos definir as menssagens de erros do banco ==============================

#define RC_ERR_PARAMETER_INVALID                1000    // parametros nulos passados para a fun��o
#define RC_ERR_NO_OPEN_DATA_BASE                1001    // n�o consegue abrir o banco de dados
#define RC_ERR_NO_INSERT                        1002    // n�o consegue fazere o insert
#define RC_ERR_NO_UPDATE                        1003    // n�o consegue fazer o update
#define RC_ERR_ZERO_DATA_UPDATE                 1004    // nenhum registro foi atualizado
#define RC_ERR_NO_SELECT                        1005    // nenhum registro retornou do select
#define RC_ERR_NO_COLUNS                        1006    // nenhuma coluna foi encontrada
#define RC_ERR_NO_REG                           1007    // nenhum registro retornado
#define RC_ERR_NO_EXEC_SQL                      1008    // n�o consegui executar um comando SQL
#define RC_ERR_NO_DELETE                        1009    // n�o consegui deletar o registro
#define RC_ERR_NO_EXEC_SELECT                   1010    // n�o consegui executar o select
#define RC_ERR_EOF                              1011    // fim de arquivo
#define RC_ERR_NO_SKIP                          1012    // n�o consegui saltar de registro
#define RC_ERR_SIZE_MISMATCH                    1013    // tamanho de buffers incompativeis

//====================== aqui vamos colocar os prototipos das fun��es ==============================

int db_sqLite_insert         (const char *databank, const char *table, const char *fields, const char *values);
int db_sqLite_update         (const char *databank, const char *table, const char *fields, const char *condition);
int db_sqLite_get_txt        (const char *databank, const char *table, const char *fields, const char *condition, char *outputt, int output_size);
int db_sqLite_get_int        (const char *databank, const char *table, const char *fields, const char *condition, int  *output);
int db_sqLite_delete         (const char *databank, const char *table, const char *condition);
int db_exec_sql              (const char *databank, char *query);
int db_sqLite_select_prepare (SQLITE_STMT *pstmt, const char *databank, const char *table, const char *fields, const char *condition, int *cols, const char *join);
int db_sqLite_select_step    (SQLITE_STMT stmt);
int db_sqLite_select_column  (SQLITE_STMT stmt, int inputIndex, char *outputName, int outputNameSize, char *outputValue, int outputValueSize);
int mens_erro_SQL           (int error, char *output);
int db_sqLite_select 		 (const char *databank, const char *table, const char *fields, const char *condition, char *final, size_t final_size);
int utl_SQL_select_original_Transaction(const char *pszDE90, char *psTransactionData, size_t uiDataSize);

int utl_SQL_GetLinesFromTable(char *inputDatabase, char *inputTable, char *inputColum, char *inputWhere);

#endif // _UTL_SQL_H_

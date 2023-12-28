#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int  DEV = 1;
char       *select_count;
char       *select_final;
size_t      select_final_size;


char *ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}
char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back + 1) = '\0';
    return s;
}
char *trim(char *s)
{
    return(rtrim(ltrim(s)));
}

static int callback(void *data, int argc, char **argv, char **azColName)
{
    int i;
    size_t flen, alen;
    if (select_count++ == 0) {
        select_final[0] = '\0';
    }
    for (i = 0; i < argc; i++) {
        flen = strlen (select_final);
        if (i == 0 && flen > 0) {
            select_final[flen++] = '\n';
            select_final += flen;
        }
        alen = argv[i] && argv[i][0] ? strlen (argv[i]) : 4;
        if (i > 0) {
            alen++;
        }
        if ((flen + alen) > (select_final_size - 1)) {
            return printf("Small buffer in select final");
        }
        if (i == 0) {
            sprintf (select_final, "%s", argv[0] && argv[0][0] ? argv[0] : "NULL");
        } else {
            sprintf (select_final, "%s;%s", select_final, argv[i] && argv[i][0] ? argv[i] : "NULL");
        }
        rtrim (select_final);
    }

    return RC_OK;
}

//	========== INSERE REGISTROS

int db_sqLite_insert(const char *databank, const char *table, const char *fields, const char *values)
{
	int         DBG_DEV   = 1;
    int         rc        = RC_OK;
    char       *zErrMsg   = 0;
    sqlite3    *db        = NULL;
    char        sql[4096] = {0x00};;
    const char *data      = "Callback Called by Insert";



    /*DEBUG*/printf("SQL INSERT FUNCTION");




    //	SANITY CHECK

    if (databank == NULL ||
    	table    == NULL ||
    	fields   == NULL ||
    	values   == NULL)
    {
    	/*DEBUG*/printf(">>> ERRO parametros invalidos em db_sqLite_insert <<<");
        return RC_ERR_PARAMETER_INVALID;
    }

    //	ABRE O DATABASE

    rc =	sqlite3_open_v2(databank, &db, SQLITE_OPEN_READWRITE, NULL);

			if(rc != SQLITE_OK)
			{
				/*DEBUG*/printf(">>> ERRO ao abrir o banco de dados %s <<<", databank);

				rc =	RC_ERR_NO_OPEN_DATA_BASE;
						sqlite3_free	(zErrMsg);
						sqlite3_close_v2(db);
						sqlite3_free	(db);
						return			(rc);
			}

    //	CRIA SQL STATEMENT

    snprintf(sql, sizeof(sql) - 1, "INSERT INTO %s (%s) VALUES (%s)", table, fields, values);


    /*DEBUG*/printf(" ");
    /*DEBUG*/printf("QUERY INSERT:");
    /*DEBUG*/printf("%s", sql);
    /*DEBUG*/printf(" \n");

    //	EXECUTA INSERT

    rc =	sqlite3_exec(db, sql, NULL, (void *) data, &zErrMsg);

			if(rc != SQLITE_OK)
			{
				if(DBG_DEV > 0)
				{
					printf(">>> ERRO ao tentar inserir registro na tabela %s <<<", table);
					printf(">>> ERRO = %d, %s", rc, zErrMsg);
				}

				rc =	RC_ERR_NO_INSERT;
						sqlite3_free	(zErrMsg);
						sqlite3_close_v2	(db);
						return			(rc);
			}

    rc =	RC_OK;
			sqlite3_free	(zErrMsg);
			sqlite3_close_v2(db);

    return(rc);
}

//	========== UPDATE REGISTROS

int db_sqLite_update (const char *databank, const char *table, const char *fields, const char *condition)
{
    int        rc       = RC_OK;
    char       *zErrMsg = 0;
    sqlite3    *db      = NULL;
    char       sql[4096];
    const char *data = "Callback function called";

    //	SANITY CHECK

    if (databank == NULL ||
    	table    == NULL ||
    	fields   == NULL)
    {
    	if(DEV == 1)
    	{
    		printf(">>> ERRO parametros invalidos em db_sqLite_update <<<");
    	}
        return RC_ERR_PARAMETER_INVALID;
    }


    //	ABRE O DATABASE

    rc =	/*sqlite3_open(databank, &db);*/sqlite3_open_v2(databank, &db, SQLITE_OPEN_READWRITE, NULL);

			if(rc != SQLITE_OK)
			{
				if(DEV == 1)
				{
					printf(">>> ERRO ao abrir o banco de dados %s <<<", databank);
				}

				rc =	RC_ERR_NO_OPEN_DATA_BASE;
						sqlite3_free	(zErrMsg);
						sqlite3_close_v2	(db);
						return			(rc);
			}

    //	CRIA STATEMENT

    if(strlen(condition) > 0 )
    {
        snprintf(sql, sizeof(sql) - 1, "UPDATE %s SET %s WHERE %s", table, fields, condition);
    }
    else
    {
        snprintf(sql, sizeof(sql) - 1, "UPDATE %s SET %s", table, fields);
    }

    if(DEV == 1)
    {
    	printf("[%s]", sql);
    }

    //	EXECUTA SQL STATEMENT

    rc =	sqlite3_exec(db, sql, NULL, (void *) data, &zErrMsg);

			if(rc != SQLITE_OK)
			{
				if(DEV == 1)
				{
					printf(">>> ERRO ao tentar atualizar registro na tabela %s <<<", table);
					printf(">>> ERRO = %d, %s", rc, zErrMsg);
				}

				rc =	RC_ERR_NO_UPDATE;
						sqlite3_free	(zErrMsg);
						sqlite3_close_v2	(db);
						return			(rc);
			}

    if(sqlite3_changes (db) == 0)
    {
        rc = RC_ERR_ZERO_DATA_UPDATE;
    }
    else
    {
        rc = RC_OK;
    }

    sqlite3_free	(zErrMsg);
    sqlite3_close_v2	(db);

    return(rc);
}

//	========== SELECT CAMPO TEXTO

int db_sqLite_get_txt(const char *databank, const char *table, const char *fields, const char *condition, char *output, int output_size)
{
	//	DATE		WHO				WTF
	//	14/07/20	RAFAEL NOVO		AJUSTE STATEMENT DANDO ERRO QUANDO NÃO FECHADO
	//								RESPOSTA \0 GRAVADA NA RESPOSTA DANDO CRASH
	//								REFACTOR NA CONSULTA QUE VINHA SEM TERMINADOR \0 E COM RESIDUO LIXO

	sqlite3      *db             = NULL;
	sqlite3_stmt *stmt;
    int           rc             = RC_OK;
    int           nCol           = 0;
    int           count          = 0;
    char	      zRes     [512] = {0x00};
    char          sqlQuery[4096] = {0x00};



    //	SANITY CHECK

    if (databank  == NULL ||
    	table     == NULL ||
    	fields    == NULL ||
    	condition == NULL)
    {
    	/*DEBUG*/printf("*** [SQL ERR] (db_sqLite_get_txt) - PARAMETRO INVALIDO");
        return(RC_ERR_PARAMETER_INVALID);
    }


    //	ABRE O DATABASE E RESETA QUALQUER STATEMENT (PARA CORRIGIR DB_CLOSE() VOLTANDO BUSY)

    rc =	sqlite3_open_v2(databank, &db, SQLITE_OPEN_READONLY, NULL);

			if(rc != SQLITE_OK)
			{
				/*DEBUG*/printf("*** [SQL ERR] (db_sqLite_get_txt) - ABERTURA BANCO DE DADOS [%s]", databank);

				rc =	RC_ERR_NO_OPEN_DATA_BASE;
						sqlite3_finalize(stmt);
						sqlite3_close_v2(db);
						return			(rc);
			}

    //	CRIA STATEMENT (SÓ PEGA PRIMEIRO REGISTRO ENCONTRADO "LIMIT 1")

    snprintf(sqlQuery, sizeof(sqlQuery) - 1, "SELECT %s FROM %s WHERE %s LIMIT 1", fields, table, condition);

    // /*DEBUG*/printf("QUERY: [%s]\n", sqlQuery);

    //	EXECUTA STATEMENT

	rc =	sqlite3_prepare_v2(db, sqlQuery, -1, &stmt, NULL);

			if(rc != SQLITE_OK)
			{
				/*DEBUG*/printf("*** [SQL ERR] (db_sqLite_get_txt) - SELECAO DE REGISTRO NA TABELA [%s]", table);

				rc =	RC_ERR_NO_SELECT;
						sqlite3_finalize(stmt);
						sqlite3_close_v2(db);
						return			(rc);
			}

    //	VERIFICA NUMERO DE COLUNAS DE RETORNO

	nCol =	sqlite3_column_count(stmt);

			if(nCol <= 0)
			{
				/*DEBUG*/printf("*** [SQL ERR] (db_sqLite_get_txt) - NENHUMA COLUNA ENCONTRADA NA TABELA [%s]", table);

				rc =	RC_ERR_NO_COLUNS;
						sqlite3_finalize(stmt);
						sqlite3_close_v2(db);
						return			(rc);
			}

	//	VARRE STATEMENT CARREGANDO OS CAMPOS NA VARAVEL

	while(sqlite3_step(stmt) != SQLITE_DONE)
	{
		count++;
		sprintf(zRes, "%s", sqlite3_column_text(stmt, 0));
	}

	if(count <= 0)
	{
		/*DEBUG*/printf("*** [SQL ERR] (db_sqLite_get_txt) - NENHUM REGISTRO ENCONTRADO");

		rc =	RC_ERR_NO_REG;
				sqlite3_finalize(stmt);
				sqlite3_close_v2(db);
				return			(rc);
	}

		strncpy(output, zRes, output_size);

		/*DEBUG*/
		// if(strlen(zRes) > 0)
			// /*DEBUG*/printf("RESP : |%s|\n", zRes);
		// else
			// /*DEBUG*/printf("RESP : |**NENHUM DADO NA RESPOSTA**|\n");

	sqlite3_finalize(stmt);
    sqlite3_close_v2(db);

    return(rc);
}

//	========== SELECT CAMPO INTEGER

int db_sqLite_get_int(const char *databank, const char *table, const char *fields, const char *condition, int *output)
{
	printf("********************** %s **********************", __FUNCTION__);
    int          rc         = RC_OK;
    int          num_cols   = 0;
    int          counter    = 0;
    int 	     zResp  	= 0;
    sqlite3      *db        = NULL;
    //SQLITE_STMT	 *stmt;
    sqlite3_stmt *stmt;
    char         sql[4096];

    //	SANITY CHECK

    if (databank  == NULL ||
    	table     == NULL ||
    	fields    == NULL ||
    	condition == NULL)
    {
    	if(DEV == 1)
    	{
    		printf(">>> ERRO parametros invalidos em db_sqLite_get_int <<<");
    	}
        return RC_ERR_PARAMETER_INVALID;
    }

    //	ABRE O DATABASE E RESETA QUALQUER STATEMENT (PARA CORRIGIR DB_CLOSE() VOLTANDO BUSY)

    rc = sqlite3_open_v2(databank, &db, SQLITE_OPEN_READONLY, NULL);

	if(rc != SQLITE_OK)
	{
		if(DEV == 1)
		{
			printf(">>> ERRO ao abrir o banco de dados %s <<<", databank);
		}

		rc =	RC_ERR_NO_OPEN_DATA_BASE;
				sqlite3_finalize	(stmt);
				sqlite3_close_v2    (db);
				return				(rc);
	}

    //	CRIA SQL STATEMENT (PEGA O PRIMEIRO REGISTRO "LIMIT 1")

    snprintf(sql, sizeof(sql) - 1, "SELECT %s FROM %s WHERE %s LIMIT 1", fields, table, condition);

    if(DEV == 1)
    {
    	// printf("[%s]", sql);
    }

    //	EXECUTA SQL STATEMENT

	rc =	sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if(rc != SQLITE_OK)
	{
		if (DEV == 1)
		{
			printf(">>> ERROR ao selecionar resgistro da tabela %s <<<", table);
		}

		rc =	RC_ERR_NO_SELECT;
				sqlite3_finalize(stmt);
				sqlite3_close_v2(db);
				return			(rc);
	}

    //	VERIFICA O NUMERO DE COLUNAS DE RETORNO

	num_cols =	sqlite3_column_count(stmt);

	if(num_cols <= 0)
	{
		if(DEV == 1)
		{
			printf(">>> ERRO nenhuma coluna a exibir na tabela %s", table);
		}

		rc =	RC_ERR_NO_COLUNS;
				sqlite3_finalize(stmt);
				sqlite3_close_v2	(db);
				return			(rc);
	}

	//	VARRE O STATEMENT CARREGANDO OS CAMPOS NA VARIAVEL

	while(sqlite3_step(stmt) != SQLITE_DONE)
	{
		counter ++;
		zResp = sqlite3_column_int(stmt, 0);
	}

	if(counter <= 0)
	{
		if(DEV == 1)
		{
			printf(">>> ERRO nenhum registro encontrado <<<");
		}

		rc =	RC_ERR_NO_REG;
				sqlite3_finalize(stmt);
				sqlite3_close_v2(db);
				return			(rc);
	}
	else
	{
		*output = zResp;
	}

	sqlite3_finalize(stmt);
	sqlite3_close_v2(db);

    return(rc);
}

//	========== DELETE

int db_sqLite_delete(const char *databank, const char *table, const char *condition)
{
    int        rc       = RC_OK;
    char       *zErrMsg = 0;
    char       sql[2048];
    const char *data    = "Callback function called";
    sqlite3    *db      = NULL;

    /*DEBUG*/printf("SQL DELETE FUNCTION");

    //	SANITY CHECK

    if (databank  == NULL ||
    	table     == NULL ||
    	condition == NULL)
    {
    	/*DEBUG*/printf("ERRO parametros invalidos em db_sqLite_delete <<<");

        return RC_ERR_PARAMETER_INVALID;

    }

    //	ABRE O DATABASE

        rc =	sqlite3_open_v2(databank, &db, SQLITE_OPEN_READWRITE, NULL);

				if(rc != SQLITE_OK)
				{
					/*DEBUG*/printf(">>> ERRO ao abrir o banco de dados %s <<<", databank);

					rc = RC_ERR_NO_OPEN_DATA_BASE;
					goto exit_func;
				}

    //	CRIA STATEMENT

    snprintf(sql, sizeof(sql) - 1, "DELETE FROM %s WHERE %s", table, condition);

    /*DEBUG*/printf(" ");
    /*DEBUG*/printf("QUERY DELETE:");
    /*DEBUG*/printf("%s", sql);
    /*DEBUG*/printf(" \n");

    //	EXECUTA O DELETE

    rc =	sqlite3_exec (db, sql, NULL, (void*) data, &zErrMsg);

			if(rc != SQLITE_OK)
			{
				if (DEV == 1)
				{
					printf(">>> ERRO ao tentar deletar registro na tabela %s <<<", table);
					printf(">>> ERRO = %d, %s", rc, zErrMsg);
				}

				rc = RC_ERR_NO_DELETE;
				goto exit_func;
			}
			else
			{
				// esse comando elimina fisicamente os registros do banco
				// pois o comando DELETE so os marca como deletados, porem
				// fisicamente eles permanecem no banco ocupando espaco.
				rc = db_exec_sql(databank, "VACUUM;");
			}

    rc = RC_OK;

exit_func:

    sqlite3_free  (zErrMsg);
    sqlite3_close_v2   (db);

    return(rc);
}

//	========== EXECUTA COMANDO SQL

int db_exec_sql(const char *databank, char *query)
{
    int        rc;
    char       sql[4096];
    const char *data    = "Callback function called";
    char       *zErrMsg = 0;
    sqlite3    *db = NULL;

    //	SANITY CHECK

    if((databank == NULL) || (strcmp(query,"") == 0))
    {
    	if (DEV == 1)
    	{
    		printf(">>> ERRO parametros invalidos em db_exec_sql <<<");
    	}
        return RC_ERR_PARAMETER_INVALID;
    }

    //	ABRE O DATABASE

        rc =	/*sqlite3_open(databank, &db);*/sqlite3_open_v2(databank, &db, SQLITE_OPEN_READWRITE, NULL);

				if(rc != SQLITE_OK)
				{
					if(DEV == 1)
					{
						printf(">>> ERRO ao abrir o banco de dados %s <<<", databank);
					}

					rc = RC_ERR_NO_OPEN_DATA_BASE;
					goto exit_func;
				}

    //	QUERY

    snprintf(sql, sizeof(sql) - 1, "%s", query);

	printf("[%s]\n", sql);

    //	EXECUTA QUERY

    rc =	sqlite3_exec (db, sql, NULL, (void *) data, &zErrMsg);

			if(rc != SQLITE_OK)
			{
				if(DEV == 1)
				{
					printf("Failed in exec SQL: %s, -%d", zErrMsg, rc);
				}

				rc = RC_ERR_NO_EXEC_SQL;
				goto exit_func;
			}

    rc = RC_OK;

exit_func:

    sqlite3_free  (zErrMsg);
    sqlite3_close_v2   (db);

    return(rc);

}

//	========== PREPARA UM SELECT DE VARIOS REGISTROS
int db_sqLite_select_prepare(SQLITE_STMT *pstmt, const char *databank, const char *table, const char *fields, const char *condition, int *cols, const char *join)
{
    int     rc = RC_OK;
    char    sql[4096];
    int     length;
    sqlite3 *db = NULL;

    //	SANITY CHECK

    if (databank == NULL ||
    	pstmt    == NULL ||
    	table    == NULL ||
    	fields   == NULL)
    {
    	if (DEV == 1)
    	{
    		printf(">>> ERRO parametros invalidos em db_sqLite_select_prepare <<<");
    	}
        return RC_ERR_PARAMETER_INVALID;
    }

    //	ABRE O DATABASE E RESETA QUALQUER STATEMENT (PARA CORRIGIR DB_CLOSE() VOLTANDO BUSY)

        rc =	/*sqlite3_open(databank, &db);*/sqlite3_open_v2(databank, &db, SQLITE_OPEN_READWRITE, NULL);

				if(rc != SQLITE_OK)
				{
					if (DEV == 1)
					{
						printf(">>> ERRO ao abrir o banco de dados %s <<<", databank);
					}
					rc = RC_ERR_NO_OPEN_DATA_BASE;
					goto exit_func;
				}

    //	CRIA STATEMENT

    if(condition != NULL)
    {
    	if(join != NULL)
    	{
    		length = snprintf(sql, sizeof(sql) - 1, "SELECT %s FROM %s %s WHERE %s", fields, table, join, condition);
    	}
    	else
    	{
    		length = snprintf(sql, sizeof(sql) - 1, "SELECT %s FROM %s WHERE %s", fields, table, condition);
    	}
    }
    else
    {
    	if(join != NULL)
    	{
    		length = snprintf(sql, sizeof(sql) - 1, "SELECT %s FROM %s %s", fields, table, join);
    	}
    	else
    	{
    		length = snprintf(sql, sizeof(sql) - 1, "SELECT %s FROM %s", fields, table);
    	}
    }

    if(DEV == 1)
    {
    	printf("[%s]", sql);
    }

    //	EXECUTA SELECT

    rc =	sqlite3_prepare_v2 (db, sql, length + 1, pstmt, NULL);

			if(rc != SQLITE_OK)
			{
				if (DEV == 1)
				{
					printf(">>> ERRO ao tentar executar um SELECT na tabela %s <<<", table);
					printf(">>> ERRO = %d", rc);
				}
				rc = RC_ERR_NO_EXEC_SELECT;
				goto exit_func;
			}

    //	CAPTURA NUMERO DE COLUNAS

    if(cols != NULL)
    {
        *cols =	sqlite3_column_count(*pstmt);

				if(*cols < 0)
				{
					if (DEV == 1)
					{
						printf(">>> ERRO ao tentar contar o numero de colunas da tabela  %s <<<", table);
					}
					goto exit_func;
				}
    }

    rc = RC_OK;

exit_func:

    sqlite3_close_v2   (db);

    return(rc);
}

//	========== VARREDURA ATE FINAL DO ARQUIVO

int db_sqLite_select_step(SQLITE_STMT stmt)
{
    int rc = RC_OK;

    if (stmt == NULL)
    {
    	if (DEV == 1)
    	{
    		printf(">>> ERRO parametros invalidos em db_sqLite_select_step <<<");
    	}
        return RC_ERR_PARAMETER_INVALID;
    }

    //	NEXT

    rc =	sqlite3_step(stmt);

			if(rc == SQLITE_ROW)          //	SE NAO FOR FIM DO ARQUIVO RETORNA OK
			{
				rc = RC_OK;
				goto exit_func;
			}

			if(rc == SQLITE_DONE)         //	SE FOR FIM DO ARQUIVO RETORNA EOF
			{
				rc = RC_ERR_EOF;
				goto exit_func;
			}

	if(DEV == 1)
	{
		printf(">>> ERRO ao tentar saltar para o proximo registro <<<");
		printf(">>> ERRO = %d", rc);
	}

	rc = RC_ERR_NO_SKIP;

exit_func:

    return(rc);
}

//	========== SELECIONA UMA DAS COLUNAS DO SELECT

int db_sqLite_select_column(SQLITE_STMT stmt, int inputIndex, char *outputName, int outputNameSize, char *outputValue, int outputValueSize)
{
    int  rc = RC_OK;
    int  cols;
    char *name;
    char *value;

    // SANITY CHECK

    if (stmt        == NULL ||
    	outputValue == NULL	)
    {
    	if(DEV == 1)
    	{
    		printf(">>> ERRO parametros invalidos em db_sqLite_select_column <<<");
    	}
        return RC_ERR_PARAMETER_INVALID;
    }

    if (outputValueSize <= 0    ||
       (outputName      != NULL &&
    	outputNameSize  <= 0))
    {
    	if(DEV == 1)
    	{
    		printf(">>> ERRO parametros invalidos em db_sqLite_select_column <<<");
    	}
        return RC_ERR_PARAMETER_INVALID;
    }

    //	VERIFICA O NUMERO DE COLUNAS NO STATEMENT

    cols =	sqlite3_column_count(stmt);

			if(cols < 0)
			{
				if(DEV == 1)
				{
					printf(">>> ERRO nenhuma coluna encontada <<<");
				}
				rc = RC_ERR_NO_COLUNS;
				goto exit_func;
			}

    // VERIFICA SE O INDICE EH INVALIDO OU SUPERIOR AO NUMERO DE COLUNAS DO STATEMENT

    if (inputIndex < 0 ||
    	inputIndex >= cols)
    {
    	if(DEV == 1)
    	{
    		printf(">>> ERRO coluna selecionada invalida ou superior ao numero de colunas <<<");
    	}
        rc = RC_ERR_NO_COLUNS;
        goto exit_func;
    }

    // CAPTURA O NOME DA COLUNA CASO SEJA NECESS�RIO

    if (outputName != NULL)
    {
        memset (outputName, 0, outputNameSize);
        name =	(char *)sqlite3_column_name(stmt, inputIndex);

				if(name != NULL)
				{
					strncpy (outputName, name, outputNameSize - 1);
				}
    }

    //	CAPTURA O VALOR DO CAMPO NO STATEMENT

    memset(outputValue, 0, outputValueSize);

    value =	(char *)sqlite3_column_text(stmt, inputIndex);

			if(value != NULL)
			{
				strncpy(outputValue, value, outputValueSize - 1);
			}

    rc = RC_OK;

exit_func:

    return(rc);
}

//	========== IDENTIFICA RETORNOS NOS LOGS

int mens_erro_SQL(int error, char *output)
{
    return RC_OK;
	char varaux[128] = {0x00};

	switch(error)
	{
	case RC_ERR_PARAMETER_INVALID: sprintf(varaux, "ERRO %04d - Parametros invalidos enviados a funcao", error); break;
	case RC_ERR_NO_OPEN_DATA_BASE: sprintf(varaux, "ERRO %04d - Ao conectar banco de dados"            , error); break;
	case RC_ERR_NO_INSERT:         sprintf(varaux, "ERRO %04d - Ao inserir registros na tabela"        , error); break;
	case RC_ERR_NO_UPDATE:         sprintf(varaux, "ERRO %04d - Ao atualizar o registro na tabela"     , error); break;
	case RC_ERR_ZERO_DATA_UPDATE:  sprintf(varaux, "ERRO %04d - Nenhum registro atualizado"            , error); break;
	case RC_ERR_NO_SELECT:         sprintf(varaux, "ERRO %04d - Nenhum registro selecionado"           , error); break;
	case RC_ERR_NO_COLUNS:         sprintf(varaux, "ERRO %04d - Nenhuma coluna encontrada"             , error); break;
	case RC_ERR_NO_REG:            sprintf(varaux, "ERRO %04d - Nenhum registro encontrado"            , error); break;
	case RC_ERR_NO_EXEC_SQL:       sprintf(varaux, "ERRO %04d - Ao executar o comando SQL"             , error); break;
	case RC_ERR_NO_DELETE:         sprintf(varaux, "ERRO %04d - Ao deletar registros da tabela"        , error); break;
	case RC_ERR_NO_EXEC_SELECT:    sprintf(varaux, "ERRO %04d - Ao executar o SELECT na tabela"        , error); break;
	case RC_ERR_EOF:               sprintf(varaux, "ERRO %04d - Fim de arquivo"                        , error); break;
	case RC_ERR_NO_SKIP:           sprintf(varaux, "ERRO %04d - Ao saltar de registro na tabela"       , error); break;
	case RC_ERR_SIZE_MISMATCH:     sprintf(varaux, "ERRO %04d - Tamanho de buffers incompativeis"      , error); break;
	}

	snprintf(output, strlen(varaux), "%s", varaux);

    return(error);
}

int db_sqLite_select(const char *databank, const char *table, const char *fields, const char *condition, char *final, size_t final_size)
{
    int rc;
    sqlite3 *db;
    char *zErrMsg    = 0;
    char sql[4096];
    const char *data = "Callback function called";

    //	SANITY CHECK
    if (databank == NULL || table == NULL || fields == NULL || final == NULL) {
        return printf("Null parameter received\n");
    }
    
    //	ABRE O DATABASE E RESETA QUALQUER STATEMENT (PARA CORRIGIR DB_CLOSE() VOLTANDO BUSY)

    rc =	/*sqlite3_open(databank, &db);*/sqlite3_open_v2(databank, &db, SQLITE_OPEN_READONLY, NULL);

			if(rc != SQLITE_OK)
			{
				rc = printf("Failed in open database: %s - %d\n", sqlite3_errmsg (db), rc);
				goto exit_func;
			}

    //	CREATE STATEMENT

    if(condition != NULL)
    {
        snprintf(sql, sizeof(sql) - 1, "SELECT %s FROM %s WHERE %s", fields, table, condition);
    }
    else
    {
        snprintf(sql, sizeof(sql) - 1, "SELECT %s FROM %s", fields, table);
    }

    //	EXECUTE STATEMENT

    printf("╔═╗╔═╗ ╦  \n");
    printf("╚═╗║═╬╗║  \n");
    printf("╚═╝╚═╝╚╩═╝\n");
    printf("[%s]\n", sql);

    select_count = 0;
    select_final = final;
    select_final_size = final_size;
    
    rc =	sqlite3_exec (db, sql, callback, (void *) data, &zErrMsg);

			if(rc != SQLITE_OK)
			{
				printf("Failed in exec SQL: %s - %d\n", zErrMsg, rc);
				sqlite3_free(zErrMsg);
				goto exit_func;
			}

			if(select_count == 0)
			{
				rc =	RC_ERR_NO_SELECT;
						printf("RC_ERR_NO_SELECT\n");
			}
			else
			{
				rc =	RC_OK;
			}

exit_func:

    sqlite3_close_v2   (db);
    
    return(rc);
}


int utl_SQL_select_original_Transaction(const char *pszDE90, char *psTransactionData, size_t uiDataSize)
{
    int  rc = 0;
    char szOutput		[1024] = {0x00};
    char szWhere		 [256] = {0x00};
    char szAux		 	 [128] = {0x00};
    char szNsuTerminal    [16] = {0x00};
    char szDatetime		  [16] = {0x00};
    char szNsuAcquirer	  [16] = {0x00};
    char szMti			   [8] = {0x00};

	// 0200 mti
	// 000007 nsu_terminal
	// 0222105151 datetime
	// 162300105742 nsu_acquirer
    //Opcionais: RespCode SerialNumber, EC, Amount, ProcessCode

    strncpy(szMti        , pszDE90 + 0 , 4);
    strncpy(szNsuTerminal, pszDE90 + 4 , 6);
    strncpy(szDatetime   , pszDE90 + 10, 10);
    strncpy(szNsuAcquirer, pszDE90 + 20, 12);

	sprintf(szWhere, "mti = '%.4s'  AND nsu_terminal = '%.6s' AND datetime = '%.10s' AND nsu_acquirer = '%.12s' LIMIT 1", szMti, szNsuTerminal, szDatetime, szNsuAcquirer);

	rc = db_sqLite_select(SQLITE_DB_DATABASE, SQLITE_TABLE_TRANSACTIONS, "*", szWhere, szOutput, sizeof(szOutput));
	if(rc != RC_OK){
		memset(szAux, 0x00, sizeof(szAux));
		mens_erro_SQL(rc, szAux);
		if(rc != RC_ERR_NO_SELECT) 
			return rc;
	}

	if (rc == RC_OK)
		memcpy(psTransactionData, szOutput, uiDataSize);

    return(rc);
}


int utl_SQL_GetLinesFromTable(char *inputDatabase, char *inputTable, char *inputColum, char *inputWhere)
{
	sqlite3      *db             = NULL;
	sqlite3_stmt *stmt;
    int           rc             = RC_OK;
    int           nCol           = 0;
    int           count          = 0;
    char	      zRes     [512] = {0x00};
    char          sqlQuery[4096] = {0x00};


    //	SANITY CHECK

    if (inputDatabase  == NULL || strlen(inputDatabase) <= 0 ||
    	inputTable     == NULL || strlen(inputTable)    <= 0)
    {
    	/*DEBUG*/printf("*** [SQL ERR] (db_sqLite_get_txt) - PARAMETRO INVALIDO");
        return(RC_ERR_PARAMETER_INVALID);
    }

    //	ABRE O DATABASE E RESETA QUALQUER STATEMENT (PARA CORRIGIR DB_CLOSE() VOLTANDO BUSY)

    rc =	sqlite3_open_v2(inputDatabase, &db, SQLITE_OPEN_READONLY, NULL);

			if(rc != SQLITE_OK)
			{
				/*DEBUG*/printf("*** [SQL ERR] (db_sqLite_get_txt) - ABERTURA BANCO DE DADOS [%s]", inputDatabase);

				rc =	RC_ERR_NO_OPEN_DATA_BASE;
						sqlite3_finalize(stmt);
						sqlite3_close_v2(db);
						return			(rc);
			}

    //	CRIA STATEMENT

	if(strlen(inputWhere) > 0)
		snprintf(sqlQuery, sizeof(sqlQuery) - 1, "SELECT count(%s) FROM %s WHERE %s", inputColum, inputTable, inputWhere);
	else
		snprintf(sqlQuery, sizeof(sqlQuery) - 1, "SELECT count(%s) FROM %s", inputColum, inputTable);

    // /*DEBUG*/printf("QUERY: [%s]\n", sqlQuery);

    //	EXECUTA STATEMENT

	rc =	sqlite3_prepare_v2(db, sqlQuery, -1, &stmt, NULL);

			if(rc != SQLITE_OK)
			{
				/*DEBUG*/printf("*** [SQL ERR] (db_sqLite_get_txt) - SELECAO DE REGISTRO NA TABELA [%s]", inputTable);

				rc =	RC_ERR_NO_SELECT;
						sqlite3_finalize(stmt);
						sqlite3_close_v2(db);
						return			(rc);
			}

    //	VERIFICA NUMERO DE COLUNAS DE RETORNO

	nCol =	sqlite3_column_count(stmt);

			if(nCol <= 0)
			{
				/*DEBUG*/printf("*** [SQL ERR] (db_sqLite_get_txt) - NENHUMA COLUNA ENCONTRADA NA TABELA [%s]", inputTable);

				rc =	RC_ERR_NO_COLUNS;
						sqlite3_finalize(stmt);
						sqlite3_close_v2(db);
						return			(rc);
			}

	//	VARRE STATEMENT CARREGANDO OS CAMPOS NA VARAVEL

	while(sqlite3_step(stmt) != SQLITE_DONE)
	{
		count++;
		sprintf(zRes, "%s", sqlite3_column_text(stmt, 0));
	}

	if(count <= 0)
	{
		/*DEBUG*/printf("*** [SQL ERR] (db_sqLite_get_txt) - NENHUM REGISTRO ENCONTRADO");

		rc =	RC_ERR_NO_REG;
				sqlite3_finalize(stmt);
				sqlite3_close_v2(db);
				return			(rc);
	}

		//strncpy(output, zRes, output_size);

		/*DEBUG*/
		if(strlen(zRes) > 0)
		{
			// /*DEBUG*/printf("RESP : |%s|\n", zRes);
			rc = atoi(zRes);
		}
		else
		{
			// /*DEBUG*/printf("RESP : |**NENHUM DADO NA RESPOSTA**|\n");
			rc = 0;
		}

	sqlite3_finalize(stmt);
    sqlite3_close_v2(db);

	return(rc);
}


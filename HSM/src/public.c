char buffer[MAX_MSG];
int gPortaServidor;

int util_log_Append(char *data)
{
    FILE *fd;
    char zName[64] = {0x00};
    int  i;

    sprintf(zName, "%s", LOG_PATH);
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


char* doDukptDecryptData(const char * psBDK, const char * psKSN, const char * psData, int iSize) 
{
	char szAux[512] = {0x00};
	char szHex  [4] = {0x00};
	u8   bAux [248] = {0x00};
    u32  iAux       = Str2Hex(32, psBDK, bAux);
	u8   *bdk       = HexDuplicate(bAux, iAux);
		 iAux       = Str2Hex(20, psKSN, bAux);
	u8   *ksn       = HexDuplicate(bAux, iAux);
		 iAux       = Str2Hex(iSize, psData, bAux);
	u8   *buf_enc   = HexDuplicate(bAux, iAux);

	DukptData_t data_enc = { 
		.Data = buf_enc,
		.Len = iAux,
	};
	
	u8 buf_dec[512];
	DukptData_t data_dec = { 
		.Data = buf_dec,
		.Len = 512,
	};
	
	DukptDecryptDataBdk(&data_dec, &data_enc, ksn, bdk);
	for (int i = 0; i < data_dec.Len; i++){
		sprintf(szHex, "%02X", buf_dec[i]);
		strncat(szAux, szHex, 2);
	}

	return HexDuplicate(szAux, iSize);
}

char* doDukptEncryptData(const char * psBDK, const char * psKSN, const char * psData, int iSize) 
{
	char szAux[512] = {0x00};
	char szHex  [4] = {0x00};
	u8   bAux [248] = {0x00};
    u32  iAux       = Str2Hex(32, psBDK, bAux);
	u8   *bdk       = HexDuplicate(bAux, iAux);
		 iAux       = Str2Hex(20, psKSN, bAux);
	u8   *ksn       = HexDuplicate(bAux, iAux);
		 iAux       = Str2Hex(iSize, psData, bAux);
	u8   *buf_dec   = HexDuplicate(bAux, iAux);

	DukptData_t data_dec = { 
		.Data = buf_dec,
		.Len = iAux,
	};
	
	u8 buf_enc[512];
	DukptData_t data_enc = { 
		.Data = buf_enc,
		.Len = 512,
	};

	DukptEncryptDataBdk(&data_enc, &data_dec, ksn, bdk);
	for (int i = 0; i < data_enc.Len; i++){
		sprintf(szHex, "%02X", buf_enc[i]);
		strncat(szAux, szHex, 2);
	}

	return HexDuplicate(szAux, iSize);
}

char* doDukptDecryptPIN(const char * psBDK, const char * psKSN, const char * psData, int iSize) 
{
	char szAux[512] = {0x00};
	char szHex  [4] = {0x00};
	u8   bAux [248] = {0x00};
    u32  iAux       = Str2Hex(32, psBDK, bAux);
	u8   *bdk       = HexDuplicate(bAux, iAux);
		 iAux       = Str2Hex(20, psKSN, bAux);
	u8   *ksn       = HexDuplicate(bAux, iAux);
		 iAux       = Str2Hex(iSize, psData, bAux);
	u8   *buf_enc   = HexDuplicate(bAux, iAux);

	DukptData_t data_enc = { 
		.Data = buf_enc,
		.Len = iAux,
	};
	
	u8 buf_dec[512];
	DukptData_t data_dec = { 
		.Data = buf_dec,
		.Len = 512,
	};
	
	DukptDecryptPINBdk(&data_dec, &data_enc, ksn, bdk);
	for (int i = 0; i < data_dec.Len; i++){
		sprintf(szHex, "%02X", buf_dec[i]);
		strncat(szAux, szHex, 2);
	}

	return HexDuplicate(szAux, iSize);
}


char* doDukptEncryptPIN(const char * psBDK, const char * psKSN, const char * psData, int iSize) 
{
	char szAux[512] = {0x00};
	char szHex  [4] = {0x00};
	u8   bAux [248] = {0x00};
    u32  iAux       = Str2Hex(32, psBDK, bAux);
	u8   *bdk       = HexDuplicate(bAux, iAux);
		 iAux       = Str2Hex(20, psKSN, bAux);
	u8   *ksn       = HexDuplicate(bAux, iAux);
		 iAux       = Str2Hex(iSize, psData, bAux);
	u8   *buf_enc   = HexDuplicate(bAux, iAux);

	DukptData_t data_enc = { 
		.Data = buf_enc,
		.Len = iAux,
	};
	
	u8 buf_dec[512];
	DukptData_t data_dec = { 
		.Data = buf_dec,
		.Len = 512,
	};
	
	DukptEncryptPINBdk(&data_dec, &data_enc, ksn, bdk);
	for (int i = 0; i < data_dec.Len; i++){
		sprintf(szHex, "%02X", buf_dec[i]);
		strncat(szAux, szHex, 2);
	}

	return HexDuplicate(szAux, iSize);
}


char* doDukptDecryptPinBlock(const char * psPIN, const char * psPAN, int iPanSize) 
{
	char szAux[512] = {0x00};
	char szPan [32] = {0x00};
	char szHex  [4] = {0x00};
	u8   bAux [248] = {0x00};
	memcpy(szPan, psPAN, sizeof(char) * (iPanSize - 1));
	memcpy(szAux, &psPAN[(iPanSize - 1) - 12], 12);
	memset(szPan, 0x00, sizeof(szPan));
	sprintf(szPan, "0000%s", szAux);
	int  iPinSize   = 0;
    u32  iAux       = Str2Hex(16, szPan, bAux);
	u8   *pan       = HexDuplicate(bAux, iAux);
		 iAux       = Str2Hex(16, psPIN, bAux);
	u8   *pin       = HexDuplicate(bAux, iAux);

	u8 pin_dec[8];	
	Xor(pin_dec, pin, pan, 8);

	memset(szPan, 0x00, sizeof(szPan));
	for (int i = 0; i < 8; i++){
		sprintf(szHex, "%02X", pin_dec[i]);
		strncat(szPan, szHex, 2);
	}

	for (int i = 2; i < 16; i++){
		if (szPan[i] != 'F'){
			szPan[iPinSize++] = szPan[i];
		}else{
			szPan[iPinSize] = '\0';
		}
	}

	return HexDuplicate(szPan, iPinSize);
}


char* doDukptEncryptPinBlock(const char * psPIN, int iPinSize, const char * psPAN, int iPanSize) 
{
	char szAux[512] = {0x00};
	char szPan [32] = {0x00};
	char szPin [32] = {0x00};
	char szHex  [4] = {0x00};
	u8   bAux [248] = {0x00};
	sprintf(szPin, "04%s%.*s", psPIN, 14 - iPinSize, "FFFFFFFFFFFFFF");
	memcpy(szPan, psPAN, sizeof(char) * (iPanSize - 1));
	memcpy(szAux, &psPAN[(iPanSize - 1) - 12], 12);
	memset(szPan, 0x00, sizeof(szPan));
	sprintf(szPan, "0000%s", szAux);
    u32  iAux       = Str2Hex(16, szPan, bAux);
	u8   *pan       = HexDuplicate(bAux, iAux);
		 iAux       = Str2Hex(16, szPin, bAux);
	u8   *pin       = HexDuplicate(bAux, iAux);

	u8 pin_enc[8];	
	Xor(pin_enc, pin, pan, 8);

	memset(szAux, 0x00, sizeof(szAux));
	for (int i = 0; i < 8; i++){
		sprintf(szHex, "%02X", pin_enc[i]);
		strncat(szAux, szHex, 2);
	}
	return HexDuplicate(szAux, 16);
}

char* doMkWkDecryptData(const char * psMK, const char * psWK, const char * psData, int iSize) 
{
	char szAux[512] = {0x00};
	char szHex  [4] = {0x00};
	u8   bAux [248] = {0x00};
    u32  iAux       = Str2Hex(32, psMK, bAux);
	u8   *mk        = HexDuplicate(bAux, iAux);
		 iAux       = Str2Hex(32, psWK, bAux);
	u8   *wk        = HexDuplicate(bAux, iAux);
		 iAux       = Str2Hex(iSize, psData, bAux);
	u8   *buf_enc   = HexDuplicate(bAux, iAux);

	MkWkData_t data_enc = { 
		.Data = buf_enc,
		.Len = iAux,
	};
	
	u8 buf_dec[512];
	MkWkData_t data_dec = { 
		.Data = buf_dec,
		.Len = 512,
	};
	
	MkWkDecryptData(&data_dec, &data_enc, wk, mk);
	for (int i = 0; i < data_dec.Len; i++){
		sprintf(szHex, "%02X", buf_dec[i]);
		strncat(szAux, szHex, 2);
	}

	return HexDuplicate(szAux, iSize);
}
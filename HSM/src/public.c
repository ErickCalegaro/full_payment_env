
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
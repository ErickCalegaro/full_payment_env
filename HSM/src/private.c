
static void And( _OUT u8 * const out,
		_IN u8 * const in1,
		_IN u8 * const in2,
		_IN fu8 len	) {
	for (fu8 i = 0; i < len; i++) {
		out[i] = in1[i] & in2[i];
	}
}

static void Or( _OUT u8 * const out,
		_IN u8 * const in1,
		_IN u8 * const in2,
		_IN fu8 len	) {
	for (fu8 i = 0; i < len; i++) {
		out[i] = in1[i] | in2[i];
	}
}

static void Xor( _OUT u8 * const out,
		_IN u8 * const in1,
		_IN u8 * const in2,
		_IN fu8 len	) {
	for (fu8 i = 0; i < len; i++) {
		out[i] = in1[i] ^ in2[i];
	}
}

static BOOLX HaveOnes( _IN u8 * const in,
		_IN fu8 len	) {
	for (fu8 i = 0; i < len; i++) {
		if( in[i] != 0 ) {
			return TRUE;
		}
	}
	
	return FALSE;
}

static int Hex2Str(int array_length, const unsigned char *byte_array, char *hex_str)
{
    char szAux[4];

    for (int i = 0; i < (array_length * 2); i++){
        sprintf(szAux, "%02X", byte_array[i]);
        strncat(hex_str, szAux, 2);
    }

    return (array_length * 2);
}


static int Str2Hex(int str_length, const char *hex_str, unsigned char *byte_array)
{
    int hex_str_len = str_length;
    int i = 0, j = 0;
    int byte_array_size = (hex_str_len+1)/2;

    for (; i < hex_str_len; i+=2, j++){
        if (sscanf(&(hex_str[i]), "%2hhx", &(byte_array[j])) != 1){
            return -1;
        }
    }

    return byte_array_size;
}


static u8* HexDuplicate(const uint8_t* pbHexIn, int iLen) 
{
    uint8_t* pbHexOut = malloc(iLen + 1);
    if (pbHexOut) memcpy(pbHexOut, pbHexIn, iLen);
    return pbHexOut;
}

static void DeriveKeyFromIpek( _OUT u8 * const ipek_drvd16,
		_IN u8 * const ksn10,
		_IN u8 * const ipek16 ) {
	memcpy( ipek_drvd16, ipek16, 16 );
	
	u8 counter[8];
	memcpy( counter, &ksn10[2], 8 );
	
	counter[0] = 0x00;
	counter[1] = 0x00;
	counter[2] = 0x00;
	counter[3] = 0x00;
	counter[4] = 0x00;
	counter[5] &= 0x1F;
	counter[6] &= 0xFF;
	counter[7] &= 0xFF;

	u8 serial[8];
	memcpy( serial, &ksn10[2], 8 );
	
	serial[5] &= 0xE0;
	serial[6] &= 0x00;
	serial[7] &= 0x00;

	u8 shiftr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00};
	
	u8 crypto_register_1[8];
	memcpy(crypto_register_1, serial, 8);
	
	u64 intShiftr = 16*65536;
	
	while( intShiftr > 0 ) {
		u8 temp[8];
		
		And( temp, shiftr, counter, 8 );
		
		if( HaveOnes(temp, 8) ) {
			Or(serial, serial, shiftr, 8);
			
			u8 crypto_register_2[8];
			Xor(crypto_register_2, serial, &ipek_drvd16[8], 8 );

			{
				u8 crypto_register_2_temp[8];

				DES_key_schedule SchKey;

				DES_set_key_unchecked((const_DES_cblock *) &ipek_drvd16[0], &SchKey);

				DES_ecb_encrypt((const_DES_cblock *) crypto_register_2, (const_DES_cblock *) crypto_register_2_temp, &SchKey, DES_ENCRYPT);
				
				memcpy(crypto_register_2, crypto_register_2_temp, 8);
			}
			
			Xor( crypto_register_2, crypto_register_2, &ipek_drvd16[8], 8);
			
			Xor( ipek_drvd16, ipek_drvd16, BDK_MASK, 16 );
			
			Xor( crypto_register_1, serial, &ipek_drvd16[8], 8 );
			
			{
				u8 crypto_register_1_temp[8];

				DES_key_schedule SchKey;

				DES_set_key_unchecked((const_DES_cblock *) &ipek_drvd16[0], &SchKey);

				DES_ecb_encrypt((const_DES_cblock *) crypto_register_1, (const_DES_cblock *) crypto_register_1_temp, &SchKey, DES_ENCRYPT);
				
				memcpy(crypto_register_1, crypto_register_1_temp, 8);
			}
			
			Xor( crypto_register_1, crypto_register_1, &ipek_drvd16[8], 8 );
			
			memcpy( &ipek_drvd16[0], crypto_register_1, 8 );
			memcpy( &ipek_drvd16[8], crypto_register_2, 8 );
		}
		
		intShiftr >>= 1;
		
		shiftr[7] = (intShiftr >> 0) & 0xFF;
		shiftr[6] = (intShiftr >> 8) & 0xFF;
		shiftr[5] = (intShiftr >> 16) & 0xFF;
		shiftr[4] = (intShiftr >> 24) & 0xFF;
		shiftr[3] = (intShiftr >> 32) & 0xFF;
		shiftr[2] = (intShiftr >> 40) & 0xFF;
		shiftr[1] = (intShiftr >> 48) & 0xFF;
		shiftr[0] = (intShiftr >> 56) & 0xFF;
	}
}

static void DeriveKeyFromBdk(_OUT u8 * const key_drvd16,
		_IN u8 * const ksn10,
		_IN u8 * const bdk16) {
	u8 ipek[16];
	
	DukptGenerateIpek(ipek, bdk16, ksn10);

	DeriveKeyFromIpek(key_drvd16, ksn10, ipek);
}

static void CalculateDataEncryptionKey(_OUT u8 * const dek16, _IN u8 * const key16) {
	u8 variant_key_L_R[16];

	u8 variant_key_L[8];
	Xor(variant_key_L, &key16[0], data_encryption_variant_constant_both_ways, 8);
	memcpy(&variant_key_L_R[0], variant_key_L, 8);

	u8 variant_key_R[8];
	Xor(variant_key_R, &key16[8], data_encryption_variant_constant_both_ways, 8);
	memcpy(&variant_key_L_R[8], variant_key_R, 8);

	{
		u8 encryption_key_L[24];
		
		DES_key_schedule SchKey[2];

		DES_set_key_unchecked((const_DES_cblock *) &variant_key_L_R[0], &SchKey[0]);
		DES_set_key_unchecked((const_DES_cblock *) &variant_key_L_R[8], &SchKey[1]);

		DES_cblock iv = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

		DES_ede3_cbc_encrypt((unsigned char*) variant_key_L, (unsigned char*) encryption_key_L, 8,
				&SchKey[0], &SchKey[1], &SchKey[0],
				&iv, DES_ENCRYPT);
		
		memcpy(&dek16[0], encryption_key_L, 8);
	}

	{
		u8 encryption_key_R[24];
		
		DES_key_schedule SchKey[2];

		DES_set_key_unchecked((const_DES_cblock *) &variant_key_L_R[0], &SchKey[0]);
		DES_set_key_unchecked((const_DES_cblock *) &variant_key_L_R[8], &SchKey[1]);

		DES_cblock iv = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

		DES_ede3_cbc_encrypt((unsigned char*) variant_key_R, (unsigned char*) encryption_key_R, 8,
				&SchKey[0], &SchKey[1], &SchKey[0],
				&iv, DES_ENCRYPT);
		
		memcpy(&dek16[8], encryption_key_R, 8);
	}

}

static void CalculatePINEncryptionKey(_OUT u8 * const dek16, _IN u8 * const key16) {
	u8 variant_key_L_R[16];

	u8 variant_key_L[8];
	Xor(variant_key_L, &key16[0], pin_encryption_variant_constant_both_ways, 8);
	memcpy(&variant_key_L_R[0], variant_key_L, 8);

	u8 variant_key_R[8];
	Xor(variant_key_R, &key16[8], pin_encryption_variant_constant_both_ways, 8);
	memcpy(&variant_key_L_R[8], variant_key_R, 8);

	memcpy(&dek16[0], variant_key_L_R, 16);
}

//-----------------------------------------------------------------------------------------

void DukptDecryptDataBdk( _OUT DukptData_t * const out_data,
		_IN DukptData_t * const in_data,
		_IN u8 * const ksn10,
		_IN u8 * const bdk16 ) {
	ASSERT(out_data);
	ASSERT(out_data->Data);
	ASSERT(in_data);
	ASSERT(in_data->Data);
	ASSERT(ksn10);
	ASSERT(bdk16);
	
	if( in_data->Len == 0 ) {
		out_data->Len = 0;
		return;
	}

	u8 derivedKey[16];
	
	DeriveKeyFromBdk(derivedKey, ksn10, bdk16);

	u8 pinKey[16];
	
	CalculateDataEncryptionKey(pinKey, derivedKey);

	DES_key_schedule SchKey[2];

	DES_set_key_unchecked((const_DES_cblock *) &pinKey[0], &SchKey[0]);
	DES_set_key_unchecked((const_DES_cblock *) &pinKey[8], &SchKey[1]);

	DES_cblock iv = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	DES_ede3_cbc_encrypt((unsigned char*) in_data->Data, (unsigned char*) out_data->Data, in_data->Len,
			&SchKey[0], &SchKey[1], &SchKey[0],
			&iv, DES_DECRYPT);
	
	out_data->Len = ((in_data->Len-1) / 8) * 8 + 8;
}

void DukptDecryptPINBdk( _OUT DukptData_t * const out_data,
		_IN DukptData_t * const in_data,
		_IN u8 * const ksn10,
		_IN u8 * const bdk16 ) {
	ASSERT(out_data);
	ASSERT(out_data->Data);
	ASSERT(in_data);
	ASSERT(in_data->Data);
	ASSERT(ksn10);
	ASSERT(bdk16);
	
	if( in_data->Len == 0 ) {
		out_data->Len = 0;
		return;
	}

	u8 derivedKey[16];
	
	DeriveKeyFromBdk(derivedKey, ksn10, bdk16);

	u8 pinKey[16];
	
	CalculatePINEncryptionKey(pinKey, derivedKey);

	DES_key_schedule SchKey[2];

	DES_set_key_unchecked((const_DES_cblock *) &pinKey[0], &SchKey[0]);
	DES_set_key_unchecked((const_DES_cblock *) &pinKey[8], &SchKey[1]);

	DES_cblock iv = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	DES_ede3_cbc_encrypt((unsigned char*) in_data->Data, (unsigned char*) out_data->Data, in_data->Len,
			&SchKey[0], &SchKey[1], &SchKey[0],
			&iv, DES_DECRYPT);
	
	out_data->Len = ((in_data->Len-1) / 8) * 8 + 8;
}

void DukptEncryptDataBdk( _OUT DukptData_t * const out_data,
		_IN DukptData_t * const in_data,
		_IN u8 * const ksn10,
		_IN u8 * const bdk16 ) {
	ASSERT(out_data);
	ASSERT(out_data->Data);
	ASSERT(in_data);
	ASSERT(in_data->Data);
	ASSERT(ksn10);
	ASSERT(bdk16);
	
	if( in_data->Len == 0 ) {
		out_data->Len = 0;
		return;
	}
	
	u8 derivedKey[16];
	
	DeriveKeyFromBdk(derivedKey, ksn10, bdk16);
	
	u8 pinKey[16];
	
	CalculateDataEncryptionKey(pinKey, derivedKey);

	DES_key_schedule SchKey[2];

	DES_set_key_unchecked((const_DES_cblock *) &pinKey[0], &SchKey[0]);
	DES_set_key_unchecked((const_DES_cblock *) &pinKey[8], &SchKey[1]);

	DES_cblock iv = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	DES_ede3_cbc_encrypt((unsigned char*) in_data->Data, (unsigned char*) out_data->Data, in_data->Len,
			&SchKey[0], &SchKey[1], &SchKey[0],
			&iv, DES_ENCRYPT);
	
	out_data->Len = ((in_data->Len-1) / 8) * 8 + 8;
}


void DukptEncryptPINBdk( _OUT DukptData_t * const out_data,
		_IN DukptData_t * const in_data,
		_IN u8 * const ksn10,
		_IN u8 * const bdk16 ) {
	ASSERT(out_data);
	ASSERT(out_data->Data);
	ASSERT(in_data);
	ASSERT(in_data->Data);
	ASSERT(ksn10);
	ASSERT(bdk16);
	
	if( in_data->Len == 0 ) {
		out_data->Len = 0;
		return;
	}
	
	u8 derivedKey[16];
	
	DeriveKeyFromBdk(derivedKey, ksn10, bdk16);
	
	u8 pinKey[16];
	
	CalculatePINEncryptionKey(pinKey, derivedKey);

	DES_key_schedule SchKey[2];

	DES_set_key_unchecked((const_DES_cblock *) &pinKey[0], &SchKey[0]);
	DES_set_key_unchecked((const_DES_cblock *) &pinKey[8], &SchKey[1]);

	DES_cblock iv = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	DES_ede3_cbc_encrypt((unsigned char*) in_data->Data, (unsigned char*) out_data->Data, in_data->Len,
			&SchKey[0], &SchKey[1], &SchKey[0],
			&iv, DES_ENCRYPT);
	
	out_data->Len = ((in_data->Len-1) / 8) * 8 + 8;
}

void DukptGenerateIpek( _OUT u8 * const ipek16,
		_IN u8 * const bdk16,
		_IN u8 * const ksn10 ) {
	ASSERT(ipek16);
	ASSERT(bdk16);
	ASSERT(ksn10);

	u8 partKsn[8];

	memcpy( partKsn, ksn10, 8);

	partKsn[7] &= 0xE0;

	{
		u8 leftIpek[24];

		DES_key_schedule SchKey[2];

		DES_set_key_unchecked((const_DES_cblock *)&bdk16[0], &SchKey[0]);
		DES_set_key_unchecked((const_DES_cblock *)&bdk16[8], &SchKey[1]);

		DES_cblock iv = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

		DES_ede3_cbc_encrypt( (unsigned char*)partKsn, (unsigned char*)leftIpek, 8,
				&SchKey[0], &SchKey[1], &SchKey[0],
				&iv, DES_ENCRYPT);

		memcpy( &ipek16[0], leftIpek, 8 );
	}

	u8 derivedBdk[16];

	for (fu8 i = 0; i < 16; i++) {
		derivedBdk[i] = bdk16[i] ^ BDK_MASK[i];
	}

	{
		u8 rightIpek[24];

		DES_key_schedule SchKey[2];

		DES_set_key_unchecked((const_DES_cblock *)&derivedBdk[0], &SchKey[0]);
		DES_set_key_unchecked((const_DES_cblock *)&derivedBdk[8], &SchKey[1]);

		DES_cblock iv = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

		DES_ede3_cbc_encrypt( (unsigned char*)partKsn, (unsigned char*)rightIpek, 8,
				&SchKey[0], &SchKey[1], &SchKey[0],
				&iv, DES_ENCRYPT);

		memcpy( &ipek16[8], rightIpek, 8 );
	}
}


//-----------------------------------------------------------------------------------------
void MkWkTranslateKey(_IN u8 * const wk16,
					  _IN u8 * const mk16,
					  _OUT u8 * const key ) {
	ASSERT(wk16);
	ASSERT(mk16);
	ASSERT(key);

	/* Triple DES key for Encryption and Decryption */
	DES_cblock Key1, Key2;
	DES_key_schedule SchKey1,SchKey2;

	memcpy(Key1, &mk16[0], 8);
	memcpy(Key2, &mk16[8], 8);

	/* Buffers for Encryption and Decryption */
	unsigned char input_temp[8];
	unsigned char output_temp[8];
	unsigned char output_data[16] = {0x00};

	/* Check for Weak key generation */
	DES_set_key_unchecked(&Key1, &SchKey1);
	DES_set_key_unchecked(&Key2, &SchKey2);

	for (int i = 0; i < 2; i++){
		/* Triple-DES ECB Encryption */
		memcpy(input_temp, &wk16[i*8], 8);
		DES_ecb2_encrypt( &input_temp, &output_temp, &SchKey1, &SchKey2, DES_DECRYPT);
		memcpy(&output_data[i*8], output_temp, 8);
	}

	memcpy(key, output_data, 16);

	return;
}

void MkWkDecryptData( _OUT MkWkData_t * const out_data,
		_IN MkWkData_t * const in_data,
		_IN u8 * const wk16,
		_IN u8 * const mk16 ) {
	ASSERT(out_data);
	ASSERT(out_data->Data);
	ASSERT(in_data);
	ASSERT(in_data->Data);
	ASSERT(wk16);
	ASSERT(mk16);
	
	if( in_data->Len == 0 || in_data->Len % 8) {
		out_data->Len = 0;
		return;
	}

	u8 derivedKey[16];
	
	MkWkTranslateKey(wk16, mk16, derivedKey);

	/* Triple DES key for Encryption and Decryption */
	DES_cblock Key1, Key2;
	DES_key_schedule SchKey1,SchKey2;

	memcpy(Key1, &derivedKey[0], 8);
	memcpy(Key2, &derivedKey[8], 8);

	/* Buffers for Encryption and Decryption */
	unsigned char input_temp[8];
	unsigned char output_temp[8];

	/* Check for Weak key generation */
	DES_set_key_unchecked(&Key1, &SchKey1);
	DES_set_key_unchecked(&Key2, &SchKey2);

	int count = in_data->Len / 8;

	for (int i = 0; i < count; i++){
		/* Triple-DES ECB Encryption */
		memcpy(input_temp, &in_data->Data[i*8], 8);
		DES_ecb2_encrypt( &input_temp, &output_temp, &SchKey1, &SchKey2, DES_DECRYPT);
		memcpy(&out_data->Data[i*8], output_temp, 8);
	}

	out_data->Len = in_data->Len;
}
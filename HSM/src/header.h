#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include <openssl/des.h>
#include <pthread.h>
#include <stdarg.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#pragma once

#define VARIANT_OFF 		0
#define VARIANT_REQUEST 	1
#define VARIANT_RESPONSE 	2
#define VARIANT_PIN 	 	3
#define VARIANT_MAC 		4
#define DATA_VARIANT  		VARIANT_REQUEST

#ifndef DATA_VARIANT
#error "DATA VARIANT NOT DEFINED"
#endif

#ifndef TRUE
#define TRUE				1
#endif

#ifndef FALSE
#define FALSE				0
#endif

#define _IN                 const
#define _OUT
#define _IO
#define ASSERT(X)    		assert(X)
#define byte				uint8_t

#define MAX_MSG 			1024
#define HSM_PORT    		55556

typedef struct {
    char sKSN 		[20+1];            /* Key Sequence Number */
    char sPINBlock 	[16+1];            /* PIN Block */
    char sPINDec 	 [8+1];            /* PIN Aberto */
    char sDataEnc  [999+1];            /* Dados Criptografados */
    char sDataDec  [999+1];            /* Dados Descriptografados */
    char sPAN 		[19+1];			   /* Personal Acount Number */
    char sTrack1	[76+1];			   /* Trilha 1 do cartão */
    char sTrack2	[37+1];			   /* Trilha 2 do cartão */
    char sDTV 		 [4+1];			   /* Data de Validade do cartão */
    char sCVV 		 [4+1];			   /* Codigo de Verificação do portador */
} st_CryptoData;

//-----------------------------------------------------------------------------------------

typedef unsigned __int128 	u128;
typedef uint64_t			u64;
typedef uint32_t			u32;
typedef uint16_t			u16;
typedef uint8_t				u8;

typedef signed __int128 	s128;
typedef int64_t				s64;
typedef int32_t				s32;
typedef int16_t				s16;
typedef int8_t				s8;

typedef uint_least32_t		lu32;
typedef uint_least16_t		lu16;
typedef uint_least8_t		lu8;

typedef int_least32_t		ls32;
typedef int_least16_t		ls16;
typedef int_least8_t		ls8;

typedef int_fast64_t		fs64;
typedef int_fast32_t		fs32;
typedef int_fast16_t		fs16;
typedef int_fast8_t			fs8;

typedef uint_fast64_t		fu64;
typedef uint_fast32_t		fu32;
typedef uint_fast16_t		fu16;
typedef uint_fast8_t		fu8;

typedef uint8_t				BOOL8;
typedef uint32_t			BOOLX;

typedef struct {
    u8 * Data;
    u16 Len;
} DukptData_t;

typedef struct {
    u8 * Data;
    u16 Len;
} MkWkData_t;

#if (DATA_VARIANT == VARIANT_REQUEST)
static const u8 data_encryption_variant_constant_both_ways[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00};
#elif (DATA_VARIANT == VARIANT_RESPONSE)
static const u8 data_encryption_variant_constant_both_ways[8] = {0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00};
#elif (DATA_VARIANT == VARIANT_PIN)
static const u8 data_encryption_variant_constant_both_ways[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
#elif (DATA_VARIANT == VARIANT_MAC)
static const u8 data_encryption_variant_constant_both_ways[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00};
#else
static const u8 data_encryption_variant_constant_both_ways[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif

static const u8 pin_encryption_variant_constant_both_ways[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};

static const u8 BDK_MASK[16] = {0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00};

//-----------------------------------------------------------------------------------------

extern void DukptGenerateIpek( _OUT u8 * const ipek16,
		_IN u8 * const bdk16,
		_IN u8 * const ksn10 );

extern void DukptDecryptDataBdk( _OUT DukptData_t * const out_data,
		_IN DukptData_t * const in_data,
		_IN u8 * const ksn10,
		_IN u8 * const bdk16 );

extern void DukptEncryptDataBdk( _OUT DukptData_t * const out_data,
		_IN DukptData_t * const in_data,
		_IN u8 * const ksn10,
		_IN u8 * const bdk16 );

extern void DukptDecryptPINBdk( _OUT DukptData_t * const out_data,
		_IN DukptData_t * const in_data,
		_IN u8 * const ksn10,
		_IN u8 * const bdk16 );

extern void DukptEncryptPINBdk( _OUT DukptData_t * const out_data,
		_IN DukptData_t * const in_data,
		_IN u8 * const ksn10,
		_IN u8 * const bdk16 );


//-----------------------------------------------------------------------------------------


extern char* doDukptDecryptData(const char * psBDK, 
	const char * psKSN, 
	const char * psData, 
	int iSize);

extern char* doDukptEncryptData(const char * psBDK, 
	const char * psKSN, 
	const char * psData, 
	int iSize);

extern char* doDukptDecryptPIN(const char * psBDK, 
	const char * psKSN, 
	const char * psPIN, 
	int iSize);

extern char* doDukptEncryptPIN(const char * psBDK, 
	const char * psKSN, 
	const char * psPinBlock, 
	int iSize);

extern char* doDukptDecryptPinBlock(const char * psPinBlock, 
	const char * psPAN, 
	int iPanSize);

extern char* doDukptEncryptPinBlock(const char * psPIN, 
	int iPinSize, 
	const char * psPAN, 
	int iPanSize);

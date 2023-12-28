#ifndef __DEF_TYPES_H__
#define __DEF_TYPES_H__

typedef struct{
	char mti			[4+1];		// 0 	N 	4 	MTI
	char bitmap			[32+1];		// 1 	B 	32 	Bitmap
	char processCode	[6+1];		// 3 	N 	6 	Código de processamento
	char amount			[12+1];		// 4 	N 	12 	Valor da transação
	char gmtDateTime	[10+1];		// 7 	N 	10 	Data e hora GMT0 (MMDDhhmmss)
	char localNSU		[6+1];		// 11 	N 	6 	NSU do terminal
	char localTime		[6+1];		// 12 	N 	6 	Hora local da transação (hhmmss)
	char localDate		[4+1];		// 13 	N 	4 	Data local da transação (MMDD)
	char countryCode	[3+1];		// 19 	N 	3 	Código de país do adiquirente
	char entryMode		[3+1];		// 22 	N 	3 	Modo de entrada
	char PanSeqNumber	[3+1];		// 23 	N 	3 	PAN Sequence Number
	char hostNSU		[6+1];		// 38 	N 	6 	NSU do HOST
	char responseCode	[2+1];		// 39 	AN 	2 	Código de resposta
	char serialNumber	[16+1];		// 41 	AN 	16 	Numero de série do terminal
	char ecDocument		[20+1];		// 42 	N 	20 	CNPJ/CPF do estabelecimento commercial
	char currencyCode	[3+1];		// 49 	N 	3 	Código da Moéda
	char pinBlock		[16+1];		// 52 	Z 	16 	PIN Block
	char emvData		[999+1];	// 55 	H 	LLL Tags EMV
	char lastTrsData	[999+1];	// 60 	ANS LLL Dados de confirmação da ultima transação
	char terminalInfo	[999+1];	// 61 	ANS LLL Informações do Ponto de Venda
	char receiptEC		[999+1];	// 62 	ANS LLL Mensagens do HOST/Comprovante Lojista
	char adtTrsData		[999+1];	// 63 	ANS LLL Dados adicionais da transação
	char installments	[2+1];		// 67 	N 	2 	Numero de parcelas
	char managementCode	[3+1];		// 70 	N 	3 	Código de gerenciamento
	char primalTrsData	[999+1];	// 90 	AN 	LLL Dados da transação Original
	char paymentMethod	[2+1];		// 118 	AN 	LLL Forma de Pagamento
	char versionDetail	[999+1];	// 120 	ANS LLL Detalhamento de Versões
	char decryptData	[999+1];	// 121 	AN 	LLL Dados para criptografia
	char simCardInfo	[999+1];	// 123 	ANS LLL Dados do SIM Card
	char receiptHolder	[999+1];	// 124 	ANS LLL Comprovante do Cliente
	char cryptedData	[999+1];	// 125 	Z 	LLL Dados sensiveis da transação
	char acquirerNSU	[999+1];	// 127 	AN 	LLL NSU adiquirente
} ISO_FIN_MODAL_T; //12kb total


typedef struct{
	char mti			[4+1];		// 0 	N 	4 	MTI
	char bitmap			[32+1];		// 1 	B 	32 	Bitmap
	char processCode	[6+1];		// 3 	N 	6 	Código de processamento
	char amount			[16+1];		// 4 	N 	12 	Valor da transação
	char gmtDateTime	[10+1];		// 7 	N 	10 	Data e hora GMT0 (MMDDhhmmss)
	char localNSU		[6+1];		// 11 	N 	6 	NSU do terminal
	char localTime		[6+1];		// 12 	N 	6 	Hora local da transação (hhmmss)
	char localDate		[4+1];		// 13 	N 	4 	Data local da transação (MMDD)
	char entryMode		[3+1];		// 22 	N 	3 	Modo de entrada
	char PanSeqNumber	[3+1];		// 23 	N 	3 	PAN Sequence Number
	char hostNSU		[6+1];		// 38 	N 	6 	NSU do HOST
	char responseCode	[2+1];		// 39 	AN 	2 	Código de resposta
	char serialNumber	[16+1];		// 41 	AN 	16 	Numero de série do terminal
	char ecDocument		[20+1];		// 42 	N 	20 	CNPJ/CPF do estabelecimento commercial
	char currencyCode	[3+1];		// 49 	N 	3 	Código da Moéda
	char emvData		[999+1];	// 55 	H 	LLL Tags EMV
	char lastTrsData	[999+1];	// 60 	ANS LLL Dados de confirmação da ultima transação
	char terminalInfo	[999+1];	// 61 	ANS LLL Informações do Ponto de Venda
	char receiptEC		[999+1];	// 62 	ANS LLL Mensagens do HOST/Comprovante Lojista
	char adtTrsData		[999+1];	// 63 	ANS LLL Dados adicionais da transação
	char managementCode	[3+1];		// 70 	N 	3 	Código de gerenciamento
	char primalTrsData	[999+1];	// 90 	AN 	LLL Dados da transação Original
	char paymentMethod	[2+1];		// 118 	AN 	LLL Forma de Pagamento
	char versionDetail	[999+1];	// 120 	ANS LLL Detalhamento de Versões
	char decryptData	[999+1];	// 121 	AN 	LLL Dados para criptografia
	char simCardInfo	[999+1];	// 123 	ANS LLL Dados do SIM Card
	char receiptHolder	[999+1];	// 124 	ANS LLL Comprovante do Cliente
	char cryptedData	[999+1];	// 125 	Z 	LLL Dados sensiveis da transação
	char acquirerNSU	[999+1];	// 127 	AN 	LLL NSU adiquirente

	char brandName	    [12+1];		// ESPECIFICAMENTE PRA RELATORIOS
	char panTruncate	[19+1];		// ESPECIFICAMENTE PRA CANCELAMENTO
} ISO_REV_MODAL_T; //12kb total

typedef struct{
	char binTableVer	[8+1];		// 001 	A 	LLL Versão das tabelas de Bins(Bandeiras)
	char labelPOS		[8+1];		// 002 	A 	LLL Versão da LabelPOS (Mascara do Cliente)
	char languages		[8+1];		// 003	A 	LLL Versão da tabela de linguagens
	char appVer			[8+1];		// 004	A 	LLL Versão da aplicação principal do terminal
	char firmwareVer	[8+1];		// 005 	A 	LLL Versão do firmware
	char hostName		[64+1];		// 006 	A 	LLL DNS ou IP utilizado para acesso FTP
	char hostPort		[8+1];		// 007 	A 	LLL Porta utilizada para acesso FTP
	char hostUser		[64+1];		// 008 	A 	LLL Nome de usuário utilizado para acesso FTP
	char hostPath		[128+1];	// 009 	A 	LLL Path a ser concatenado antes da versão
	char hostPass		[64+1];		// 010 	A 	LLL Senha a ser utilizada para acesso FTP
	char cnpjSubAcq		[32+1];		// 011 	A 	LLL CNPJ do subadquirente (RFU)
	char emvTableVer	[16+1];		// 012 	A 	LLL Versão da tabela EMV configurada
	char especVer		[8+1];		// 013 	A 	LLL Versão da documentação de ISO em uso
	char needInit		[2+1];		// 014 	N 	LLL Indica obrigatoriedade na carga de tabelas EMV
	char modelPOS		[32+1];		// 015 	A  	LLL Modelo do terminal requirente
} ISO_TERM_VERSION; //394b total

typedef struct{
	char mti			[4+1];		// 0 	N 	4 	MTI
	char bitmap			[32+1];		// 1 	B 	32 	Bitmap
	char gmtDateTime	[10+1];		// 7 	N 	10 	Data e hora GMT0 (MMDDhhmmss)
	char localNSU		[6+1];		// 11 	N 	6 	NSU do terminal
	char localTime		[6+1];		// 12 	N 	6 	Hora local da transação (hhmmss)
	char localDate		[4+1];		// 13 	N 	4 	Data local da transação (MMDD)
	char countryCode	[3+1];		// 19 	N 	3 	Código de país do adiquirente
	char responseCode	[2+1];		// 39 	AN 	2 	Código de resposta
	char serialNumber	[16+1];		// 41 	AN 	16 	Numero de série do terminal
	char ecDocument		[20+1];		// 42 	N 	20 	CNPJ/CPF do estabelecimento commercial
	char terminalInfo	[999+1];	// 61 	ANS LLL Informações do Ponto de Venda
	char tableTrade1	[999+1];	// 62 	ANS LLL Mensagens do HOST/Comprovante Lojista
	char tableTrade2	[999+1];	// 63 	ANS LLL Dados adicionais da transação
	char managementCode	[3+1];		// 70 	N 	3 	Código de gerenciamento
	char versionDetail	[999+1];	// 120 	ANS LLL Detalhamento de Versões
	char simCardInfo	[999+1];	// 123 	ANS LLL Dados do SIM Card
	char acquirerNSU	[999+1];	// 127 	AN 	LLL NSU adiquirente
	ISO_TERM_VERSION versions; 		// Elementos presentes no bit 120
} ISO_NET_MODAL_T; //6,5kb total

#endif
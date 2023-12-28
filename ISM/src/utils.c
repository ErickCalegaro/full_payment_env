char  g_zList[100][1024];
int   g_listSize = 0;

int util_ConvertToDecimal_Header(char *msg)
{
    char size[8] = {0x00};
    int decimal = 0;

    snprintf(size, 5, "%.04s", msg);

    decimal = atoi(size);

    printf("Size HEADER: size = [%s] decimal = [%d]\n", size, decimal);

    return(decimal);
}

void util_Convert_HexaToBin(char h[], char *result)
{
    int  tamanho  = strlen(h);
    int  id       = 0;
    int  i        = 0;

    for (i = 0; i < tamanho; i++)
    {
        if (h[i] == '0')  {result[id]   = '0'; result[id+1] = '0'; result[id+2] = '0'; result[id+3] = '0'; id += 4;}
        if (h[i] == '1')  {result[id]   = '0'; result[id+1] = '0'; result[id+2] = '0'; result[id+3] = '1'; id += 4;}
        if (h[i] == '2')  {result[id]   = '0'; result[id+1] = '0'; result[id+2] = '1'; result[id+3] = '0'; id += 4;}
        if (h[i] == '3')  {result[id]   = '0'; result[id+1] = '0'; result[id+2] = '1'; result[id+3] = '1'; id += 4;}
        if (h[i] == '4')  {result[id]   = '0'; result[id+1] = '1'; result[id+2] = '0'; result[id+3] = '0'; id += 4;}
        if (h[i] == '5')  {result[id]   = '0'; result[id+1] = '1'; result[id+2] = '0'; result[id+3] = '1'; id += 4;}
        if (h[i] == '6')  {result[id]   = '0'; result[id+1] = '1'; result[id+2] = '1'; result[id+3] = '0'; id += 4;}
        if (h[i] == '7')  {result[id]   = '0'; result[id+1] = '1'; result[id+2] = '1'; result[id+3] = '1'; id += 4;}
        if (h[i] == '8')  {result[id]   = '1'; result[id+1] = '0'; result[id+2] = '0'; result[id+3] = '0'; id += 4;}
        if (h[i] == '9')  {result[id]   = '1'; result[id+1] = '0'; result[id+2] = '0'; result[id+3] = '1'; id += 4;}
        if (h[i] == 'a')  {result[id]   = '1'; result[id+1] = '0'; result[id+2] = '1'; result[id+3] = '0'; id += 4;}
        if (h[i] == 'b')  {result[id]   = '1'; result[id+1] = '0'; result[id+2] = '1'; result[id+3] = '1'; id += 4;}
        if (h[i] == 'c')  {result[id]   = '1'; result[id+1] = '1'; result[id+2] = '0'; result[id+3] = '0'; id += 4;}
        if (h[i] == 'd')  {result[id]   = '1'; result[id+1] = '1'; result[id+2] = '0'; result[id+3] = '1'; id += 4;}
        if (h[i] == 'e')  {result[id]   = '1'; result[id+1] = '1'; result[id+2] = '1'; result[id+3] = '0'; id += 4;}
        if (h[i] == 'f')  {result[id]   = '1'; result[id+1] = '1'; result[id+2] = '1'; result[id+3] = '1'; id += 4;}
        if (h[i] == 'A')  {result[id]   = '1'; result[id+1] = '0'; result[id+2] = '1'; result[id+3] = '0'; id += 4;}
        if (h[i] == 'B')  {result[id]   = '1'; result[id+1] = '0'; result[id+2] = '1'; result[id+3] = '1'; id += 4;}
        if (h[i] == 'C')  {result[id]   = '1'; result[id+1] = '1'; result[id+2] = '0'; result[id+3] = '0'; id += 4;}
        if (h[i] == 'D')  {result[id]   = '1'; result[id+1] = '1'; result[id+2] = '0'; result[id+3] = '1'; id += 4;}
        if (h[i] == 'E')  {result[id]   = '1'; result[id+1] = '1'; result[id+2] = '1'; result[id+3] = '0'; id += 4;}
        if (h[i] == 'F')  {result[id]   = '1'; result[id+1] = '1'; result[id+2] = '1'; result[id+3] = '1'; id += 4;}
    }

    printf("DUMP BITMAP = [%s]\n", h);
    printf("DUMP BITMAP = [%s]\n", result);
    for (int i = 0; i <= 128; ++i){
        printf("POS[%d] = {%c}\n", i+1, result[i]);
    }

    return;
}

int util_ConvertCharToHex(char caracter)
{
    if (caracter > 47 && caracter < 58)
        return caracter - 48;
    else if (caracter > 64 && caracter < 71)
        return caracter - 55;
    else if (caracter > 96 && caracter < 103)
        return caracter - 87;
    else
        return 0;
}

char util_ConvertHexToChar(int hex)
{
    if (hex >= 0 && hex <= 9)
        return hex + 48;
    else if (hex >= 10 && hex <= 15)
        return hex + 55;
    else
        return 0;
}

char util_AddBitToByte(char byte, int bit)
{
    return (util_ConvertHexToChar(util_ConvertCharToHex(byte) | bit));
}

int util_ConvertStrigToHexa(char * string, char * hexa)
{
    int sizeFinal = 0;
    int sizeInitial = strlen(string);
    char zAux[4+1] = {0x00};

    for (int i = 0; i < sizeInitial; i++){
        memset(zAux, 0x00, sizeof(zAux));
        sprintf(zAux, "%02X", string[i]);
        strcat(hexa, zAux);
    }

    sizeFinal = strlen(hexa);

    printf("string(%d) = [%s]\n", sizeInitial, string);
    printf("hexa(%d) = [%s]\n", sizeFinal, hexa);

    return sizeFinal;
}

int util_ConvertHexaToString(char * hexa, char * string)
{
    int sizeFinal = 0;
    int sizeInitial = strlen(string);
    char zAux[4+1] = {0x00};

    for (int i = 0; i < sizeInitial; i++){
        memset(zAux, 0x00, sizeof(zAux));
        sprintf(zAux, "%02X", string[i]);
        strcat(hexa, zAux);
    }

    sizeFinal = strlen(hexa);

    printf("string(%d) = [%s]\n", sizeInitial, string);
    printf("hexa(%d) = [%s]\n", sizeFinal, hexa);

    return sizeFinal;
}

void utl_Config_SetCondition(int field, char *condition){
    
    switch(field){
        case CFG_NSU_HOST:          strcpy(condition, "key = 'nsu_host'");        break;
        case CFG_NSU_ACQUIRER:      strcpy(condition, "key = 'nsu_acquirer'");    break;
        case CFG_TIMESTAMP:         strcpy(condition, "key = 'timestamp'");       break;
        case CFG_MP35_VER_APP:      strcpy(condition, "key = 'MP35_app_ver'");    break;
        case CFG_MP35_VER_FW:       strcpy(condition, "key = 'MP35_fw_ver'");     break;
        case CFG_HOSTNAME:          strcpy(condition, "key = 'hostname'");        break;
        case CFG_HOSTUSER:          strcpy(condition, "key = 'hostuser'");        break;
        case CFG_HOSTPASS:          strcpy(condition, "key = 'hostpass'");        break;
        case CFG_HOSTPORT:          strcpy(condition, "key = 'hostport'");        break;
    }
}

int util_Config_GetTableField(int field, char *output){

    int  rc               = RC_OK;
    char result     [64]  = {0x00};
    char fields     [32]  = {0x00};
    char condition  [128] = {0x00};
    char sqlError   [128] = {0x00};

    utl_Config_SetCondition(field, condition);

    sprintf(fields, "value");
    rc = db_sqLite_get_txt (SQLITE_DB_DATABASE, SQLITE_TABLE_CONFIG, fields, condition, result, sizeof(result));
    if (rc != RC_OK){
        return rc;
    }

    if (strlen(result) <= 0){
        return RC_ERR_GENERIC;
    }

    strcpy(output, result);

    return RC_OK;
}

int util_Config_SetTableField(int field, char *input){

    int  rc               = RC_OK;
    char fields     [32]  = {0x00};
    char condition  [128] = {0x00};
    char sqlError   [128] = {0x00};

    utl_Config_SetCondition(field, condition);

    sprintf(fields, "value = '%s'", input);
    rc = db_sqLite_update(SQLITE_DB_DATABASE, SQLITE_TABLE_CONFIG, fields, condition);
    if (rc != RC_OK){
        return rc;
    }

    return RC_OK;
}

void util_String_Substring(const char source[], int init, int length, char output[])
{
   int i = 0;
   while (i < length)
   {
      output[i] = source[init+i];
      i++;
   }
   output[i] = '\0';
   // printf("util_String_Substring [%s]\n", output);
   return;
}

int util_String_GetValueTLV(int targetTag, char *input, char *output){
    int offset  = 0;
    int size    = 0;
    int tag     = 0;
    int length  = 0;

    char aux  [1024] = {0x00};
    char value[512]  = {0x00};
    strcpy(output, "\0");

    if(!strlen(input) < 7){
        while(offset < strlen(input)){
            memset(aux, 0, sizeof(aux));
            util_String_Substring(input, offset, 3, aux);           // ID DA TAG
            tag = atoi(aux);

            util_String_Substring(input, offset += 3, 3, aux);      // TAMANHO DO CONTEÚDO DA TAG
            size = atoi(aux);
            length = atoi(aux);
            memset(aux, 0, sizeof(aux));

            util_String_Substring(input, offset += 3, size, aux);   // CONTEÚDO DA TAG
            strcpy(value, aux);
            offset += size;

            if(tag == targetTag){
               strcpy(output, value);
                    return 0;
            }
        }
    }else{
        return -1;
    }

    return -2;
}

int util_String_MountTLV(int inputId, char *inputData, char *output)
{
    char zFmt[1024] = {0x00};
    char zAUX  [10] = {0x00};

    sprintf(zFmt, "%03d", inputId);
    sprintf(zAUX, "%03d", (int)strlen(inputData));
    strcat (zFmt, zAUX);
    strcat (zFmt, inputData);

    // printf("TAG [%s]\n", zFmt);
    // printf("LEN [%s]\n", zAUX);
    // printf("VAL [%s]\n", inputData);

    strcat (output, zFmt);

    return(0);
}

void util_String_DumpStringLog(char *input, int charPerLine)
{
    char buffDBG[MAX_MSG] = {0x00};
    int  iCounter        = 0;
    int  iQuebra         = 0;

    if(charPerLine <= 0)
    {
        charPerLine = 100;
    }
    else
    {
        if(charPerLine >  300)
            charPerLine = 300;
    }

    memset(buffDBG, 0x00, sizeof(buffDBG));

    for(iQuebra = 0; iQuebra < strlen(input); iQuebra++)
    {
        buffDBG[iCounter] = input[iQuebra];
        iCounter++;

        if(iCounter >= charPerLine)
        {
            printf("       >>(%03d) |   [%s]   |\n", (int)strlen(buffDBG), buffDBG);
            iCounter = 0;
            memset(buffDBG, 0x00, sizeof(buffDBG));
        }
    }

    printf("       >>(%03d) |   [%s]   |\n", (int)strlen(buffDBG), buffDBG);

    return;
}


int utl_String_FormatByMask(const char *input, char *output, const char format[])
{
    char aux[100];
    char cpy[100];
    int  i     = 0;
    int  len   = 0;
    int  count = 0;

    strcpy(cpy, input);
    len = strlen(cpy);

    if(len == 0){
        while(format[i] != '\0'){
            if(format[i] != '#'){
                aux[i] = format[i];
                i++;
            }
        }
    }

    while(count < len)
    {
       if(format[i] != '#')
       {
          aux[i] = format[i];
          i++;
       }
       else
       {
          aux[i] = cpy[count];
          count++;
          i++;
       }
    }
    aux[i] = '\0';

    strcpy(output, aux);

    return(0);
}
void utl_String_FormatAmount(const char *input, char *output)
{
    char zAux[64] = {0x00};
    int i = 0;

    for (i = 0; i < 12; i++) if (input[i] > '0') break;
    strcpy(zAux, &input[i]);

    if (strlen(zAux) == 0){
        utl_String_FormatByMask(zAux, output, "                 R$ 0,00");
    }else if (strlen(zAux) == 1){
        utl_String_FormatByMask(zAux, output, "                 R$ 0,0#");
    }else if (strlen(zAux) == 2){
        utl_String_FormatByMask(zAux, output, "                 R$ 0,##");
    }else if (strlen(zAux) == 3){
        utl_String_FormatByMask(zAux, output, "                 R$ #,##");
    }else if (strlen(zAux) == 4){
        utl_String_FormatByMask(zAux, output, "                R$ ##,##");
    }else if (strlen(zAux) == 5){
        utl_String_FormatByMask(zAux, output, "               R$ ###,##");
    }else if (strlen(zAux) == 6){
        utl_String_FormatByMask(zAux, output, "             R$ #.###,##");
    }else if (strlen(zAux) == 7){
        utl_String_FormatByMask(zAux, output, "            R$ ##.###,##");
    }else if (strlen(zAux) == 8){
        utl_String_FormatByMask(zAux, output, "           R$ ###.###,##");
    }else if (strlen(zAux) == 9){
        utl_String_FormatByMask(zAux, output, "         R$ #.###.###,##");
    }else if (strlen(zAux) == 10){
        utl_String_FormatByMask(zAux, output, "        R$ ##.###.###,##");
    }else if (strlen(zAux) == 11){
        utl_String_FormatByMask(zAux, output, "       R$ ###.###.###,##");
    }else if (strlen(zAux) == 12){
        utl_String_FormatByMask(zAux, output, "     R$ #.###.###.###,##");
    }
}


int util_String_Trim(char *texto, char *result)
{
    char zAux[256] = {0x00};
    char sAux[256] = {0x00};
    strcpy(zAux, texto);
    strcpy(sAux, zAux);

    for (int i = 0; i <= strlen(zAux) + 1; i++){
        if (zAux[i] == ' ' && zAux[i+1] == ' '){
            strcpy(&sAux[i], &zAux[i+1]);
            memset(zAux, 0x00, sizeof(zAux));
            strcpy(zAux, sAux);
            i = 0;
            continue;
        }else if (zAux[i] == ' ' && zAux[i+1] == '\0'){
            zAux[i] = '\0';
        }
    }

    strcpy(result, zAux);
    return (strlen(zAux));
}

int util_invertLastName(char *input, char *result)
{
    int  pos = 0, size = strlen(input);

    for (int i = 0; i < size; i++){
        if (i > 4 && //Cartoes bugados do ITAU podem ter / no segundo caractere
            input[i] == '/'){
            pos = i;
            break;
        }
    }

    if (pos > 0){
        char zAux[64+1] = {0x00};
        strcpy(zAux, &input[pos+1]);
        strcat(zAux, " ");
        strncat(zAux, input, pos);
        strcpy(result, zAux);
    }else{
        strcpy(result, input);
    }

    return 0;

}

int util_getNumberOfTables(char * timestamp)
{
    char filePath[32+1] = {0x00};
    FILE *arq;
    char c, letra = '\n';
    int vezes = 0;

    sprintf(filePath, "tables/%.12s.DAT", timestamp);

    arq = fopen(filePath,"r");

    if(arq == NULL){
        printf("Erro na abertura do arquivo!\n");
        return 0;
    }

    //Lendo o arquivo 1 por 1
    while(fread (&c, sizeof(char), 1, arq)) {
        if(c == letra) {
            vezes++;
        }
    } 

    fclose(arq);

    return ++vezes; //Ultima linha não tem quebra
}

void util_needUpdateTimestamp(int table)
{
    char timestamp[10+1] = {0x00};

    switch(table){
        case TABLE_AMEX:
            strncpy(timestamp, "TAB001AMEX", 10);
            break;
        case TABLE_REDE:
            strncpy(timestamp, "TAB002REDE", 10);
            break;
        case TABLE_CIELO:
            strncpy(timestamp, "TAB03CIELO", 10);
            break;
        case TABLE_VERO:
            strncpy(timestamp, "TAB004VERO", 10);
            break;
        case TABLE_STONE:
            strncpy(timestamp, "TAB05STONE", 10);
            break;
        case TABLE_GERTEC:
        default:
            strncpy(timestamp, "TAB0GERTEC", 10);
            break;

    }
    util_Config_SetTableField(CFG_TIMESTAMP, timestamp);
}

void util_String_RemoveNewLine(char input[], char output[])
{
    int  len        =   strlen(input);
    int  c          =   0;
    char out[2050]  =   {0x00};

    while (c < len){
        if(input[c] == '\r' || input[c] == '\n'){
            // DO NOTHING
        }else{
            out[c] = input[c];
        }

        c++;
    }

    out[c] = '\0';

    strcpy(output, out);

    return;
}


int util_TableReadToList(char * timestamp)
{
    FILE    *fd;
    int     count       = 0;
    char    zName  [50] = {0x00};
    char    zRead[1024] = {0x00};
    char    zBuff[1024] = {0x00};

    sprintf(zName, "tables/%.12s.DAT", timestamp);

    fd  =   fopen(zName, "r");
    count = 1;
    
    memset(g_zList, 0x00, sizeof(g_zList));

    while(!feof(fd))
    {
        memset(zRead, 0x00, sizeof(zRead));
        memset(zBuff, 0x00, sizeof(zBuff));

        fgets(zRead, 1024, fd);

        if(strlen(zRead) > 0)
        {
            util_String_RemoveNewLine(zRead, zBuff);
            printf("zBuff = [%s]\n", zBuff);

            if(count < 100)
            {
                strcpy(g_zList[count], zBuff);
            }
            else
            {
                break;
            }

            count++;
        }

    }

    fclose(fd);

    g_listSize = count;

    return 0;
}

int util_tableSerialization(int * currIndex, char * output)
{
    char plainTable[999+1] = {0x00};
    char zAux[650+1] = {0x00};
    char sAux[32+1] = {0x00};
    int iAux = 0;

    while(TRUE)
    {
        memset(zAux, 0x00, sizeof(zAux));
        memset(sAux, 0x00, sizeof(sAux));
        if (*currIndex > g_listSize)
            break;
        
        strncpy(sAux, g_zList[*currIndex], 3);
        iAux = atoi(sAux);

        if ((strlen(plainTable) + iAux) >= 999)
            break;

        sprintf(zAux, "%03d%s", *currIndex, g_zList[*currIndex]);
        printf("g_zList[%d] = [%s]\n", *currIndex, g_zList[*currIndex]);
        strcat(plainTable, zAux);
        (*currIndex)++;
    }

    strcpy(output, plainTable);

    return strlen(plainTable);
}


int util_sendDataToHSM(char * psKSN, char * psPINBlock, char * psDataEnc)
{
    struct sockaddr_in serverAddrHSM;
    char        buffer[MAX_MSG] = {0x00};
    char        password[8+1] = {0x00};
    ssize_t     bytesRead;
    int         commSocketHSM;

    strncpy(buffer, &psKSN[6], 20);
    strcat(buffer, psPINBlock);
    strcat(buffer, psDataEnc);

    // Criação do socket do ISM
    commSocketHSM = socket(AF_INET, SOCK_STREAM, 0);
    if (commSocketHSM == -1) {
        perror("Erro ao criar o socket do cliente");
        return 0;
    }

    // Configuração do endereço do HSM
    serverAddrHSM.sin_family = AF_INET;
    serverAddrHSM.sin_port = htons(HSM_PORT);
    if (inet_pton(AF_INET, LOCALHOST, &serverAddrHSM.sin_addr) <= 0) {
        perror("Erro ao configurar o endereço do HSM");
        close(commSocketHSM);
        return 0;
    }

    // Conecta-se ao HSM
    if (connect(commSocketHSM, (struct sockaddr *)&serverAddrHSM, sizeof(serverAddrHSM)) == -1) {
        perror("Erro ao conectar ao HSM");
        close(commSocketHSM);
        return 0;
    }

    // Envia a mensagem ao HSM
    send(commSocketHSM, buffer, strlen(buffer), 0);

    // Limpa o buffer
    memset(buffer, 0, sizeof(buffer));

    // Recebe a resposta do HSM
    bytesRead = recv(commSocketHSM, buffer, sizeof(buffer), 0);

    if (bytesRead == -1) {
        perror("Erro ao receber dados do servidor");
    } else if (bytesRead == 0) {
        // O servidor fechou a conexão
        printf("O servidor fechou a conexão\n");
    } else {
        // Imprime a resposta recebida
        printf("Resposta do servidor: %.*s\n", (int)bytesRead, buffer);
    }

    // Fecha o socket do HSM
    close(commSocketHSM);

    strncpy(password, &buffer[strlen(buffer) - 8], 8);
    printf("password: [%s]\n", password);

    int i = 0;
    for (; i < 8; i++) if (password[i] != ' ') break;

    printf("password: [%s]\n", &password[i]);

    // Impede senha com todos os digitos iguais
    if (password[i] == password[i+1] &&
        password[i] == password[i+2] &&
        password[i] == password[i+3] &&
        password[i+1] == password[i+2] &&
        password[i+1] == password[i+3] &&
        password[i+2] == password[i+3] ){
        return 1;
    }

    //Impede senhas 1234 e 4321
    if ((password[i+0] == '1' &&
         password[i+1] == '2' &&
         password[i+2] == '3' &&
         password[i+3] == '4' )||(
         password[i+0] == '4' &&
         password[i+1] == '3' &&
         password[i+2] == '2' &&
         password[i+3] == '1' )){
        return 1;
    }

    return 0;
}
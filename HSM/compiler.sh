clear
echo -e '\e[1;40;93mCompilando HSM...\e[m'
gcc "./src/main.c" -o "./hsm" -lpthread -lssl -lcrypto
echo -e '\e[1;40;36mCompilação Concluida!\e[m' 
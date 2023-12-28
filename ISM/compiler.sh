clear
echo -e '\e[1;40;93mCompilando ISO Module...\e[m'
gcc "./src/server.c" -o "./ism" -lsqlite3
echo -e '\e[1;40;36mCompilação Concluida!\e[m'

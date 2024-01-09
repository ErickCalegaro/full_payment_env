clear
echo -e '\e[1;40;93mCompilando Comunicator...\e[m'
gcc "./src/cm_tcp.c" -o "./cm_tcp" -lpthread -std=c99
echo -e '\e[1;40;36mCompilação Concluida!\e[m'

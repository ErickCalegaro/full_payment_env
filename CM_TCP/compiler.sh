clear
echo -e '\e[1;40;93mCompilando Comunicator...\e[m'
gcc "./src/cm_tcp.c" -o "./cm_tcp" -lpthread
echo -e '\e[1;40;36mCompilação Concluida!\e[m'

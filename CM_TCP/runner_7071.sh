clear
echo -e '\e[1;40;93mExecutando Comunicator 7071...\e[m'
echo -e '\e[1;40;93mLogs em ../PUB/log_cm_tcp_7071.txt\e[m'
for ((;1;)); do 
  ./cm_tcp 7071
  sleep 1m
done
echo -e '\e[1;40;36mExecução Abortada!\e[m'
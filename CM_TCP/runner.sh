clear
echo -e '\e[1;40;93mExecutando Comunicator...\e[m'
echo -e '\e[1;40;93mLogs em ../PUB/log_cm.txt\e[m'
for ((;1;)); do 
  ./cm_tcp 7072 >> ../PUB/log_cm.txt
  sleep 1m
done
echo -e '\e[1;40;36mExecução Abortada!\e[m'

clear
echo -e '\e[1;40;93mExecutando ISO Module...\e[m'
echo -e '\e[1;40;93mLogs em ../PUB/log_ism.txt\e[m'

for ((;1;)); do 
  ./ism >> ../PUB/log_ism.txt
  sleep 1m
done
echo -e '\e[1;40;36mExecução Abortada!\e[m'
 
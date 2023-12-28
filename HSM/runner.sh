clear
echo -e '\e[1;40;93mExecutando HSM...\e[m'
echo -e '\e[1;40;93mLogs em ../PUB/log_hsm.txt\e[m'
for ((;1;)); do 
  ./hsm >> ../PUB/log_hsm.txt
  sleep 1m
done
echo -e '\e[1;40;36mExecução Abortada!\e[m'

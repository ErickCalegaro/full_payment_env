clear
echo -e '\e[1;40;93mCompilando Test Comunicator Online...\e[m'
gcc "./src/test_cm_tcp.c" -o "./test_cm_tcp_online"
echo -e '\e[1;40;93mCompilando Test Comunicator Local...\e[m'
gcc "./src/test_cm_tcp.c" -o "./test_cm_tcp_local" -DLOCAL
echo -e '\e[1;40;93mCompilando Test ISO Module Online...\e[m'
gcc "./src/test_ism.c" -o "./test_ism_online"
echo -e '\e[1;40;93mCompilando Test ISO Module Local...\e[m'
gcc "./src/test_ism.c" -o "./test_ism_local" -DLOCAL
echo -e '\e[1;40;93mCompilando Test HSM Local...\e[m'
gcc "./src/test_hsm.c" -o "./test_hsm_local" -DLOCAL
echo -e '\e[1;40;36mCompilação Concluida!\e[m'
 
#!/bin/bash
x="teste"

delete ()
{
while true $x != "teste"
do
clear
echo "================================================"
echo "Mini script - Exclusao de Logs"
echo "Criado por: Erick Calegaro"
echo ""
echo "1) Comunicator 7071"
echo""
echo "2) Comunicator 7072"
echo ""
echo "3) ISO-Module"
echo ""
echo "4) Fake-HSM"
echo""
echo "5) Acquirer"
echo""
echo "6) Issuer"
echo""
echo "7) Voltar para o Menu"
echo""
echo "q) Sair do programa"
echo ""
echo "================================================"

echo "Qual log deseja deletar?"
read x
echo "Opção informada ($x)"
echo "================================================"

case "$x" in

    1)
      echo "Excluindo log do Comunicator porta 7071"
      rm log_cm_tcp_7071.txt
      sleep 1;
      return;
      
echo "================================================"
;;
    2)
      echo "Excluindo log do Comunicator porta 7072"
      rm log_cm_tcp_7072.txt
      sleep 1;
      return;

echo "================================================"
;;
   3)
      echo "Excluindo log do ISO-Module"
      rm log_ism.txt
      sleep 1;
      return;

echo "================================================"
;;
    4)
       echo "Excluindo log do Fake-HSM"
      rm log_hsm.txt
      sleep 1;
      return;

echo "================================================"
;;
     5)
       echo "Excluindo log do Acquirer"
       sleep 1
       echo "Nenhum arquivo encontrado"
       sleep 1
       return;

echo "================================================"
;;
    6)
       echo "Excluindo log do Issuer"
       sleep 1
       echo "Nenhum arquivo encontrado"
       sleep 1
       return;

echo "================================================"
;;
    7)
       echo "Voltando para o menu..."
       return;
echo "================================================"
;;
    q)
       echo "Saindo..."
       exit;
echo "================================================"
;;

*)
        echo "Opção inválida!"
        sleep 1
esac
done

}

menu ()
{
while true $x != "teste"
do
clear
echo "================================================"
echo "Mini script - Acesso a Logs"
echo "Criado por: Erick Calegaro"
echo ""
echo "1) Comunicator 7071"
echo""
echo "2) Comunicator 7072"
echo ""
echo "3) ISO-Module"
echo ""
echo "4) Fake-HSM"
echo""
echo "5) Acquirer"
echo""
echo "6) Issuer"
echo""
echo "7) Apagar um log"
echo ""
echo "8) Sair do programa"
echo ""
echo "================================================"

echo "Digite a opção desejada:"
read x
echo "Opção informada ($x)"
echo "================================================"

case "$x" in

    1)
      echo "Exibindo log do Comunicator porta 7071"
      tail -f log_cm_tcp_7071.txt
      exit;
      
echo "================================================"
;;
    2)
      echo "Exibindo log do Comunicator porta 7072"
      tail -f log_cm_tcp_7072.txt
      exit;

echo "================================================"
;;
   3)
      echo "Exibindo log do ISO-Module"
      tail -f log_ism.txt
      exit;

echo "================================================"
;;
    4)
       echo "Exibindo log do Fake-HSM"
      tail -f log_hsm.txt
      exit;

echo "================================================"
;;
     5)
       echo "Exibindo log do Acquirer"
       sleep 1
       echo "Nenhuma implementacao encontrada"
       exit;

echo "================================================"
;;
    6)
       echo "Exibindo log do Issuer"
       sleep 1
       echo "Nenhuma implementacao encontrada"
       exit;

echo "================================================"
;;
    7)
       echo "Limpar Logs..."
       delete
       
echo "================================================"
;;
    8)
       echo "Saindo..."
       exit;
echo "================================================"
;;
    q)
       echo "Saindo..."
       exit;
echo "================================================"
;;

*)
        echo "Opção inválida!"
        sleep 1
esac
done

}
menu
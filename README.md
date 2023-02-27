# dea
Repository to be used in the DEA software project

Para compilar será necessário o visual studio 2019

Na main() do arquivo serialTest.c, você deve passar o nome da porta correspondente
à porta serial do seu computador, no ex está COM6, mas pode ser diferente.

Comente o #define Publish para usar a rotina subscribe, ou deixe descomentado para usar a rotina publish
  
Abra o arquivo serialTest.sln com o visual studio 2019, e em seguida 
compile o projeto serialTest

Após compilar copie o arquivo da pasta dlls/ para x64/Debug 
(Essa pasta só aparece após compilar, se tentar rodar sem colocar a dll nessa pasta,
 ocorrerá um erro em tempo de execução).

Agora é só rodar o programa pelo visual studio. f5 para debugar ou ctrl+f5 para rodar sem ser debug  

Obs(Caso queira usar o código em outro SO):
  O visual studio só é necessário porque a dll usada para comunicação serial no windows é compilada
com o visual c compiler, também é possível compilar usando via linha de comando usando o msvc, porém
com o visual studio é mais fácil.
  Caso queira portar o código para outro compilador ou plataforma, será necessário compilar a biblioteca
da comunicação serial, instruções nesse link: https://sigrok.org/wiki/Libserialport. Além disso,
será necessário mudar o código na parte de criação de threads 
_________________________________________________________________________________________________________

Frontend: http://3.230.85.247:3000

Backend: http://3.230.85.247:3001

MQTT backend: http://3.230.85.247:3002

MQTT Web Simulator: http://3.230.85.247:3004

Documentação da API backend: http://3.230.85.247:3001/api/

Documentação da API MQTT backend http://3.230.85.247:3002/api/

_________________________________________________________________________________________________________

ssh -i DEAMQTT.pem ubuntu@3.230.85.247

_________________________________________________________________________________________________________
mosquitto_sub -h 3.230.85.247 -t gps/004401083467833

mosquitto_pub -h 3.230.85.247 -t gps/004401083467833 -m 50121136000041245514655564445515646535674131


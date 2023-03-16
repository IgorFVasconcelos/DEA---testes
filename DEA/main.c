#include <stdio.h>
#include "libserialport.h"
#include <Windows.h>
#include <process.h>
#include <string.h>
#include <time.h>
#include "mqtt/mqtt.h"
#include "tcp/tcp.h"
#include "udp/udp.h"
#include "sms/sms.h"
#include "dea/dea.h"
#include <stdint.h>
#include "gps/gps.h"

/**************************************************************************************************
 * PRIVATE DEFINES
 *************************************************************************************************/

#define PORT "COM3"
//#define PUBLISH //REMOVER COMENTARIO QND RODAR MQTT
//#define SEND
#define ANTENA

#define TELEFONE "+5584999528031"

//#define MQTT
//#define TCP
//#define UDP
//#define SMS
//#define DEA
#define GPS
 
#define DEA_GPS         0
#define DEA_AUTOTEST    1
#define DEA_ECG         2

#pragma warning(disable : 4996)


/**************************************************************************************************
 * PUBLIC TYPES
 *************************************************************************************************/
typedef enum {
    VAR_LEN_SHORT = 2,
    VAR_LEN_LONG = 4
} var_name_len_t;


/**************************************************
 * FUNCTIONS
 **************************************************/
void tcp()
{
    struct sp_port* port = NULL;
    configure_serial_tcp(PORT, &port);
#ifdef SEND
    _beginthread(read_serial_tcp, 1, port);
    configure_tcp_send_routine(port);
    start_tcp_routine(port);
#else
    _beginthread(read_serial_for_listen_tcp, 1, port);
    configure_tcp_listen_routine(port);
    start_tcp_routine_listen(port);
#endif
}

void udp()
{
    struct sp_port* port = NULL;
    configure_serial_udp(PORT, &port);
#ifdef SEND
    _beginthread(read_serial_udp, 1, port);
    configure_udp_send_routine(port);
    start_udp_routine(port);
#else
    _beginthread(read_serial_for_listen_udp, 1, port);
    configure_udp_listen_routine(port);
    start_udp_routine_listen(port);
#endif
}

void mqtt()
{
    struct sp_port* port = NULL;
    configure_serial_mqtt(PORT, &port);
#ifdef PUBLISH
    _beginthread(read_serial_mqtt, 1, port);
    configure_mqtt_publish_routine(port);
    start_mqtt_routine(port);
#else
    _beginthread(read_serial_for_subscribe, 1, port);
    configure_mqtt_subscribe_routine(port);
    start_mqtt_routine_subscribe(port);
#endif
}

void sms()
{
    struct sp_port* port = NULL;
    configure_serial_sms(PORT, &port);
    _beginthread(read_serial_sms, 1, port);
    send_sms(port, TELEFONE, "TESTE");
}


int hex_to_int(char c)
{
    if (c >= 97)
        c = c - 32;
    int first = c / 16 - 3;
    int second = c % 16;
    int result = first * 10 + second;
    if (result > 9) result--;
    return result;
}

int hex_to_ascii(char c, char d) {
    int high = hex_to_int(c) * 16;
    int low = hex_to_int(d);
    return high + low;
}


void pacote_1(uint8_t* ecg) {
    uint8_t* todos_dados[3000];
    uint8_t* pacote1[3000];
    int i = 0, j = 0, w = 0, k = 0, z = 0, n = 0, h = 0, a = 0;
    char aux[500000], aux2[2606];
    char pacote[27] = "/////////////////PACOTE 1:";
    uint8_t df[3000];


    // lendo o arquivo
    FILE* arquivo;
    arquivo = fopen("C:\\Users\\igorf\\Downloads\\dea-develop\\ecg-75s.txt", "r");
    //arquivo = fopen("C:\\Users\\telef\\Documentos\\DEA develop\\DEA---testes\\ecg-75s.txt", "r");
    // arquivo = fopen("ecg-75s.txt", "r");


    // verifica se nao esta vazio
    if (arquivo == NULL) {
        printf("Arquivo vazio\r\n");
    }
        

    while (fgets(df, sizeof df, arquivo) != NULL)
    {
        // Adiciona cada linha no vetor
        todos_dados[i] = strdup(df);
        i++;
    }

    fclose(arquivo); //fecha o arquivo


    // percorre o arquivo procurando pacote 1 e se encontrar armazena em um lugar separado
    for (j; j < i; j++) {
        if (strncmp(pacote, todos_dados[j], 26) == 0) {
            w = j + 2;
            // j + 21 coloquei se for valores de linhas fixos
            for (w; w < (j + 21); w++) {
                pacote1[k] = todos_dados[w];
                k++;

            }
            break; // tirando o break recebe todos os pacotes1       

        }

    }



    // gera uma string apenas com os hexadecimal sem o 0x
    for (n; n < k; n++) {
        strcpy(aux, pacote1[n]);
        if (aux[0] == '0' && aux[1] == 'x') {

            for (int m = 0; m < (strlen(pacote1[n]) - 3); m++) {
                aux2[a] = aux[m + 2];
                a++;

            }
        }
        else {

            for (int s = 0; s < (strlen(pacote1[n]) - 1); s++) {
                aux2[a] = aux[s];
                a++;

            }
        }
    }



    // gera uma string com os hexadecimal
    int length = 2606;
    char buf = 0;
    for (z = 0; z < length; z++) {
        if (z % 2 != 0) {
            ecg[h] = hex_to_ascii(buf, aux2[z]);
            h++;
        }
        else {
            buf = aux2[z];
        }
    }

}




void dea(int dea_routine)
{
    // NMEA GPS position in this example is ENEA: 3958.51252,N,11628.34582,E (https://onlinestringtools.com/convert-string-to-hexadecimal)
    uint8_t buffer_gps[26] = { 0x33, 0x39, 0x35, 0x38, 0x2E, 0x35, 0x31, 0x32, 0x35, 0x32, 0x2C, 0x4E, 0x2C, 0x31, 0x31, 0x36, 0x32, 0x38, 0x2E, 0x33, 0x34, 0x35, 0x38, 0x32, 0x2C, 0x45 };
    
    //Auto test String to Hex: 3958.51252,N,11628.34582,E,1,1,30,3,3,3,85999569243 (https://onlinestringtools.com/convert-string-to-hexadecimal)
    //(ENEA, Status AutoTest, Status Battery, Level Battery, Type Battery, Count shock, Frequency, Phone)
    uint8_t buffer_autotest[408] = { 0x33, 0x39, 0x35, 0x38, 0x2e, 0x35, 0x31, 0x32, 0x35, 0x32, 0x2c, 0x4e, 0x2c, 0x31, 0x31, 0x36, 0x32, 0x38, 0x2e, 0x33, 0x34, 0x35, 0x38, 0x32, 0x2c, 0x45, 0x2c, 0x31, 0x2c, 0x31, 0x2c, 0x33, 0x30, 0x2c, 0x33, 0x2c, 0x33, 0x2c, 0x33, 0x2c, 0x38, 0x35, 0x39, 0x39, 0x39, 0x35, 0x36, 0x39, 0x32, 0x34, 0x33, 0x33, 0x39, 0x35, 0x38, 0x2e, 0x35, 0x31, 0x32, 0x35, 0x32, 0x2c, 0x4e, 0x2c, 0x31, 0x31, 0x36, 0x32, 0x38, 0x2e, 0x33, 0x34, 0x35, 0x38, 0x32, 0x2c, 0x45, 0x2c, 0x31, 0x2c, 0x31, 0x2c, 0x33, 0x30, 0x2c, 0x33, 0x2c, 0x33, 0x2c, 0x33, 0x2c, 0x38, 0x35, 0x39, 0x39, 0x39, 0x35, 0x36, 0x39, 0x32, 0x34, 0x33,0x33, 0x39, 0x35, 0x38, 0x2e, 0x35, 0x31, 0x32, 0x35, 0x32, 0x2c, 0x4e, 0x2c, 0x31, 0x31, 0x36, 0x32, 0x38, 0x2e, 0x33, 0x34, 0x35, 0x38, 0x32, 0x2c, 0x45, 0x2c, 0x31, 0x2c, 0x31, 0x2c, 0x33, 0x30, 0x2c, 0x33, 0x2c, 0x33, 0x2c, 0x33, 0x2c, 0x38, 0x35, 0x39, 0x39, 0x39, 0x35, 0x36, 0x39, 0x32, 0x34, 0x33, 0x33, 0x39, 0x35, 0x38, 0x2e, 0x35, 0x31, 0x32, 0x35, 0x32, 0x2c, 0x4e, 0x2c, 0x31, 0x31, 0x36, 0x32, 0x38, 0x2e, 0x33, 0x34, 0x35, 0x38, 0x32, 0x2c, 0x45, 0x2c, 0x31, 0x2c, 0x31, 0x2c, 0x33, 0x30, 0x2c, 0x33, 0x2c, 0x33, 0x2c, 0x33, 0x2c, 0x38, 0x35, 0x39, 0x39, 0x39, 0x35, 0x36, 0x39, 0x32, 0x34, 0x33,0x33, 0x39, 0x35, 0x38, 0x2e, 0x35, 0x31, 0x32, 0x35, 0x32, 0x2c, 0x4e, 0x2c, 0x31, 0x31, 0x36, 0x32, 0x38, 0x2e, 0x33, 0x34, 0x35, 0x38, 0x32, 0x2c, 0x45, 0x2c, 0x31, 0x2c, 0x31, 0x2c, 0x33, 0x30, 0x2c, 0x33, 0x2c, 0x33, 0x2c, 0x33, 0x2c, 0x38, 0x35, 0x39, 0x39, 0x39, 0x35, 0x36, 0x39, 0x32, 0x34, 0x33, 0x33, 0x39, 0x35, 0x38, 0x2e, 0x35, 0x31, 0x32, 0x35, 0x32, 0x2c, 0x4e, 0x2c, 0x31, 0x31, 0x36, 0x32, 0x38, 0x2e, 0x33, 0x34, 0x35, 0x38, 0x32, 0x2c, 0x45, 0x2c, 0x31, 0x2c, 0x31, 0x2c, 0x33, 0x30, 0x2c, 0x33, 0x2c, 0x33, 0x2c, 0x33, 0x2c, 0x38, 0x35, 0x39, 0x39, 0x39, 0x35, 0x36, 0x39, 0x32, 0x34, 0x33,0x33, 0x39, 0x35, 0x38, 0x2e, 0x35, 0x31, 0x32, 0x35, 0x32, 0x2c, 0x4e, 0x2c, 0x31, 0x31, 0x36, 0x32, 0x38, 0x2e, 0x33, 0x34, 0x35, 0x38, 0x32, 0x2c, 0x45, 0x2c, 0x31, 0x2c, 0x31, 0x2c, 0x33, 0x30, 0x2c, 0x33, 0x2c, 0x33, 0x2c, 0x33, 0x2c, 0x38, 0x35, 0x39, 0x39, 0x39, 0x35, 0x36, 0x39, 0x32, 0x34, 0x33, 0x33, 0x39, 0x35, 0x38, 0x2e, 0x35, 0x31, 0x32, 0x35, 0x32, 0x2c, 0x4e, 0x2c, 0x31, 0x31, 0x36, 0x32, 0x38, 0x2e, 0x33, 0x34, 0x35, 0x38, 0x32, 0x2c, 0x45, 0x2c, 0x31, 0x2c, 0x31, 0x2c, 0x33, 0x30, 0x2c, 0x33, 0x2c, 0x33, 0x2c, 0x33, 0x2c, 0x38, 0x35, 0x39, 0x39, 0x39, 0x35, 0x36, 0x39, 0x32, 0x34, 0x33 };

    // variavel para receber os dados do pacote_1 pela função // recebimento bit a bit
    uint8_t ecg[1303];
    pacote_1(ecg);
    

    
    struct sp_port* port = NULL;
    configure_serial_dea(PORT, &port);
#ifdef PUBLISH
    _beginthread(read_serial_dea, 1, port);
    start_dea_gps_autotest(ecg, sizeof(ecg));
    configure_dea_publish_routine(port);
    start_dea_routine(port);
    exit(1);
#else
    _beginthread(read_serial_for_subscribe, 1, port);
    configure_mqtt_subscribe_routine(port);
    start_mqtt_routine_subscribe(port);
#endif

}

void gps() {
    struct sp_port* port = NULL;
    configure_serial_gps(PORT, &port);
    _beginthread(read_serial_gps, 1, port);
#ifdef ANTENA
    start_triangulation_routine(port);
#else
    start_gps_routine(port);
#endif


}

int main()
{
#ifdef MQTT
    mqtt();
#endif

#ifdef TCP
    tcp();
#endif

#ifdef UDP
    udp();
#endif

#ifdef SMS
    sms();
#endif

#ifdef DEA
    dea(DEA_GPS);
#endif

#ifdef GPS
    gps();
#endif


    return 0;

}




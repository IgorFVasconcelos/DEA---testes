#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <time.h> 
#include <stdint.h>
#include <assert.h>
#include "../common/common.h"
#include "libserialport.h"


#define GOTO_IF_FALSE(x, y) if (!(x)) goto y
#define MESSAGE_SIZE 1500

int ok_received_gps = FALSE;
int connected_gps = FALSE;
int response_received_gps = FALSE;
int mcc_gps = 0;
int mnc_gps = 0;
int readNext_gps = 1;
int itter_gps = 0;
int sucessos_antena = 0;

//Função que verifica quais campos da mensagem NMEA não estão presentes retornando sua posição.
int verificarCampos(char* string) {
    int count = 0;
    for (int i = 0; i < strlen(string); i++) {
        if (string[i] == ',') {
            count++;
            if (string[i] == string[i - 1]) {
                switch (count)
                {
                case 0:
                    printf("Mensagem completa");
                    return 0;

                case 1:
                    printf("Campo ausente: GPS\n\r");
                    return 1;

                case 2:
                    printf("Campo ausente: UTC\n\r");
        
                case 3:
                    printf("Campo ausente: Latitude\n\r");
        
                case 4:
                    printf("Campo ausente: Norte-Sul\n\r");
        
                case 5:
                    printf("Campo ausente: Longitude\n\r");
        
                case 6:
                    printf("Campo ausente: Leste-Oeste\n\r");
                    return 2;

                case 7:
                    printf("Campo ausente: Indicador de qualidade\n\r");
                    return 7;

                case 8:
                    printf("Campo ausente: Quantidade de satélites\n\r");
                    return 8;

                case 9:
                    printf("Campo ausente: HDOP\n\r");
                    return 9;

                case 10:
                    printf("Campo ausente: Altitude da Antena\n\r");
                    return 10;

                case 11:
                    printf("Campo ausente: Unidade de altitude\n\r");
                    return 11;

                case 12:
                    printf("Campo ausente: Geoidal\n\r");
                    return 12;

                case 13:
                    printf("Campo ausente: Unidade geoidal\n\r");
                    return 13;

                case 14:
                    printf("Campo ausente: Correção\n\r");
                    return 14;
                
                }
            }
        }

    }
}
//Separa uma string em um vetor de strings a partir de um caractére específico (delimitador)
//Créditos: hmjd --> https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c
char** str_split(char* a_str, const char a_delim)
{

    printf("string recebida:\n %s\n", a_str);
    char** result = 0;
    size_t count = 0;
    char* tmp = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = _strdup(token);
            token = strtok(0, delim);
        }
        *(result + idx) = 0;
    }
    printf("result:\n %s\n", result);
    return result;
}


//Realiza o tratamento adequado da mensagem NMEA, imprimindo latitude e longitude
void lerCoordenadasGps(const char* string) {

    //const char* gpgga = strrchr(string, '$');
    const char* gpgga = strstr(string, "GPGGA");

    //Enum com nome de cada campo da mensagem NMEA
    enum info {
        GPS = 0, UTC, S_Latitude, NS, S_Longitude, WE, IndicadorQualidade, QuantSatelite,
        HDOP, AltitudeAntena, UnidadeAltitude, Geoidal, UnidadeGeoidal, Correcao, EstacaoCorrecao};
    char* campos[15] = { 0 };
    char** tokens;

    int verificador;
    verificador = verificarCampos(gpgga);

    switch (verificador) {
    case 1:
        printf("String não referente ao GPS");
        return;
    case 2:
        itter_gps++;
        printf("Mensagem vazia, tentando novamente\nN umero de tentativas: %d\n\n", itter_gps);
        if (itter_gps >= 25) {
            printf("Numero maximo de tentativas atingido\n\n\n");
        }
        return;

    }

    tokens = str_split(gpgga, ',');

    if (tokens)
    {
        int i;
        for (i = 0; *(tokens + i); i++)
        {
            campos[i] = *(tokens + i);
        }
        printf("\n");
    }

    //processamento das coordenadas
    typedef struct {
        int graus;
        int minutos;
        float segundos;
    }coordenada;

    char sgrauLatitude[2] = "";
    strncpy(sgrauLatitude, campos[S_Latitude], 2);
    int grau = atof(sgrauLatitude);
    int minuto = atof((campos[S_Latitude] + 2));
    double segundo = atof((campos[S_Latitude] + 2));
    segundo = segundo - minuto;
    segundo = segundo * 60;

    coordenada latitude1;
    latitude1.graus = grau;
    latitude1.minutos = minuto;
    latitude1.segundos = segundo;

    printf("latitude graus = %d\n", latitude1.graus);
    printf("latitude minutos = %d\n", latitude1.minutos);
    printf("latitude segundos = %lf\n", latitude1.segundos);

    char sgrauLongitude[3] = "";
    strncpy(sgrauLongitude, campos[S_Longitude], 3);
    //strcat(sgrauLongitude, enter);
    grau = atof(sgrauLongitude);
    minuto = atof((campos[S_Longitude] + 3));
    segundo = atof((campos[S_Longitude] + 3));
    segundo = segundo - minuto;
    segundo = segundo * 60;

    coordenada longitude1;
    longitude1.graus = grau;
    longitude1.minutos = minuto;
    longitude1.segundos = segundo;

    printf("longitude graus = %d\n", longitude1.graus);
    printf("longitude minutos = %d\n", longitude1.minutos);
    printf("longitude segundos = %lf\n", longitude1.segundos);
    itter_gps = 0;
}



struct Antena{
    char sinal[4];
    char mcc[5];
    char mnc[5];
    char lac[5];
    char cell[5];
};

typedef struct Antenas {
    struct Antena antena1;
    struct Antena antena2;
    struct Antena antena3;
};

struct Antena processarLinhasTri(char* string) {
    printf("string recebida:\n%s\n", string);
    struct Antena antena = { 0 };
    char* campos[5];
    char** tkn;
    // criando um retorno caso não encontre todos os dados.
    struct Antena erro = { '0', '0', '0', '0', '0' };
    int aux = 0;

    // fazer a divisão por ,
    tkn = str_split(string, ',');
    if (tkn) {
        int i;
        for (i = 0; *(tkn + i); i++) {
            campos[i] = *(tkn + i);
        }
    }


    char lixo[3] = "---";
    for (int i = 0; i < 13; i++) {
        int ret = strcmp(lixo, campos[i]);

        if (ret == 0) {
            printf("Falta um item\n");
            aux = 1;
        }
    }

    if (aux == 0) {
        strcpy(antena.sinal, campos[2]);
        strcpy(antena.mcc, campos[3]);
        strcpy(antena.mnc, campos[4]);
        strcpy(antena.lac, campos[9]);
        strcpy(antena.cell, campos[10]);

        return antena;
    }
    else {
        return erro;
    }
}



struct Antenas coordenadasTri(char* string) {
    //Pegando as linhas da função

    char** linhas = str_split(string, '\n');
    struct Antenas antenasMqtt = {0};
    
    printf("linhas + 1:\n %s\n", linhas + 1);
    printf("linhas + 2:\n %s\n", linhas + 2);
    printf("linhas + 3:\n %s\n", linhas + 3);
    printf("linhas + 4:\n %s\n", linhas + 4);
    printf("linhas + 5:\n %s\n", linhas + 5);
    printf("linhas + 6:\n %s\n", linhas + 6);
    if (linhas)
    {
        antenasMqtt.antena1 = processarLinhasTri(linhas + 2);
        antenasMqtt.antena2 = processarLinhasTri(linhas + 4);
        antenasMqtt.antena3 = processarLinhasTri(linhas + 6);
        
        printf("\n");
    }
    printf("cell id 1 = %s\n", antenasMqtt.antena1.cell);
    printf("cell id 2 = %s\n", antenasMqtt.antena2.cell);
    printf("cell id 3 = %s\n", antenasMqtt.antena3.cell);
    return antenasMqtt;
}


//OK da serial:
int wait_for_response_gps(int* verifier)
{
    if (verifier == NULL) {
        delay(1);
        return TRUE;
    }
    //Caso especifico para conexão.
    if (verifier == &connected_gps) {
        int timeout = 3000;
        clock_t start_time = clock();
        while (!connected_gps && (clock() < start_time + timeout)) {
            response_received_gps = FALSE;
            ok_received_gps = FALSE;
        }
        //Tempo necessário para estabilizar conexão.
        delay(2);
        return connected_gps;
    }
    else {
        while (!response_received_gps);
        int previous_verifier = *verifier;
        *verifier = FALSE;
        response_received_gps = FALSE;
        return previous_verifier;
    }
}


//Executar comando AT
void execute_at_command_gps(char* command, struct sp_port* port, int* verifier) {
    //Executa o comando, checando se não houve erro na comunicação serial
    check(sp_blocking_write(port, command, strlen(command), 0));
    //espera ok da serial
    return wait_for_response_gps(verifier);
}

//Leitura da serial
//ReadSerialInternal
void read_serial_internal_gps(struct sp_port* port) {
    enum sp_return code;
    char buffer[1025];
    clear_buffer(buffer, 1025);
    do {
        if (!readNext_gps)
            code = sp_blocking_read(port, buffer, 1024, 8000);
        else
            code = sp_blocking_read_next(port, buffer, 1024, 8000);
        if (code > 0)
        {
            int startString = 0;
            for (; startString < 1025 && buffer[startString] == 0; startString++);
            fix_end_line(buffer + startString);
            printf("%s", buffer + startString);
            //printf("%s", buffer + startString);
            int got_ok = strstr(buffer + startString, "OK") != NULL || strstr(buffer + startString, "^SYSSTART") != NULL;
            //Verifica se retornou o código
            if (strstr(buffer + startString, "^SGPSC: \"Nmea / GPS\",\"on\"")) {
                printf("GPS ligado\r\n");
            }

            if (strstr(buffer + startString, "^SGPSC: \"Engine/StartMode\",\"1\"")) {
                printf("START MODE 1 confirmado\n\n\r\n");
            }
       
            if (strstr(buffer + startString, "GPGGA")) {
                char NMEA[1024] = "";
                //Fazer isso para string não atualizar enquanto estamos na função
                strcpy(NMEA, buffer);
                lerCoordenadasGps(NMEA);
            }

            //AT^SMONP
            if (strstr(buffer + startString, "2G")) {
                coordenadasTri(buffer);
            }
            else if (got_ok || strstr(buffer + startString, "ERROR") != NULL) {
                ok_received_gps = got_ok;
                response_received_gps = TRUE;
            }
            clear_buffer(buffer, 1025);
        }
    } while (code >= 0);
    printf("ERROR READING BUFFER\n");
}


//ReadSerial (comando usado na thread)
void read_serial_gps(struct sp_port* port) {
    read_serial_internal_gps(port);
}


//Configuração da placa serial
void configure_serial_gps(char* port_name, struct sp_port** port_pointer) {
    enum sp_return code = sp_get_port_by_name(port_name, port_pointer);
    if (code != SP_OK)
        return;
    code = sp_open(port_pointer[0], SP_MODE_READ_WRITE);
    if (code != SP_OK)
    {
        printf("%s", sp_last_error_message());
        return;
    }
    check(sp_set_baudrate(port_pointer[0], 115200));
    check(sp_set_bits(port_pointer[0], 8));
    check(sp_set_parity(port_pointer[0], SP_PARITY_NONE));
    check(sp_set_stopbits(port_pointer[0], 1));
    check(sp_set_flowcontrol(port_pointer[0], SP_FLOWCONTROL_XONXOFF));
}

void start_gps_routine(struct sp_port* port) {
    printf("==========================GPS============================\n\n");

    printf("                   , , ggddY~~~Ybbgg,,\n");
    printf("               ,agd888b,_ ~Y8, ___`~~Ybga,\n");
    printf("            ,gdP~~88888888baa,.~~8b    '888g,\n");
    printf("          ,dP~     ]888888888P'  ~Y     `888Yb,\n");
    printf("        ,dP~      ,88888888P~  db,       ~8P~~Yb,\n");
    printf("       ,8~       ,888888888b, d8888a           ~8,\n");
    printf("      ,8'        d88888888888,88P~' a,          `8,\n");
    printf("     ,8'         88888888888888PP~  ~~           `8,\n");
    printf("     d'          I88888888888P~                   `b\n");
    printf("     8           `8~88P~~Y8P'                      8\n");
    printf("     8            Y 8[  _ ~                        8\n");
    printf("     8              ~Y8d8b  ~Y a                   8\n");
    printf("     8                 `~~8d,   __                 8\n");
    printf("     Y,                    `~8bd888b,             ,P\n");
    printf("     `8,                     ,d8888888baaa       ,8'\n");
    printf("      `8,                    888888888888'      ,8'\n");
    printf("       `8a                   ~8888888888I      a8'\n");
    printf("        `Yba                  `Y8888888P'    adP'\n");
    printf("          `Yba                 `888888P'   adY'\n");
    printf("            `'Yba,             d8888P' ,adP''\n");
    printf("               `'Y8baa,      ,d888P,ad8P''\n");
    printf("                    ``~~YYba8888P''''\n\n");

    printf("=========================================================\n\n\n\n");


    //Configura o GPS
    execute_at_command_gps("AT^SGPSC=\"Nmea/GPS\",\"on\"\r", port, &ok_received_gps);
    readNext_gps = 0;
    execute_at_command_gps("AT^SGPSC=\"Engine/StartMode\",\"1\"\r", port, &ok_received_gps);
    readNext_gps = 1;

    //Inicia o motor do GPS
    printf("Inicializando Motor de GPS...\n");
    readNext_gps = 0;
    execute_at_command_gps("AT^SGPSC=\"Engine\",\"3\"\r", port, &ok_received_gps);
    readNext_gps = 1;

    //esperar tentativas
retry:
    if (itter_gps < 25) {
        goto retry;
    }
    else {
        printf("GPS indisponivel...\n");
        printf("Encerrando motor...\n\n");
        execute_at_command_gps("AT^SGPSC=\"Engine\",\"0\"\r", port, &ok_received_gps);

    }

    //printf("Iniciando triangulacao de Antena...\n\n");

    //Triangulação de Antena
    /*readNext_gps = 0;
    execute_at_command_gps("AT^SMONP\r", port, &ok_received_gps);
    readNext_gps = 1;*/

    //MQTT


}

void start_triangulation_routine(struct sp_port* port) {
    printf("==============================================\n");
    printf("====== Rotina de triangulacao de Antena ======\n");
    printf("==============================================\n");

    readNext_gps = 0;
    execute_at_command_gps("AT^SMONP\r", port, &ok_received_gps);
    readNext_gps = 1; 

    //MQTT

    
}
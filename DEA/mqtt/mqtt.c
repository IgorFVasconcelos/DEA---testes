#include <stdio.h>
#include "mqtt.h"
#include "libserialport.h"
#include <Windows.h>
#include <string.h>
#include <time.h> 
#include "../common/common.h"

#define GOTO_IF_FALSE(x, y) if (!(x)) goto y
#define MESSAGE_SIZE 1500

int ok_received_mqtt = FALSE;
int connected_mqtt = FALSE;
int response_received_mqtt = FALSE;
int mcc = 0;
int mnc = 0;
int readNext_mqtt = 1;
char* apn_mqtt = NULL;

void mount_large_message(char* msg)
{
	int i = 0;
	for (i = 0; i < 1500; ++i)
		msg[i] = 'a';
	msg[1500] = 0;
}

void get_message_part(char* msg, char *out_msg, int part)
{
	int offset = part * 1460;
	int i;
	for (i = 0; i < 1460; ++i)
		out_msg[i] = msg[i + offset];
	out_msg[1461] = 0;
}

void mount_size_message_cmd(char *cmd, int size)
{
	char partSizeStr[5] = "";
	_itoa_s(size, partSizeStr, 5, 10);
	strcpy_s(cmd, 50, "AT^SISD=1,\"setParam\",\"hcContLen\",");
	strcat_s(cmd, 50, partSizeStr);
	strcat_s(cmd, 50, "\r");
}

void mount_write_message_cmd(char* msg, int size)
{
	char partSizeStr[5] = "";
	_itoa_s(size, partSizeStr, 5, 10);
	strcpy_s(msg, 50, "AT^SISW=1,");
	strcat_s(msg, 50, partSizeStr);
	strcat_s(msg, 50, "\r");
}

//obtem o mcc e o mnc a partir do resultado do comando.
void bind_mcc_and_mnc(char* result)
{
	int mcc_pos = 3;
	int mnc_pos = 4;
	int i;
	int current_pos = 0;
	for (i = 0; i <= strlen(result); ++i)
	{
		if (current_pos == mcc_pos)
		{
			int j = i;
			char str_mcc[3];
			while (result[j] != ',')
			{
				str_mcc[j - i] = result[j];
				++j;
			}
			mcc = atoi(str_mcc);
			i = j;
		}
		else if (current_pos == mnc_pos)
		{
			int j = i;
			char str_mcc[2];
			while (result[j] != ',')
			{
				str_mcc[j - i] = result[j];
				++j;
			}
			mnc = atoi(str_mcc);
			i = j;
		}
		if (result[i] == ',')
		{
			++current_pos;
		}
	}
}

//Verifica se chegou mensagem no mqtt ex: ^SIS: 1,0,3488,"topic=test; bytes=4"
int verify_if_received_message_mqtt(char* str)
{
	char* refChar = str;
	while (*refChar != 0) {
		if (refChar[0] == 'b' && refChar[1] == 'y' && refChar[2] == 't' && refChar[3] == 'e' && refChar[4] == 's' && refChar[5] == '=') {
			char* numStr = &refChar[6];
			char* auxNumStr = numStr;
			int numStrSize = 0;
			while (*auxNumStr++ != '\"')
				numStrSize++;
			return str_to_int(numStr, numStrSize);
		}
		refChar++;
	}
	return 0;
}

// Aguarda o OK do comando AT
int wait_for_response_mqtt(int* verifier)
{
	if (verifier == NULL) {
		delay(1);
		return TRUE;
	}
	//Caso especifico para conexão.
	if (verifier == &connected_mqtt) {
		int timeout = 4000;
		clock_t start_time = clock();
		while (!connected_mqtt && (clock() < start_time + timeout)) {
			response_received_mqtt = FALSE;
			ok_received_mqtt = FALSE;
		}
		//Tempo necessário para estabilizar conexão.
		delay(2);
		return connected_mqtt;
	}
	else {
		while (!response_received_mqtt);
		int previous_verifier = *verifier;
		*verifier = FALSE;
		response_received_mqtt = FALSE;
		return previous_verifier;
	}
}

// Envia um comando AT para a porta serial, time_to_wait é um tempo de espera necessário para alguns comandos.
int execute_at_command_mqtt(char* command, struct sp_port* port, int* verifier)
{
	//Executa o comando, checando se não houve erro na comunicação serial
	check(sp_blocking_write(port, command, strlen(command), 0));
	//Espera a resposta OK do comando AT
	return wait_for_response_mqtt(verifier);
}

void get_apn(char* apn)
{
	if (mcc == 724)
	{
		switch (mnc)
		{
		case 5:
			//strcpy_s(apn, 50, "java.claro.com.br");
		case 6:
			//strcpy_s(apn, 50, "zap.vivo.com.br");
		default:
			break;
		}
	}

	strcpy_s(apn, 50, "zap.vivo.com.br");
}

void mount_apn_command(char* command)
{
	strcat_s(command, 100, "AT+CGDCONT=1,\"IP\",\"");
	char apn[50];
	get_apn(apn);
	printf("APN: %s", apn);
	strcat_s(command, 100, apn);
	strcat_s(command, 100, "\"\r");
}

//Configura o ínicio da rotina mqtt
void configure_mqtt_routine(struct sp_port* port)
{
	//AT^SMONI Recupera o código da operadora do chip
	readNext_mqtt = 0;
	execute_at_command_mqtt("AT^SMONI\r", port, &ok_received_mqtt);
	readNext_mqtt = 1;
	char apn_command[100] = "";
	mount_apn_command(apn_command);
restart:
	GOTO_IF_FALSE(
		// Configura internet no chip
		execute_at_command_mqtt("AT^SXRAT=0\r", port, &ok_received_mqtt) &&
		execute_at_command_mqtt(apn_command, port, &ok_received_mqtt)
		, restart);
	// Reseta a conexão, se necessário. (Não precisa dar OK)
	execute_at_command_mqtt("AT^SISC=1\r", port, &ok_received_mqtt);
	GOTO_IF_FALSE(
		//Inicia a configuração no index 1.
		execute_at_command_mqtt("AT^SICA=0,1\r", port, &ok_received_mqtt) &&
		//Define que o protocolo utilizado será o Mqtt
		execute_at_command_mqtt("AT^SISS=1,srvType,\"Mqtt\"\r", port, &ok_received_mqtt) &&
		//Define o Id da conexão
		execute_at_command_mqtt("AT^SISS=1,conId,\"1\"\r", port, &ok_received_mqtt) &&
		//Define o endereço da conexão
		execute_at_command_mqtt("AT^SISS=1,address,\"mqtt://3.230.85.247\"\r", port, &ok_received_mqtt) &&
		//Define o clientId da conexão
		execute_at_command_mqtt("AT^SISS=1,clientId,\"1111\"\r", port, &ok_received_mqtt)
		, restart);
}

//Configura a rotina mqtt para enviar dados
void configure_mqtt_publish_routine(struct sp_port* port)
{
	//Espera a thread de ler dados inicilizar
	delay(5);
	configure_mqtt_routine(port);
restart:
	GOTO_IF_FALSE(
		//Define o método Mqtt, nesse caso publish
		execute_at_command_mqtt("AT^SISS=1,cmd,\"publish\"\r", port, &ok_received_mqtt) &&
		//Define o tópico a ser usado na conexão
		execute_at_command_mqtt("AT^SISS=1,Topic, \"test\"\r", port, &ok_received_mqtt)
		, restart);
}

//Configura a rotina mqtt para receber dados
void configure_mqtt_subscribe_routine(struct sp_port* port)
{
	//Espera a thread de ler dados inicilizar
	delay(5);
	configure_mqtt_routine(port);
restart:
	GOTO_IF_FALSE(
		//Define o método Mqtt, nesse caso subscribe
		execute_at_command_mqtt("AT^SISS=1,cmd,\"subscribe\"\r", port, &ok_received_mqtt) &&
		//Define o tópico a ser usado na conexão
		execute_at_command_mqtt("AT^SISS=1,topicFilter, \"test\"\r", port, &ok_received_mqtt)
		, restart);
}

void update_message_parts_variables(int* size_to_send, char *msg, char *msg_chunk,  char* cmd_size, char* cmd_write, int* msg_part)
{
	//vamos dividir a mensagem
	if (*size_to_send > 1460)
	{
		mount_size_message_cmd(cmd_size, 1460);
		mount_write_message_cmd(cmd_write, 1460);
		*size_to_send -= 1460;
	}
	else
	{
		mount_size_message_cmd(cmd_size, *size_to_send);
		mount_write_message_cmd(cmd_write, *size_to_send);
		*size_to_send = 0;
	}
	*msg_part += 1;
	get_message_part(msg, msg_chunk, *msg_part);
}

//Loop infinito de enviar publish mqtt 
void start_mqtt_routine(struct sp_port* port)
{
	int size_to_send = MESSAGE_SIZE;
	int msg_part = -1;
	char msg[1501] = "";
	mount_large_message(msg);
	//variavel que contem o comando do tamanho da mensagem
	char cmdSizeSISD[100] = "";
	char cmd_write[50] = "";
	char msg_chunk[1461] = "";
	while (TRUE)
	{
		//AT^SISS = 1,hcContLen,8
		GOTO_IF_FALSE
		(
			//Prepara a conexão
			execute_at_command_mqtt("AT^SICA=1,1\r", port, &ok_received_mqtt) &&
			//Efetua a conexão
			execute_at_command_mqtt("AT^SISO=1,2\r", port, &connected_mqtt) 
			, restart);
			while (TRUE) {
				do {
					update_message_parts_variables(&size_to_send, msg, msg_chunk, cmdSizeSISD, cmd_write, &msg_part);
					GOTO_IF_FALSE(
						//Define o método mqtt
						execute_at_command_mqtt("AT^SISD=1,\"setParam\",\"cmd\",publish\r", port, &ok_received_mqtt) &&
						//Define o tópico mqtt 
						execute_at_command_mqtt("AT^SISD=1,\"setParam\",\"topic\",test\r", port, &ok_received_mqtt) &&
						//Define o tamanho da mensagem
						execute_at_command_mqtt(cmdSizeSISD, port, &ok_received_mqtt) &&
						//Envia a mensagem mqtt
						execute_at_command_mqtt("AT^SISU=1\r", port, &ok_received_mqtt) && 
						execute_at_command_mqtt(cmd_write, port, NULL) &&
						execute_at_command_mqtt(msg_chunk, port, &ok_received_mqtt)
						, restart);
				} while (size_to_send > 0);
				int size_to_send = MESSAGE_SIZE;
				int msg_part = -1;
				delay(4);
			}
		restart:
			//Reseta a conexão.
			execute_at_command_mqtt("AT^SISC=1\r", port, &ok_received_mqtt);
			connected_mqtt = FALSE;
	}
}

//Loop infinito de receber subscribe mqtt 
void start_mqtt_routine_subscribe(struct sp_port* port)
{
restart:
	while (TRUE)
	{
		while (!connected_mqtt)
		{
			// Reseta a conexão, se necessário.
			execute_at_command_mqtt("AT^SISC=1\r", port, &ok_received_mqtt);
			GOTO_IF_FALSE
			(
				//Prepara a conexão
				execute_at_command_mqtt("AT^SICA=1,1\r", port, &ok_received_mqtt) &&
				//Efetua a conexão
				execute_at_command_mqtt("AT^SISO=1,2\r", port, &connected_mqtt)
				, restart);
		}
		while (connected_mqtt);
	}
}

//Lê a mensagem recebida AT^SISR=1,byteCount
void read_bytes(struct sp_port* port, int byteCount)
{
	char result[20] = "AT^SISR=1,";
	get_command_string(byteCount, result);
	strcat_s(result, 20, "\r");
	execute_at_command_mqtt(result, port, NULL);
}

void read_serial_internal_mqtt(struct sp_port* port, int subscribe)
{
	enum sp_return code;
	char buffer[1025];
	clear_buffer(buffer, 1025);
	do {
		if (!readNext_mqtt)
			code = sp_blocking_read(port, buffer, 1024, 8000);
		else
			code = sp_blocking_read_next(port, buffer, 1024, 8000);
		if (code > 0)
		{
			int startString = 0;
			for (; startString < 1025 && buffer[startString] == 0; startString++);
			fix_end_line(buffer + startString);
			printf("%s", buffer + startString);
			int got_ok = strstr(buffer + startString, "OK") != NULL || strstr(buffer + startString, "^SYSSTART") != NULL;
			//Verifica se retornou o código
			if (strstr(buffer + startString, "^SMONI:"))
			{
				bind_mcc_and_mnc(buffer + startString);
			}
			if (strstr(buffer + startString, "Connection accepted on clean session.") != NULL) {
				connected_mqtt = TRUE;
				response_received_mqtt = TRUE;
			}
			else if (got_ok || strstr(buffer + startString, "ERROR") != NULL) {
				ok_received_mqtt = got_ok;
				response_received_mqtt = TRUE;
			}
			else if (subscribe)
			{
				int bytesReceived = verify_if_received_message_mqtt(buffer + startString);
				if (bytesReceived > 0) {
					read_bytes(port, bytesReceived);
				}
			}
			clear_buffer(buffer, 1025);
		}
	} while (code >= 0);
	printf("ERROR READING BUFFER");
}

// Lê os retornos da porta serial para publish, essa função deve ser chamada em uma thread diferente.
void read_serial_mqtt(struct sp_port* port)
{
	read_serial_internal_mqtt(port, 0);
}

// Lê os retornos da porta serial para subscribe, essa função deve ser chamada em uma thread diferente.
void read_serial_for_subscribe(struct sp_port* port)
{
	read_serial_internal_mqtt(port, 1);
}

//Configura a porta serial
void configure_serial_mqtt(char* port_name, struct sp_port** port_pointer)
{
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
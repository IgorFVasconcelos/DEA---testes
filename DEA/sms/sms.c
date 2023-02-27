#include <stdio.h>
#include "sms.h"
#include "libserialport.h"
#include <Windows.h>
#include <string.h>
#include <time.h> 
#include "../common/common.h"

#define GOTO_IF_FALSE(x, y) if (!(x)) goto y

int ok_received_sms = FALSE;
int connected_sms = FALSE;
int response_received_sms = FALSE;

//Verifica se chegou mensagem no mqtt ex: ^SIS: 1,0,3488,"topic=test; bytes=4"
int verify_if_received_message_sms(char* str)
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
int wait_for_response_sms(int* verifier)
{
	if (verifier == NULL) {
		delay(1);
		return TRUE;
	}
	//Caso especifico para conexão.
	if (verifier == &connected_sms) {
		int timeout = 4000;
		clock_t start_time = clock();
		while (!connected_sms && (clock() < start_time + timeout)) {
			response_received_sms = FALSE;
			ok_received_sms = FALSE;
		}
		//Tempo necessário para estabilizar conexão.
		delay(2);
		return connected_sms;
	}
	else {
		while (!response_received_sms);
		int previous_verifier = *verifier;
		*verifier = FALSE;
		response_received_sms = FALSE;
		return previous_verifier;
	}
}

// Envia um comando AT para a porta serial, time_to_wait é um tempo de espera necessário para alguns comandos.
int execute_at_command_sms(char* command, struct sp_port* port, int* verifier)
{
	//Executa o comando, checando se não houve erro na comunicação serial
	check(sp_blocking_write(port, command, strlen(command), 0));
	//Espera a resposta OK do comando AT
	return wait_for_response_sms(verifier);
}

//coloca o telefone no comando de sms
void construct_sms_command(char* tel, char* command) 
{
	strcat_s(command, 26, "\"");
	strcat_s(command, 26, tel);
	strcat_s(command, 26, "\"\r");
}

//Monta a mensagem para ser aceita pelo mnic
void construct_final_msg_sms(char* final_msg, char* msg)
{
	strcpy_s(final_msg, strlen(msg) + 3, msg);
	strcat_s(final_msg, strlen(msg) + 3, "\x1A\r");
}

//AT+CMGF=1
//AT+CMGS="+5585991966743"
void send_sms(struct sp_port* port, char* tel, char* msg)
{
	//Configura o modo sms para texto
	execute_at_command_sms("AT+CMGF=1\r", port, &ok_received_sms);
	//Começa o envio
	char command[26] = "AT+CMGS=";
	construct_sms_command(tel, command);
	execute_at_command_sms(command, port, NULL);
	//Envia a mensagem
	char* final_msg = malloc(sizeof(char) * (strlen(msg) + 3));
	construct_final_msg_sms(final_msg, msg);
	execute_at_command_sms(final_msg, port, &ok_received_sms);
	free(final_msg);
}

void read_serial_internal_sms(struct sp_port* port)
{
	enum sp_return code;
	char buffer[1025];
	clear_buffer(buffer, 1025);
	do {
		code = sp_blocking_read_next(port, buffer, 1024, 8000);
		if (code > 0)
		{
			int startString = 0;
			for (; startString < 1025 && buffer[startString] == 0; startString++);
			fix_end_line(buffer + startString);
			printf("%s", buffer + startString);
			int got_ok = strstr(buffer + startString, "OK") != NULL || strstr(buffer + startString, "^SYSSTART") != NULL;
			if (strstr(buffer + startString, "Connection accepted on clean session.") != NULL) {
				connected_sms = TRUE;
				response_received_sms = TRUE;
			}
			else if (got_ok || strstr(buffer + startString, "ERROR") != NULL) {
				ok_received_sms = got_ok;
				response_received_sms = TRUE;
			}
			clear_buffer(buffer, 1025);
		}
	} while (code >= 0);
	printf("ERROR READING BUFFER");
}

// Lê os retornos da porta serial para publish, essa função deve ser chamada em uma thread diferente.
void read_serial_sms(struct sp_port* port)
{
	read_serial_internal_sms(port);
}

//Configura a porta serial
void configure_serial_sms(char* port_name, struct sp_port** port_pointer)
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
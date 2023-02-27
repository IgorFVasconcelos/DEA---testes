#include <stdio.h>
#include "tcp.h"
#include "libserialport.h"
#include <Windows.h>
#include <string.h>
#include <time.h>
#include "../common/common.h"

#define TCP_ENDPOINT "servercsgoclub01.brazilsouth.cloudapp.azure.com:27015"

#define GOTO_IF_FALSE(x, y) if (!(x)) goto y

int ok_received_tcp = FALSE;
int connected_tcp = FALSE;
int response_received_tcp = FALSE;

//Verifica se chegou mensagem no mqtt ex: ^SIS: 1,0,3488,"topic=test; bytes=4"
int verify_if_received_message_tcp(char* str)
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
int wait_for_response_tcp(int* verifier)
{
	if (verifier == NULL) {
		delay(1);
		return TRUE;
	}
	//Caso especifico para conexão.
	if (verifier == &connected_tcp) {
		int timeout = 4000;
		clock_t start_time = clock();
		while (!connected_tcp && (clock() < start_time + timeout)) {
			response_received_tcp = FALSE;
			ok_received_tcp = FALSE;
		}
		//Tempo necessário para estabilizar conexão.
		delay(2);
		return connected_tcp;
	}
	else {
		while (!response_received_tcp);
		int previous_verifier = *verifier;
		*verifier = FALSE;
		response_received_tcp = FALSE;
		return previous_verifier;
	}
}

// Envia um comando AT para a porta serial, time_to_wait é um tempo de espera necessário para alguns comandos.
int execute_at_command_tcp(char* command, struct sp_port* port, int* verifier)
{
	//Executa o comando, checando se não houve erro na comunicação serial
	check(sp_blocking_write(port, command, strlen(command), 0));
	//Espera a resposta OK do comando AT
	return wait_for_response_tcp(verifier);
}

//Configura o ínicio da rotina mqtt
void configure_tcp_routine(struct sp_port* port)
{
restart:
	GOTO_IF_FALSE(
		// Configura internet no chip
		execute_at_command_tcp("AT^SXRAT=0\r", port, &ok_received_tcp) &&
		execute_at_command_tcp("AT+CGDCONT=1,\"IP\",\"java.claro.com.br\"\r", port, &ok_received_tcp)
		, restart);
	// Reseta a conexão, se necessário. (Não precisa dar OK)
	execute_at_command_tcp("AT^SISC=1\r", port, &ok_received_tcp);
	GOTO_IF_FALSE(
		//Inicia a configuração no index 1.
		execute_at_command_tcp("AT^SICA=0,1\r", port, &ok_received_tcp) &&
		//Define que o protocolo utilizado será o Mqtt
		execute_at_command_tcp("AT^SISS=1,srvType,socket\r", port, &ok_received_tcp) &&
		//Define o Id da conexão
		execute_at_command_tcp("AT^SISS=1,conId,1\r", port, &ok_received_tcp) &&
		//AT^SISS=0,"tcpMR","3"
		execute_at_command_tcp("AT^SISS=1,\"tcpMR\",\"3\"\r", port, &ok_received_tcp) &&
		//AT^SISS=0,"tcpOT","5555"
		execute_at_command_tcp("AT^SISS=1,\"tcpOT\",\"5555\"\r", port, &ok_received_tcp)
		, restart);
}

//Configura a rotina mqtt para enviar dados
void configure_tcp_send_routine(struct sp_port* port)
{
	//Espera a thread de ler dados inicilizar
	delay(5);
	configure_tcp_routine(port);
restart:
	GOTO_IF_FALSE(
		//Define o endereço da conexão
		execute_at_command_tcp("AT^SISS=1,address,\"socktcp://" TCP_ENDPOINT "\"\r", port, &ok_received_tcp)
		, restart);
}

//Configura a rotina mqtt para enviar dados
void configure_tcp_listen_routine(struct sp_port* port)
{
	//Espera a thread de ler dados inicilizar
	delay(5);
	configure_tcp_routine(port);
restart:
	GOTO_IF_FALSE(
		//Define o endereço da conexão
		execute_at_command_tcp("AT^SISS=1,address,\"socktcp://listener:6666\"\r", port, &ok_received_tcp)
		, restart);
}

//Loop infinito de enviar publish mqtt 
void start_tcp_routine(struct sp_port* port)
{
	while (TRUE)
	{
		GOTO_IF_FALSE
		(
			//Prepara a conexão
			execute_at_command_tcp("AT^SICA=1,1\r", port, &ok_received_tcp) &&
			//Efetua a conexão
			execute_at_command_tcp("AT^SISO=1\r", port, &ok_received_tcp)
			, restart);
		while (TRUE) {
			//Envia a mensagem udp
			execute_at_command_tcp("AT^SISW=1,8\r", port, NULL);
			execute_at_command_tcp("ABCDEFGH\r", port, &ok_received_tcp);
			delay(4);
		}
	restart:
		//Reseta a conexão.
		execute_at_command_tcp("AT^SISC=1\r", port, &ok_received_tcp);
	}
}

//Loop infinito de enviar publish mqtt 
void start_tcp_routine_listen(struct sp_port* port)
{
restart:
	// Reseta a conexão, se necessário.
	execute_at_command_tcp("AT^SISC=1\r", port, &ok_received_tcp);
	GOTO_IF_FALSE
	(
		//Prepara a conexão
		execute_at_command_tcp("AT^SICA=1,1\r", port, &ok_received_tcp) &&
		//Efetua a conexão
		execute_at_command_tcp("AT^SISO=1\r", port, NULL)
		, restart);
	//AT^SISO=0,1 mostra o ip do listener
	execute_at_command_tcp("AT^SISO=1,1\r", port, &ok_received_tcp);
	while (TRUE);
}

//Lê a mensagem recebida AT^SISR=1,byteCount
void read_bytes_tcp(struct sp_port* port, int byteCount)
{
	char result[20] = "AT^SISR=1,";
	get_command_string(byteCount, result);
	strcat_s(result, 20, "\r");
	execute_at_command_tcp(result, port, NULL);
}

void read_serial_internal_tcp(struct sp_port* port, int subscribe)
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
				connected_tcp = TRUE;
				response_received_tcp = TRUE;
			}
			else if (got_ok || strstr(buffer + startString, "ERROR") != NULL) {
				ok_received_tcp = got_ok;
				response_received_tcp = TRUE;
			}
			else if (subscribe)
			{
				int bytesReceived = verify_if_received_message_tcp(buffer + startString);
				if (bytesReceived > 0) {
					read_bytes_tcp(port, bytesReceived);
				}
			}
			clear_buffer(buffer, 1025);
		}
	} while (code >= 0);
	printf("ERROR READING BUFFER");
}

// Lê os retornos da porta serial para publish, essa função deve ser chamada em uma thread diferente.
void read_serial_tcp(struct sp_port* port)
{
	read_serial_internal_tcp(port, 0);
}

// Lê os retornos da porta serial para subscribe, essa função deve ser chamada em uma thread diferente.
void read_serial_for_listen_tcp(struct sp_port* port)
{
	read_serial_internal_tcp(port, 1);
}

//Configura a porta serial
void configure_serial_tcp(char* port_name, struct sp_port** port_pointer)
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
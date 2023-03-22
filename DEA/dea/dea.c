#include <stdio.h>
#include "dea.h"
#include "libserialport.h"
#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> 
#include "../common/common.h"
#include "../dealm/dealm.h"
#include "../dealm/dea_objects.h"
#include "../dealm/dealm_debug.h"
#include "../gps/gps.h"

#define GOTO_IF_FALSE(x, y) if (!(x)) goto y
//#define MESSAGE_SIZE 280

int ok_received_dea = FALSE;
int connected_dea = FALSE;
int response_received_dea = FALSE;
int readNext_dea = 1;
char* apn_dea = NULL;
int mcc_dea = 0;
int mnc_dea = 0;
static char imei_dea[15] = {0};
static char str2send[5300] = {0};
static size_t size_str2send = 0;
static char str3send[5300] = { 0 };
static size_t size_str3send = 0;





void start_dea_gps_autotest(uint8_t* buffer, uint16_t buffer_size)
{
	printf("\n\n********** DEALM Layer **********\n");

	printf("buffer_size %d\r\n", buffer_size);

	//printf(buffer + buffer_size)

	if ((int)buffer_size > 700) {

		//´ponteiro para pegar a segunda metade da mensagem
		uint8_t* pnt = &buffer[buffer_size / 2];
	

		//variavel para primeira parte da msg
		uint8_t buffer_11[1480] = { 0 };
		memcpy(buffer_11, buffer, buffer_size/2);
		Data_t io_in1 = DATA_FULL_DEA(buffer_11, buffer_size/2);


		// variavel para segunda parte da msg
		uint8_t buffer_12[1480] = { 0 };
		memcpy(buffer_12, pnt, ((buffer_size / 2)+1));
		Data_t io_in2 = DATA_FULL_DEA(buffer_12, (buffer_size / 2)+1);

		
		
		

		uint8_t buffer_21[1480] = { 0 };
		Data_t io_data1 = DATA_FLUSH(buffer_21);

		uint8_t buffer_22[1480] = { 0 };
		Data_t io_data2 = DATA_FLUSH(buffer_22);

		dealm_create_information_report(&io_in1, 0, 1, (Dea_Obj_t*)&Object_ECG_Frame, &io_data1);
		dealm_debug_print_w_label("Packet created", io_data1.data_p.p_out, io_data1.has);

		dealm_create_information_report(&io_in2, 0, 1, (Dea_Obj_t*)&Object_ECG_Frame, &io_data2);
		dealm_debug_print_w_label("Packet created", io_data2.data_p.p_out, io_data2.has);

		const size_t arrlen1 = io_data1.has;
		const size_t arrlen2 = io_data2.has;

		const size_t hexlen = 2;

		const size_t outstrlen1 = arrlen1 * hexlen;
		const size_t outstrlen2 = arrlen2 * hexlen;
		



		//Aloca memoria para outstr1
		char* outstr1 = malloc(outstrlen1 + 1);
		if (!outstr1) {
			fprintf(stderr, "Failed to allocate memory\n");
			exit(1);
		}




		//Cria ponteiro para outstr que vai receber os bytes da msg codificando para uma string a ser enviada pelo modem
		char* p1 = outstr1;
		for (size_t i = 0; i < arrlen1; i++) {
			p1 += sprintf(p1, "%.2x", io_data1.data_p.p_out[i]);
		}


		if (outstr1 != NULL) {
			strcpy(str2send, outstr1);
			size_str2send = outstrlen1;
		}



		// Aloca memoria para outstr2
			char* outstr2 = malloc(outstrlen2 + 1);
		if (!outstr2) {
			fprintf(stderr, "Failed to allocate memory\n");
			exit(1);
		}




		//Cria ponteiro para outstr que vai receber os bytes da msg codificando para uma string a ser enviada pelo modem
		char* p2 = outstr2;
		for (size_t j = 0; j < arrlen2; j++) {
			p2 += sprintf(p2, "%.2x", io_data2.data_p.p_out[j]);
		}


		if (outstr2 != NULL) {
			strcpy(str3send, outstr2);
			size_str3send = outstrlen2;
		}
		



	}
	else {


		uint16_t buffer_1[1480] = { 0 };
		memcpy(buffer_1, buffer, buffer_size);


		Data_t io_in = DATA_FULL_DEA(buffer_1, buffer_size);

		uint16_t buffer_2[1480] = { 0 };
		Data_t io_data = DATA_FLUSH(buffer_2);

		dealm_create_information_report(&io_in, 0, 1, (Dea_Obj_t*)&Object_ECG_Frame, &io_data);
		dealm_debug_print_w_label("Packet created", io_data.data_p.p_out, io_data.has);
		const size_t arrlen = io_data.has;
		const size_t hexlen = 2;
		const size_t outstrlen = arrlen * hexlen;



		//Aloca memoria para outstr
		char* outstr = malloc(outstrlen + 1);
		if (!outstr) {
			fprintf(stderr, "Failed to allocate memory\n");
			exit(1);
		}




		//Cria ponteiro para outstr que vai receber os bytes da msg codificando para uma string a ser enviada pelo modem
		char* p = outstr;
		for (size_t i = 0; i < arrlen; i++) {
			p += sprintf(p, "%.2x", io_data.data_p.p_out[i]);
		}


		if (outstr != NULL) {
			strcpy(str2send, outstr);
			size_str2send = outstrlen;
		}


	}

}

void mount_large_message_dea(char* msg)
{
	int i = 0;
	for (i = 0; i < 1480; ++i)
		msg[i] = 'a';
	msg[1480] = 0;
}

void get_message_part_dea(char* msg, char* out_msg, int part)
{
	int offset = part * 1480;
	int i;
	for (i = 0; i < 1480; ++i)
	out_msg[i] = msg[i + offset];
	out_msg[1480] = 0;
}

void mount_size_message_cmd_dea(char* cmd, int size)
{
	char partSizeStr[5] = "";
	_itoa_s(size, partSizeStr, 5, 10);
	strcpy_s(cmd, 50, "AT^SISD=1,\"setParam\",\"hcContLen\",");
	
	
	strcat_s(cmd, 50, partSizeStr);
	strcat_s(cmd, 50, "\r");
}

void mount_write_message_cmd_dea(char* msg, int size)
{
	char partSizeStr[5] = "";
	_itoa_s(size, partSizeStr, 5, 10);
	strcpy_s(msg, 50, "AT^SISW=1,");
	strcat_s(msg, 50, partSizeStr);
	strcat_s(msg, 50, "\r");
	printf("WM = %s\r\n", msg);
}


//obtem o IMEI a partir do resultado do comando.
void bind_imei_dea(char* result)
{
	char* null_token = NULL;
	char* line = strtok_s(result, "\n", &null_token);
	line = strtok_s(NULL, "\n", &null_token);
	strcpy(imei_dea, line);
	printf("IMEI: %s\r\n", imei_dea);
}


//obtem o mcc e o mnc a partir do resultado do comando.
void bind_mcc_and_mnc_dea(char* result)
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
			mcc_dea = atoi(str_mcc);
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
			mnc_dea = atoi(str_mcc);
			i = j;
		}
		if (result[i] == ',')
		{
			++current_pos;
		}
	}
}

//Verifica se chegou mensagem no mqtt ex: ^SIS: 1,0,3488,"topic=test; bytes=4"
int verify_if_received_message_dea(char* str)
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
int wait_for_response_dea(int* verifier)
{
	if (verifier == NULL) {
		delay(1);
		return TRUE;
	}
	//Caso especifico para conexão.
	if (verifier == &connected_dea) {
		int timeout = 4000;
		clock_t start_time = clock();
		while (!connected_dea && (clock() < start_time + timeout)) {
			response_received_dea = FALSE;
			ok_received_dea = FALSE;
		}
		//Tempo necessário para estabilizar conexão.
		delay(2);
		return connected_dea;
	}
	else {
		while (!response_received_dea);
		int previous_verifier = *verifier;
		*verifier = FALSE;
		response_received_dea = FALSE;
		return previous_verifier;
	}
}

// Envia um comando AT para a porta serial, time_to_wait é um tempo de espera necessário para alguns comandos.
int execute_at_command_dea(uint8_t* command, struct sp_port* port, int* verifier)
{
	//Executa o comando, checando se não houve erro na comunicação serial
	printf("execute_at_command_dea = (%zd) : %s\r\n", strlen(command), command);
	check(sp_blocking_write(port, command, strlen(command), 0));
	//Espera a resposta OK do comando AT
	return wait_for_response_dea(verifier);
}

void get_apn_dea(char* apn)
{
	if (mcc_dea == 724)
	{

		switch (mnc_dea)
		{
		case 5:
			strcpy_s(apn, 50, "java.claro.com.br");
			break;

		case 10:
			strcpy_s(apn, 50, "zap.vivo.com.br");
			break;

		case 11:
			strcpy_s(apn, 50, "virtueyes.com.br");
			break;

			
		default:
			break;
		}
	}
}

void mount_apn_command_dea(char* command)
{
	strcat_s(command, 100, "AT+CGDCONT=1,\"IP\",\"");
	char apn[50];
	get_apn_dea(apn);
	printf("APN: %s", apn);
	strcat_s(command, 100, apn);
	strcat_s(command, 100, "\"\r");
}

//Configuração inicial do modem com as rotinas do MqTT
void configure_dea_routine(struct sp_port* port)
{
	//AT+CGSN Recupera número IMEI do modem
	readNext_dea = 0;
	execute_at_command_dea("AT+CGSN\r", port, &ok_received_dea);
	readNext_dea = 1;
	//AT^SMONI Recupera o código da operadora do chip
	readNext_dea = 0;
	execute_at_command_dea("AT^SMONI\r", port, &ok_received_dea);
	readNext_dea = 1;
	char apn_command[100] = "";
	mount_apn_command_dea(apn_command);
restart:
	GOTO_IF_FALSE(
		// Configura internet no chip
		execute_at_command_dea("AT^SXRAT=0\r", port, &ok_received_dea) &&
		execute_at_command_dea(apn_command, port, &ok_received_dea)
		, restart);
	// Reseta a conexão, se necessário. (Não precisa dar OK)
	execute_at_command_dea("AT^SISC=1\r", port, &ok_received_dea);
	GOTO_IF_FALSE(
		//Inicia a configuração no index 1.
		execute_at_command_dea("AT^SICA=0,1\r", port, &ok_received_dea) &&
		//Define que o protocolo utilizado será o Mqtt
		execute_at_command_dea("AT^SISS=1,srvType,\"Mqtt\"\r", port, &ok_received_dea) &&
		//Define o Id da conexão
		execute_at_command_dea("AT^SISS=1,conId,\"1\"\r", port, &ok_received_dea) &&
		//Choose ASCII alphabet
		execute_at_command_dea("AT^SISS=1,alphabet,\"1\"\r", port, &ok_received_dea) &&
		//Define o endereço da conexão
		execute_at_command_dea("AT^SISS=1,address,\"mqtt://3.230.85.247\"\r", port, &ok_received_dea) &&
		//Define o clientId da conexão
		execute_at_command_dea("AT^SISS=1,clientId,\"1111\"\r", port, &ok_received_dea)
		, restart);
		

}

//Configura a rotina mqtt para enviar dados
void configure_dea_publish_routine(struct sp_port* port)
{
	//Espera a thread de ler dados inicilizar
	//delay(5);
	configure_dea_routine(port);
restart:
	GOTO_IF_FALSE(
		//Define o método Mqtt, nesse caso publish
		execute_at_command_dea("AT^SISS=1,cmd,\"publish\"\r", port, &ok_received_dea) &&
		//Define o tópico a ser usado na conexão
		execute_at_command_dea("AT^SISS=1,Topic, \"test\"\r", port, &ok_received_dea)
		, restart);
}

//Configura a rotina mqtt para receber dados
void configure_dea_subscribe_routine(struct sp_port* port)
{
	//Espera a thread de ler dados inicilizar
	delay(5);
	configure_dea_routine(port);
restart:
	GOTO_IF_FALSE(
		//Define o método Mqtt, nesse caso subscribe
		execute_at_command_dea("AT^SISS=1,cmd,\"subscribe\"\r", port, &ok_received_dea) &&
		//Define o tópico a ser usado na conexão
		execute_at_command_dea("AT^SISS=1,topicFilter, \"test\"\r", port, &ok_received_dea)
		, restart);
}

void update_message_parts_variables_dea(int* size_to_send, char* msg, char* msg_chunk, char* cmd_size, char* cmd_write, int* msg_part)
{
	//vamos dividir a mensagem
	if (*size_to_send > 1460)
	{
		mount_size_message_cmd_dea(cmd_size, 1460);
		mount_write_message_cmd_dea(cmd_write, 1460);
		*size_to_send -= 1460;
	}
	else
	{
		mount_size_message_cmd_dea(cmd_size, *size_to_send);
		mount_write_message_cmd_dea(cmd_write, *size_to_send);
		*size_to_send = 0;
	}
	*msg_part += 1;
	//get_message_part_dea(msg, msg_chunk, *msg_part);
}

//Loop infinito de enviar publish mqtt 
void start_dea_routine(struct sp_port* port)
{
	//int size_to_send = MESSAGE_SIZE;
	int size_to_send1 = (int)(size_str2send);
	int size_to_send2 = (int)(size_str3send);

	int msg_part = -1;
	char cmdSizeSISD1[100] = "";
	char cmd_write1[50] = "";
	char cmdSizeSISD2[100] = "";
	char cmd_write2[50] = "";
	char msg_chunk[1461] = "";
	uint8_t msg[43] = {0x05, 0x00, 0x01, 0x02, 0x01, 0x08, 0x03, 0x06, 0x80, 0x00, 0x00, 0x01, 0x09, 0x1A, 0x33, 0x39, 0x35, 0x38, 0x2E, 0x35, 0x31, 0x32, 0x35, 0x32, 0x2C, 0x4E, 0x2C, 0x31, 0x31, 0x36, 0x32, 0x38, 0x2E, 0x33, 0x34, 0x35, 0x38, 0x32, 0x2C, 0x45, 0x12, 0xD6, 0x29};
	const size_t arrlen = sizeof(msg);
	const size_t hexlen = 4;
	const size_t outstrlen = arrlen * hexlen;
		
	uint8_t string_address[64] = { 0 };
	uint8_t* p_string_address = string_address;

	//Aloca memoria para outstr
	char* outstr = malloc(outstrlen + 1);
	if (!outstr) {
		fprintf(stderr, "Failed to allocate memory\n");
		//return 1;
	}
	
	//Cria ponteiro para outstr que vai receber os bytes da msg codificando para uma string a ser enviada pelo modem
	char* p = outstr;
	for (size_t i = 0; i < arrlen; i++) {
		p += sprintf(p, "%.2x", msg[i]);
	}

	printf("DEBUG str2send: %s\r\n", str2send);
	printf("DEBUG str3send: %s\r\n", str3send);

	//exit(1);
	
	//Cria a string do topico passando o IMEI como parâmetro
	snprintf((char*)p_string_address, 64, "%s%s%s", "AT^SISD=1,\"setParam\",\"topic\",ecg/",imei_dea,"\r");
	printf("DEBUG: %s\r\n", p_string_address);

	


	while (TRUE)
	{
		//AT^SISS = 1,hcContLen,8
		GOTO_IF_FALSE
		(
			//Prepara a conexão
			execute_at_command_dea("AT^SICA=1,1\r", port, &ok_received_dea) &&
			//Efetua a conexão
			execute_at_command_dea("AT^SISO=1,2\r", port, &connected_dea)
			, restart);
		while (TRUE) {
			do {
				update_message_parts_variables_dea(&size_to_send1, msg, msg_chunk, cmdSizeSISD1, cmd_write1, &msg_part);
				update_message_parts_variables_dea(&size_to_send2, msg, msg_chunk, cmdSizeSISD2, cmd_write2, &msg_part);

				if (size_str3send != 0) {

					GOTO_IF_FALSE(

						//Define o método mqtt
						execute_at_command_dea("AT^SISD=1,\"setParam\",\"cmd\",publish\r", port, &ok_received_dea) &&
						//Define o tópico mqtt 
						execute_at_command_dea(p_string_address, port, &ok_received_dea) &&
						//Define o tamanho da mensagem
						execute_at_command_dea(cmdSizeSISD1, port, &ok_received_dea) &&
						//Envia a mensagem mqtt
						execute_at_command_dea("AT^SISU=1\r", port, &ok_received_dea) &&
						//Configura o modem para envio de char via porta serial
						// AT^SISD=1,"setParam","hcContent","\05\51\01\02\01\08" + str2send_gps
						// var_send = "\05\51\01\02\01\08\" + str2send_gps 0X01 0X05
						execute_at_command_dea(cmd_write1, port, NULL) &&
						//Envia a mensagem escrevendo na porta serial
						execute_at_command_dea(str2send, port, &ok_received_dea) &&
						//Define o método mqtt
						execute_at_command_dea("AT^SISD=1,\"setParam\",\"cmd\",publish\r", port, &ok_received_dea) &&
						//Define o tópico mqtt 
						execute_at_command_dea(p_string_address, port, &ok_received_dea) &&
						//Define o tamanho da mensagem
						execute_at_command_dea(cmdSizeSISD2, port, &ok_received_dea) &&
						//Envia a mensagem mqtt
						execute_at_command_dea("AT^SISU=1\r", port, &ok_received_dea) &&
						//Configura o modem para envio de char via porta serial
						// AT^SISD=1,"setParam","hcContent","\05\51\01\02\01\08" + str2send_gps
						// var_send = "\05\51\01\02\01\08\" + str2send_gps 0X01 0X05
						execute_at_command_dea(cmd_write2, port, NULL) &&
						//Envia a mensagem escrevendo na porta serial
						execute_at_command_dea(str3send, port, &ok_received_dea)
						, restart);

				}
				else {

					GOTO_IF_FALSE(

						//Define o método mqtt
						execute_at_command_dea("AT^SISD=1,\"setParam\",\"cmd\",publish\r", port, &ok_received_dea) &&
						//Define o tópico mqtt 
						execute_at_command_dea(p_string_address, port, &ok_received_dea) &&
						//Define o tamanho da mensagem
						execute_at_command_dea(cmdSizeSISD1, port, &ok_received_dea) &&
						//Envia a mensagem mqtt
						execute_at_command_dea("AT^SISU=1\r", port, &ok_received_dea) &&
						//Configura o modem para envio de char via porta serial
						// AT^SISD=1,"setParam","hcContent","\05\51\01\02\01\08" + str2send_gps
						// var_send = "\05\51\01\02\01\08\" + str2send_gps 0X01 0X05
						execute_at_command_dea(cmd_write1, port, NULL) &&
						//Envia a mensagem escrevendo na porta serial
						execute_at_command_dea(str2send, port, &ok_received_dea)
						, restart);


				}

			} while (size_to_send1 > 0);
			size_to_send1 = (int)(size_str2send);
			size_to_send2 = (int)(size_str3send);
			msg_part = -1;
			delay(30);
		}
	restart:
		//Reseta a conexão.
		execute_at_command_dea("AT^SISC=1\r", port, &ok_received_dea);
		connected_dea = FALSE;
	}
}

//Loop infinito de receber subscribe mqtt 
void start_dea_routine_subscribe(struct sp_port* port)
{
restart:
	while (TRUE)
	{
		while (!connected_dea)
		{
			// Reseta a conexão, se necessário.
			execute_at_command_dea("AT^SISC=1\r", port, &ok_received_dea);
			GOTO_IF_FALSE
			(
				//Prepara a conexão
				execute_at_command_dea("AT^SICA=1,1\r", port, &ok_received_dea) &&
				//Efetua a conexão
				execute_at_command_dea("AT^SISO=1,2\r", port, &connected_dea)
				, restart);
		}
		while (connected_dea);
	}
}

//Lê a mensagem recebida AT^SISR=1,byteCount
void read_bytes_dea(struct sp_port* port, int byteCount)
{
	char result[20] = "AT^SISR=1,";
	get_command_string(byteCount, result);
	strcat_s(result, 20, "\r");
	execute_at_command_dea(result, port, NULL);
}

void read_serial_internal_dea(struct sp_port* port, int subscribe)
{
	enum sp_return code;
	char buffer[1025];
	clear_buffer(buffer, 1025);
	
	do {
		if (!readNext_dea)
			code = sp_blocking_read(port, buffer, 1024, 8000);
		else
			code = sp_blocking_read_next(port, buffer, 1024, 8000);
		if (code > 0)
		{
			int startString = 0;
			for (; startString < 1025 && buffer[startString] == 0; startString++);
			//Vê onde começa a string
			fix_end_line(buffer + startString);
			printf("%s", buffer + startString);
			int got_ok = strstr(buffer + startString, "OK") != NULL || strstr(buffer + startString, "^SYSSTART") != NULL;
			//Verifica se retornou o código 
			if (strstr(buffer + startString, "^SMONI:"))
			{
				bind_mcc_and_mnc_dea(buffer + startString);
			}

			if (strstr(buffer + startString, "+CGSN"))
			{
				printf("Buffer: %s \r\n", buffer);
				printf("StartString: %d \r\n", startString);
				bind_imei_dea(buffer + startString);
			}

			if (strstr(buffer + startString, "Connection accepted on clean session.") != NULL) {
				connected_dea = TRUE;
				response_received_dea = TRUE;
			}
			else if (got_ok || strstr(buffer + startString, "ERROR") != NULL) {
				ok_received_dea = got_ok;
				response_received_dea = TRUE;
			}
			else if (subscribe)
			{
				int bytesReceived = verify_if_received_message_dea(buffer + startString);
				if (bytesReceived > 0) {
					read_bytes_dea(port, bytesReceived);
				}
			}
			clear_buffer(buffer, 1025);
		}
	} while (code >= 0);
	printf("ERROR READING BUFFER");
}

// Lê os retornos da porta serial para publish, essa função deve ser chamada em uma thread diferente.
void read_serial_dea(struct sp_port* port)
{
	read_serial_internal_dea(port, 0);
}

// Lê os retornos da porta serial para subscribe, essa função deve ser chamada em uma thread diferente.
void read_dea_serial_for_subscribe(struct sp_port* port)
{
	read_serial_internal_dea(port, 1);
}

//Configura a porta serial
void configure_serial_dea(char* port_name, struct sp_port** port_pointer)
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
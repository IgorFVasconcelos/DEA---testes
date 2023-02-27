#include <time.h>
#include "libserialport.h"
#include <stdio.h>
#include <Windows.h>
#include <string.h>

int str_to_int(char* str, int size)
{
	int multiplier = 1;
	int result = 0;
	for (int i = size - 1; i >= 0; --i) {
		int num = str[i] - '0';
		result += num * multiplier;
		multiplier *= 10;
	}

	return result;
}

//Obtem o commando em string
void get_command_string(int byteCount, char* buffer)
{
	char str[10];
	_itoa_s(byteCount, str, 10, 10);
	strcat_s(buffer, 20, str);
}

//Substitui o \r por \n para que o print no terminal fique adequado.
void fix_end_line(char* string) {
	while (*string != 0) {
		if (*string == '\r')
			*string = '\n';
		string++;
	}
}

// Aguarda um tempo em segundos
void delay(int number_of_seconds)
{
	// Converting time into milli_seconds 
	int milli_seconds = 1000 * number_of_seconds;

	// Storing start time 
	clock_t start_time = clock();

	// looping till required time is not achieved 
		while (clock() < start_time + milli_seconds)
		;
}

void clear_buffer(char* buffer, int size)
{
	for (int i = 0; i < size; ++i)
		buffer[i] = 0;
}

/* Helper function for error handling. */
int check(enum sp_return result)
{
	/* For this example we'll just exit on any error by calling abort(). */
	char* error_message;

	switch (result) {
	case SP_ERR_ARG:
		printf("Error: Invalid argument.\n");
		abort();
	case SP_ERR_FAIL:
		error_message = sp_last_error_message();
		printf("Error: Failed: %s\n", error_message);
		sp_free_error_message(error_message);
		abort();
	case SP_ERR_SUPP:
		printf("Error: Not supported.\n");
		abort();
	case SP_ERR_MEM:
		printf("Error: Couldn't allocate memory.\n");
		abort();
	case SP_OK:
	default:
		return result;
	}
}
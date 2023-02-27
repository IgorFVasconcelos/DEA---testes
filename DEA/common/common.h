#ifndef COMMON
#define COMMON
void get_command_string(int byteCount, char* buffer);

int str_to_int(char* str, int size);

void fix_end_line(char* string);

void clear_buffer(char* buffer, int size);

void delay(int number_of_seconds);

int check(enum sp_return result);

#endif
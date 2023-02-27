#include "stdint.h"
#include "stdio.h"

void dealm_debug_print(uint8_t * p_data, uint32_t size)
{
	printf("\r\n");
	for (uint32_t i = 0; i < size; i++)
	{
		printf("0x%02X", p_data[i]);
		if (i < (size - 1))
		{
			printf(", ");
		}
	}
	printf("\r\n");
}

void dealm_debug_print_w_label(const char * text, uint8_t * p_data, uint32_t size)
{
	printf("\r\n");
	printf(text);
	printf(" with size of %d:", size);
	printf("\r\n");
	for (uint32_t i = 0; i < size; i++)
	{
		printf("0x%02X", p_data[i]);
		//printf("%c", p_data[i]);
		if (i < (size - 1))
		{
			printf(", ");
		}
	}
	printf("\r\n");
}

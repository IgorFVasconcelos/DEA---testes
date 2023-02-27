///@file

/**************************************************************************************************
 * INCLUDES
 *************************************************************************************************/
#include <stddef.h>
#include "string.h"
#include "stdbool.h"
#include "asn1.h"

#ifdef ASSERT_LIB
FILENUM(1);
#endif
/**************************************************************************************************
 * PRIVATE DEFINES
 *************************************************************************************************/
#ifndef UNIT_TESTS
/* !
 * Unexposed's module's local functions
 */
#define STATIC static
#else
/* !
 * Exposes module's local functions
 */
#define STATIC

#endif

/**************************************************************************************************
 * PRIVATE TYPES
 *************************************************************************************************/

/**************************************************************************************************
 * PRIVATE PROTOTYPES
 *************************************************************************************************/

/**************************************************************************************************
 * PRIVATE DATA
 *************************************************************************************************/

/**************************************************************************************************
 * FUNCTIONS
 *************************************************************************************************/

Status asn1_put_octet(Data_t *p_data, uint32_t data_size)
{
	if (p_data == NULL)
	{
		return ENC_STATUS((SysStatus)STATUS_NULL_POINTER, ASN1_CLASS);
	}

	uint8_t octet_string_tag = OCTET_STRING_TAG;

	if (Data_put(p_data, &octet_string_tag, sizeof(uint8_t)) != ENC_STATUS((SysStatus)STATUS_SUCCESS, DATA_STRUCT_CLASS)){
		return ENC_STATUS((SysStatus)STATUS_BUFFER_USE_ERROR, ASN1_CLASS);
	}

	if (Data_put(p_data, (uint8_t*)&data_size, sizeof(uint8_t)) != ENC_STATUS((SysStatus)STATUS_SUCCESS, DATA_STRUCT_CLASS)){
		return ENC_STATUS((SysStatus)STATUS_BUFFER_USE_ERROR, ASN1_CLASS);
	}

	return ENC_STATUS((SysStatus)STATUS_SUCCESS, ASN1_CLASS);
}

Status asn1_put_uint16_tag(Data_t *p_data)
{
	if (p_data == NULL)
	{
		return ENC_STATUS((SysStatus)STATUS_NULL_POINTER, ASN1_CLASS);
	}

	uint8_t uint16_t_tag = UINT16_TAG;

	if (Data_put(p_data, &uint16_t_tag, sizeof(uint8_t)) != ENC_STATUS((SysStatus)STATUS_SUCCESS, DATA_STRUCT_CLASS)){
		return ENC_STATUS((SysStatus)STATUS_BUFFER_USE_ERROR, ASN1_CLASS);
	}

	return ENC_STATUS((SysStatus)STATUS_SUCCESS, ASN1_CLASS);
}

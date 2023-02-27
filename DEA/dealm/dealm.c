///@file

/**************************************************************************************************
 * INCLUDES
 *************************************************************************************************/
#include <stddef.h>
#include "string.h"
#include "stdbool.h"
#include "dealm.h"
#include "asn1.h"
#include "endian.h"
#include "checksum.h"

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
information_report_t g_info_rep = {
		.service_tag = INFORMATION_REPORT,
		.version = 0x00,
		.list_var_name = 1,
		.var_name_len = VAR_LEN_SHORT,
		.list_data = 3,
		.block_data_type = 0x06
};
/**************************************************************************************************
 * FUNCTIONS
 *************************************************************************************************/

Status dealm_create_information_report(Data_t *p_in, uint32_t last_block, bool finish, Dea_Obj_t *p_object, Data_t *p_data)
{
	if (p_data == NULL)
	{
		return ENC_STATUS((SysStatus)STATUS_NULL_POINTER, DEALM_COMM_CLASS);
	}

	memcpy(&g_info_rep.var_name.var_name, &p_object->var_name, sizeof(uint16_t));

	g_info_rep.block_info.raw_data = 0;
	g_info_rep.block_info.raw_data = last_block+1;

	if (finish) {
		g_info_rep.block_info.last_block = 1;
	}

	/*Convert data to uplink (web)*/

	g_info_rep.block_info.raw_data = get_int32_t_be(&g_info_rep.block_info.raw_data);

	if (Data_put(p_data, (uint8_t*)&g_info_rep, sizeof(g_info_rep)) != ENC_STATUS((SysStatus)STATUS_SUCCESS, DATA_STRUCT_CLASS)) {
		return ENC_STATUS((SysStatus)STATUS_BUFFER_USE_ERROR, DEALM_COMM_CLASS);
	}

	/*Put octet string asn1*/
	if (asn1_put_octet(p_data, p_in->has) != ENC_STATUS((SysStatus)STATUS_SUCCESS, ASN1_CLASS)) {
		return ENC_STATUS((SysStatus)STATUS_FAILED, DEALM_COMM_CLASS);
	}

	if (Data_copy(p_data, p_in) != ENC_STATUS((SysStatus)STATUS_SUCCESS, DATA_STRUCT_CLASS)) {
		return ENC_STATUS((SysStatus)STATUS_BUFFER_USE_ERROR, DEALM_COMM_CLASS);
	}

	uint16_t crc = crc_16((const unsigned char *)&p_data->data_p.p_in[p_data->start], p_data->has);
	crc = get_int16_t_be(&crc);

	if (asn1_put_uint16_tag(p_data) != ENC_STATUS((SysStatus)STATUS_SUCCESS, ASN1_CLASS)) {
		return ENC_STATUS((SysStatus)STATUS_FAILED, DEALM_COMM_CLASS);
	}

	if (Data_put(p_data, (uint8_t*)&crc, sizeof(crc)) != ENC_STATUS((SysStatus)STATUS_SUCCESS, DATA_STRUCT_CLASS)) {
		return ENC_STATUS((SysStatus)STATUS_BUFFER_USE_ERROR, DEALM_COMM_CLASS);
	}

	return ENC_STATUS((SysStatus)STATUS_SUCCESS, DEALM_COMM_CLASS);
}

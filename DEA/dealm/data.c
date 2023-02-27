///@file

/**************************************************
 * INCLUDES
 **************************************************/
#include <stddef.h>
#include <string.h>

#include "data.h"


/**************************************************
 * PRIVATE DEFINES
 **************************************************/
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

/**************************************************
 * PRIVATE TYPES
 **************************************************/



/**************************************************
 * PRIVATE PROTOTYPES
 **************************************************/
#ifndef UNIT_TESTS

#endif

/**************************************************
 * PRIVATE DATA
 **************************************************/



/**************************************************
 * FUNCTIONS
 **************************************************/

STATIC Status Data_has_data(Data_t *p_from, uint32_t size) {
	if (p_from == NULL) {
		return ENC_STATUS((SysStatus)STATUS_NULL_POINTER, DATA_STRUCT_CLASS);
	}

	if ((p_from->data_p.p_out == NULL) || (p_from->start > p_from->end)) {
		return ENC_STATUS((SysStatus)STATUS_INVALID_PARAMETER_DEA, DATA_STRUCT_CLASS);
	}

	if ((p_from->has == 0U) || (p_from->has < size)) {
		return ENC_STATUS((SysStatus)STATUS_BUFFER_OVERFLOW, DATA_STRUCT_CLASS);
	}

	return ENC_STATUS((SysStatus)STATUS_SUCCESS, DATA_STRUCT_CLASS);
}

STATIC Status Data_has_room(Data_t *p_to, uint32_t size) {
	if (p_to == NULL) {
		return ENC_STATUS((SysStatus)STATUS_NULL_POINTER, DATA_STRUCT_CLASS);
	}

	if ((p_to->data_p.p_in == NULL) || (p_to->start > p_to->end)) {
		return ENC_STATUS((SysStatus)STATUS_INVALID_PARAMETER_DEA, DATA_STRUCT_CLASS);
	}

	if ((p_to->size - p_to->end) < size) {
		return ENC_STATUS((SysStatus)STATUS_BUFFER_OVERFLOW, DATA_STRUCT_CLASS);
	}

	return ENC_STATUS((SysStatus)STATUS_SUCCESS, DATA_STRUCT_CLASS);
}

Status Data_put(Data_t *p_to, uint8_t *p_from, uint32_t from_size) {
	if (p_from == NULL) {
		return ENC_STATUS((SysStatus)STATUS_NULL_POINTER, DATA_STRUCT_CLASS);
	}

	SysStatus system_status = Data_has_room(p_to, from_size);
	if (system_status != ENC_STATUS((SysStatus)STATUS_SUCCESS, DATA_STRUCT_CLASS)) {
		return system_status;
	}

	uint8_t *p_data = &p_to->data_p.p_in[p_to->end];
	for (uint32_t index = 0U; index < from_size; index++) {
		p_data = &p_to->data_p.p_in[p_to->end];
		*p_data = *p_from++;
		p_to->end++;
		p_to->has++;
	}

	return system_status;
}

Status Data_get(Data_t *p_from, uint8_t *p_to, uint32_t to_size) {
	if (p_to == NULL) {
		return ENC_STATUS((SysStatus)STATUS_NULL_POINTER, DATA_STRUCT_CLASS);
	}

	SysStatus system_status = Data_has_data(p_from, to_size);
	if (system_status != ENC_STATUS((SysStatus)STATUS_SUCCESS, DATA_STRUCT_CLASS)) {
		return system_status;
	}
	uint8_t *p_data = &p_from->data_p.p_out[p_from->start];
	for (uint32_t index = 0U; index < to_size; index++) {
		*p_to++ = *p_data++;
		p_from->start++;
		p_from->has--;
	}

	return system_status;
}

Status Data_copy(Data_t *p_to, Data_t *p_from) {
	if (p_from == NULL) {
		return ENC_STATUS((SysStatus)STATUS_NULL_POINTER, DATA_STRUCT_CLASS);
	}

	SysStatus system_status = ENC_STATUS((SysStatus)STATUS_BUFFER_OVERFLOW, DATA_STRUCT_CLASS);
	uint8_t temp;

	for (uint32_t index = p_from->start ; index < p_from->end; index++) {
		system_status = Data_get(p_from, &temp, sizeof(temp));
		if (system_status != ENC_STATUS((SysStatus)STATUS_SUCCESS, DATA_STRUCT_CLASS)) {
			return system_status;
		}

		system_status = Data_put(p_to, &temp, sizeof(temp));
		if (system_status != ENC_STATUS((SysStatus)STATUS_SUCCESS, DATA_STRUCT_CLASS)) {
			return system_status;
		}
	}

	return system_status;
}

Status Data_mirror(Data_t *p_to, Data_t *p_from) {
	if ((p_to == NULL)||(p_from == NULL)) {
		return ENC_STATUS((SysStatus)STATUS_NULL_POINTER, DATA_STRUCT_CLASS);
	}
	memcpy(p_to, p_from, sizeof(Data_t));

	return ENC_STATUS((SysStatus)STATUS_SUCCESS, DATA_STRUCT_CLASS);
}

Status Data_flush(Data_t *p_from) {
	if (p_from == NULL) {
		return ENC_STATUS((SysStatus)STATUS_NULL_POINTER, DATA_STRUCT_CLASS);
	}

	p_from->start = 0U;
	p_from->end = 0U;
	p_from->has = 0U;

	return ENC_STATUS((SysStatus)STATUS_SUCCESS, DATA_STRUCT_CLASS);
}

Status Data_drop(Data_t *p_from, uint32_t drop_size) {
	if (p_from == NULL) {
		return ENC_STATUS((SysStatus)STATUS_NULL_POINTER, DATA_STRUCT_CLASS);
	}

	SysStatus system_status = Data_has_data(p_from, drop_size);
	if (system_status != ENC_STATUS((SysStatus)STATUS_SUCCESS, DATA_STRUCT_CLASS)) {
		return system_status;
	}
	p_from->start += drop_size;
	p_from->has -= drop_size;

	return system_status;
}

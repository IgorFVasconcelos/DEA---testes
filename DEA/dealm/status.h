///@file

/*
 * @brief
 * @par
 * COPYRIGHT NOTICE: (c) 2020 LESC
 * All rights reserved.
 */

#ifndef __STATUS_H__
#define __STATUS_H__

/**************************************************************************************************
 * INCLUDES
 *************************************************************************************************/
#include <stdint.h>

/**************************************************************************************************
 * PUBLIC TYPES
 *************************************************************************************************/
/*! Encodes system Status type */
typedef uint32_t Status;

/*! Encodes classes and error into StatusCode value */
typedef enum SysStatus {
	STATUS_SUCCESS,
	STATUS_INVALID_PARAMETER_DEA,
	STATUS_NULL_POINTER,
	STATUS_NOT_READY,
	STATUS_BUFFER_OVERFLOW,
	STATUS_FAILED,
	STATUS_BUFFER_USE_ERROR,

	STATUS_ERROR_NOT_DEFINED = 0xFFFF
}SysStatus;

/**************************************************************************************************
 * PUBLIC DEFINES
 *************************************************************************************************/
/*! Encodes System Class error */
#define SYS_CLASS 			      					0x10000U
/*! Encodes DEALM_COMM Class error */
#define DEALM_COMM_CLASS 			      			0x20000U
/*! Encodes DATA STRUCT Class error */
#define DATA_STRUCT_CLASS 			      			0x30000U
/*! Encodes ASN1 Class error */
#define ASN1_CLASS 			      					0x40000U

/* Private macro -------------------------------------------------------------*/
#define ENC_STATUS(e, c)   ((uint32_t)((SysStatus)c) | e)

#endif

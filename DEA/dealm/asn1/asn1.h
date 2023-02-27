///@file

#ifndef ASN1_H_
#define ASN1_H_

/**************************************************************************************************
 * INCLUDES
 *************************************************************************************************/
#include <stdint.h>
#include "data.h"
#include "status.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 * PUBLIC TYPES
 *************************************************************************************************/
typedef enum Asn_Type_t {
	ARRAY_TAG = 1,
	STRUCT_TAG = 2,
	BOOLEAN_TAG = 3,
	INT32_TAG = 5,
	UINT32_TAG = 6,
	OCTET_STRING_TAG = 9,
	INT8_TAG = 15,
	INT16_TAG = 16,
	UINT8_TAG = 17,
	UINT16_TAG = 18,
	INT64_TAG = 20,
	UINT64_TAG = 21,
	ASN_MAX_TAGS
}Asn_Type_t;

/**************************************************************************************************
 * PUBLIC DEFINES
 *************************************************************************************************/
#ifndef UNIT_TESTS
  /*! Unexposed's module's local functions */
#define STATIC  static
#else
  /*! Exposed's module's local functions */
#define STATIC

#endif

/**************************************************************************************************
 * PUBLIC PROTOTYPES
 *************************************************************************************************/
/**
 * @brief
 * @param
 * @param
 * @return:
 */
Status asn1_put_octet(Data_t *p_data, uint32_t data_size);

/**
 * @brief
 * @param
 * @param
 * @return:
 */
Status asn1_put_uint16_tag(Data_t *p_data);

#ifdef __cplusplus
}
#endif
#endif /* ASN1_H_ */

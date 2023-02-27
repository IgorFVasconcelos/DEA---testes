///@file

#ifndef DEALM_H_
#define DEALM_H_

/**************************************************************************************************
 * INCLUDES
 *************************************************************************************************/
#include <stdint.h>
#include "data.h"
#include "status.h"
#include "dea_objects.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdbool.h>

/**************************************************************************************************
 * PUBLIC TYPES
 *************************************************************************************************/
typedef enum {
	/*! Read request service*/
	READ_RQ = 1,
	/*! Write request service*/
	WRITE_RQ = 2,
	/*! Read response service*/
	READ_RP = 3,
	/*! Write response service*/
	WRITE_RP = 4,
	/*! Information report service*/
	INFORMATION_REPORT = 5
} service_tag_t;

typedef enum {
	VAR_LEN_SHORT = 2,
	VAR_LEN_LONG = 4
} var_name_len_t;

typedef struct {
	uint8_t service_tag;
	uint8_t list_var_name;
	uint8_t var_name_len;
	uint16_t var_name;
	uint16_t crc16;
} read_service_rq_t;

typedef struct {
	uint8_t service_tag;
	uint8_t list_var_name;
	uint8_t var_name_len;
	uint16_t var_name;
	uint8_t list_data;
	Data_t *p_data;
	uint16_t crc1;
} write_service_rq_t;

typedef union block_info_t{
	struct {
		uint32_t block_number: 31;
		uint32_t last_block: 1;
	};
	uint32_t raw_data;
} block_info_t;

#ifdef UNIT_TESTS
#pragma pack(push, 1)
#endif
typedef struct {
	/*! Tag of service*/
	uint8_t service_tag;
	/*! Tag of payload*/
	uint8_t version;
	/*! Quantity of objects in list*/
	uint8_t list_var_name;
	/*! Variable name len (2 or 4 bytes) - short uint16_t | long uint32_t*/
	uint8_t var_name_len;
	/*! Pointer to variable name value*/
	Dea_Obj_t var_name;
	/*! Quantity of data in list - first ever data block*/
	uint8_t list_data;
	/*! Data block data type*/
	uint8_t block_data_type;
	/*! Data block info*/
	block_info_t block_info;
/*	Data_t *p_data;
	uint16_t crc16;*/
} information_report_t;
#ifdef UNIT_TESTS
#pragma pack(pop)
#endif

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
Status dealm_create_information_report(Data_t *p_in, uint32_t last_block, bool finish, Dea_Obj_t *p_object, Data_t *p_data);

#ifdef __cplusplus
}
#endif
#endif /* DEALM_H_ */

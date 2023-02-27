///@file

#ifndef DEALM_DATA_H_
#define DEALM_DATA_H_

/**************************************************
 * INCLUDES
 **************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include "status.h"

/**************************************************
 * PUBLIC DEFINES
 **************************************************/
#define DATA_FLUSH(BUFFER)	{.data_p = {.p_in = BUFFER}, .start = 0, .end = 0, .has = 0, .size = sizeof(BUFFER)}
#define DATA_FULL(BUFFER)	{.data_p = {.p_in = BUFFER}, .start = 0, .end = sizeof(BUFFER), .has = sizeof(BUFFER), .size = sizeof(BUFFER)}
#define DATA_FULL_DEA(BUFFER, SIZEOFBUFFER)	{.data_p = {.p_in = BUFFER}, .start = 0, .end = SIZEOFBUFFER, .has = SIZEOFBUFFER, .size = SIZEOFBUFFER}

/**************************************************
 * PUBLIC TYPES
 **************************************************/
/*! In-Out data buffer address */
	typedef union Data_p {
		/*! In address*/
		uint8_t *p_in;
		/*! Out address*/
		uint8_t *p_out;
	} Data_p;

/*! In-Out data structure */
	typedef struct Data_t {
		/*! In-Out data buffer address */
		Data_p data_p;
		/*! Data start index*/
		uint32_t start;
		/*! Data end index*/
		uint32_t end;
		/*! Data has index*/
		uint32_t has;
		/*! Data size index*/
		uint32_t size;
	} Data_t;


/**************************************************
 * PUBLIC PROTOTYPES
 **************************************************/
	Status Data_put(Data_t *p_to, uint8_t *p_from, uint32_t from_size);
	Status Data_get(Data_t *p_from, uint8_t *p_to, uint32_t to_size);
	Status Data_copy(Data_t *p_to, Data_t *p_from);
	Status Data_mirror(Data_t *p_to, Data_t *p_from);
	Status Data_flush(Data_t *p_from);
	Status Data_drop(Data_t *p_from, uint32_t drop_size);

#ifdef __cplusplus
}
#endif

#endif /* DEALM_DATA_H_ */

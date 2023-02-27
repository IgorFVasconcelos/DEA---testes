/*
 * dealm_debug.h
 *
 *  Created on: 26 de abr de 2021
 *      Author: Developer
 */

#ifndef DEALM_DEALM_DEBUG_H_
#define DEALM_DEALM_DEBUG_H_

#include "stdint.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*! Funcao auxiliar para print debug de pacotes*/
void dealm_debug_print(uint8_t * p_data, uint32_t size);

/*! Funcao auxiliar para print debug de pacotes com inclusao de texto*/
void dealm_debug_print_w_label(const char * text, uint8_t * p_data, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* DEALM_DEALM_DEBUG_H_ */

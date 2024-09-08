/* 
 * File:   voc_sgp40.h
 * Author: husak
 *
 * Created on 10. prosince 2020, 21:45
 */

#include "stdint.h"
#include "sensirion_voc_algorithm.h"

#ifndef VOC_SGP40_H
#define	VOC_SGP40_H

static VocAlgorithmParams voc_algorithm_params;

void voc_SGP40_Init();
void voc_SGP40_Task();
int16_t voc_SGP40_get_air_quality();

#endif	/* VOC_SGP40_H */


/* 
 * File:   light_VEML7700.h
 * Author: husak
 *
 * Created on July 30, 2019, 9:37 PM
 */

#ifndef LIGHT_VEML7700_H
#define	LIGHT_VEML7700_H

int light_VEML7700_init();
int light_VEML7700_meas(int32_t *lux);

#endif	/* LIGHT_VEML7700_H */


#ifndef FRAM_MB85_H
#define FRAM_MB85_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif



void fram_MB85_init(void);
int fram_MB85_write64(uint8_t index, uint64_t value);
int fram_MB85_read64(uint8_t index, uint64_t *value);

#ifdef __cplusplus
}
#endif

#endif /* FRAM_MB85_H */

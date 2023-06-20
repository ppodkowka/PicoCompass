#include <stdio.h>
#include <hardware/i2c.h>
#include "pico/stdlib.h"
#include "math.h"
#include "time.h"

int reg_write(i2c_inst_t *i2c, const uint addr, const uint8_t reg, const uint8_t *buf, const uint8_t nbytes);
int reg_read(i2c_inst_t *i2c,const uint addr, const uint8_t reg, uint8_t *buf, const uint8_t nbytes);

void hmc_init();
int twobyte_twocompliment_tosigned(uint8_t x_lsb, uint8_t x_msb);
void calibration(int* data_min_max);
int hmc_read_angle(int* data_min_max);
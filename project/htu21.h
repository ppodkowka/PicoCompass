#include <stdio.h>
#include <hardware/i2c.h>
#include "pico/stdlib.h"
#include "math.h"

double htu21_read_temp();
double htu21_read_hum();
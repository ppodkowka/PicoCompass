#include "htu21.h"

double htu21_read_temp(){

    uint8_t temp_raw[2];
    int temp_raw_int;
    double temperature = 0;
    const uint8_t data1 = 0xF3;

    // temperature
    i2c_write_blocking(i2c0, 0x40, &data1, 1, false);
    sleep_ms(100); // measurement delay
    i2c_read_blocking(i2c0, 0x40, temp_raw, 2, false);
    temp_raw_int = temp_raw[0] * 256 + temp_raw[1];
    temperature = ((175.72 * temp_raw_int)/65536)-48.85;
    
    return temperature;
}

double htu21_read_hum(){

    uint8_t hum_raw[2];
    int hum_raw_int;
    double humidity = 0;
    const uint8_t data2 = 0xF5;

    // humidity
    i2c_write_blocking(i2c0, 0x40, &data2, 1, false);
    sleep_ms(100); // measurement delay
    i2c_read_blocking(i2c0, 0x40, hum_raw, 2, false);
    hum_raw_int = hum_raw[0] * 256 + hum_raw[1];
    humidity = ((175.72 * hum_raw_int)/65536)-48.85;
    
    return humidity;
}
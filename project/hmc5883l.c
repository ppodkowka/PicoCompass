#include "hmc5883l.h"

#define PI 3.14159


int reg_write(i2c_inst_t *i2c, const uint addr, const uint8_t reg, const uint8_t *buf, const uint8_t nbytes) {
    // Write 1 byte to the specified register

    int num_bytes_read = 0;
    uint8_t msg[nbytes + 1];

    // Check to make sure caller is sending 1 or more bytes
    if (nbytes < 1) {
        return 0;
    }

    // Append register address to front of data packet
    msg[0] = reg;
    for (int i = 0; i < nbytes; i++) {
        msg[i + 1] = buf[i];
    }

    // Write data to register(s) over I2C
    i2c_write_blocking(i2c, addr, msg, (nbytes + 1), false);

    return num_bytes_read;
}

int reg_read(i2c_inst_t *i2c,const uint addr, const uint8_t reg, uint8_t *buf, const uint8_t nbytes) {
    // Read byte(s) from specified register. If nbytes > 1, read from consecutive
    // registers.
    int num_bytes_read = 0;

    // Check to make sure caller is asking for 1 or more bytes
    if (nbytes < 1) {
        return 0;
    }

    // Read data from register(s) over I2C
    i2c_write_blocking(i2c, addr, &reg, 1, true);
    num_bytes_read = i2c_read_blocking(i2c, addr, buf, nbytes, false);

    char str[50];
    sprintf(str, "Number of bytes read: %d\n", num_bytes_read);
    printf(str);

    return num_bytes_read;
}

void hmc_init(){
    const uint8_t data1 = 0x01;
    const uint8_t data2 = 0b00011101;

    // Define Set/Reset period to recommended value
    reg_write(i2c0, 13, 0xB, &data1, 1);

    // Configuration registers
    // Define OSR = 512, Full Scale Range = 8 Gauss, ODR = 200Hz, set continuous measurement mode
    reg_write(i2c0, 13, 0x9, &data2, 1);
}

int twobyte_twocompliment_tosigned(uint8_t x_lsb, uint8_t x_msb){
    // 2's compliment to signed integer conversion
    // requires separately LSB and MSB byte of measurement for each axis
    int result = 0;

    if((x_msb>>7) == 0){
        result = x_msb*256 + x_lsb;   
    }
    else if((x_msb>>7) == 1){
        result = (-128 + (x_msb & 0b01111111))*256 + x_lsb;
    }

    return result;
}

void calibration(int* data_min_max){
    uint8_t data_bytes[6];
    int x_signed = 0;
    int y_signed = 0;
    int z_signed = 0;

    for(int i=0; i<2000; i++){

        reg_read(i2c0, 13, 0x00, data_bytes, 6);
        sleep_ms(5);

        x_signed = twobyte_twocompliment_tosigned(data_bytes[0], data_bytes[1]);
        y_signed = twobyte_twocompliment_tosigned(data_bytes[2], data_bytes[3]);
        z_signed = twobyte_twocompliment_tosigned(data_bytes[4], data_bytes[5]);
        
        if(x_signed < data_min_max[0]){
           data_min_max[0] = x_signed; 
        }
        if(x_signed > data_min_max[1]){
           data_min_max[1] = x_signed; 
        }
        if(y_signed < data_min_max[2]){
           data_min_max[2] = y_signed; 
        }
        if(y_signed > data_min_max[3]){
           data_min_max[3] = y_signed; 
        }
        if(z_signed < data_min_max[4]){
           data_min_max[4] = z_signed; 
        }
        if(z_signed > data_min_max[5]){
           data_min_max[5] = z_signed; 
        }

        char str[50];
        sprintf(str, "CALIBRATION - X: %d\n", x_signed);
        printf(str);
        sprintf(str, "CALIBRATION - Y: %d\n", y_signed);
        printf(str);
        sprintf(str, "CALIBRATION - Z: %d\n", z_signed);
        printf(str);
    }
}

int hmc_read_angle(int* data_min_max){
    uint8_t data_bytes[6];
    int x_signed = 0;
    int y_signed = 0;
    int z_signed = 0;
    double heading;
    double normalized_x_y_z[3];
    
    // Read measurement data from sensor's registers
    reg_read(i2c0, 13, 0x00, data_bytes, 6); // read six bytes
    sleep_ms(5);
  
    x_signed = twobyte_twocompliment_tosigned(data_bytes[0], data_bytes[1]);
    y_signed = twobyte_twocompliment_tosigned(data_bytes[2], data_bytes[3]);
    z_signed = twobyte_twocompliment_tosigned(data_bytes[4], data_bytes[5]);

    // normalization values within -1 and 1 (with usage of min and max values measured during calibration)
    // normalization_minus_one_to_one(x_signed, y_signed, z_signed, data_min_max, normalized_x_y_z);
    // normalization of x, y, z values:
    // -1 is now min
    // 1 is now max
    // x or y or z is normalized in (-1, 1) range
    normalized_x_y_z[0] = (((double)x_signed - (double)data_min_max[0])/((double)data_min_max[1] - (double)data_min_max[0]))*2 - 1;
    normalized_x_y_z[1] = (((double)y_signed - (double)data_min_max[2])/((double)data_min_max[3] - (double)data_min_max[2]))*2 - 1;
    normalized_x_y_z[2] = (((double)z_signed - (double)data_min_max[4])/((double)data_min_max[5] - (double)data_min_max[4]))*2 - 1;
    
    char str[70];
    sprintf(str, "ODCZYT ZNORMALIZOWANY - X: %0.5f\n", normalized_x_y_z[0]);
    printf(str);
    sprintf(str, "ODCZYT ZNORMALIZOWANY - Y: %0.5f\n", normalized_x_y_z[1]);
    printf(str);
    sprintf(str, "ODCZYT ZNORMALIZOWANY - Z: %0.5f\n", normalized_x_y_z[2]);
    printf(str);
    
    heading = atan2(normalized_x_y_z[1], normalized_x_y_z[0]);
        
    // check for sign
    if(heading < 0.0) heading = heading + 2.0 * PI;
        
    // convert into angle
    int heading_angle = (int)(heading * 180.0 / PI);

    if(heading_angle == 360) heading_angle = 0;

    return heading_angle;
    // When X vector is pointing North - heading angle is 0 degrees
    // When X vector is poining East - heading angle is 90 degrees
    // South - 180 degrees
    // West - 270 degrees
}










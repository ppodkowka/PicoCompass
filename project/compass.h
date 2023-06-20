#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "Debug.h"
#include <stdlib.h> // malloc() free()
#include "LCD_1in28.h"
#include <stdio.h>
#include "hardware/gpio.h"
#include "math.h"
#include "time.h"
#include "htu21.h"
#include "hmc5883l.h"

#define PI 3.14159
#define BUTTON_PIN 17
#define DECLINATION_ANGLE 6 // In degrees, +6 is for Cracow [magnetic-declination.com]
enum state{compass, temp_and_hum};

void moving_line(int angle);
void paint_compass_scale(void);
void paint_data(int angle);
void paint_temp_and_hum();
void I2C_init();
void gpio_callback(uint gpio, uint32_t events);
int run_program(void);
#include "compass.h"

enum state device_state;


void moving_line(int angle){
    int x_start=120, y_start=120;
    double x_stop_double, y_stop_double;

    // angle 0 is when direction is N
    int x0_stop = 0;
    int y0_stop = 120;

    x_stop_double = x0_stop * cos(-angle*3.14/180) - y0_stop * sin(-angle*3.14/180);
    y_stop_double = x0_stop * sin(-angle*3.14/180) + y0_stop * cos(-angle*3.14/180);

    x_stop_double+=120;
    y_stop_double-=120;
    y_stop_double*=-1;

    Paint_DrawLine(x_start, y_start, (int)x_stop_double, (int)y_stop_double, RED, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
}

void paint_compass_scale(void){
    Paint_DrawString_EN(112, 0, "N", &Font24, WHITE, DARK_GREY_1);
    Paint_DrawString_EN(112, 220, "S", &Font24, WHITE, DARK_GREY_1);
    Paint_DrawString_EN(1, 108, "W", &Font24, WHITE, DARK_GREY_1);
    Paint_DrawString_EN(223, 108, "E", &Font24, WHITE, DARK_GREY_1);
} 

void paint_data(int angle){
    Paint_DrawCircle(120, 120, 60, DARK_GREY_2, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    if(angle <10){
        Paint_DrawNum(110, 90, angle, &Font24, 0, WHITE, DARK_GREY_2);
    }
    else if(angle >=10 && angle < 100){
        Paint_DrawNum(105, 90, angle, &Font24, 0, WHITE, DARK_GREY_2);
    }
    else Paint_DrawNum(95, 90, angle, &Font24, 0, WHITE, DARK_GREY_2);


    if((angle >=337 && angle <=360)||(angle >=0 && angle <=23)){
        Paint_DrawString_EN(110, 125, "N", &Font24, WHITE, DARK_GREY_2);
    }
    else if(angle >=24 && angle <=66){
        Paint_DrawString_EN(105, 125, "NE", &Font24, WHITE, DARK_GREY_2);
    }
    else if(angle >=67 && angle <=113){
        Paint_DrawString_EN(110, 125, "E", &Font24, WHITE, DARK_GREY_2);
    }
    else if(angle >=114 && angle <=156){
        Paint_DrawString_EN(105, 125, "SE", &Font24, WHITE, DARK_GREY_2);
    }
    else if(angle >=157 && angle <=203){
        Paint_DrawString_EN(110, 125, "S", &Font24, WHITE, DARK_GREY_2);
    }
    else if(angle >=204 && angle <=247){
        Paint_DrawString_EN(105, 125, "SW", &Font24, WHITE, DARK_GREY_2);
    }
    else if(angle >=248 && angle <=293){
        Paint_DrawString_EN(110, 125, "W", &Font24, WHITE, DARK_GREY_2);
    }
    else if(angle >=294 && angle <=336){
        Paint_DrawString_EN(105, 125, "NW", &Font24, WHITE, DARK_GREY_2);
    }
}

void paint_temp_and_hum(){
    static double temperature = 0;
    static double humidity = 0;
    static unsigned long last_time = 0;

    if((to_ms_since_boot(get_absolute_time())-last_time)>1000) { //refresh only every 1 second (because of long measurment time)
        last_time = to_ms_since_boot(get_absolute_time());
        temperature = htu21_read_temp();
        humidity = htu21_read_hum();
    }

    Paint_DrawCircle(120, 120, 60, DARK_GREY_2, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawString_EN(65, 105, "TEMP = ", &Font16, WHITE, DARK_GREY_2);
    Paint_DrawNum(110, 105, temperature, &Font16, 2, WHITE, DARK_GREY_2);
    Paint_DrawString_EN(70, 130, "HUM = ", &Font16, WHITE, DARK_GREY_2);
    Paint_DrawNum(110, 130, humidity, &Font16, 2, WHITE, DARK_GREY_2);
}

void I2C_init(){
    i2c_init(i2c0, 1000*100); // i2c0 bus at 100kHz
    gpio_set_function(0, GPIO_FUNC_I2C); // SDA
    gpio_set_function(1, GPIO_FUNC_I2C); // SCL
    gpio_pull_up(0);
    gpio_pull_up(1);
}

void gpio_callback(uint gpio, uint32_t events) {
    // IRQ Handler
    static unsigned long last_irq_time = 0;
    if((to_ms_since_boot(get_absolute_time())-last_irq_time)>200) {
        last_irq_time = to_ms_since_boot(get_absolute_time()); // debouncing 200ms
        
        if(gpio_get(BUTTON_PIN) == 0){ // switch state
            if(device_state == compass){
                device_state = temp_and_hum;
            }
            else device_state = compass;
        }
    }
}

// ------------ MAIN PROGRAM --------------------------- 

int run_program(void){
    // important decalarations for hmc5883l
    uint8_t data_bytes[6]; // Reserve some memory for the raw xyz measurements
    int data_min_max[] = {32767, -32768, 32767, -32768, 32767, -32768};
    int normalized_x_y_z[3];

    if (DEV_Module_Init() != 0){
        return -1;
    }

    // GPIO Init
    stdio_init_all();
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);  // IRQ when pressing button
    gpio_pull_up(BUTTON_PIN);

    // LCD Init
    LCD_1IN28_Init(HORIZONTAL);
    LCD_1IN28_Clear(WHITE);
    DEV_SET_PWM(60);
    UDOUBLE Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
    UWORD *BlackImage;
    if ((BlackImage = (UWORD *)malloc(Imagesize)) == NULL)
    {
        printf("Failed to apply for black memory...\r\n");
        exit(0);
    }
    
    // Create a new image cache named IMAGE_RGB and fill it with white
    Paint_NewImage((UBYTE *)BlackImage, LCD_1IN28.WIDTH, LCD_1IN28.HEIGHT, 0, WHITE);
    Paint_SetScale(65);
    Paint_Clear(WHITE);
    Paint_SetRotate(ROTATE_0);
    Paint_Clear(DARK_GREY_1);

    device_state = compass; // default state for state machine

    I2C_init();
    hmc_init();

    //"Starting calibration"
    //"Rotate the sensor in all directions, especially on HORIZONTAL PLANE"
    //"Stay away from things generating magnetic field!"
    Paint_DrawString_EN(60, 115, "Calibration", &Font16, WHITE, DARK_GREY_1);
    LCD_1IN28_Display(BlackImage);
    calibration(data_min_max);

    int angle = 0;
    char str[50];

    for(;;){
        
        angle = hmc_read_angle(data_min_max);

        // UART for debugging
        sprintf(str, "READ ANGLE: %d\n", angle);
        printf(str);

        paint_compass_scale();
        angle = angle + 90 - DECLINATION_ANGLE;
        if(angle > 359) angle = angle -359;
        moving_line(angle); 

        //state machine
        if(device_state == compass){
            paint_data(angle);
        }
        else if(device_state == temp_and_hum){
            paint_temp_and_hum();
        }

        LCD_1IN28_Display(BlackImage);
        DEV_Delay_ms(1);
        Paint_Clear(DARK_GREY_1);
    }
    

    // Refresh the picture in RAM to LCD*
    LCD_1IN28_Display(BlackImage);
    DEV_Delay_ms(1000);

    // Module Exit
    free(BlackImage);
    BlackImage = NULL;

    DEV_Module_Exit();
    return 0; 
}

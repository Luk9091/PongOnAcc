#include <string.h>

#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <pico/multicore.h>


#include "tcp_client.h"
#include "i2c.h"
#include "mpu6050.h"



int main(){
    stdio_init_all();
    
    I2C_Init();
    mpu6050_Init();


    queue_init(&sender_queue, sizeof(tcp_data_t), 10);
    multicore_launch_core1(run_tcp_client);

    gpio_init(16);
    gpio_set_dir(16, GPIO_OUT);
    tcp_data_t data;
    axisf_t acc;
    while(1){
        gpio_put(16, !gpio_get(16));
        
        mpu6050_readAcc(&acc);
        sprintf(data.data, "%.3f %.3f %.3f", acc.x, acc.y, acc.z);
        data.len = strlen(data.data);
        queue_add_blocking(&sender_queue, &data);
        sleep_ms(5);
    }
    
    return 0;
}
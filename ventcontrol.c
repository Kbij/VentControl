
#include "ventcontrol.h"
#include "types.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void ventcontrol_task(void *params)
{
    SendReceiveQueues queues;
    queues =  *(SendReceiveQueues*)params;
    printf("ventcontrol_task, receive queue: %p, send queue: %p\n", queues.receive_queue, queues.send_queue);
    bool on = true;

    while (true) {
        int value;
        if (xQueueReceive(queues.receive_queue, (void *)&value, 0) == pdTRUE) {
            cyw43_arch_gpio_put(0, on);
            on = !on;
            vTaskDelay(200);
        }
    }
    // bool on = false;
    // printf("blink_task starts\n");
    //     cyw43_arch_gpio_put(0, on);
    //     on = !on;
    //     vTaskDelay(200);
    // }    
}

#include "ventcontrol.h"
#include "connections.h"
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
    int count = 0;

    while (true) {
        int value;
        if (xQueueReceive(queues.receive_queue, (void *)&value, 0) == pdTRUE) {
            count++;
            cyw43_arch_gpio_put(0, on);
            on = !on;
            vTaskDelay(200);
            printf("ventcontrol received, sending count: %d\n", count);
            send_queue_message(count);
            //xQueueSend(queues.send_queue, (void *)&count, 10);
        }
    }
    // bool on = false;
    // printf("blink_task starts\n");
    //     cyw43_arch_gpio_put(0, on);
    //     on = !on;
    //     vTaskDelay(200);
    // }    
}
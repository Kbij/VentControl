#include "blink.h"
#include "types.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void blink_task(void *params)
{
    SendReceiveQueues queues;
    queues =  *(SendReceiveQueues*)params;

    while(true)
    {
        int value;
        if (xQueueReceive(queues.blink_queue, (void *)&value, (TickType_t) 1000) == pdTRUE) {
            cyw43_arch_gpio_put(0, true);
            vTaskDelay(value);
            cyw43_arch_gpio_put(0, false);
        }
    }
}
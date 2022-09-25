
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
    int current_speed = 1;

    while (true) {
        message_t message;
        if (xQueueReceive(queues.receive_queue, (void *)&message, 0) == pdTRUE) {
            printf("Message received from client: %d, type: %d\n", message.client, message.message_type);

            vTaskDelay(200);

            if (message.message_type == MSG_SET_SPEED)
            {
                current_speed = message.value;
            }
            
            message_t reply_message;
            reply_message.client = message.client;
            reply_message.message_type = MSG_CURRENT_SPEEED;
            reply_message.value = current_speed;

            send_queue_message(reply_message);
            vTaskDelay(200);
        }
    }
}
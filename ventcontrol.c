
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
    int current_speed = 1;

    while (true) {
        message_t message;
        if (xQueueReceive(queues.receive_queue, (void *)&message, (TickType_t) 1000) == pdTRUE) {
            printf("Message received from client: %d, type: %d\n", message.client, message.message_type);

            if (message.message_type == MSG_SET_SPEED)
            {
                current_speed = message.value;

                //Blink the led in a different task
                int blink_time = 200;
                xQueueSend(queues.blink_queue, (void *)&blink_time, 10);
            }

            message_t reply_message;
            reply_message.client = message.client;
            reply_message.message_type = MSG_CURRENT_SPEEED;
            reply_message.value = current_speed;

            send_queue_message(reply_message);
        }
    }
}
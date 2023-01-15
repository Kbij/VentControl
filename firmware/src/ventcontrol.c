#include "ventcontrol.h"

#include "types.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


#define REL_SPEED_2 15
#define REL_SPEED_3 14
#define REL_VAKANTIE 7


#define OFF 0
#define ON 1
#define RELAIS_SWITCH_DELAY_MS 500

void ventcontrol_task(void *params)
{
    server_data_t* server_data = (server_data_t*) params;
    printf("Ventcontrol task started.\n");

    int current_speed = 1;
    bool current_vakantie = false;
    gpio_init(REL_SPEED_2);
    gpio_set_dir(REL_SPEED_2, GPIO_OUT);
    gpio_init(REL_SPEED_3);
    gpio_set_dir(REL_SPEED_3, GPIO_OUT);
    gpio_init(REL_VAKANTIE);
    gpio_set_dir(REL_VAKANTIE, GPIO_OUT);

    gpio_put(REL_SPEED_2, OFF);
    gpio_put(REL_SPEED_3, OFF);
    gpio_put(REL_VAKANTIE, OFF);


    while (true) {
        message_t message;
        if (xQueueReceive(server_data->receive_queue, (void *)&message, (TickType_t) 500) == pdTRUE)
        {
         //   printf("PR, CL: %d, T: %d\n", message.client, message.message_type);

            if (message.message_type == MSG_SET_SPEED)
            {
                current_speed = message.value;
                printf("SP: %d\n", current_speed);
                if (current_speed == 1)
                {
                    gpio_put(REL_SPEED_2, OFF);
                    gpio_put(REL_SPEED_3, OFF);
                }

                if (current_speed == 2)
                {
                    gpio_put(REL_SPEED_3, OFF);
                    vTaskDelay(RELAIS_SWITCH_DELAY_MS/ portTICK_PERIOD_MS);
                    gpio_put(REL_SPEED_2, ON);
                }

                if (current_speed == 3)
                {
                    gpio_put(REL_SPEED_2, OFF);
                    vTaskDelay(RELAIS_SWITCH_DELAY_MS/ portTICK_PERIOD_MS);
                    gpio_put(REL_SPEED_3, ON);
                }

                message_t reply_message;
                reply_message.client = message.client;
                reply_message.message_type = MSG_CURRENT_SPEEED;
                reply_message.value = current_speed;

                xQueueSend(server_data->send_queue, (void *)&reply_message, 10);

                // //Blink the led in a different task
                // int blink_time = 200;
                // xQueueSend(queues.blink_queue, (void *)&blink_time, 10);
            }

            if (message.message_type == MSG_SET_VAKANTIE)
            {
                current_vakantie = message.value > 0 ? true : false;
                printf("VAK: %s\n", current_vakantie ? "true" : "false");

                gpio_put(REL_VAKANTIE, current_vakantie);

                message_t reply_message;
                reply_message.client = message.client;
                reply_message.message_type = MSG_CURRENT_VAKANTIE;
                reply_message.value = current_vakantie;

                xQueueSend(server_data->send_queue, (void *)&reply_message, 10);
            }

            if (message.message_type == MSG_GET_STATUS)
            {
                message_t reply_message;
                reply_message.client = message.client;
                reply_message.message_type = MSG_CURRENT_SPEEED;
                reply_message.value = current_speed;
                xQueueSend(server_data->send_queue, (void *)&reply_message, 10);

                reply_message.client = message.client;
                reply_message.message_type = MSG_CURRENT_VAKANTIE;
                reply_message.value = current_vakantie;
                xQueueSend(server_data->send_queue, (void *)&reply_message, 10);
            }
        }

        int input;
        if (xQueueReceive(server_data->input_queue, (void *)&input, (TickType_t) 500) == pdTRUE)
        {
        }
    }
}

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


#define OFF      0
#define ON       1
#define RELAIS_SWITCH_DELAY_MS 500
#define BOOST_SPEED 2
#define NORMAL_SPEED 1

void ventcontrol_task(void *params)
{
    server_data_t* server_data = (server_data_t*) params;
    printf("Ventcontrol task started.\n");

    gpio_init(REL_SPEED_2);
    gpio_set_dir(REL_SPEED_2, GPIO_OUT);
    gpio_init(REL_SPEED_3);
    gpio_set_dir(REL_SPEED_3, GPIO_OUT);
    gpio_init(REL_VAKANTIE);
    gpio_set_dir(REL_VAKANTIE, GPIO_OUT);

    gpio_put(REL_SPEED_2, OFF);
    gpio_put(REL_SPEED_3, OFF);
    gpio_put(REL_VAKANTIE, OFF);

    bool currentVakantie = false;
    int actualSpeed = 0;
    int requestedSpeed = 1;
    int boostSpeed = 1;
    while (true) {
        message_t message;
        bool boost;
        if (xQueueReceive(server_data->receive_queue, (void *)&message, (TickType_t) 500) == pdTRUE)
        {
         //   printf("PR, CL: %d, T: %d\n", message.client, message.message_type);

            if (message.message_type == MSG_SET_SPEED)
            {
                if (requestedSpeed != message.value)
                {
                    boost = false;
                    requestedSpeed = message.value;
                    //Reset the boostSpeed also
                    boostSpeed = requestedSpeed;
                    printf("SP: %d\n", requestedSpeed);
                }
                // //Blink the led in a different task
                // int blink_time = 200;
                // xQueueSend(queues.blink_queue, (void *)&blink_time, 10);
            }

            if (message.message_type == MSG_SET_VAKANTIE)
            {
                currentVakantie = message.value > 0 ? true : false;
                printf("VAK: %s\n", currentVakantie ? "true" : "false");

                gpio_put(REL_VAKANTIE, currentVakantie);

                message_t reply_message;
                reply_message.client = message.client;
                reply_message.message_type = MSG_CURRENT_VAKANTIE;
                reply_message.value = currentVakantie;

                xQueueSend(server_data->send_queue, (void *)&reply_message, 10);
            }

            if (message.message_type == MSG_GET_STATUS)
            {
                printf("get status, SP: %d, VK: %s\n", actualSpeed, currentVakantie ? "true" : "false");
                message_t reply_message;
                reply_message.client = message.client;
                reply_message.message_type = MSG_CURRENT_SPEEED;
                reply_message.value = actualSpeed;
                xQueueSend(server_data->send_queue, (void *)&reply_message, 10);

                reply_message.client = message.client;
                reply_message.message_type = MSG_CURRENT_VAKANTIE;
                reply_message.value = currentVakantie;
                xQueueSend(server_data->send_queue, (void *)&reply_message, 10);
            }
        }

        if (xQueueReceive(server_data->input_queue, (void *)&boost, (TickType_t) 500) == pdTRUE)
        {
            boostSpeed = boost ? BOOST_SPEED : NORMAL_SPEED;
        }

        int wantedSpeed = boostSpeed > requestedSpeed ? boostSpeed : requestedSpeed;
        if (wantedSpeed != actualSpeed)
        {
                if (wantedSpeed == 1)
                {
                    gpio_put(REL_SPEED_2, OFF);
                    gpio_put(REL_SPEED_3, OFF);
                }

                if (wantedSpeed == 2)
                {
                    gpio_put(REL_SPEED_3, OFF);
                    vTaskDelay(RELAIS_SWITCH_DELAY_MS/ portTICK_PERIOD_MS);
                    gpio_put(REL_SPEED_2, ON);
                }

                if (wantedSpeed == 3)
                {
                    gpio_put(REL_SPEED_2, OFF);
                    vTaskDelay(RELAIS_SWITCH_DELAY_MS/ portTICK_PERIOD_MS);
                    gpio_put(REL_SPEED_3, ON);
                }

                actualSpeed = wantedSpeed;
                message_t reply_message;
                reply_message.client = message.client;
                reply_message.message_type = MSG_CURRENT_SPEEED;
                reply_message.value = actualSpeed;
                xQueueSend(server_data->send_queue, (void *)&reply_message, 10);
        }
    }
}

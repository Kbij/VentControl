#include "input.h"
#include "types.h"
#include "pico/stdlib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define INPUT_READ_DELAY_MS 100
#define INPUT_1 3
#define INPUT_2 4
#define INPUT_3 5
#define INPUT_4 6
#define INPUT_DELAY_MS  (3 * 60 * 1000)
#define BOOST_DELAY_MS  (5 * 60 * 1000)

void handle_inputs(int input_gpio, int input_id, int* active_time_ms, bool* input_state);

void input_task(void* params)
{
    server_data_t* server_data = (server_data_t*) params;
    gpio_init(INPUT_1);
    gpio_set_dir(INPUT_1, GPIO_IN);
    gpio_init(INPUT_2);
    gpio_set_dir(INPUT_2, GPIO_IN);
    gpio_init(INPUT_3);
    gpio_set_dir(INPUT_3, GPIO_IN);
    gpio_init(INPUT_4);
    gpio_set_dir(INPUT_4, GPIO_IN);
    int  input_active[4] = {0,0,0,0};
    bool input_state[4] = {0,0,0,0};
    bool boost = false;
    int boost_delay = 0;

    while(true)
    {
        vTaskDelay(INPUT_READ_DELAY_MS / portTICK_PERIOD_MS);

        handle_inputs(INPUT_1, 0, &input_active[0], &input_state[0]);
        handle_inputs(INPUT_2, 1, &input_active[1], &input_state[1]);
        handle_inputs(INPUT_3, 2, &input_active[2], &input_state[2]);
        handle_inputs(INPUT_4, 3, &input_active[3], &input_state[3]);

        bool input_activated = false;
        for(int i = 0; i < 4; i++)
        {
            input_activated |= input_state[i];
        }

        if (input_activated && !boost)
        {
            printf("boost on\n");
            boost = true;
            boost_delay = 0;
            xQueueSend(server_data->input_queue, (void *)&boost, 10);
        }
        if (!input_activated && boost)
        {
            boost = false;
            boost_delay = BOOST_DELAY_MS;
        }
        if (!boost && boost_delay > 0)
        {
            boost_delay -= INPUT_READ_DELAY_MS;
            if (boost_delay <= 0)
            {
                printf("boost off\n");
                xQueueSend(server_data->input_queue, (void *)&boost, 10);
                boost_delay = 0;
            }
        }
    }
}

void handle_inputs(int input_gpio, int input_id, int* active_time_ms, bool* input_state)
{
    if (gpio_get(input_gpio))
    {
        (*active_time_ms) += INPUT_READ_DELAY_MS;
        if (((*active_time_ms) > INPUT_DELAY_MS) && !(*input_state))
        {
            printf("high: %d\n", input_id);
            (*input_state) = true;
        }
    }
    else
    {
        (*active_time_ms) = 0;
        if ((*input_state))
        {
            printf("low: %d\n", input_id);
            (*input_state) = false;
        }
    }
}

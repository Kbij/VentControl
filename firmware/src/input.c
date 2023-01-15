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
#define INPUT_DELAY_MS  5 * 60 * 1000

void handle_inputs(QueueHandle_t input_queue, int input_gpio, int input_id, int* active_time_ms, bool* input_state);

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

    while(true)
    {
        vTaskDelay(INPUT_READ_DELAY_MS / portTICK_PERIOD_MS);

        handle_inputs(server_data->input_queue, INPUT_1, 0, &input_active[0], &input_state[0]);
        handle_inputs(server_data->input_queue, INPUT_2, 0, &input_active[1], &input_state[1]);
        handle_inputs(server_data->input_queue, INPUT_3, 0, &input_active[2], &input_state[2]);
        handle_inputs(server_data->input_queue, INPUT_4, 0, &input_active[3], &input_state[3]);
    }
}

void handle_inputs(QueueHandle_t input_queue, int input_gpio, int input_id, int* active_time_ms, bool* input_state)
{
    if (gpio_get(input_gpio))
    {
        (*active_time_ms) += INPUT_READ_DELAY_MS;
        if (((*active_time_ms) > INPUT_DELAY_MS) && !(*input_state))
        {
            (*input_state) = true;

            input_state_t input_state;
            input_state.input = 1;
            input_state.state = true;
            xQueueSend(input_queue, (void *)&input_state, 10);
        }
    }
    else
    {
        if (input_state[1])
        {
            input_state[1] = false;
            input_state_t input_state;
            input_state.input = 1;
            input_state.state = false;
            xQueueSend(input_queue, (void *)&input_state, 10);
        }
    }
}

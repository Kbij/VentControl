#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include "lwip/apps/lwiperf.h"

#include "FreeRTOS.h"
#include "task.h"

#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif

#define TEST_TASK_PRIORITY				( tskIDLE_PRIORITY + 2UL )
#define BLINK_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )


void blink_task(__unused void *params) {
    bool on = false;
    printf("blink_task starts\n");
    while (true) {
#if 0 && configNUM_CORES > 1
        static int last_core_id;
        if (portGET_CORE_ID() != last_core_id) {
            last_core_id = portGET_CORE_ID();
            printf("blinking now from core %d\n", last_core_id);
        }
#endif
        cyw43_arch_gpio_put(0, on);
        on = !on;
        vTaskDelay(200);
    }
}

void main_task(__unused void *params) {
    
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return;
    }
    cyw43_arch_enable_sta_mode();
    netif_set_hostname(netif_default, "ventcontrol");
    int previous_link_status = cyw43_tcpip_link_status(&cyw43_state, 0);

    printf("Connecting to WiFi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms("MyBeer", "jolien is een trees", CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect.\n");
       // exit(1);
    } else {
        printf("Connected.\n");
    }

    xTaskCreate(blink_task, "BlinkThread", configMINIMAL_STACK_SIZE, NULL, BLINK_TASK_PRIORITY, NULL);


    while(true) {
        int current_link_status = cyw43_tcpip_link_status(&cyw43_state, 0);
        switch(current_link_status)
        {
            case CYW43_LINK_DOWN:///< link is down
            {
                if (current_link_status != previous_link_status)
                {
                    printf("Link down.\n");
                }
                break;
            }
            case CYW43_LINK_JOIN:///< Connected to wifi
            {
                if (current_link_status != previous_link_status)
                {
                    printf("Link Joined.\n");
                }
                break;
            }
            case CYW43_LINK_NOIP:///< Connected to wifi, but no IP address
            {
                if (current_link_status != previous_link_status)
                {
                    printf("No ip.\n");
                }
                break;
            }
            case CYW43_LINK_UP:///< Connect to wifi with an IP address
            {
                if (current_link_status != previous_link_status)
                {
                    printf("Link up.\n");
                }
                break;
            }
            case CYW43_LINK_FAIL:///< Connection failed
            {
                if (current_link_status != previous_link_status)
                {
                    printf("Link fail.\n");
                }
                break;
            }
            case CYW43_LINK_NONET:///< No matching SSID found (could be out of range, or down)
            {
                if (current_link_status != previous_link_status)
                {
                    printf("No net.\n");
                }
                break;
            }
            case CYW43_LINK_BADAUTH:///< Authenticatation failure
            {
                if (current_link_status != previous_link_status)
                {
                    printf("Bad auth.\n");
                }
                break;
            }
        }
        previous_link_status = current_link_status;

        const TickType_t delay_1s = 1000 / portTICK_PERIOD_MS;
        vTaskDelay(delay_1s);
    }

    cyw43_arch_deinit();
}

void vLaunch( void) {
    TaskHandle_t task;
    xTaskCreate(main_task, "TestMainThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &task);

#if NO_SYS && configUSE_CORE_AFFINITY && configNUM_CORES > 1
    // we must bind the main task to one core (well at least while the init is called)
    // (note we only do this in NO_SYS mode, because cyw43_arch_freertos
    // takes care of it otherwise)
    vTaskCoreAffinitySet(task, 1);
#endif

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main( void )
{
    stdio_init_all();

    /* Configure the hardware ready to run the demo. */
    const char *rtos_name;
#if ( portSUPPORT_SMP == 1 )
    rtos_name = "FreeRTOS SMP";
#else
    rtos_name = "FreeRTOS";
#endif

#if ( portSUPPORT_SMP == 1 ) && ( configNUM_CORES == 2 )
    printf("Starting %s on both cores:\n", rtos_name);
    vLaunch();
#elif ( RUN_FREE_RTOS_ON_CORE == 1 )
    printf("Starting %s on core 1:\n", rtos_name);
    multicore_launch_core1(vLaunch);
    while (true);
#else
    printf("Starting %s on core 0:\n", rtos_name);
    vLaunch();
#endif
    return 0;
}
#include "ventcontrol.h"
#include "server.h"
#include "types.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include "lwip/apps/lwiperf.h"
#include <lwip/sockets.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define TEST_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )
#define VENTCONTROL_TASK_PRIORITY		( tskIDLE_PRIORITY + 1UL )


void main_task(void *params) {
    SendReceiveQueues queues;
    queues =  *(SendReceiveQueues*)params;

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
    } else {
        printf("Connected.\n");
    }

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
                    TaskHandle_t server_task_handle;
                    xTaskCreate(server_task, "Server", configMINIMAL_STACK_SIZE, &queues, TEST_TASK_PRIORITY, &server_task_handle);

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

int main( void )
{
    stdio_init_all();
    SendReceiveQueues queues;
    queues.receive_queue = xQueueCreate(MAX_QUEUE_LENGTH, sizeof(int));
    queues.send_queue = xQueueCreate(MAX_QUEUE_LENGTH, sizeof(int));

    TaskHandle_t main_task_handle;
    xTaskCreate(main_task, "MainThread", configMINIMAL_STACK_SIZE, &queues, TEST_TASK_PRIORITY, &main_task_handle);

    TaskHandle_t ventcontrol_task_handle;
    xTaskCreate(ventcontrol_task, "VentControlThread", configMINIMAL_STACK_SIZE, &queues, VENTCONTROL_TASK_PRIORITY, &ventcontrol_task_handle);

    vTaskStartScheduler();

    return 0;
}
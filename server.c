#include "server.h"
#include "connections.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include "lwip/apps/lwiperf.h"
#include <lwip/sockets.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "types.h"


#define CONNECTION_TASK_PRIORITY		( tskIDLE_PRIORITY + 1UL )

// enum { MAX_CONN = 64 };
// static connection_t connections[MAX_CONN];

const int kConnectionThreadCount = 3;
static xSemaphoreHandle s_ConnectionSemaphore;
connection_list_t connection_list;

//Socket example found here:
//https://visualgdb.com/tutorials/raspberry/pico_w/freertos/

static void send_message(int socket, char *msg)
{
    int len = strlen(msg);
    int done = 0;
    while (done < len)
    {
        int done_now = send(socket, msg + done, len - done, 0);
        if (done_now <= 0)
            return;

        done += done_now;
    }
}

static int handle_single_command(connection_t* connection)
{
    char buffer[128];
    int done = 0;
    send_message(connection->sock, "Enter command: ");
 
    while (done < sizeof(buffer))
    {
//  int32_t flags = MSG_PEEK | (nowait ? MSG_DONTWAIT : 0);
//   int32_t bytesRecv = lwip_recv(m_socket, buffer, maxLength, flags);        
        int done_now = recv(connection->sock, buffer + done, sizeof(buffer) - done, MSG_DONTWAIT);

        //printf("done_now: %d, errno: %d\n", done_now, errno);

        if (done_now <= 0 && errno != EAGAIN )
            return -1;

        char *end = 0;
        if (done_now > 0) {
            done += done_now;
            end = strnstr(buffer, "\r", done);

            if (xQueueSend(connection->connection_receive_queue, (void *)&done_now, 10) != pdTRUE) {

            }            
        }

        int value;
        if (xQueueReceive(connection->connection_send_queue, (void *)&value,  ( TickType_t ) 10) == pdTRUE) {
            printf("Ventcontrol receive: %d\n", value);
            char buffer[200];
            sprintf(buffer, "\nVentrol number: %d\n", value);
            send_message(connection->sock, buffer);
        }

        if (!end)
            continue;
        *end = 0;
 
        if (!strcmp(buffer, "on"))
        {
            cyw43_arch_gpio_put(0, true);
            send_message(connection->sock, "The LED is now on\r\n");
        }
        else if (!strcmp(buffer, "off"))
        {
            cyw43_arch_gpio_put(0, false);
            send_message(connection->sock, "The LED is now off\r\n");
        }
        else
        {
            send_message(connection->sock, "Unknown command\r\n");
        }
        break;
    }
 
    return 0;
}

static void do_handle_connection(void *params)
{
    connection_t* connection = ( connection_t* ) params;
    //int conn_sock = (int)arg;
    while (!handle_single_command(connection))
    {
    }

    remove_connection(connection);

    closesocket(connection->sock);
    xSemaphoreGive(s_ConnectionSemaphore);
    vTaskDelete(NULL);
}
 
static void handle_connection(connection_t* connection)
{
    TaskHandle_t task;
    xSemaphoreTake(s_ConnectionSemaphore, portMAX_DELAY);
    xTaskCreate(do_handle_connection, "Connection Thread", configMINIMAL_STACK_SIZE, (void *)connection, CONNECTION_TASK_PRIORITY, &task);
}

// static void do_send_task(void *params)
// {
//     QueueHandle_t send_queue;
//     send_queue = (QueueHandle_t) params;

//     while (true) {
//         int value;
//         if (xQueueReceive(send_queue, (void *)&value, 0) == pdTRUE) {
//             send_queue_message(value);
//         }
//     }    
// }

void server_task(void *params)
{
    SendReceiveQueues queues;
    queues =  *(SendReceiveQueues*)params;

    connection_list_t connection_list;
    init_connections(queues.receive_queue);
    s_ConnectionSemaphore = xSemaphoreCreateCounting(kConnectionThreadCount, kConnectionThreadCount);

    // TaskHandle_t send_task;
    // xTaskCreate(do_send_task, "Send Thread", configMINIMAL_STACK_SIZE, (void *)queues.send_queue, CONNECTION_TASK_PRIORITY, &send_task);

    int server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    struct sockaddr_in listen_addr =
        {
            .sin_len = sizeof(struct sockaddr_in),
            .sin_family = AF_INET,
            .sin_port = htons(1234),
            .sin_addr = 0,
        };
 
    if (server_sock < 0)
    {
        printf("Unable to create socket: error %d", errno);
        return;
    }
 
    if (bind(server_sock, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) < 0)
    {
        printf("Unable to bind socket: error %d\n", errno);
        return;
    }
 
    if (listen(server_sock, 1) < 0)
    {
        printf("Unable to listen on socket: error %d\n", errno);
        return;
    }
 
    printf("Starting server at %s on port %u\n", ip4addr_ntoa(netif_ip4_addr(netif_list)), ntohs(listen_addr.sin_port));

    while (true)
    {
        struct sockaddr_storage remote_addr;
        socklen_t len = sizeof(remote_addr);
        int conn_sock = accept(server_sock, (struct sockaddr *)&remote_addr, &len);
        if (conn_sock < 0)
        {
            printf("Unable to accept incoming connection: error %d\n", errno);
            return;
        }

        connection_t* connection = add_connection(conn_sock);
        if (connection == NULL)
        {
            printf("Unable to accept incoming connection, no more connections available\n");
            closesocket(conn_sock);
            return;
        }
        else
        {
            handle_connection(connection);
        }
    }
}
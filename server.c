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

static bool handle_single_command(connection_t* connection)
{
    bool client_alive = true;

    while (client_alive)
    {
        if (!receive_data(connection))
        {
            //Connection is closed
            return false;
        }

        message_t received_message;
        //Check for received TCP messages
        while(handle_receive_bufffer(connection, &received_message))
        {
            printf("Message received from tcp client: %d, message_type: %d\n", received_message.client, received_message.message_type);
            if (xQueueSend(connection->connection_receive_queue, (void *)&received_message, 10) != pdTRUE) {
                printf("Unable to put message on receive_queue");
            }
        }

        //Check for messages from the connection_send_queue
        while (xQueueReceive(connection->connection_send_queue, (void *)&received_message,  ( TickType_t ) 10) == pdTRUE)
        {
            if (received_message.message_type = MSG_CURRENT_SPEEED)
            {
                char buffer[10];
                sprintf(buffer, "S%d#", received_message.value);
                send_message(connection->sock, buffer);
            }
            if (received_message.message_type = MSG_REMAINING_TIME)
            {
                char buffer[10];
                sprintf(buffer, "T%d#", received_message.value);
                send_message(connection->sock, buffer);
            }
        }
    }

    return true;
}

static void do_handle_connection(void *params)
{
    connection_t* connection = ( connection_t* ) params;

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

void server_task(void *params)
{
    SendReceiveQueues queues;
    queues =  *(SendReceiveQueues*)params;

    connection_list_t connection_list;
    init_connections(queues.receive_queue);
    s_ConnectionSemaphore = xSemaphoreCreateCounting(kConnectionThreadCount, kConnectionThreadCount);

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
#include "connections.h"
#include "pico/stdlib.h"
#include <lwip/sockets.h>
#include "FreeRTOS.h"
#include "queue.h"

extern connection_list_t connection_list;

void init_connections(QueueHandle_t receive_queue)
{
    connection_list.connections_mutex = xSemaphoreCreateMutex();
    for(int i = 0; i < MAX_CONN; i++)
    {
		connection_t* conn = connection_list.connections + i;
        conn->sock = -1;
        conn->receiveBufferLength = 0;
        conn->connection_send_queue = xQueueCreate(MAX_QUEUE_LENGTH, sizeof(int));
        conn->connection_receive_queue = receive_queue;
    }
}

connection_t* add_connection(int socket)
{
    if( xSemaphoreTake(connection_list.connections_mutex, ( TickType_t ) 10 ) == pdTRUE )
    {    
        for(int i = 0; i < MAX_CONN; i++)
        {        
            connection_t* conn = connection_list.connections + i;
            if (conn->sock == -1)
            {
                conn->sock = socket;
                xSemaphoreGive(connection_list.connections_mutex);
                return conn;
            }

        }
    }
    xSemaphoreGive(connection_list.connections_mutex);
    return NULL;
}

void remove_connection(connection_t* connection)
{
    if( xSemaphoreTake(connection_list.connections_mutex, ( TickType_t ) 10 ) == pdTRUE )
    {    
        for(int i = 0; i < MAX_CONN; i++)
        {        
            connection_t* conn = connection_list.connections + i;
            if (conn == connection)
            {
                conn->sock = -1;
                xQueueReset(conn->connection_send_queue);
            }
        }
    }
    xSemaphoreGive(connection_list.connections_mutex);
}
// void init_connections(connection_t* connections, int count)
// {
//     for(int i = 0; i < count; i++)
//     {
// 		connection_t* conn = connections + i;
//         conn->sock = -1;
//         conn->receiveBufferLength = 0;
//         conn->sendBufferLength = 0;
//     }
// }

// bool add_connection(connection_t* connections, int count, int socket)
// {
//     for(int i = 0; i < count; i++)
//     {
//         connection_t* conn = connections + i;
//         if (conn->sock == -1)
//         {
//             conn->sock = socket;
//             printf("Socket accepted: %d\n", conn->sock);
//             return true;
//         }
//     }
//     return false;
// }

// void handle_connections(connection_t* connections, int count)
// {
//     for(int i = 0; i < count; i++)
//     {
//         connection_t* conn = connections + i;
//         if (conn->sock > 0)
//         {
//             int receiveLength = read(conn->sock, conn->receiveBuffer + conn->receiveBufferLength, BUFFER_SIZE - conn->receiveBufferLength);
//             if (receiveLength > 0)
//             {
//                 conn->receiveBufferLength + receiveLength;
//                 printf("Socket received: %d, bufffer size: %d\n", receiveLength, conn->receiveBufferLength);
//             }
//             else if (receiveLength == 0)
//             {
//                 printf("Socket closed: %d\n", conn->sock);
//                 closesocket(conn->sock);
//                 conn->sock = -1;
//             }
//         }
//     }
// }
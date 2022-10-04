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
        conn->receiveBufferPos = 0;
        conn->connection_send_queue = xQueueCreate(MAX_QUEUE_LENGTH, sizeof(message_t));
        conn->connection_receive_queue = receive_queue;
    }
}

connection_t* add_connection(int socket)
{
    connection_t* result = NULL;
    if( xSemaphoreTake(connection_list.connections_mutex, ( TickType_t ) 10 ) == pdTRUE )
    {
        for(int i = 0; i < MAX_CONN; i++)
        {
            connection_t* conn = connection_list.connections + i;
            if (conn->sock == -1)
            {
                conn->sock = socket;
                result = conn;
                break;
            }
        }
    }
    xSemaphoreGive(connection_list.connections_mutex);
    return result;
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

bool receive_data(connection_t* connection)
{
    int bytes_received = recv(connection->sock, connection->receiveBuffer + connection->receiveBufferPos, sizeof(connection->receiveBuffer) - connection->receiveBufferPos, MSG_DONTWAIT);

    if (bytes_received <= 0 && errno != EAGAIN )
    {
        return false;
    }

    if (bytes_received > 0)
    {
        connection->receiveBufferPos += bytes_received;
        printf("Receive buffer size: %d\n", connection->receiveBufferPos);
    }

    return true;
}

bool handle_receive_bufffer(connection_t* connection, message_t* message)
{
    char *end  = strnstr(connection->receiveBuffer , "#", connection->receiveBufferPos);
    if (end)
    {
        *end = 0;
        if (!strcmp(connection->receiveBuffer, "GET"))
        {
            message->message_type = MSG_GET_STATUS;
            message->client = connection->sock;
            message->value = 0;

            connection->receiveBufferPos = 0;
            return true;
        }
        if (!strcmp(connection->receiveBuffer, "SET0"))
        {
            message->message_type = MSG_SET_SPEED;
            message->client = connection->sock;
            message->value = 0;

            connection->receiveBufferPos = 0;
            return true;
        }
        if (!strcmp(connection->receiveBuffer, "SET1"))
        {
            message->message_type = MSG_SET_SPEED;
            message->client = connection->sock;
            message->value = 1;

            connection->receiveBufferPos = 0;
            return true;
        }
        if (!strcmp(connection->receiveBuffer, "SET2"))
        {
            message->message_type = MSG_SET_SPEED;
            message->client = connection->sock;
            message->value = 2;

            connection->receiveBufferPos = 0;
            return true;
        }
        if (!strcmp(connection->receiveBuffer, "SET3"))
        {
            message->message_type = MSG_SET_SPEED;
            message->client = connection->sock;
            message->value = 3;

            connection->receiveBufferPos = 0;
            return true;
        }
        if (!strcmp(connection->receiveBuffer, "HB"))
        {
            message->message_type = MSG_KEEPALIVE;
            message->client = connection->sock;
            message->value = 0;

            connection->receiveBufferPos = 0;
            return true;
        }

        //We received a end character, but did not recognise the type; reset the buffer
        connection->receiveBufferPos = 0;
    }

    return false;
}

void send_queue_message(message_t message)
{
    if( xSemaphoreTake(connection_list.connections_mutex, ( TickType_t ) 10 ) == pdTRUE )
    {
        for(int i = 0; i < MAX_CONN; i++)
        {
            connection_t* conn = connection_list.connections + i;

            if (conn->sock > -1 && (message.client == conn->sock || message.client == 0))
            {
                if (xQueueSend(conn->connection_send_queue, (void *)&message, 10) != pdTRUE) {
                }
            }
        }
    }
    xSemaphoreGive(connection_list.connections_mutex);
}

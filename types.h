#ifndef CB8DCCC7_0E38_4AB7_BA6B_19FAF3A72F28
#define CB8DCCC7_0E38_4AB7_BA6B_19FAF3A72F28
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#define MAX_CONN          3
#define MAX_QUEUE_LENGTH  5
#define BUFFER_SIZE       128

typedef struct SendReceiveQueues {
  QueueHandle_t receive_queue;
  QueueHandle_t send_queue;
} SendReceiveQueues;

typedef struct connection_t {
    int sock;
    uint8_t receiveBuffer[BUFFER_SIZE];
    uint8_t receiveBufferLength;
    QueueHandle_t connection_send_queue;
    QueueHandle_t connection_receive_queue;
} connection_t;

typedef struct connection_list_t {
  SemaphoreHandle_t  connections_mutex;
  connection_t connections[MAX_CONN];
} connection_list_t;

#endif /* CB8DCCC7_0E38_4AB7_BA6B_19FAF3A72F28 */

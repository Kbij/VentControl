#ifndef CB8DCCC7_0E38_4AB7_BA6B_19FAF3A72F28
#define CB8DCCC7_0E38_4AB7_BA6B_19FAF3A72F28
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#define MAX_CONN             5
#define MAX_QUEUE_LENGTH     10
#define BUFFER_SIZE          128

#define MSG_GET_STATUS       1
#define MSG_CURRENT_SPEEED   2
#define MSG_SET_SPEED        3
#define MSG_REMAINING_TIME   4


typedef struct SendReceiveQueues {
  QueueHandle_t receive_queue;
  QueueHandle_t send_queue;
} SendReceiveQueues;

typedef struct message_t {
  int client;
  int value;
  int message_type;
} message_t;

typedef struct connection_t {
    int sock;
    uint8_t receiveBuffer[BUFFER_SIZE];
    uint8_t receiveBufferPos;
    QueueHandle_t connection_send_queue;
    QueueHandle_t connection_receive_queue;
} connection_t;

typedef struct connection_list_t {
  SemaphoreHandle_t  connections_mutex;
  connection_t connections[MAX_CONN];
} connection_list_t;

#endif /* CB8DCCC7_0E38_4AB7_BA6B_19FAF3A72F28 */

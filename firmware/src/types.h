#ifndef B0B500A7_6A18_4F4B_9AF0_44F78775ED2E
#define B0B500A7_6A18_4F4B_9AF0_44F78775ED2E
#include <stdbool.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#define MAX_CONN             5
#define MAX_QUEUE_LENGTH     10
#define BUFFER_SIZE          128

#define NO_MESSAGE           0
#define MSG_GET_STATUS       1
#define MSG_CURRENT_SPEEED   2
#define MSG_CURRENT_VAKANTIE 3
#define MSG_SET_SPEED        4
#define MSG_SET_VAKANTIE     5
#define MSG_REMAINING_TIME   6
#define MSG_KEEPALIVE        7

typedef struct server_data_t
{
  SemaphoreHandle_t ip_assigned_sem;
  bool server_run;
  QueueHandle_t receive_queue;
  QueueHandle_t send_queue;
  QueueHandle_t blink_queue;
  QueueHandle_t input_queue;
} server_data_t;

typedef struct message_t {
  int client;
  int value;
  int message_type;
} message_t;

typedef struct input_state_t
{
  uint8_t input;
  bool state;
} input_state_t;

#endif /* B0B500A7_6A18_4F4B_9AF0_44F78775ED2E */

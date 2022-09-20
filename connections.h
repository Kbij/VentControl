#ifndef D823CC87_8B46_4CF7_A544_86D975643F77
#define D823CC87_8B46_4CF7_A544_86D975643F77
// #include <lwip/sockets.h>
// #include <stdbool.h>
// #include <stdint.h>
#include "types.h"


void init_connections(QueueHandle_t receive_queue);
connection_t* add_connection(int socket);
void remove_connection(connection_t* connection);
// void handle_connections(connection_t* connections, int count);

#endif /* D823CC87_8B46_4CF7_A544_86D975643F77 */

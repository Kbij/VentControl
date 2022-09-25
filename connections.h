#ifndef D823CC87_8B46_4CF7_A544_86D975643F77
#define D823CC87_8B46_4CF7_A544_86D975643F77
#include "types.h"

/// @brief Initialise all connections
/// @param receive_queue
void init_connections(QueueHandle_t receive_queue);

/// @brief Add a new connection
/// @param socket
/// @return
connection_t* add_connection(int socket);

/// @brief Remove a closed connection
/// @param connection
void remove_connection(connection_t* connection);

/// @brief Receive data in the supplied connection
/// @param connection
/// @return false if connection is closed
bool receive_data(connection_t* connection);

/// @brief Handle the receive buffer of the connection
/// @param connection
/// @param message
/// @return true if a message is received
bool handle_receive_bufffer(connection_t* connection, message_t* message);

/// @brief Send a message to the queue
/// @param message
void send_queue_message(message_t message);

#endif /* D823CC87_8B46_4CF7_A544_86D975643F77 */

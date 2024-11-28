/**
 * @file network.h
 * @brief Network interface monitoring functionality
 */

#ifndef NETWORK_H
#define NETWORK_H

#include <stddef.h>

#define MAX_INTERFACES 16
#define INTERFACE_NAME_MAX 32

/**
 * @brief Structure to hold statistics for a single network interface
 */
typedef struct {
    char interface[INTERFACE_NAME_MAX];  /**< Interface name (e.g., eth0, wlan0) */
    unsigned long bytes_received;        /**< Total bytes received */
    unsigned long bytes_sent;            /**< Total bytes sent */
    unsigned long packets_received;      /**< Total packets received */
    unsigned long packets_sent;          /**< Total packets sent */
    double receive_speed;               /**< Current receive speed in bytes/sec */
    double send_speed;                  /**< Current send speed in bytes/sec */
    unsigned long errors_in;            /**< Input errors */
    unsigned long errors_out;           /**< Output errors */
    unsigned long drops_in;             /**< Input packets dropped */
    unsigned long drops_out;            /**< Output packets dropped */
} NetworkInterfaceStats;

/**
 * @brief Structure to hold all network interface statistics
 */
typedef struct {
    NetworkInterfaceStats interfaces[MAX_INTERFACES];  /**< Array of interface statistics */
    int interface_count;                              /**< Number of active interfaces */
} NetworkStats;

/**
 * @brief Initialize network monitoring
 * @return 0 on success, -1 on failure
 */
int init_network_monitoring(void);

/**
 * @brief Update network statistics
 * @param stats Pointer to NetworkStats structure to update
 * @return 0 on success, -1 on failure
 */
int update_network_stats(NetworkStats *stats);

/**
 * @brief Clean up network monitoring resources
 */
void cleanup_network_monitoring(void);

#endif /* NETWORK_H */ 
/**
 * @file network.c
 * @brief Implementation of network monitoring functionality
 */

#include "network.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define PROC_NET_DEV "/proc/net/dev"
#define LINE_BUF_SIZE 512

// Structure to hold previous readings for speed calculation
typedef struct {
    unsigned long bytes_received;
    unsigned long bytes_sent;
    struct timespec timestamp;
} PreviousStats;

static PreviousStats previous_stats[MAX_INTERFACES];

/**
 * @brief Initialize network monitoring
 * @return 0 on success, -1 on failure
 */
int init_network_monitoring(void) {
    // Check if we can read network statistics
    FILE *fp = fopen(PROC_NET_DEV, "r");
    if (!fp) return -1;
    fclose(fp);
    
    // Initialize previous stats
    memset(previous_stats, 0, sizeof(previous_stats));
    return 0;
}

/**
 * @brief Parse a single line from /proc/net/dev
 * @param line The line to parse
 * @param stats Pointer to NetworkInterfaceStats to update
 * @return 0 on success, -1 on failure
 */
static int parse_interface_line(char *line, NetworkInterfaceStats *stats) {
    char *colon = strchr(line, ':');
    if (!colon) return -1;
    
    // Extract interface name
    *colon = '\0';
    char *if_name = line;
    while (*if_name == ' ') if_name++;  // Skip leading spaces
    strncpy(stats->interface, if_name, INTERFACE_NAME_MAX - 1);
    stats->interface[INTERFACE_NAME_MAX - 1] = '\0';
    
    // Parse statistics
    return sscanf(colon + 1,
        "%lu %lu %lu %lu %*u %*u %*u %*u "  // Receive
        "%lu %lu %lu %lu",                  // Transmit
        &stats->bytes_received,
        &stats->packets_received,
        &stats->errors_in,
        &stats->drops_in,
        &stats->bytes_sent,
        &stats->packets_sent,
        &stats->errors_out,
        &stats->drops_out) == 8 ? 0 : -1;
}

/**
 * @brief Calculate interface speeds
 * @param current Current interface statistics
 * @param prev Previous interface statistics
 */
static void calculate_speeds(NetworkInterfaceStats *current, PreviousStats *prev) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    
    double time_diff = (now.tv_sec - prev->timestamp.tv_sec) +
                      (now.tv_nsec - prev->timestamp.tv_nsec) / 1e9;
    
    if (time_diff > 0) {
        current->receive_speed = (current->bytes_received - prev->bytes_received) / time_diff;
        current->send_speed = (current->bytes_sent - prev->bytes_sent) / time_diff;
    }
    
    // Update previous stats
    prev->bytes_received = current->bytes_received;
    prev->bytes_sent = current->bytes_sent;
    prev->timestamp = now;
}

/**
 * @brief Update network statistics
 * @param stats Pointer to NetworkStats structure to update
 * @return 0 on success, -1 on failure
 */
int update_network_stats(NetworkStats *stats) {
    if (!stats) return -1;
    
    FILE *fp = fopen(PROC_NET_DEV, "r");
    if (!fp) return -1;
    
    char line[LINE_BUF_SIZE];
    int interface_count = 0;
    
    // Skip header lines
    fgets(line, sizeof(line), fp);
    fgets(line, sizeof(line), fp);
    
    // Read interface statistics
    while (fgets(line, sizeof(line), fp) && interface_count < MAX_INTERFACES) {
        if (parse_interface_line(line, &stats->interfaces[interface_count]) == 0) {
            calculate_speeds(&stats->interfaces[interface_count],
                           &previous_stats[interface_count]);
            interface_count++;
        }
    }
    
    stats->interface_count = interface_count;
    fclose(fp);
    return 0;
}

/**
 * @brief Clean up network monitoring resources
 */
void cleanup_network_monitoring(void) {
    // Nothing to clean up
} 
/**
 * @file system_monitor.h
 * @brief Main header file for the system monitor application
 * @author Your Name
 * @date 2024
 */

#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cpu.h"

/**
 * @brief Structure to hold system statistics
 */
typedef struct {
    CPUStats cpu;        /**< CPU statistics */
    double memory_usage;  /**< Memory usage percentage */
    long total_memory;    /**< Total system memory */
    long free_memory;     /**< Free system memory */
} SystemStats;

/**
 * @brief Initialize the ncurses interface
 * @return 0 on success, -1 on failure
 */
int init_display(void);

/**
 * @brief Clean up and close the ncurses interface
 */
void cleanup_display(void);

/**
 * @brief Update system statistics
 * @param stats Pointer to SystemStats structure to update
 * @return 0 on success, -1 on failure
 */
int update_stats(SystemStats *stats);

/**
 * @brief Display system statistics on the screen
 * @param stats Pointer to SystemStats structure containing current statistics
 */
void display_stats(const SystemStats *stats);

#endif /* SYSTEM_MONITOR_H */ 
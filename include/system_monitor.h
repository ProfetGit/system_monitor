/**
 * @file system_monitor.h
 * @brief Main header file for the system monitor application
 * 
 * This header defines the main structures and functions for the system monitoring
 * application. It includes functionality for monitoring CPU, memory, disk, and GPU
 * statistics, as well as the ncurses-based user interface.
 */

#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cpu.h"
#include "memory.h"
#include "disk.h"
#include "gpu.h"

/**
 * @brief Structure to hold system statistics
 * 
 * This structure serves as the central data container for all system metrics.
 * It aggregates statistics from various subsystems including CPU, memory,
 * disk, and GPU information.
 */
typedef struct {
    CPUStats cpu;        /**< CPU statistics including usage and frequency information */
    MemoryStats memory;  /**< Memory statistics including RAM and swap usage */
    DiskInfo disks;      /**< Disk statistics including space and I/O metrics */
    GPUInfo gpus;        /**< GPU statistics including temperature and memory usage */
} SystemStats;

/**
 * @brief Initialize the ncurses interface
 * @return 0 on successful initialization, -1 on failure
 * 
 * @details Sets up the ncurses window, configures colors if available,
 * and prepares the display layout. Must be called before any display operations.
 * 
 * @note The terminal must support ncurses operations
 */
int init_display(void);

/**
 * @brief Clean up and close the ncurses interface
 * 
 * @details Properly closes the ncurses window and restores terminal settings.
 * Should be called before program exit to ensure proper cleanup.
 */
void cleanup_display(void);

/**
 * @brief Update system statistics
 * @param stats Pointer to SystemStats structure to update
 * @return 0 on success, -1 on failure
 * 
 * @details Gathers fresh statistics from all monitored subsystems and updates
 * the provided SystemStats structure. This is the main function for collecting
 * system metrics.
 * 
 * @warning The stats parameter must not be NULL
 */
int update_stats(SystemStats *stats);

/**
 * @brief Display system statistics on the screen
 * @param stats Pointer to SystemStats structure containing current statistics
 * 
 * @details Renders the current system statistics in a formatted layout using
 * ncurses. Updates the entire display with fresh data from the stats structure.
 * 
 * @note init_display() must be called before using this function
 * @warning The stats parameter must not be NULL and contain valid data
 */
void display_stats(const SystemStats *stats);

#endif /* SYSTEM_MONITOR_H */ 
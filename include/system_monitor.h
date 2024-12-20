/**
 * @file system_monitor.h
 * @brief Main header file for the system monitor application
 * 
 * This header defines the main structures and functions for the system monitoring
 * application. It includes functionality for monitoring CPU, memory, disk, GPU,
 * and network statistics, as well as the ncurses-based user interface.
 */

#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

/**
 * @defgroup core Core System
 * @{
 * @brief Core system functionality and data structures
 *
 * The core system module provides the main functionality for coordinating
 * different monitoring subsystems and managing the user interface.
 */

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cpu.h"
#include "memory.h"
#include "disk.h"
#include "gpu.h"
#include "network.h"

/**
 * @brief Structure to hold system statistics
 * 
 * This structure serves as the central data container for all system metrics.
 * It aggregates statistics from various subsystems including CPU, memory,
 * disk, GPU, and network information.
 *
 * @see CPUStats
 * @see MemoryStats
 * @see DiskInfo
 * @see GPUInfo
 * @see NetworkStats
 */
typedef struct {
    CPUStats cpu;        /**< CPU statistics including usage and frequency information */
    MemoryStats memory;  /**< Memory statistics including RAM and swap usage */
    DiskInfo disks;      /**< Disk statistics including space and I/O metrics */
    GPUInfo gpus;        /**< GPU statistics including temperature and memory usage */
    NetworkStats network; /**< Network interface statistics */
} SystemStats;

/**
 * @brief Initialize the ncurses interface
 * @return 0 on successful initialization, -1 on failure
 * 
 * @details Sets up the ncurses window, configures colors if available,
 * and prepares the display layout. Must be called before any display operations.
 * 
 * @note The terminal must support ncurses operations
 * @see cleanup_display
 */
int init_display(void);

/**
 * @brief Clean up and close the ncurses interface
 * 
 * @details Properly closes the ncurses window and restores terminal settings.
 * Should be called before program exit to ensure proper cleanup.
 *
 * @see init_display
 */
void cleanup_display(void);

/**
 * @brief Update system statistics
 * @param[in,out] stats Pointer to SystemStats structure to update with fresh data
 * @return 0 on success, -1 on failure
 * 
 * @details Gathers fresh statistics from all monitored subsystems and updates
 * the provided SystemStats structure. This is the main function for collecting
 * system metrics.
 * 
 * @warning The stats parameter must not be NULL
 * @see SystemStats
 */
int update_stats(SystemStats *stats);

/**
 * @brief Display system statistics on the screen
 * @param[in] stats Pointer to SystemStats structure containing current statistics to display
 * 
 * @details Renders the current system statistics in a formatted layout using
 * ncurses. Updates the entire display with fresh data from the stats structure.
 * 
 * @note init_display() must be called before using this function
 * @warning The stats parameter must not be NULL and contain valid data
 * @see init_display
 * @see SystemStats
 */
void display_stats(const SystemStats *stats);

/** @} */ // end of core group

#endif /* SYSTEM_MONITOR_H */ 
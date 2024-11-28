/**
 * @file stats.c
 * @brief Implementation of system statistics gathering functions
 * 
 * This file contains the implementation of functions that gather and update
 * various system statistics including CPU, memory, disk, and GPU information.
 * It serves as the central point for collecting all system metrics.
 */

#include "system_monitor.h"
#include <stdio.h>

/**
 * @brief Updates all system statistics
 * @param stats Pointer to SystemStats structure to be updated
 * @return 0 on successful update of all statistics, -1 if any update fails
 * 
 * @details This function coordinates the update of all system statistics by calling
 * individual update functions for each subsystem (CPU, memory, disk, GPU).
 * If any of these updates fail, the function returns immediately with -1.
 * 
 * @note All statistics are updated atomically - either all succeed or none are updated
 * 
 * @warning The stats parameter must not be NULL
 */
int update_stats(SystemStats *stats) {
    if (!stats) return -1;

    // Update CPU statistics
    if (update_cpu_stats(&stats->cpu) != 0) return -1;

    // Update Memory statistics
    if (update_memory_stats(&stats->memory) != 0) return -1;

    // Update Disk statistics
    if (update_disk_stats(&stats->disks) != 0) return -1;

    // Update GPU statistics
    if (update_gpu_stats(&stats->gpus) != 0) return -1;

    return 0;
} 
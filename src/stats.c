/**
 * @file stats.c
 * @brief Implementation of system statistics gathering functions
 */

#include "system_monitor.h"
#include <stdio.h>

int update_stats(SystemStats *stats) {
    if (!stats) return -1;

    // Update CPU statistics
    if (update_cpu_stats(&stats->cpu) != 0) return -1;

    // Update Memory statistics
    if (update_memory_stats(&stats->memory) != 0) return -1;

    // Update Disk statistics
    if (update_disk_stats(&stats->disks) != 0) return -1;

    return 0;
} 
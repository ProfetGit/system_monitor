/**
 * @file stats.c
 * @brief Implementation of system statistics gathering functions
 */

#include "system_monitor.h"
#include <stdio.h>
#include <sys/sysinfo.h>

int update_stats(SystemStats *stats) {
    if (!stats) return -1;

    // Get memory information
    struct sysinfo si;
    if (sysinfo(&si) != 0) return -1;

    stats->total_memory = si.totalram * si.mem_unit;
    stats->free_memory = si.freeram * si.mem_unit;
    stats->memory_usage = 100.0 * (1.0 - ((double)stats->free_memory / stats->total_memory));

    // Update CPU statistics
    if (update_cpu_stats(&stats->cpu) != 0) return -1;

    return 0;
} 
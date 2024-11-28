/**
 * @file memory.c
 * @brief Implementation of memory monitoring functionality
 */

#include "memory.h"
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>

// Constants for memory size calculations
#define KB_TO_BYTES (1024UL)
#define MB_TO_BYTES (1024UL * 1024UL)
#define GB_TO_BYTES (1024UL * 1024UL * 1024UL)

/**
 * @brief Initialize memory monitoring
 * @return 0 on success, -1 on failure
 */
int init_memory_monitoring(void) {
    // Check if we can read memory information
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return -1;
    fclose(fp);
    return 0;
}

/**
 * @brief Clean up memory monitoring resources
 */
void cleanup_memory_monitoring(void) {
    // No cleanup needed for memory monitoring
}

/**
 * @brief Read memory information from /proc/meminfo
 * @param stats Pointer to MemoryStats structure to update
 * @return 0 on success, -1 on failure
 */
static int read_proc_meminfo(MemoryStats *stats) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return -1;

    char line[256];
    unsigned long memTotal = 0, memFree = 0, memAvailable = 0;
    unsigned long buffers = 0, cached = 0, swapTotal = 0, swapFree = 0;
    unsigned long shmem = 0, sreclaimable = 0;

    while (fgets(line, sizeof(line), fp)) {
        unsigned long value;
        if (sscanf(line, "MemTotal: %lu kB", &value) == 1)
            memTotal = value * KB_TO_BYTES;
        else if (sscanf(line, "MemFree: %lu kB", &value) == 1)
            memFree = value * KB_TO_BYTES;
        else if (sscanf(line, "MemAvailable: %lu kB", &value) == 1)
            memAvailable = value * KB_TO_BYTES;
        else if (sscanf(line, "Buffers: %lu kB", &value) == 1)
            buffers = value * KB_TO_BYTES;
        else if (sscanf(line, "Cached: %lu kB", &value) == 1)
            cached = value * KB_TO_BYTES;
        else if (sscanf(line, "SwapTotal: %lu kB", &value) == 1)
            swapTotal = value * KB_TO_BYTES;
        else if (sscanf(line, "SwapFree: %lu kB", &value) == 1)
            swapFree = value * KB_TO_BYTES;
        else if (sscanf(line, "Shmem: %lu kB", &value) == 1)
            shmem = value * KB_TO_BYTES;
        else if (sscanf(line, "SReclaimable: %lu kB", &value) == 1)
            sreclaimable = value * KB_TO_BYTES;
    }

    fclose(fp);

    if (memTotal == 0) return -1;  // Failed to read memory info

    // Calculate actual memory values
    stats->total = memTotal;
    stats->free = memFree;
    stats->available = memAvailable;
    stats->buffers = buffers;
    stats->cached = cached + sreclaimable - shmem;  // Actual cache
    stats->swap_total = swapTotal;
    stats->swap_free = swapFree;

    // Calculate used memory (excluding cache and buffers)
    unsigned long used = memTotal - memFree - buffers - (cached + sreclaimable - shmem);
    stats->used = used;

    // Calculate usage percentages
    stats->usage = 100.0 * ((double)used / memTotal);
    if (swapTotal > 0) {
        stats->swap_usage = 100.0 * (1.0 - ((double)swapFree / swapTotal));
    } else {
        stats->swap_usage = 0.0;
    }

    return 0;
}

int update_memory_stats(MemoryStats *stats) {
    if (!stats) return -1;
    return read_proc_meminfo(stats);
}

void cleanup_memory_monitor(void) {
    // Nothing to clean up
} 
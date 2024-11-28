/**
 * @file memory.h
 * @brief Memory monitoring functionality
 */

#ifndef MEMORY_H
#define MEMORY_H

/**
 * @brief Structure to hold memory statistics
 */
typedef struct {
    unsigned long total;      /**< Total physical memory in bytes */
    unsigned long free;       /**< Free physical memory in bytes */
    unsigned long available;  /**< Available memory in bytes */
    unsigned long cached;     /**< Cached memory in bytes */
    unsigned long swap_total; /**< Total swap space in bytes */
    unsigned long swap_free;  /**< Free swap space in bytes */
    double usage;            /**< Memory usage percentage */
    double swap_usage;       /**< Swap usage percentage */
} MemoryStats;

/**
 * @brief Initialize memory monitoring
 * @return 0 on success, -1 on failure
 */
int init_memory_monitor(void);

/**
 * @brief Update memory statistics
 * @param stats Pointer to MemoryStats structure to update
 * @return 0 on success, -1 on failure
 */
int update_memory_stats(MemoryStats *stats);

/**
 * @brief Clean up memory monitoring resources
 */
void cleanup_memory_monitor(void);

#endif /* MEMORY_H */ 
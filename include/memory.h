/**
 * @file memory.h
 * @brief Memory monitoring functionality
 * 
 * This header provides structures and functions for monitoring system memory usage,
 * including physical RAM and swap space. It tracks various memory metrics such as
 * total, free, and cached memory.
 */

#ifndef MEMORY_H
#define MEMORY_H

/**
 * @defgroup memory Memory Monitoring
 * @{
 * @brief Memory usage monitoring functionality
 *
 * The memory monitoring module provides functionality for tracking system memory usage,
 * including both physical RAM and swap space. It can monitor various memory metrics
 * such as total memory, free memory, cached memory, and swap usage.
 */

/**
 * @brief Structure to hold memory statistics
 * 
 * This structure contains comprehensive information about the system's memory usage,
 * including both physical RAM and swap space. All values are in bytes unless
 * otherwise specified.
 *
 * @see init_memory_monitoring
 * @see update_memory_stats
 */
typedef struct {
    unsigned long total;      /**< Total physical memory in bytes */
    unsigned long free;       /**< Free physical memory in bytes (immediately available) */
    unsigned long available;  /**< Available memory in bytes (including reclaimable) */
    unsigned long used;       /**< Used memory in bytes (excluding cache/buffers) */
    unsigned long buffers;    /**< Memory used by kernel buffers (can be reclaimed) */
    unsigned long cached;     /**< Memory used for cache (can be reclaimed if needed) */
    unsigned long swap_total; /**< Total swap space in bytes */
    unsigned long swap_free;  /**< Free swap space in bytes */
    double usage;            /**< Memory usage percentage (0-100, excluding cache/buffers) */
    double swap_usage;       /**< Swap usage percentage (0-100) */
} MemoryStats;

/**
 * @brief Initialize memory monitoring
 * @return 0 on successful initialization, -1 on failure
 * 
 * @details Sets up necessary resources for memory monitoring. Must be called
 * before using update_memory_stats().
 * 
 * @note This function is idempotent - calling it multiple times has no effect
 * @see cleanup_memory_monitoring
 * @see update_memory_stats
 */
int init_memory_monitoring(void);

/**
 * @brief Update memory statistics
 * @param stats Pointer to MemoryStats structure to update
 * @return 0 on success, -1 on failure
 * 
 * @details Reads current memory information from the system and updates all
 * fields in the provided MemoryStats structure. This includes calculating
 * derived values like usage percentages.
 * 
 * @note Requires prior call to init_memory_monitoring()
 * @warning The stats parameter must not be NULL
 * @see MemoryStats
 * @see init_memory_monitoring
 */
int update_memory_stats(MemoryStats *stats);

/**
 * @brief Clean up memory monitoring resources
 * 
 * @details Releases any resources allocated during memory monitoring.
 * Should be called before program exit for proper cleanup.
 * 
 * @note This function is safe to call even if init_memory_monitoring() was not called
 * @see init_memory_monitoring
 */
void cleanup_memory_monitoring(void);

/** @} */ // end of memory group

#endif /* MEMORY_H */ 
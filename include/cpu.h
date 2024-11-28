/**
 * @file cpu.h
 * @brief CPU monitoring functionality
 */

#ifndef CPU_H
#define CPU_H

/**
 * @brief Structure to hold CPU statistics
 */
typedef struct {
    double usage;         /**< CPU usage percentage */
    unsigned int cores;   /**< Number of CPU cores */
    char model_name[256]; /**< CPU model name */
} CPUStats;

/**
 * @brief Initialize CPU monitoring
 * @return 0 on success, -1 on failure
 */
int init_cpu_monitor(void);

/**
 * @brief Update CPU statistics
 * @param stats Pointer to CPUStats structure to update
 * @return 0 on success, -1 on failure
 */
int update_cpu_stats(CPUStats *stats);

/**
 * @brief Clean up CPU monitoring resources
 */
void cleanup_cpu_monitor(void);

#endif /* CPU_H */ 
/**
 * @file gpu.h
 * @brief GPU monitoring functionality
 */

#ifndef GPU_H
#define GPU_H

#define MAX_GPUS 4
#define MAX_GPU_NAME 128

/**
 * @brief Structure to hold GPU statistics
 */
typedef struct {
    char name[MAX_GPU_NAME];    /**< GPU model name */
    int temperature;            /**< GPU temperature in Celsius */
    double utilization;         /**< GPU utilization percentage */
    unsigned long memory_total; /**< Total GPU memory in bytes */
    unsigned long memory_used;  /**< Used GPU memory in bytes */
    unsigned long memory_free;  /**< Free GPU memory in bytes */
    int power_usage;           /**< Power usage in milliwatts */
    int fan_speed;             /**< Fan speed percentage */
    int supported;             /**< Whether this GPU is supported and accessible */
} GPUStats;

/**
 * @brief Structure to hold all GPU information
 */
typedef struct {
    GPUStats gpus[MAX_GPUS];   /**< Array of GPU statistics */
    unsigned int count;         /**< Number of GPUs found */
    int nvidia_available;       /**< Whether NVIDIA driver is available */
} GPUInfo;

/**
 * @brief Initialize GPU monitoring
 * @return 0 on success, -1 on failure
 */
int init_gpu_monitor(void);

/**
 * @brief Update GPU statistics
 * @param info Pointer to GPUInfo structure to update
 * @return 0 on success, -1 on failure
 */
int update_gpu_stats(GPUInfo *info);

/**
 * @brief Clean up GPU monitoring resources
 */
void cleanup_gpu_monitor(void);

#endif /* GPU_H */ 
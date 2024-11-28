/**
 * @file disk.h
 * @brief Disk monitoring functionality
 */

#ifndef DISK_H
#define DISK_H

#define MAX_DISKS 8
#define MAX_DISK_NAME 32
#define MAX_MOUNT_PATH 256

/**
 * @brief Structure to hold disk partition information
 */
typedef struct {
    char device[MAX_DISK_NAME];     /**< Device name (e.g., /dev/sda1) */
    char mount_point[MAX_MOUNT_PATH]; /**< Mount point path */
    unsigned long total;            /**< Total space in bytes */
    unsigned long free;             /**< Free space in bytes */
    unsigned long available;        /**< Available space in bytes */
    double usage;                   /**< Usage percentage */
    unsigned long reads;            /**< Number of reads since boot */
    unsigned long writes;           /**< Number of writes since boot */
    unsigned long io_in_progress;   /**< Number of I/O operations in progress */
} DiskStats;

/**
 * @brief Structure to hold all disk statistics
 */
typedef struct {
    DiskStats disks[MAX_DISKS];    /**< Array of disk statistics */
    int count;                      /**< Number of monitored disks */
} DiskInfo;

/**
 * @brief Initialize disk monitoring
 * @return 0 on success, -1 on failure
 */
int init_disk_monitor(void);

/**
 * @brief Update disk statistics
 * @param stats Pointer to DiskInfo structure to update
 * @return 0 on success, -1 on failure
 */
int update_disk_stats(DiskInfo *stats);

/**
 * @brief Clean up disk monitoring resources
 */
void cleanup_disk_monitor(void);

#endif /* DISK_H */ 
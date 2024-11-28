/**
 * @file disk.c
 * @brief Implementation of disk monitoring functionality
 */

#include "disk.h"
#include <stdio.h>
#include <string.h>
#include <sys/statvfs.h>
#include <mntent.h>
#include <unistd.h>
#include <ctype.h>

// Static variables for disk I/O statistics
static unsigned long prev_reads[MAX_DISKS] = {0};
static unsigned long prev_writes[MAX_DISKS] = {0};

/**
 * @brief Check if a device name corresponds to a real disk
 * @param device Device name to check
 * @return 1 if it's a real disk, 0 otherwise
 */
static int is_real_disk(const char *device) {
    // Skip loop, ram, and other virtual devices
    if (strstr(device, "loop") || strstr(device, "ram") ||
        strstr(device, "dm-") || strstr(device, "sr")) {
        return 0;
    }
    return 1;
}

/**
 * @brief Get disk I/O statistics from /proc/diskstats
 * @param device Device name
 * @param reads Pointer to store number of reads
 * @param writes Pointer to store number of writes
 * @param io_in_progress Pointer to store number of I/O operations in progress
 * @return 0 on success, -1 on failure
 */
static int get_disk_io_stats(const char *device, unsigned long *reads,
                            unsigned long *writes, unsigned long *io_in_progress) {
    FILE *fp = fopen("/proc/diskstats", "r");
    if (!fp) return -1;

    char line[256];
    char dev_name[64];
    unsigned long rd_ios, rd_merges, rd_sectors, rd_ticks;
    unsigned long wr_ios, wr_merges, wr_sectors, wr_ticks;
    unsigned long ios_pgr, tot_ticks, rq_ticks;

    // Extract the base device name (e.g., "sda" from "/dev/sda1")
    const char *base_dev = strrchr(device, '/');
    if (base_dev) {
        base_dev++;
    } else {
        base_dev = device;
    }
    
    // Remove partition numbers for matching
    char base_name[32];
    strncpy(base_name, base_dev, sizeof(base_name) - 1);
    base_name[sizeof(base_name) - 1] = '\0';
    char *p = base_name;
    while (*p) {
        if (isdigit(*p)) {
            *p = '\0';
            break;
        }
        p++;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%*d %*d %s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
                   dev_name,
                   &rd_ios, &rd_merges, &rd_sectors, &rd_ticks,
                   &wr_ios, &wr_merges, &wr_sectors, &wr_ticks,
                   &ios_pgr, &tot_ticks, &rq_ticks) == 12) {
            if (strcmp(dev_name, base_name) == 0) {
                *reads = rd_ios;
                *writes = wr_ios;
                *io_in_progress = ios_pgr;
                fclose(fp);
                return 0;
            }
        }
    }

    fclose(fp);
    return -1;
}

int init_disk_monitor(void) {
    memset(prev_reads, 0, sizeof(prev_reads));
    memset(prev_writes, 0, sizeof(prev_writes));
    return 0;
}

int update_disk_stats(DiskInfo *info) {
    if (!info) return -1;

    FILE *mtab = setmntent("/etc/mtab", "r");
    if (!mtab) return -1;

    struct mntent *ent;
    info->count = 0;

    while ((ent = getmntent(mtab)) && info->count < MAX_DISKS) {
        // Skip non-disk filesystems
        if (!is_real_disk(ent->mnt_fsname)) continue;

        DiskStats *disk = &info->disks[info->count];
        struct statvfs fs_stats;

        // Get filesystem statistics
        if (statvfs(ent->mnt_dir, &fs_stats) == 0) {
            strncpy(disk->device, ent->mnt_fsname, MAX_DISK_NAME - 1);
            disk->device[MAX_DISK_NAME - 1] = '\0';
            
            strncpy(disk->mount_point, ent->mnt_dir, MAX_MOUNT_PATH - 1);
            disk->mount_point[MAX_MOUNT_PATH - 1] = '\0';

            disk->total = fs_stats.f_blocks * fs_stats.f_frsize;
            disk->free = fs_stats.f_bfree * fs_stats.f_frsize;
            disk->available = fs_stats.f_bavail * fs_stats.f_frsize;
            
            if (disk->total > 0) {
                disk->usage = 100.0 * (1.0 - ((double)disk->available / disk->total));
            } else {
                disk->usage = 0.0;
            }

            // Get I/O statistics
            unsigned long reads, writes, io_in_progress;
            if (get_disk_io_stats(disk->device, &reads, &writes, &io_in_progress) == 0) {
                disk->reads = reads - prev_reads[info->count];
                disk->writes = writes - prev_writes[info->count];
                disk->io_in_progress = io_in_progress;
                
                prev_reads[info->count] = reads;
                prev_writes[info->count] = writes;
            } else {
                disk->reads = 0;
                disk->writes = 0;
                disk->io_in_progress = 0;
            }

            info->count++;
        }
    }

    endmntent(mtab);
    return 0;
}

void cleanup_disk_monitor(void) {
    // Nothing to clean up for now
} 
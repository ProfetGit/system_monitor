/**
 * @file cpu.c
 * @brief Implementation of CPU monitoring functionality
 */

#include "cpu.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Static variables for CPU usage calculation
static unsigned long long prev_idle = 0;
static unsigned long long prev_total = 0;

/**
 * @brief Read CPU statistics from /proc/stat
 * @param idle Pointer to store idle time
 * @param total Pointer to store total time
 * @return 0 on success, -1 on failure
 */
static int read_cpu_stats(unsigned long long *idle, unsigned long long *total) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return -1;

    unsigned long long user, nice, system, idle_time, iowait, irq, softirq, steal;
    if (fscanf(fp, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
               &user, &nice, &system, &idle_time, &iowait, &irq, &softirq, &steal) != 8) {
        fclose(fp);
        return -1;
    }

    *idle = idle_time + iowait;
    *total = *idle + user + nice + system + irq + softirq + steal;

    fclose(fp);
    return 0;
}

/**
 * @brief Get CPU model name from /proc/cpuinfo
 * @param model_name Buffer to store the model name
 * @param size Size of the buffer
 * @return 0 on success, -1 on failure
 */
static int get_cpu_model(char *model_name, size_t size) {
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (!fp) return -1;

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "model name", 10) == 0) {
            char *colon = strchr(line, ':');
            if (colon) {
                // Skip the colon and any whitespace
                colon++;
                while (*colon == ' ') colon++;
                // Remove trailing newline
                char *newline = strchr(colon, '\n');
                if (newline) *newline = '\0';
                strncpy(model_name, colon, size - 1);
                model_name[size - 1] = '\0';
                fclose(fp);
                return 0;
            }
        }
    }

    fclose(fp);
    return -1;
}

int init_cpu_monitor(void) {
    prev_idle = 0;
    prev_total = 0;
    return 0;
}

int update_cpu_stats(CPUStats *stats) {
    if (!stats) return -1;

    // Get CPU model name (only if not already set)
    if (stats->model_name[0] == '\0') {
        if (get_cpu_model(stats->model_name, sizeof(stats->model_name)) != 0) {
            strcpy(stats->model_name, "Unknown CPU");
        }
    }

    // Get number of CPU cores
    stats->cores = sysconf(_SC_NPROCESSORS_ONLN);

    // Get CPU usage
    unsigned long long idle, total;
    if (read_cpu_stats(&idle, &total) != 0) return -1;

    if (prev_total != 0) {
        unsigned long long total_diff = total - prev_total;
        unsigned long long idle_diff = idle - prev_idle;

        if (total_diff > 0) {
            stats->usage = 100.0 * (1.0 - ((double)idle_diff / total_diff));
        }
    }

    prev_idle = idle;
    prev_total = total;

    return 0;
}

void cleanup_cpu_monitor(void) {
    // Nothing to clean up for now
} 
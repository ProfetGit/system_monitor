/**
 * @file main.c
 * @brief Main entry point for the system monitor application
 */

#include "system_monitor.h"
#include <signal.h>

static volatile int keep_running = 1;

/**
 * @brief Signal handler for clean program termination
 * @param sig Signal number
 */
void sig_handler(int sig) {
    (void)sig;  // Unused parameter
    keep_running = 0;
}

int main(void) {
    SystemStats stats = {0};
    
    // Set up signal handler for clean exit
    signal(SIGINT, sig_handler);
    
    // Initialize CPU monitoring
    if (init_cpu_monitor() != 0) {
        fprintf(stderr, "Failed to initialize CPU monitor\n");
        return EXIT_FAILURE;
    }
    
    // Initialize Memory monitoring
    if (init_memory_monitoring() != 0) {
        fprintf(stderr, "Failed to initialize Memory monitor\n");
        cleanup_cpu_monitor();
        return EXIT_FAILURE;
    }
    
    // Initialize Disk monitoring
    if (init_disk_monitor() != 0) {
        fprintf(stderr, "Failed to initialize Disk monitor\n");
        cleanup_memory_monitoring();
        cleanup_cpu_monitor();
        return EXIT_FAILURE;
    }
    
    // Initialize GPU monitoring
    if (init_gpu_monitor() != 0) {
        fprintf(stderr, "Failed to initialize GPU monitor\n");
        cleanup_disk_monitor();
        cleanup_memory_monitoring();
        cleanup_cpu_monitor();
        return EXIT_FAILURE;
    }
    
    // Initialize Network monitoring
    if (init_network_monitoring() != 0) {
        fprintf(stderr, "Failed to initialize Network monitor\n");
        cleanup_gpu_monitor();
        cleanup_disk_monitor();
        cleanup_memory_monitoring();
        cleanup_cpu_monitor();
        return EXIT_FAILURE;
    }
    
    // Initialize ncurses
    if (init_display() != 0) {
        fprintf(stderr, "Failed to initialize display\n");
        cleanup_network_monitoring();
        cleanup_gpu_monitor();
        cleanup_disk_monitor();
        cleanup_memory_monitoring();
        cleanup_cpu_monitor();
        return EXIT_FAILURE;
    }
    
    // Main program loop
    while (keep_running) {
        if (update_stats(&stats) == 0) {
            display_stats(&stats);
        }
        napms(1000);  // Update every second
    }
    
    // Cleanup
    cleanup_display();
    cleanup_network_monitoring();
    cleanup_gpu_monitor();
    cleanup_disk_monitor();
    cleanup_memory_monitoring();
    cleanup_cpu_monitor();
    return EXIT_SUCCESS;
} 
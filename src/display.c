/**
 * @file display.c
 * @brief Implementation of display-related functions
 */

#include "system_monitor.h"
#include <stdio.h>

// Window dimensions
#define CPU_WIN_HEIGHT 4
#define MEM_WIN_HEIGHT 5
#define DISK_WIN_HEIGHT 12
#define GPU_WIN_HEIGHT 12
#define WIN_WIDTH 60

// Global window pointers
static WINDOW *cpu_win = NULL;
static WINDOW *mem_win = NULL;
static WINDOW *disk_win = NULL;
static WINDOW *gpu_win = NULL;

/**
 * @brief Convert bytes to human readable format
 * @param bytes Number of bytes
 * @param buf Buffer to store the result
 * @param size Size of the buffer
 */
static void format_bytes(unsigned long bytes, char *buf, size_t size) {
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    int i = 0;
    double size_d = bytes;
    
    while (size_d >= 1024 && i < 4) {
        size_d /= 1024;
        i++;
    }
    
    snprintf(buf, size, "%.1f %s", size_d, units[i]);
}

int init_display(void) {
    // Initialize ncurses
    if (!initscr()) {
        fprintf(stderr, "Failed to initialize ncurses\n");
        return -1;
    }

    // Check terminal size
    if (LINES < (CPU_WIN_HEIGHT + MEM_WIN_HEIGHT + DISK_WIN_HEIGHT + GPU_WIN_HEIGHT + 4) ||
        COLS < (WIN_WIDTH + 2)) {
        endwin();
        fprintf(stderr, "Terminal window too small. Minimum size required: %dx%d\n",
                WIN_WIDTH + 2, CPU_WIN_HEIGHT + MEM_WIN_HEIGHT + DISK_WIN_HEIGHT + GPU_WIN_HEIGHT + 4);
        return -1;
    }

    // Set up colors if supported
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        init_pair(4, COLOR_CYAN, COLOR_BLACK);
    }

    // Configure ncurses
    cbreak();              // Line buffering disabled
    noecho();             // Don't echo keystrokes
    curs_set(0);          // Hide cursor if possible
    timeout(0);           // Non-blocking input
    keypad(stdscr, TRUE); // Enable keypad input

    // Calculate window positions
    int start_y = (LINES - (CPU_WIN_HEIGHT + MEM_WIN_HEIGHT + DISK_WIN_HEIGHT + GPU_WIN_HEIGHT + 3)) / 2;
    int start_x = (COLS - WIN_WIDTH) / 2;

    // Create windows with error checking
    cpu_win = newwin(CPU_WIN_HEIGHT, WIN_WIDTH, start_y, start_x);
    if (!cpu_win) {
        endwin();
        fprintf(stderr, "Failed to create CPU window\n");
        return -1;
    }

    mem_win = newwin(MEM_WIN_HEIGHT, WIN_WIDTH, 
                     start_y + CPU_WIN_HEIGHT + 1, start_x);
    if (!mem_win) {
        delwin(cpu_win);
        endwin();
        fprintf(stderr, "Failed to create Memory window\n");
        return -1;
    }

    disk_win = newwin(DISK_WIN_HEIGHT, WIN_WIDTH, 
                      start_y + CPU_WIN_HEIGHT + MEM_WIN_HEIGHT + 2, start_x);
    if (!disk_win) {
        delwin(cpu_win);
        delwin(mem_win);
        endwin();
        fprintf(stderr, "Failed to create Disk window\n");
        return -1;
    }

    gpu_win = newwin(GPU_WIN_HEIGHT, WIN_WIDTH,
                     start_y + CPU_WIN_HEIGHT + MEM_WIN_HEIGHT + DISK_WIN_HEIGHT + 3, start_x);
    if (!gpu_win) {
        delwin(cpu_win);
        delwin(mem_win);
        delwin(disk_win);
        endwin();
        fprintf(stderr, "Failed to create GPU window\n");
        return -1;
    }

    // Enable scrolling for disk and GPU windows
    scrollok(disk_win, TRUE);
    scrollok(gpu_win, TRUE);

    // Clear the screen and refresh
    clear();
    refresh();

    return 0;
}

void cleanup_display(void) {
    if (gpu_win) delwin(gpu_win);
    if (disk_win) delwin(disk_win);
    if (mem_win) delwin(mem_win);
    if (cpu_win) delwin(cpu_win);
    endwin();
}

static int get_usage_color(double usage) {
    if (!has_colors()) return 0;
    if (usage < 60.0) return 1;        // Green
    else if (usage < 85.0) return 2;   // Yellow
    return 3;                          // Red
}

static int get_temp_color(int temp) {
    if (!has_colors()) return 0;
    if (temp < 60) return 1;           // Green
    else if (temp < 80) return 2;      // Yellow
    return 3;                          // Red
}

void display_stats(const SystemStats *stats) {
    if (!stats || !cpu_win || !mem_win || !disk_win || !gpu_win) return;

    char buf[32];

    // Update CPU window
    werase(cpu_win);
    box(cpu_win, 0, 0);
    mvwprintw(cpu_win, 0, 2, " CPU Information ");
    
    // Display CPU model and cores
    mvwprintw(cpu_win, 1, 2, "Model: %s", stats->cpu.model_name);
    mvwprintw(cpu_win, 2, 2, "Cores: %u", stats->cpu.cores);
    
    // Display CPU usage with color
    int color = get_usage_color(stats->cpu.usage);
    if (color) wattron(cpu_win, COLOR_PAIR(color));
    mvwprintw(cpu_win, 2, 30, "Usage: %.1f%%", stats->cpu.usage);
    if (color) wattroff(cpu_win, COLOR_PAIR(color));
    
    wrefresh(cpu_win);

    // Update Memory window
    werase(mem_win);
    box(mem_win, 0, 0);
    mvwprintw(mem_win, 0, 2, " Memory Information ");

    // Physical memory
    format_bytes(stats->memory.available, buf, sizeof(buf));
    color = get_usage_color(stats->memory.usage);
    if (color) wattron(mem_win, COLOR_PAIR(color));
    mvwprintw(mem_win, 1, 2, "RAM: %.1f%% (%s available)", 
              stats->memory.usage, buf);
    if (color) wattroff(mem_win, COLOR_PAIR(color));

    format_bytes(stats->memory.total, buf, sizeof(buf));
    mvwprintw(mem_win, 2, 4, "Total: %s", buf);
    format_bytes(stats->memory.cached, buf, sizeof(buf));
    mvwprintw(mem_win, 2, 30, "Cached: %s", buf);

    // Swap memory
    if (stats->memory.swap_total > 0) {
        color = get_usage_color(stats->memory.swap_usage);
        if (color) wattron(mem_win, COLOR_PAIR(color));
        mvwprintw(mem_win, 3, 2, "Swap: %.1f%% used", stats->memory.swap_usage);
        if (color) wattroff(mem_win, COLOR_PAIR(color));
        format_bytes(stats->memory.swap_total - stats->memory.swap_free, buf, sizeof(buf));
        mvwprintw(mem_win, 3, 30, "Used: %s", buf);
    } else {
        mvwprintw(mem_win, 3, 2, "Swap: Not available");
    }

    wrefresh(mem_win);

    // Update Disk window
    werase(disk_win);
    box(disk_win, 0, 0);
    mvwprintw(disk_win, 0, 2, " Disk Information ");

    int y = 1;
    for (int i = 0; i < stats->disks.count && y < DISK_WIN_HEIGHT - 1; i++) {
        const DiskStats *disk = &stats->disks.disks[i];
        
        // Display device and mount point
        mvwprintw(disk_win, y, 2, "%s (%s)", disk->device, disk->mount_point);
        y++;

        // Display usage with color
        color = get_usage_color(disk->usage);
        if (color) wattron(disk_win, COLOR_PAIR(color));
        mvwprintw(disk_win, y, 4, "Usage: %.1f%%", disk->usage);
        if (color) wattroff(disk_win, COLOR_PAIR(color));

        // Display space information
        format_bytes(disk->total, buf, sizeof(buf));
        mvwprintw(disk_win, y, 25, "Total: %s", buf);
        y++;

        format_bytes(disk->available, buf, sizeof(buf));
        mvwprintw(disk_win, y, 4, "Available: %s", buf);

        // Display I/O information
        mvwprintw(disk_win, y, 25, "I/O: %lu R, %lu W", disk->reads, disk->writes);
        if (disk->io_in_progress > 0) {
            mvwprintw(disk_win, y, 50, "[Busy]");
        }
        y++;

        // Add a separator line between disks
        if (i < stats->disks.count - 1 && y < DISK_WIN_HEIGHT - 1) {
            mvwhline(disk_win, y, 1, ACS_HLINE, WIN_WIDTH - 2);
            y++;
        }
    }

    wrefresh(disk_win);

    // Update GPU window
    werase(gpu_win);
    box(gpu_win, 0, 0);
    
    if (stats->gpus.count > 0) {
        mvwprintw(gpu_win, 0, 2, " GPU Information ");
        
        int y = 1;
        for (unsigned int i = 0; i < stats->gpus.count && y < GPU_WIN_HEIGHT - 1; i++) {
            const GPUStats *gpu = &stats->gpus.gpus[i];
            
            // Display GPU name and basic info
            if (has_colors()) wattron(gpu_win, COLOR_PAIR(4));
            mvwprintw(gpu_win, y, 2, "GPU %u: %s", i + 1, gpu->name);
            if (has_colors()) wattroff(gpu_win, COLOR_PAIR(4));
            y++;

            if (gpu->supported) {
                // Display temperature
                int temp_color = get_temp_color(gpu->temperature);
                if (temp_color) wattron(gpu_win, COLOR_PAIR(temp_color));
                mvwprintw(gpu_win, y, 4, "Temp: %d°C", gpu->temperature);
                if (temp_color) wattroff(gpu_win, COLOR_PAIR(temp_color));

                // Display utilization
                int util_color = get_usage_color(gpu->utilization);
                if (util_color) wattron(gpu_win, COLOR_PAIR(util_color));
                mvwprintw(gpu_win, y, 25, "Usage: %.1f%%", gpu->utilization);
                if (util_color) wattroff(gpu_win, COLOR_PAIR(util_color));

                // Display fan speed
                mvwprintw(gpu_win, y, 45, "Fan: %d%%", gpu->fan_speed);
                y++;

                // Display memory information
                if (gpu->memory_total > 0) {
                    format_bytes(gpu->memory_total, buf, sizeof(buf));
                    mvwprintw(gpu_win, y, 4, "Memory Total: %s", buf);
                    
                    format_bytes(gpu->memory_used, buf, sizeof(buf));
                    mvwprintw(gpu_win, y, 30, "Used: %s", buf);
                    y++;

                    double mem_usage = 100.0 * ((double)gpu->memory_used / gpu->memory_total);
                    int mem_color = get_usage_color(mem_usage);
                    if (mem_color) wattron(gpu_win, COLOR_PAIR(mem_color));
                    mvwprintw(gpu_win, y, 4, "Memory Usage: %.1f%%", mem_usage);
                    if (mem_color) wattroff(gpu_win, COLOR_PAIR(mem_color));
                    y++;
                }

                // Display power usage if available
                if (gpu->power_usage > 0) {
                    mvwprintw(gpu_win, y, 4, "Power Usage: %.1f W", gpu->power_usage / 1000.0);
                    y++;
                }
            } else {
                mvwprintw(gpu_win, y, 4, "Limited information available");
                y++;
            }

            // Add a separator line between GPUs
            if (i + 1 < stats->gpus.count && y < GPU_WIN_HEIGHT - 1) {
                mvwhline(gpu_win, y, 1, ACS_HLINE, WIN_WIDTH - 2);
                y++;
            }
        }
    } else {
        mvwprintw(gpu_win, 0, 2, " GPU Information (No GPUs detected) ");
        mvwprintw(gpu_win, 1, 2, "No GPU information available");
    }

    wrefresh(gpu_win);

    // Refresh the screen
    refresh();
} 
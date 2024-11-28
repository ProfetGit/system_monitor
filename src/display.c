/**
 * @file display.c
 * @brief Implementation of display-related functions
 */

#include "system_monitor.h"
#include <stdio.h>

// Window dimensions and positions
#define HEADER_HEIGHT 3
#define CPU_WIN_HEIGHT 6
#define MEM_WIN_HEIGHT 7
#define DISK_WIN_HEIGHT 8
#define NET_WIN_HEIGHT 8
#define GPU_WIN_HEIGHT 8
#define WIN_WIDTH 70
#define PADDING 1

// Color pairs
#define COLOR_HEADER 1
#define COLOR_NORMAL 2
#define COLOR_WARNING 3
#define COLOR_CRITICAL 4
#define COLOR_GOOD 5
#define COLOR_BORDER 6

// Global window pointers
static WINDOW *header_win = NULL;
static WINDOW *cpu_win = NULL;
static WINDOW *mem_win = NULL;
static WINDOW *disk_win = NULL;
static WINDOW *net_win = NULL;
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

/**
 * @brief Format network speed in human readable format
 * @param bytes_per_sec Speed in bytes per second
 * @param buf Buffer to store the result
 * @param size Size of the buffer
 */
static void format_speed(double bytes_per_sec, char *buf, size_t size) {
    const char *units[] = {"B/s", "KB/s", "MB/s", "GB/s"};
    int i = 0;
    double speed = bytes_per_sec;
    
    while (speed >= 1024 && i < 3) {
        speed /= 1024;
        i++;
    }
    
    snprintf(buf, size, "%.1f %s", speed, units[i]);
}

/**
 * @brief Draw a fancy box around a window
 * @param win Window to draw box around
 * @param title Title of the box
 */
static void draw_fancy_box(WINDOW *win, const char *title) {
    int width, height;
    getmaxyx(win, height, width);
    (void)height;  // Suppress unused variable warning
    
    wattron(win, COLOR_PAIR(COLOR_BORDER) | A_BOLD);
    box(win, 0, 0);
    mvwprintw(win, 0, (width - strlen(title) - 4) / 2, "┤ %s ├", title);
    wattroff(win, COLOR_PAIR(COLOR_BORDER) | A_BOLD);
}

/**
 * @brief Create a new centered window
 * @param height Window height
 * @param width Window width
 * @param starty Starting Y position
 * @return Pointer to the new window
 */
static WINDOW *create_centered_win(int height, int width, int starty) {
    int startx = (COLS - width) / 2;
    WINDOW *win = newwin(height, width, starty, startx);
    return win;
}

int init_display(void) {
    // Initialize ncurses
    if (!initscr()) {
        fprintf(stderr, "Failed to initialize ncurses\n");
        return -1;
    }

    // Check terminal size
    int required_height = HEADER_HEIGHT + CPU_WIN_HEIGHT + MEM_WIN_HEIGHT + 
                         DISK_WIN_HEIGHT + NET_WIN_HEIGHT + GPU_WIN_HEIGHT + 
                         PADDING * 6;
    
    if (LINES < required_height || COLS < (WIN_WIDTH + 2)) {
        endwin();
        fprintf(stderr, "Terminal too small. Minimum size: %dx%d\n", 
                WIN_WIDTH + 2, required_height);
        return -1;
    }

    // Setup colors
    start_color();
    use_default_colors();
    init_pair(COLOR_HEADER, COLOR_CYAN, -1);
    init_pair(COLOR_NORMAL, -1, -1);
    init_pair(COLOR_WARNING, COLOR_YELLOW, -1);
    init_pair(COLOR_CRITICAL, COLOR_RED, -1);
    init_pair(COLOR_GOOD, COLOR_GREEN, -1);
    init_pair(COLOR_BORDER, COLOR_BLUE, -1);

    // Enable keyboard input and function keys
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    timeout(0);

    // Reduce screen flicker
    nodelay(stdscr, TRUE);    // Non-blocking input
    leaveok(stdscr, TRUE);    // Don't care where cursor is left
    cbreak();                 // Disable line buffering
    noecho();                 // Don't echo input
    curs_set(0);             // Hide cursor

    // Calculate window positions
    int current_y = 0;
    header_win = create_centered_win(HEADER_HEIGHT, WIN_WIDTH, current_y);
    current_y += HEADER_HEIGHT + PADDING;
    
    cpu_win = create_centered_win(CPU_WIN_HEIGHT, WIN_WIDTH, current_y);
    current_y += CPU_WIN_HEIGHT + PADDING;
    
    mem_win = create_centered_win(MEM_WIN_HEIGHT, WIN_WIDTH, current_y);
    current_y += MEM_WIN_HEIGHT + PADDING;
    
    disk_win = create_centered_win(DISK_WIN_HEIGHT, WIN_WIDTH, current_y);
    current_y += DISK_WIN_HEIGHT + PADDING;
    
    net_win = create_centered_win(NET_WIN_HEIGHT, WIN_WIDTH, current_y);
    current_y += NET_WIN_HEIGHT + PADDING;
    
    gpu_win = create_centered_win(GPU_WIN_HEIGHT, WIN_WIDTH, current_y);

    if (!header_win || !cpu_win || !mem_win || !disk_win || !net_win || !gpu_win) {
        cleanup_display();
        return -1;
    }

    // Enable scrolling and optimize window updates
    scrollok(stdscr, FALSE);
    
    // Optimize all windows
    WINDOW *windows[] = {header_win, cpu_win, mem_win, disk_win, net_win, gpu_win};
    for (int i = 0; i < 6; i++) {
        scrollok(windows[i], FALSE);
        leaveok(windows[i], TRUE);
        idlok(windows[i], TRUE);
        idcok(windows[i], TRUE);
    }

    // Draw initial window borders
    draw_fancy_box(header_win, "");
    draw_fancy_box(cpu_win, "CPU");
    draw_fancy_box(mem_win, "Memory");
    draw_fancy_box(disk_win, "Disk");
    draw_fancy_box(net_win, "Network");
    draw_fancy_box(gpu_win, "GPU");
    
    refresh();
    return 0;
}

void cleanup_display(void) {
    if (header_win) delwin(header_win);
    if (cpu_win) delwin(cpu_win);
    if (mem_win) delwin(mem_win);
    if (disk_win) delwin(disk_win);
    if (net_win) delwin(net_win);
    if (gpu_win) delwin(gpu_win);
    endwin();
}

/**
 * @brief Display system statistics
 * @param stats Pointer to SystemStats structure containing current statistics
 */
void display_stats(const SystemStats *stats) {
    char buf[256];
    int row;
    
    // Header - only update the time
    mvwprintw(header_win, 1, (WIN_WIDTH - 14) / 2, "SYSTEM MONITOR");
    wrefresh(header_win);
    
    // CPU
    werase(cpu_win);
    row = 1;
    mvwprintw(cpu_win, row++, 2, "CPU Usage: ");
    wattron(cpu_win, A_BOLD);
    wprintw(cpu_win, "%.1f%%", stats->cpu.usage);
    wattroff(cpu_win, A_BOLD);
    mvwprintw(cpu_win, row++, 2, "Model: %s", stats->cpu.model_name);
    mvwprintw(cpu_win, row++, 2, "Cores: %u", stats->cpu.cores);
    draw_fancy_box(cpu_win, "CPU");
    wrefresh(cpu_win);
    
    // Memory
    werase(mem_win);
    row = 1;
    format_bytes(stats->memory.total, buf, sizeof(buf));
    mvwprintw(mem_win, row++, 2, "Total Memory: %s", buf);
    format_bytes(stats->memory.used, buf, sizeof(buf));
    mvwprintw(mem_win, row++, 2, "Used Memory:  %s (%.1f%%)", 
              buf, stats->memory.usage);
    format_bytes(stats->memory.free, buf, sizeof(buf));
    mvwprintw(mem_win, row++, 2, "Free Memory:  %s", buf);
    format_bytes(stats->memory.cached, buf, sizeof(buf));
    mvwprintw(mem_win, row++, 2, "Cache:        %s", buf);
    mvwprintw(mem_win, row++, 2, "Swap Usage:   %.1f%%", stats->memory.swap_usage);
    draw_fancy_box(mem_win, "Memory");
    wrefresh(mem_win);
    
    // Network
    werase(net_win);
    row = 1;
    for (int i = 0; i < stats->network.interface_count && i < 3; i++) {
        const NetworkInterfaceStats *if_stats = &stats->network.interfaces[i];
        mvwprintw(net_win, row++, 2, "Interface: %s", if_stats->interface);
        
        format_speed(if_stats->receive_speed, buf, sizeof(buf));
        mvwprintw(net_win, row++, 4, "RX: %s", buf);
        
        format_speed(if_stats->send_speed, buf, sizeof(buf));
        mvwprintw(net_win, row++, 4, "TX: %s", buf);
        row++;
    }
    draw_fancy_box(net_win, "Network");
    wrefresh(net_win);
    
    // Disk
    werase(disk_win);
    row = 1;
    mvwprintw(disk_win, row++, 2, "Disk Usage:");
    for (int i = 0; i < stats->disks.count && i < 3; i++) {
        const DiskStats *disk = &stats->disks.disks[i];
        mvwprintw(disk_win, row++, 2, "  %s: %.1f%% used",
                  disk->mount_point,
                  disk->usage);
        format_bytes(disk->total, buf, sizeof(buf));
        mvwprintw(disk_win, row++, 4, "Total: %s", buf);
        format_bytes(disk->available, buf, sizeof(buf));
        mvwprintw(disk_win, row++, 4, "Free: %s", buf);
    }
    draw_fancy_box(disk_win, "Disk");
    wrefresh(disk_win);
    
    // GPU
    werase(gpu_win);
    row = 1;
    for (unsigned int i = 0; i < stats->gpus.count && i < 2; i++) {
        const GPUStats *gpu = &stats->gpus.gpus[i];
        mvwprintw(gpu_win, row++, 2, "GPU %u: %s", i, gpu->name);
        mvwprintw(gpu_win, row++, 4, "Usage: %.1f%%", gpu->utilization);
        mvwprintw(gpu_win, row++, 4, "Temperature: %d°C", gpu->temperature);
        if (gpu->memory_total > 0) {
            format_bytes(gpu->memory_used, buf, sizeof(buf));
            mvwprintw(gpu_win, row++, 4, "Memory Used: %s", buf);
        }
        row++;
    }
    draw_fancy_box(gpu_win, "GPU");
    wrefresh(gpu_win);
    
    // Use doupdate() instead of refresh() for smoother updates
    doupdate();
} 
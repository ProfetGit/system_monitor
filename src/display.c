/**
 * @file display.c
 * @brief Implementation of display-related functions
 */

#include "system_monitor.h"

// Window dimensions
#define CPU_WIN_HEIGHT 4
#define MEM_WIN_HEIGHT 5
#define WIN_WIDTH 60

// Global window pointers
static WINDOW *cpu_win = NULL;
static WINDOW *mem_win = NULL;

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
    initscr();
    if (!has_colors()) {
        endwin();
        fprintf(stderr, "Your terminal does not support colors\n");
        return -1;
    }

    // Set up colors
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);

    // Configure ncurses
    cbreak();
    noecho();
    curs_set(0);
    timeout(0);

    // Calculate window positions
    int start_y = (LINES - (CPU_WIN_HEIGHT + MEM_WIN_HEIGHT + 1)) / 2;
    int start_x = (COLS - WIN_WIDTH) / 2;

    // Create windows
    cpu_win = newwin(CPU_WIN_HEIGHT, WIN_WIDTH, start_y, start_x);
    mem_win = newwin(MEM_WIN_HEIGHT, WIN_WIDTH, start_y + CPU_WIN_HEIGHT + 1, start_x);

    if (!cpu_win || !mem_win) {
        cleanup_display();
        return -1;
    }

    return 0;
}

void cleanup_display(void) {
    if (cpu_win) delwin(cpu_win);
    if (mem_win) delwin(mem_win);
    endwin();
}

static int get_usage_color(double usage) {
    if (usage < 60.0) return 1;        // Green
    else if (usage < 85.0) return 2;   // Yellow
    return 3;                          // Red
}

void display_stats(const SystemStats *stats) {
    if (!stats || !cpu_win || !mem_win) return;

    char buf[32];

    // Update CPU window
    werase(cpu_win);
    box(cpu_win, 0, 0);
    mvwprintw(cpu_win, 0, 2, " CPU Information ");
    
    // Display CPU model and cores
    mvwprintw(cpu_win, 1, 2, "Model: %s", stats->cpu.model_name);
    mvwprintw(cpu_win, 2, 2, "Cores: %u", stats->cpu.cores);
    
    // Display CPU usage with color
    wattron(cpu_win, COLOR_PAIR(get_usage_color(stats->cpu.usage)));
    mvwprintw(cpu_win, 2, 30, "Usage: %.1f%%", stats->cpu.usage);
    wattroff(cpu_win, COLOR_PAIR(get_usage_color(stats->cpu.usage)));
    
    wrefresh(cpu_win);

    // Update Memory window
    werase(mem_win);
    box(mem_win, 0, 0);
    mvwprintw(mem_win, 0, 2, " Memory Information ");

    // Physical memory
    format_bytes(stats->memory.available, buf, sizeof(buf));
    wattron(mem_win, COLOR_PAIR(get_usage_color(stats->memory.usage)));
    mvwprintw(mem_win, 1, 2, "RAM: %.1f%% (%s available)", 
              stats->memory.usage, buf);
    wattroff(mem_win, COLOR_PAIR(get_usage_color(stats->memory.usage)));

    format_bytes(stats->memory.total, buf, sizeof(buf));
    mvwprintw(mem_win, 2, 4, "Total: %s", buf);
    format_bytes(stats->memory.cached, buf, sizeof(buf));
    mvwprintw(mem_win, 2, 30, "Cached: %s", buf);

    // Swap memory
    if (stats->memory.swap_total > 0) {
        wattron(mem_win, COLOR_PAIR(get_usage_color(stats->memory.swap_usage)));
        mvwprintw(mem_win, 3, 2, "Swap: %.1f%% used", stats->memory.swap_usage);
        wattroff(mem_win, COLOR_PAIR(get_usage_color(stats->memory.swap_usage)));
        format_bytes(stats->memory.swap_total - stats->memory.swap_free, buf, sizeof(buf));
        mvwprintw(mem_win, 3, 30, "Used: %s", buf);
    } else {
        mvwprintw(mem_win, 3, 2, "Swap: Not available");
    }

    wrefresh(mem_win);

    // Refresh the screen
    refresh();
} 
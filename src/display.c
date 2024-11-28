/**
 * @file display.c
 * @brief Implementation of display-related functions
 */

#include "system_monitor.h"

// Window dimensions
#define CPU_WIN_HEIGHT 4
#define MEM_WIN_HEIGHT 3
#define WIN_WIDTH 60

// Global window pointers
static WINDOW *cpu_win = NULL;
static WINDOW *mem_win = NULL;

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
    mvwprintw(mem_win, 0, 2, " Memory Usage ");
    wattron(mem_win, COLOR_PAIR(get_usage_color(stats->memory_usage)));
    mvwprintw(mem_win, 1, 2, "%.1f%% (%.1f GB / %.1f GB)",
              stats->memory_usage,
              (stats->total_memory - stats->free_memory) / 1024.0 / 1024.0,
              stats->total_memory / 1024.0 / 1024.0);
    wattroff(mem_win, COLOR_PAIR(get_usage_color(stats->memory_usage)));
    wrefresh(mem_win);

    // Refresh the screen
    refresh();
} 
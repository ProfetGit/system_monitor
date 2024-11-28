# Terminal System Monitor

A terminal-based system monitoring application written in C using ncurses.

## Features

- Real-time CPU usage monitoring
- Memory usage statistics
- Clean ncurses-based interface
- Documented using Doxygen

## Prerequisites

- GCC compiler
- Make build system
- ncurses library
- Doxygen (for generating documentation)

On Ubuntu/Debian, install the required packages:

```bash
sudo apt-get install gcc make libncurses5-dev doxygen
```

## Building

To build the project:

```bash
make
```

To generate documentation:

```bash
make docs
```

To clean build files:

```bash
make clean
```

## Running

After building, run the system monitor:

```bash
./build/system_monitor
```

Press Ctrl+C to exit the program.

## Project Structure

- `src/` - Source files
- `include/` - Header files
- `docs/` - Documentation
- `tests/` - Test files (upcoming)

## Documentation

After generating the documentation with `make docs`, open `docs/html/index.html` in your web browser to view the complete API documentation. 
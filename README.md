# System Monitor

A real-time system monitoring application that provides detailed information about CPU, memory, disk, and GPU usage.

## Overview

This application monitors various system resources in real-time and displays them in a user-friendly ncurses-based interface. It provides comprehensive information about:

- CPU usage and frequency
- Memory usage (RAM and swap)
- Disk space and I/O statistics
- GPU temperature and memory usage

## Features

- Real-time monitoring of system resources
- Ncurses-based user interface
- Detailed statistics for CPU, memory, disk, and GPU
- Configurable update intervals
- Low system overhead

## Building

To build the application, you need:
- GCC or Clang compiler
- ncurses development libraries
- CMake (version 3.10 or higher)

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
./system_monitor [update_interval_ms]
```

The optional `update_interval_ms` parameter specifies the update interval in milliseconds (default: 1000).

## Documentation

The complete API documentation is available in the `docs/html` directory. To generate the documentation:

```bash
doxygen
```

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit pull requests. 
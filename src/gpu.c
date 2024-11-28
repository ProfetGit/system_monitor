/**
 * @file gpu.c
 * @brief Implementation of GPU monitoring functionality
 */

#include "gpu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

// NVIDIA NVML function pointers
typedef struct {
    void *handle;
    int (*nvmlInit)(void);
    int (*nvmlShutdown)(void);
    int (*nvmlDeviceGetCount)(unsigned int *);
    int (*nvmlDeviceGetHandleByIndex)(unsigned int, void **);
    int (*nvmlDeviceGetName)(void *, char *, unsigned int);
    int (*nvmlDeviceGetTemperature)(void *, int, unsigned int *);
    int (*nvmlDeviceGetUtilizationRates)(void *, void *);
    int (*nvmlDeviceGetMemoryInfo)(void *, void *);
    int (*nvmlDeviceGetPowerUsage)(void *, unsigned int *);
    int (*nvmlDeviceGetFanSpeed)(void *, unsigned int *);
} NVMLFunctions;

static NVMLFunctions nvml = {0};

/**
 * @brief Load NVIDIA NVML library and functions
 * @return 0 on success, -1 on failure
 */
static int load_nvml(void) {
    // Try to load NVML library
    nvml.handle = dlopen("libnvidia-ml.so", RTLD_LAZY);
    if (!nvml.handle) {
        nvml.handle = dlopen("libnvidia-ml.so.1", RTLD_LAZY);
    }
    if (!nvml.handle) return -1;

    // Load NVML functions
    *(void **)(&nvml.nvmlInit) = dlsym(nvml.handle, "nvmlInit_v2");
    *(void **)(&nvml.nvmlShutdown) = dlsym(nvml.handle, "nvmlShutdown");
    *(void **)(&nvml.nvmlDeviceGetCount) = dlsym(nvml.handle, "nvmlDeviceGetCount_v2");
    *(void **)(&nvml.nvmlDeviceGetHandleByIndex) = dlsym(nvml.handle, "nvmlDeviceGetHandleByIndex_v2");
    *(void **)(&nvml.nvmlDeviceGetName) = dlsym(nvml.handle, "nvmlDeviceGetName");
    *(void **)(&nvml.nvmlDeviceGetTemperature) = dlsym(nvml.handle, "nvmlDeviceGetTemperature");
    *(void **)(&nvml.nvmlDeviceGetUtilizationRates) = dlsym(nvml.handle, "nvmlDeviceGetUtilizationRates");
    *(void **)(&nvml.nvmlDeviceGetMemoryInfo) = dlsym(nvml.handle, "nvmlDeviceGetMemoryInfo");
    *(void **)(&nvml.nvmlDeviceGetPowerUsage) = dlsym(nvml.handle, "nvmlDeviceGetPowerUsage");
    *(void **)(&nvml.nvmlDeviceGetFanSpeed) = dlsym(nvml.handle, "nvmlDeviceGetFanSpeed");

    // Check if all functions were loaded
    return (nvml.nvmlInit && nvml.nvmlShutdown && nvml.nvmlDeviceGetCount &&
            nvml.nvmlDeviceGetHandleByIndex && nvml.nvmlDeviceGetName &&
            nvml.nvmlDeviceGetTemperature && nvml.nvmlDeviceGetUtilizationRates &&
            nvml.nvmlDeviceGetMemoryInfo && nvml.nvmlDeviceGetPowerUsage &&
            nvml.nvmlDeviceGetFanSpeed) ? 0 : -1;
}

/**
 * @brief Try to read GPU information from sysfs (for non-NVIDIA GPUs)
 * @param info Pointer to GPUInfo structure
 */
static void read_sysfs_gpu_info(GPUInfo *info) {
    FILE *fp;
    char path[256];
    char line[256];
    unsigned int gpu_count = 0;

    // Try to find GPUs in sysfs
    for (unsigned int i = 0; i < MAX_GPUS; i++) {
        snprintf(path, sizeof(path), "/sys/class/drm/card%u/device/vendor", i);
        fp = fopen(path, "r");
        if (!fp) continue;

        GPUStats *gpu = &info->gpus[gpu_count];
        gpu->supported = 0;  // Limited support for non-NVIDIA GPUs

        // Try to read GPU name
        snprintf(path, sizeof(path), "/sys/class/drm/card%u/device/product", i);
        FILE *name_fp = fopen(path, "r");
        if (name_fp) {
            if (fgets(line, sizeof(line), name_fp)) {
                line[strcspn(line, "\n")] = 0;
                strncpy(gpu->name, line, MAX_GPU_NAME - 1);
                gpu->name[MAX_GPU_NAME - 1] = '\0';
            }
            fclose(name_fp);
        } else {
            strcpy(gpu->name, "Unknown GPU");
        }

        gpu_count++;
        fclose(fp);
        
        if (gpu_count >= MAX_GPUS) break;
    }

    info->count = gpu_count;
}

int init_gpu_monitor(void) {
    // Try to load NVML
    if (load_nvml() == 0 && nvml.nvmlInit() == 0) {
        return 0;
    }

    // If NVML failed, we'll fall back to sysfs for basic GPU detection
    return 0;
}

int update_gpu_stats(GPUInfo *info) {
    if (!info) return -1;

    info->count = 0;
    info->nvidia_available = 0;

    // Try NVIDIA GPUs first
    if (nvml.handle) {
        unsigned int device_count = 0;
        if (nvml.nvmlDeviceGetCount(&device_count) == 0) {
            info->nvidia_available = 1;
            info->count = (device_count > MAX_GPUS) ? MAX_GPUS : device_count;

            for (unsigned int i = 0; i < info->count; i++) {
                void *device_handle = NULL;
                GPUStats *gpu = &info->gpus[i];
                gpu->supported = 1;

                if (nvml.nvmlDeviceGetHandleByIndex(i, &device_handle) == 0) {
                    // Get GPU name
                    nvml.nvmlDeviceGetName(device_handle, gpu->name, MAX_GPU_NAME);

                    // Get temperature
                    unsigned int temp;
                    if (nvml.nvmlDeviceGetTemperature(device_handle, 0, &temp) == 0) {
                        gpu->temperature = (int)temp;
                    }

                    // Get utilization
                    struct {
                        unsigned int gpu;
                        unsigned int memory;
                    } utilization = {0};
                    if (nvml.nvmlDeviceGetUtilizationRates(device_handle, &utilization) == 0) {
                        gpu->utilization = utilization.gpu;
                    }

                    // Get memory info
                    struct {
                        unsigned long long total;
                        unsigned long long free;
                        unsigned long long used;
                    } memory = {0};
                    if (nvml.nvmlDeviceGetMemoryInfo(device_handle, &memory) == 0) {
                        gpu->memory_total = memory.total;
                        gpu->memory_free = memory.free;
                        gpu->memory_used = memory.used;
                    }

                    // Get power usage
                    unsigned int power;
                    if (nvml.nvmlDeviceGetPowerUsage(device_handle, &power) == 0) {
                        gpu->power_usage = (int)power;
                    }

                    // Get fan speed
                    unsigned int fan;
                    if (nvml.nvmlDeviceGetFanSpeed(device_handle, &fan) == 0) {
                        gpu->fan_speed = (int)fan;
                    }
                }
            }
            return 0;
        }
    }

    // Fall back to sysfs for non-NVIDIA GPUs
    read_sysfs_gpu_info(info);
    return 0;
}

void cleanup_gpu_monitor(void) {
    if (nvml.handle) {
        if (nvml.nvmlShutdown) {
            nvml.nvmlShutdown();
        }
        dlclose(nvml.handle);
        nvml.handle = NULL;
    }
} 
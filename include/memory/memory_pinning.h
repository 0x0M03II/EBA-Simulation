
#ifndef SLIMEBA_MEM
#define SLIMEBA_MEM

#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <linux/memfd.h>
#include <linux/types.h>

#include "common/comomon.h"
#include "memory/eballoc.h"

#ifdef __cplusplus
#include <limits>
#endif

#if __GNUC__ >= 3
    #define __attribute_const __attribute__((const))
#else
    #define __attribute_const
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Shared memory name
#define SHM_NAME            "/buffer_table_shm"
#define DEVICESHM           "SOMEBLOCKDEV"

#define MAX_BUFFERS         1024

// These values represent percentages used by the allocation matrix
#define BUFFER_COUNT_514    45
#define BUFFER_COUNT_1024   20
#define BUFFER_COUNT_2048   20
#define BUFFER_COUNT_4096   15

#define BUFFER_514          514
#define BUFFER_1024         1024
#define BUFFER_2048         2048
#define BUFFER_4096         4096

// May use these later
#define HUGEPAGE_2MB_PATH "/sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages"
#define HUGEPAGE_1GB_PATH "/sys/kernel/mm/hugepages/hugepages-1048576kB/nr_hugepages"

#if defined(__x86_64__) || defined(_M_X64)
        // x86_64 supports either option, we default to the largest
#define ARCH            "x86_64"
#define HUGEPAGE_FLAG   (MFD_HUGETLB | MFD_HUGE_1GB)
#define MEM_SIZE        (1L * 1024 * 1024 * 1024)

#else   // Non x86 will default to 2MB, until I can add additional support

#define ARCH            "NA"
#define HUGEPAGE_FLAG   (MFD_HUGETLB | MFD_HUGE_1GB)
#define MEM_SIZE        (2L * 1024 * 1024)

#endif

int memfd;                                  // global descriptor for sealed memory
void *shared_memory;                        // Shared memory global pointer
size_t alloc_offset = 0;                            // We are assuming native EBA is trusted


#define BUFFER_RESERVED 0
#define BUFFER_AVAIL    1

typedef struct {
    size_t buffsize;
    void *buffaddr;
    int is_available;
} buffer_ent;

typedef struct {
    buffer_ent *allocated_buffers[MAX_BUFFERS]; // Arr of pointers
    int no_entries;
} buffer_table;

/* The manager state tracks all components of the manager */
typedef struct {
    union {
        int (* init_memory_shared)(const char *name, );
        int (* init_device_memory)(int fd, int op, ...);
    } memory_type;

    // ioctl and physical memory will point to different
        // operations lists
    void *ctx_operations;
    
    buffer_table *tbl_index;                // the allocation mgmt table
    
} memory_manager_state;

/**
 * This ensures the structs are in BSS memory to reserve Heap */
buffer_table *bufftab;

struct eba_zone {
    char zone_name[EBA_MEMORY_ZONE_SIZE];
    io_virtaddr_t io_virt_addr;
    union {
        void *addr;
        uint64_t addr64
    }

struct __eba_cache_aligned _pool {

    char pool_name[]
}

#ifdef __cplusplus
}
#endif

#endif //SLIMEBA_MEM
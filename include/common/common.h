
#ifndef LIBEBA_COMMON
#define LIBEBA_COMMON

#include <limits.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdalign.h>
//#include <sys/epoll.h>

#ifdef __cplusplus
exter "C" {
#endif

#ifndef typeof
#define typeof __typeof__
#endif

#ifndef __cplusplus
#ifndef asm
#define asm __asm__
#endif
#endif

#define EBA_IS_GNU 0
#if defined __clang__ 
#define EBA_CLANG
#elif defined __GNUC__
#define EBA_GCC
#undef EBA_IS_GNU
#define EBA_IS_GNU 1
#endif
#if EBA_IS_GNU
#define GCC_VERSION (__GNUC__ * 1000 + __GNUC_MINOR__ + 100 + \
                __GNUC_PATCHLEVEL__)



/**
 * * * * Mechanisms and Macros for Runtime Efficiency * * * *
 * 
 * 
 * the GCC builtin for prefetching has the following variadic form
 *      __builtin_prefetch(const void *addr, ...)
 * 
 * See: https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html for 
 * a detailed description.  Usage is pretty straightforward; the 
 * optional arguments include rw and locality.
 * 
 * rw is a compile-time constant.  The default value of zero indicates
 * preparation to read, while 1 indicates prepartion to write.
 * 
 * locality is a compile-time constant.  The four optional values are 
 * 0 - 3.  Temporal locality refers to the liklihood that a recently
 * access memory address will be accessed repeatedly in a short period.
 * 
 * 0    = No temporal  locality
 * 1    = low temporal locality
 * 2    = moderate temporal locality
 * 3    = high temporal locality
 * 
 */

// for struct packed
#define __eba_packed __attribute__((__packed__))

// mark type if ont type-based alias type
#define __eba_alias __attribute__((__may_alias__))

// mark weak reference
#define __eba_weak __attribute__((__weak__))

// mark function pure
#define __eba_pure __attribute__((pure))

// force symbol generation
#define __eba_used __attribute__((used))

//  mark function unused
#define __eba__unused __attribute__((__unused__))


// advisible to use -fprofile-arcs to confirm assumptions

// branch prediction, mark branch unlikely to be taken
#define unlikely(x) __builtin_expect(!!(x), 0);

// branch prediction, mark branch likely to be taken
#define likely(x)   __builint_expect(!!(x), 1);

/** Instruct compiler that return points to memory. */
#if defined(EBA_GCC) || defined(EBA_CLANG)
#define __eba_alloc_size(...)   \
        __attribute__((alloc_size(__VA_ARGS__)))
#else 
#define __eba_alloc_size(...)
#endif

#if defined(EBA_GCC) || defined(EBA_CLANG)
#define __eba_malloc __attribute__((__malloc__))
#else
#define __eba_malloc
#endif

#if defined(EBA_GCC) && defined(GCC_VERSION >= 110000)
#define __eba_dealloc(dealloc, no)  \
        __attribute__((malloc(dealloc, no)))
#else
#define __eba_dealloc(dealloc, no)
#endif

#define CACHE_LINE_SIZE     64
#define N_CACHE_LINE_SIZE   128
#define CACHE_LINE_SIZE_MIN 64

#define __eba_aligned(a) __attribute__((__aligned__(a)))

#define __eba_cache_aligned __eba_aligned(CACHE_LINE_SIZE)

#define __eba_cache_min_aligned __eba_aligned(CACHE_LINE_SIZE_MIN)

#define __slimeba_always_inline inline __attribute__((always_inline))

#define __slimeba_never_inline __attribute__((noinline))

#define __slimeba_cold __attribute__((cold))

#define __slimeba_hot __attribute__((hot))

// functions may need additional stack space 
#define _slimeba_align(x, a)   \
    if (!__builtin_has_attribute(typeof(x), aligned)) \
        x = __builtin_alloca_with_align(ALIGNMENT, a)

// Ensure pointers are always aligned
// #define always_alloc_align 

static inline void prefetch_writeH(const void *p)
{   // see prefetch comment on usage above on above

    // __builtin_prefetch(const void *addr, rw, locality)
    __builtin_prefetch(p, 1, 3); // high temporal locality
}

static inline void prefetch_writeM(const void *p)
{   // see prefetch comment on usage above on above

    // __builtin_prefetch(const void *addr, rw, locality)
    __builtin_prefetch(p, 1, 2); // moderate temporal locality
}

static inline void prefetch_writeL(const void *p)
{   // see prefetch comment on usage above on above

    // __builtin_prefetch(const void *addr, rw, locality)
    __builtin_prefetch(p, 1, 1); // low temporal locality
}

static inline void prefetch_writeN(const void *p)
{   // see prefetch comment on usage above on above

    // __builtin_prefetch(const void *addr, rw, locality)
    __builtin_prefetch(p, 1, 0); // no temporal locality
}


/** Mechanisms and Macros to support logging */
#define DEFAULT_LVL     RTE_LOG_ERR
#define MBUF_LOG        RTE_LOGTYPE_USER1
#define MEMPOOL_LOG     RTE_LOGTYPE_USER2
#define PACKET_LOG      RTE_LOGTYPE_USER3
#define PROCESSING_LOG  RTE_LOGTYPE_USER4

static void log_handler(void);

/** Mechanisms and Macros to support Latency */

#define LATENCY_MAX

struct latency_t {
    uint64_t stampl;
    uint64_t check;
    uint16_t type;
};

struct mbuf_private {

    char ts[32];
    struct latency_t;
};



/** Mechanisms and Macros for MBUF */

#define MBUF_RXQUEUE_SIZE       
#define MBUFPOOL_RESERVE_NUM (2 * get_global_cfg_params()->rxqueue_size + 1024);



/** Mechanisms and Macros for EBA Protocol stack */

#define EPOLL_MAX       512

#define MAXIMUM_QUEUES  8

#define TX_RING_LABEL   64
#define RX_RING_LABEL   64
#define MBUF_POOL_LABEL 64

#define MAX_PROCESS_NUM 30

#define MBUF_RING_SIZE  2048
#define MBUF_CACHE_SIZE 512
#define MBUF_PKT_BURST  32
#define MIN_POOL_SIZE   65536U

#define RX_INIT_RING_SIZE   1024
#define TX_INIT_RING_SIZE   1024

struct device_addr {
#define DEV_ADDR_BASE   0
#define DEV_ADDR_MAC    1
#define DEV_ADDR_PCI    2
    uint8_t addr_type;
    union addr_dev_addr {
        struct rte_ether_addr mac_addr;
        struct rte_pci_addr pci_addr;
    };
};

struct config {

    char log_file;

    struct {
        // dpdk configuration params
        uint8_t socket_num;
        uint64_t socket_size;
        uintptr_t base_virtual_address;
    };

    struct {
        uint16_t rxqueue_size;
        uint16_t txqueue_size;
        uint16_t no_queues;
    };

    struct {    // system stack
        uint8_t     dma;
        uint16_t    no_cpu;
        uint8_t     no_nic;

    };

};

struct eba_stack {
    struct rte_mempool *txrx_pool;
    struct rte_ring *rx_ring;
    struct rte_ring *tx_ring;

    struct rte_mempool *eba_data_pool;

    int32_t kernel_event;
    struct epoll_event kernel_event[EPOLL_MAX];
    uint16_t active_connections;
    
};

static struct config *global_configuration(void);     // get configurations

typedef uint64_t physical_addr_t;

typedef uint64_t io_virtaddr_t;

/** Error handling */

static void init_errhandler(int init_error);


#ifdef __cplusplus
}
#endif

#endif //LIBEBA_COMMON
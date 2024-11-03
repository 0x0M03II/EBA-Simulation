#include <errno.h>
#include <stdint.h>

#include "memory.h"
#include "common/common.h"
#include "jemalloc/jemalloc.h"

/** Memory implementation of error handler */
static void init_errhandler(int init_error)
{   // ascertain error and close descriptor

    switch (init_error) {
        case EPERM:     fprintf(stderr, "Insufficient priviliges to use MFD_HUGETLB\n"); break;
        case EFAULT:    fprintf(stderr, "Address points to invalid memory!\n"); break;
        case EINVAL:    fprintf(stderr, "EINVAL: See Man page for possibilities.\n"); break;
        case EMFILE:    fprintf(stderr, "Per-process limit on number of file descriptors met\n"); break;
        case ENOMEM:    fprintf(stderr, "Insufficient memory\n"); break;

        // Something is very wrong if this is printed
        default:        fprinf(stderr, "Unkown Error\n"), break;
    }

    close(memfd);   // memfd is globalized
    exit(1);        // there may be a better way to handle our exit
}


struct rte_mempool *create_slimeba_pool(cosnt char *name, 
                                        unsigned num_elements,
                                        unsigned mbuf_cache_size,
                                        uint16_t droom_size,
                                        int socketID)
{

    uint32_t res;
    struct rte_mempool *pool;

    // using goto exits to properly handle program exit
    if (unlikely(num_elements < MBUF_MAX_NUM)) { init_errhandler(errno); }

    uint16_t private = RTE_ALIGN(sizeof(struct mbuf_private), RTE_MBUF_PRIV_ALIGN);

    pool = rte_pktmbuf_pool_create(name, num_elements, mbuf_cache_size,
                                   droom_size, socketID);

    if (unlikely(pool == NULL)) { init_errhandler(errno); }

    return pool;
}

/** should not be called outside of translation unit */
static __slimeba_always_inline
void slimeba_bulk_allocation(struct rte_mempool *pool,
                                    struct rte_mbuf *mbufs,
                                    unsigned count)
{
    int res;
    res = rte_pktmbuf_alloc_bulk(pool, 
    
    if (unlikely(res != 0)) {
        RTE_LOG(LOG_LEVEL, MBUF_LOG, 
            "Failed to allocate memory!\n");
        return -1;
    }
}

struct rte_mbuf *slimeba_allocate_mbuf(struct rte_mempool *pool)
{

}
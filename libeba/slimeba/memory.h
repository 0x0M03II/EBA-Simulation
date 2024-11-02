
#ifndef SLIMEBA_MEM
#define SLIMEBA_MEM

#include <rte_ring.h>
#include <rte_mbuf.h>
#include <rte_mempool.h>


struct rte_mempool *create_slimeba_pool(cosnt char *name, 
                                        unsigned num_elements,
                                        unsigned mbuf_cache_size,
                                        uint16_t priv_size,
                                        uint16_t droom_size,
                                        int socketID);


#endif
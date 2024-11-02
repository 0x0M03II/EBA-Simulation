
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <errno.h>
#include <stdarg.h>
#include <inttypes.h>

#include <rte_common.h>
#include <rte_memory.h>
#include <rte_memzone.h>
#include <rte_eal.h>
#include <rte_byteorder.h>
#include <rte_launch.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_debug.h>
#include <rte_ring.h>
#include <rte_log.h>
#include <rte_mempool.h>
#include <rte_memcpy.h>
#include <rte_mbuf.h>
#include <rte_interrupts.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_malloc.h>
#include <rte_string_fns.h>
#include <rte_cycles.h>

#include "slimeba/memory_pinning.h"
#include "init.h"


/**
 * The custom EBA memory pool allocator, slimeba, that is provided in 
 * libeba integrates with mbufs.  EBA Node operators have the option
 * of using mbufs provided by DPDK, or the slimeba buffer allocator.
 * 
 * slimeba has functions for memory pool management and buffer allocation.
 * 
 */

memory_manager_state *pktpool;

int init(int argc, char *argv[])
{
    int retval;
    uint16_t i;
    const struct rte_memzone *mz;
    
    retval = rte_eal_init(argc, argv);
    if (retval < 0) return -1;

    argc -= retval;
    argv += retval;

    mz = rte_memzone_reserve(MZ_PORT_INFO, sizeof(*ports),
                            rte_socket_id(), NO_FLAGS);
    
    
}
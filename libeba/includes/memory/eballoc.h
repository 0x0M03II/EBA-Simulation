/** 
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 *
 * 
 * This memory allocator is a heavily modified version of Inte's
 * DPDK allocator to fit the specific needs of EBA.  That said,
 * this allocator IS based on the DPDK allocator; thus, it is 
 * fitting to include the original source code License / copyright
 * statement above.
 * 
*/

#ifndef _EBA_ALLOC_H
#define _EBA_ALLOC_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include "common/common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTE_PGSIZE_4K   (1ULL << 12)
#define RTE_PGSIZE_64K  (1ULL << 16)
#define RTE_PGSIZE_256K (1ULL << 18)
#define RTE_PGSIZE_2M   (1ULL << 21)
#define RTE_PGSIZE_16M  (1ULL << 24)
#define RTE_PGSIZE_256M (1ULL << 28)
#define RTE_PGSIZE_512M (1ULL << 29)
#define RTE_PGSIZE_1G   (1ULL << 30)
#define RTE_PGSIZE_4G   (1ULL << 32)
#define RTE_PGSIZE_16G  (1ULL << 34)

struct segment {
    io_virtaddr_t io_virtaddr;

    union {
        void *addr;
        uint64_t addr64;
    };
    
    size_t len;
    uint64_t hpsize; // hugepage
    uint32_t nrank;
    uint32_t flags;
} __eba_packed;

int rte_mem_lock_page(const void *vaddr);


#ifdef __cplusplus
}
#endif

#endif //_EBA_ALLOC_H
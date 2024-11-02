
#include "rte_dmadev.h"
#include "common/dma.h"


void log_handler(char *message)
{
#if defined(DMA_LOGLEVEL)
    RTE_LOG(DMA_LOGLEVEL, MBUF_LOG, 
            message);
#else
    RTE_LOG(DEFAULT_LVL, MBUF_LOG,
            message);
#endif
}

int init_dma(void)
{
    if (unlikely(rte_dma_info_get() < 0)) {
        log_handler()
        init_errhandler(int init_error)
    }
}
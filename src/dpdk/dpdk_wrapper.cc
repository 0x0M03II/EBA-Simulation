#include "dpdk_wrapper.h"


void dpdkint::DpdkWrapper::dpdk_initinitialize_port() {
    port_id = 0;

    /**
     * mbuf pool can actually replace the memory pool created by
     * EBA::NodeManager
     */
    mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", 8192, 250, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
    if (mbuf_pool == NULL) {
        rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");
    }

    if (rte_eth_dev_configure(port_id, 1, 1, &port_conf) < 0) {
        rte_exit(EXIT_FAILURE, "Cannot configure port\n");
    }

    /**
     * rx is accessed using dpdk API to access network packets
     * without CPU interrupts.
     */
    if (rte_eth_rx_queue_setup(port_id, 0, 128, rte_eth_dev_socket_id(port_id), NULL, mbuf_pool) < 0) {
        rte_exit(EXIT_FAILURE, "Cannot setup RX queue\n");
    }

    // starting the port here
    if (rte_eth_dev_start(port_id) < 0) {
        rte_exit(EXIT_FAILURE, "Cannot start port\n");
    }

    std::cout << "Port " << port_id << " initialized and started." << std::endl;
}

/**
 * This is our handler for packets received on the NIC.  The PMD
 * driver listens for incoming packets.
 */
void dpdkint::DpdkWrapper::receivePackets() {
    struct rte_mbuf *bufs[32]; // to mbuf

    // Poll the queue
    uint16_t nb_rx = rte_eth_rx_burst(port_id, 0, bufs, 32);
    if (nb_rx > 0) {

        /**
         * TODO: log these instead of print
         */
        std::cout << "Received " << nb_rx << " packets." << std::endl;

        for (uint16_t i = 0; i < nb_rx; i++) {
            struct rte_mbuf *mbuf = bufs[i];

            /*
                * DPDK uses rte_pktmbuf_mtod(mbuf, void*) to access packet data */
            processPacket(mbuf);

            rte_pktmbuf_free(mbuf);
        }
    }
}

/**
 * This is NOT the actual processing behavior, just dummy
 * behavior to help visualize the operation.  This will
 * be changed
 */
void dpdkint::DpdkWrapper::process_packet(struct rte_mbuf *mbuf) {
    void *data = rte_pktmbuf_mtod(mbuf, void*);
    uint16_t data_len = rte_pktmbuf_data_len(mbuf);

    std::cout << "Processing packet of length: " << data_len << std::endl;
}

// Same as above, dummy behavior to get basic structure
void dpdkint::DpdkWrapper::transmit_packets(struct rte_mbuf **bufs, uint16_t nb_tx) {
    uint16_t sent = rte_eth_tx_burst(port_id, 0, bufs, nb_tx);
    std::cout << "Transmitted " << sent << " packets." << std::endl;
}

// stop nic
void dpdkint::DpdkWrapper::stop_port() {
    rte_eth_dev_stop(port_id);
    rte_eth_dev_close(port_id);
    std::cout << "Port " << port_id << " stopped and closed." << std::endl;
}

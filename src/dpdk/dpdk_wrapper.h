// extern "C" { // include DPDK headers
//     #include <rte_eal.h>
//     #include <ret_mbuf.h>
// }

#ifndef DPDKWRAPER
#define DPDKWRAPER

#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_mbuf.h>
#include <stdexcept>
#include <iostream>


/**
 * @brief This is a DPDK wrapper to allow the OMNeT++ simulation to
 * use DPDK to create a more realistic EBA simulation.
 *
 * EBA Nodes can use use the DPDK Poll Mode Driver to catch packets
 * once recieved on the NIC.  This allows the application to access
 * rx & tx descriptors without neededing interrupts.
 *
 * Althought not true EBA, it is a more realistic simulation of what
 * EBA will look like in productin.
 */

namespace intdpdk
{

    struct rte_eth_conf port_conf = {
        .rxmode = {
            .max_rx_ptk_len = RTE_ETHER_MAX_LEN,
        },
    };

    class DpdkWrapper {
    private:
        uint16_t port_id;
        struct rte_mempool *mbuf_pool;

    public:
        DpdkWrapper(int argc, char **argv) {
            if (rte_eal_init(argc, argv) < 0) {
                throw std::runtime_error("Failed to initialize DPDK EAL");
            }
            initializePort();
        }


        // stop nic
        void stop_port();

        void initialize_port(void);

        void receive_packets(void);

        void process_packet(struct rte_mbuf *mbuf);

        void transmit_packets(struct rte_mbuf **bufs, uint16_t nb_tx);
    };



} // namespace intdpdk

#endif

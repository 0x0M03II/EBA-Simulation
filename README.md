> **_NOTE:_**  This is a work in progress and is meant to compile at this time.
> DPDK integration is not complete, and a network topology has not
> been created yet.  Furthermore, the DPDK processing behavior has
> not been developed and the EBA::NodeManager::NodeManager() should
> use mbuf instead of the simulated vector buffer.
> Lastly, the EBA operations don't exist, but this isn't necessary for
> simulation.
>
> The code structure will be changed to align with professional quality.  The 
> current source code simply illustrates the basic workflow.


# EBA Simulation

Implementation of a [python EBA simulation](https://github.com/DSMishler/EBA_models/tree/master) in OMNeT++.

## Basic Workflow

1. Node A requests a block from the Manager
2. Manager allocates from the pool, updates table to reflect ownership
3. Node A uses the block for operations
4. Node A's local queue is populated waiting for response
5. Node A informs manager that B should read
6. Node B reads and Responds following steps 1-4
7. Node A removes queued entries and can respond to Node B if necessary.
8. Update Route tables with state::neighbor information

## DPDK Integration
This is a DPDK wrapper to allow the OMNeT++ simulation to 
use DPDK to create a more realistic EBA simulation.

EBA Nodes can use use the DPDK Poll Mode Driver to catch packets
once recieved on the NIC.  This allows the application to access
rx & tx descriptors without neededing interrupts.

Althought not true EBA, it is a more realistic simulation of what
EBA will look like in productin.

### Opportunity for Research

* Compare CPU Cycles using DPDK
* Compare traffic throughput


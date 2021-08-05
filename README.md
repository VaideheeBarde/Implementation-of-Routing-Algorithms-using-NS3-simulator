# Implementation-of-Routing-Algorithms-using-NS3-simulator

1.) Installation software - NS-3
Programming languages used - C++, Python
VM Node - Ubuntu

2.) Simulation
- DSDV routing algorithm
- DSR routing algorithm
- AODV routing algorithm

3.) Parameters to be modified 
- number of packets
- range of nodes
- energy detection threshold (accept those packets that have energy/power more than threshold)
- cca threshold 

4.) Parameters to be calculated 
- average end-to-end delay to deliver a packet sent to the time the packet being received
- range of nodes
- number of packets received

5.) Post simulation - 4 outputs are generated
.routes file - file containing routing tables of each nodes.
.tr file - file containing traces of all the transmissions and receptions on the network.
.csv file - file containing a summary of successful transmitted packets.
.pcap file - files containing traffic through different nodes.

6.) Determination of successful packet transmission
- To determine when a packet is successfully received from a .tr file, we use the following criteria:
Every successful trace entry in the trace file should contain payload in the trace entry.
Node number of the transmitter must match the original source IP. Eg. if the transmitter node number is 8, the original source IP must be 9.
The receiver should have the same source address and the destination address as the valid receiver.
After this, match the sequence id of the transmitter and the receiver.
Then we also check if the packet is transmitted for the first time Tx(0) or the second time Tx(1).
 
7.) Conclusion
- We concluded that DSDV protocols transmits most packets successfully whereas DSR has the maximum and average delay.
- By changing the number of nodes and measuring the parameter values such as number of packets received, average end to end delay and average delay for all three routing protocols by keeping the speed of the node constant, we can see that on running the simulation, the average delay is higher in DSR followed by AODV and then DSDV. DSDV has lowest end-to-end delay in mobility.
- We also change the speed of the node, and measure the average and the maximum delay for all the three routing protocols and the results turn out to be the same in case of average and maximum delay.

# Implementation-of-Routing-Algorithms-using-NS3-simulator

Initially we installed NS-3 on our Linux based OS system. We run the given code for the simulation. After simulation, 4 outputs are generated. They are as follows.
A .routes file containing routing tables of each nodes.
A .tr file containing all the transmissions and receptions on the network.
A .csv file containing a summary of successful transmitted packets.
.pcap files containing traffic through different nodes.
The first simulation that we run is the simulation of an adhoc network using DSDV routing algorithm. That is obtained by uncommenting the code for the DSDV algorithm part. After we run the simulation, we obtain the .csv file which gives a summary of successfully transmitted packets.
A code called ‘parser.py’ was written that parsed this .csv file and calculated the total number of packets received during simulation.
We opened and read the csv file using csv.reader command. We created an empty list called parser in which we appended the number of packets received during simulation for each row. Also, we calculated the sum and printed both these results.
Next, we changed the range of nodes and also uncommented the energy detection threshold to -75 and the cca threshold to -76. Receivers will accept only those packets which will have energy/power more than threshold.
Now, we calculate the delay to deliver a packet. Time from the packet being sent to the time the packet being received.  We get this information from the trace file. We write a python code to extract the maximum delay and the average delay to deliver a packet from the trace file.
To determine when a packet is successfully received from a .tr file, we use the following criteria:
Every successful trace entry in the trace file should contain payload in the trace entry.
Node number of the transmitter must match the original source IP. Eg. if the transmitter node number is 8, the original source IP must be 9.
The receiver should have the same source address and the destination address as the valid receiver.
After this, match the sequence id of the transmitter and the receiver.
Then we also check if the packet is transmitted for the first time Tx(0) or the second time Tx(1).
Explanation of delay.py

Other routing protocols - 
Now we uncomment the appropriate function and use the flexibility of the code to switch between appropriate routing protocols. We run the simulation and both the python codes to calculate the number of packets and delays using the resulting files for both DSR and AODV protocols.
We concluded that DSDV protocols transmits most packets successfully whereas DSR has the maximum and average delay.
Number of users-
By changing the number of nodes and measuring the parameter values such as number of packets received, average end to end delay and average delay for all three routing protocols by keeping the speed of the node constant, we can see that on running the simulation, the average delay is higher in DSR followed by AODV and then DSDV. DSDV has lowest end-to-end delay in mobility.
Effect of speed:
We also change the speed of the node, and measure the average and the maximum delay for all the three routing protocols and the results turn out to be the same in case of average and maximum delay.

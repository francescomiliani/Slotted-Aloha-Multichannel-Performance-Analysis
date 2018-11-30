# Slotted Aloha Multichannel Performance Analysis

## Description

The project consists in modeling a slotted random-access network system, where N couples transmitter-receiver share the same communication medium, which consists of C separate channels. Multiple attempts to use the same channel in the same time-slot for different transmissions will lead to a collision, hence no receiver listening on that channel will be able to decode the corrupted message.
We assume that each one of the N transmitters generate packets according to an exponential interarrival distribution, and pick one channel at random for every new transmission. Before sending a packet, it will extract a value from a Bernoullian RV with success probability p on every time-slot, until it achieves success. 
Then it will transmit the packet and starts over. If a collision occurs, then the transmitter will wait for a random number of slots (explained later), and then starts over the whole Bernoullian experiment. The number of backoff slots (during which the transmitter waits before trying to retransmit) is extracted as U( 1, 2^(X+1) ), where x is the number of consecutive collisions experienced by the packet that is trying to be transmitted.
The goal of this paper is to study the network throughput and the packet delay as the system workload increases.
For a clearer analysis, the following assumptions are taken:
- The population U is infinite, where U is the number of packets that the transmitters can potentially receive: in fact, the number of packets across the network can be very large and potentially infinite.
- Each transmitter gets its packets from its own queue which dimension is infinite.
- FIFO is the chosen queue policy.
- The time-slot duration is modeled to allow each transmitter to possibly extract a channel, to test the Bernoullian RV, to potentially send only one packet and during the transmission checking whether a collision is happening.
- Channels are chosen by extracting a value from a uniform RV U~[1,C].
- Once a channel has been chosen for the transmission of one packet, the latter can only be transmitted on the said channel even in case of a collision.
- The “transmitter-receiver” couples are fixed and the communication is unidirectional: Txi will transmit only to Rxi .
- If a collision occurs, the interested packet will be inserted again ahead of the related queue.

---

## Simulator code explanation

In order to implement with OMNeT++ the system described above, the SRAWN model is composed by two Compound Modules, TransmitterCluster and ReceiverCluster, linked by C*N channels, implemented through directional connections.
TransmitterCluster contains two arrays of simple submodules: one for Generators and the other for Transmitters.
Each Transmitter will receive packets from its own generator and store them in its own queue, so the number of Generator’s and Transmitter’s instances are the same.
ReceiverCluster contains an array of simple Receiver submodules: the number of instantiated Receivers in equal to the number of Transmitters.

## Implementation

According to the model, each Transmitter can use one of the C channels to send packets to its own Receiver. We instantiated C connections, and so C interfaces, for each couple Transmitter-Receiver, between the TransmitterCluster and the ReceiverCluster compound modules.
TransmitterCluster
Each Transmitter can communicate with its own Receiver through a set of C connections with the output interfaces of the parent compound module.

### ReceiverCluster

Each Receiver receives packets through a set of C connections from the input interfaces of the parent compound module.

### Generator

The purpose of the Generator module is to create Packets with an exponential inter-arrival time, through a scheduleAt call, and sending them to its own Transmitter through the output gate.
Packet extends the cMessage class and it has two double values:
- “generation_time” - the simulation time at which it has been created;
- “transmission_time” - the simulation time spent by the packet in the system until it leaves the receiver.

### Transmitter

When the transmitter receives a new Packet from the input gate, it puts it into its own queue.
At the initialization and after each period, it sends to itself a slotBeep message denoting when a new time-slot starts.
If there is at least one Packet to send in the queue, the Transmitter will try to send it at the beginning of a new time-slot.
In the event that it is the first time that it tries to send that Packet, first it chooses one of the channels by picking up a value from the Uniform RV U~[1,C], by calling the C++ uniform function.
Then it tests the Bernoullian RV calling the C++ bernoulli function, whose probability variable is p (its value will be discussed later).
Failing the test means it will not transmit the Packet, so it will wait the next time-slot to test again the RV.
Otherwise, if a success occurs, it will increment the counter related to the chosen channel just to notify that it used it. Then it will send itself a “collisionDetectionBeep” message that will be received before the next time-slot ( due to simplicity: tslot / 2 ): at that time it will check if the sent packet caused a collision on the chosen channel by parsing the number of Transmitters which incremented the previously mentioned channel counter.
If the result is bigger than 1, it means that more than one Transmitter used the same channel during the same time-slot and a collision occurred: all the involved Transmitters will reinsert the engaged Packet in the head of the queue and will try to retransmit it after a backoff time.
Differently, the result is 1 and the Transmitter will effectively send out the packet through the output gate related to the channel.
The backoff time is calculated on the Uniform RV U~[1,2^(X+1)], through the C++ uniform function, where X is the number of successive collisions caused by the same packet.
Both X and the number of time-slots that the Transmitter has to wait are two INT variables allocated in the Transmitter’s instance.
At tslot * 3⁄4 an arbitrary Transmitter (called “chosen_one” and set in the .ini file) will reset the counters and will gather the channel_throughput statistics.
Finally, all the Transmitters will acquire queue_dimension and transmitter_throughput statistics. Receiver
The Receiver can receive incoming Packets from one of the C input gates connected to its parent module.
Once it received a Packet it will calculate its delay time by subtracting “generation_time” to “transmission_time” and will emit it through a signal.
Then it deletes it.

---

## Configuration file

The configuration file allows us to control the configuration’s variables regarding the simulations and it contains:

### SIMULATION’S FACTORS

- Simulation-time
- Warm-up period
- Number of Replicas
- Seed-set : repetition

### MODEL’S FACTORS

- Number of Transmitters
- Number of Receivers
- Number of Channels
- Exponential mean: the mean value of the inter- arrival time’s exponential distribution
- Slot Time
- Collision Detection Time • Clear Time
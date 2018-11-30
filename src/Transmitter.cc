//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without //EVen the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Transmitter.h"

Define_Module(Transmitter);

Transmitter::Transmitter() {

    // Initialize internal variables
    id = id_number++;
    slot_counter = 0;
    slot_to_wait = 0;

    clear_packet_variables();

    // Initialize the queue
    char queue_name[10] = "/0";
    sprintf(queue_name, "queue_%d", id);
    queue.setName(queue_name);

    // Initialize statistic's support variables
    sent_packets = 0;

}

Transmitter::~Transmitter() {

}

void Transmitter::initialize() {

    // Initialize beep messages.
    slotBeep = new cMessage("slot beep");
    collisionDetectionBeep = new cMessage("cd beep");
    clearBeep = new cMessage("clear beep");

    // Initialized internal variables [ depending from parameters ].
    local_channel_size = (int) par("channel_size");
    transmitter_size = (int) getParentModule()->par("transmitter_size");
    int total_channel_size = local_channel_size * transmitter_size;

    if (channels == 0) {
        channels = new int[total_channel_size];
        for (int i = 0; i < total_channel_size; i++)
            channels[i] = 0;
    }

    if (channel_successful_slot_counter_array == 0) {
        channel_successful_slot_counter_array = new int[local_channel_size];
        for (int i = 0; i < local_channel_size; ++i)
            channel_successful_slot_counter_array[i] = 0;
    }

    // Initialize bernoullian probability.
    //bernoullian_prob = 0.3678;    1/e : for C = 1
    if (local_channel_size > transmitter_size)
        bernoullian_prob = 1;    //C>T
    else
        bernoullian_prob = local_channel_size / (double) transmitter_size;
    //EV << "Bernoullian Prob:\t" << bernoullian_prob << endl;

    // The transmitter starts to work by sending it self a synchronization message
    sendSlotBeep();

    //REGISTERING SIGNALS
    // - Throughput for each channel

    if (channel_throughput_array == 0)
        channel_throughput_array = new simsignal_t[local_channel_size];

    simsignal_t signal;    //temp signal
    cProperty *statisticTemplate;

    for (int i = 0; i < local_channel_size; ++i) {

        char signalName[32];

        sprintf(signalName, "channel_throughput%d", i);

        signal = registerSignal(signalName);
        statisticTemplate = getProperties()->get("statisticTemplate",
                "channel_throughput_signal");
        getEnvir()->addResultRecorders(this, signal, signalName,
                statisticTemplate);
        channel_throughput_array[i] = registerSignal(signalName);
    }


    // - queue dimension per slot time

    queue_dimension_per_slot_time_signal = registerSignal(
            "queue_dimension_per_slot_time");

    // - throughput of the transmitter

    transmitter_throughput_signal = registerSignal("transmitter_throughput");
}

/**
 * This function handles the received messages
 */
void Transmitter::handleMessage(cMessage *msg) {

    if (msg == slotBeep) {

        handleBeepMessage();
    }

    else if (msg == collisionDetectionBeep) {

        cancelEvent(msg);
        handleCDMessage();
    }

    else if (msg == clearBeep) {

        cancelEvent(msg);
        handleClearMessage();
    }

    else if (strcmp(msg->getName(), packetName) == 0) {

        handlePacketMessage(msg);
    }
}

void Transmitter::finish() {

    // Cancel and delete the messages and the related //EVents.
    cancelAndDelete(slotBeep);
    cancelAndDelete(collisionDetectionBeep);
    cancelAndDelete(clearBeep);

    // Clear the queue
    while (!queue.isEmpty())
        delete queue.pop();

    // Delete global variables.
    if (par("choosen_one")) {
        delete[] channels;
        delete[] channel_successful_slot_counter_array;
        delete[] channel_throughput_array;
    }

}

// SELF-SENT MESSAGES

void Transmitter::sendSlotBeep() {

    double time = getParentModule()->par("slot_time");
    scheduleAt(simTime() + time, slotBeep);
}

void Transmitter::sendCDBeep() {

    double time = getParentModule()->par("collision_detection_time");
    scheduleAt(simTime() + time, collisionDetectionBeep);
}

void Transmitter::sendClearBeep() {

    double time = getParentModule()->par("clear_time");
    scheduleAt(simTime() + time, clearBeep);
}

// ********** HANDLING FUNCTIONS **********

void Transmitter::handleBeepMessage() {

    if (!queue.isEmpty()) {

        // Get the pointer of the first element from the queue without unlinking it
        Packet* packet_to_send = check_and_cast<Packet*>(queue.front());

        // New packet
        if (get_extracted_channel() == -1) {

            //Uniform returns a value between 0(included) and local_channel_size EXCLUDED therefore not returnable
            int extracted_channel = uniform(0, local_channel_size);

            EV << "EXTRACTED CHANNEL: " << extracted_channel << endl;

            // Update the channel extracted for the packet.
            set_extracted_channel(extracted_channel);

            // Once the channel is choosen try to transmit the packet.
            transmit(packet_to_send);
        }

        else {

            // In this case the channel has already been chosen: this means that the extracted packet
            // already caused a collision or that the Bernoullian RV previously extracted a failure.
            // Check if it has to wait due to a collision: the packet must to attend yet because slot_to_wait must be 0

            if (slot_to_wait > 0) {

                slot_to_wait--;
            }

            else {

                // slot_to_wait = 0 :
                // - test the bernoullian variable to transmit and try to transmit the packet.

                transmit(packet_to_send);

            }

        }
    }

    // Cancel the related event and send the synch beep again to notify the next slot time
    cancelEvent(slotBeep);
    sendSlotBeep();

    // Increment the number of slots

    slot_counter++;

    EV << "SLOT NUMBER: " << slot_counter << endl;

    // If I am the chosen one, then do my duty
    if (par("choosen_one"))
        sendClearBeep();

    // GATHERING SIGNAL:
    // - QUEUE DIMENSION PER SLOT TIME
    emit(queue_dimension_per_slot_time_signal, queue.getLength());

    // - TRANSMITTER's THROUGHPUT
    double tr_th = sent_packets/(double)slot_counter;
    emit(transmitter_throughput_signal,tr_th);

}

void Transmitter::handleCDMessage() {

    Packet* packet_to_check = check_and_cast<Packet*>(queue.front());
    collisionDetection(packet_to_check);
}

/*
 * The chosen one transmitter has to gather the throughput of the channels
 * and it has to clear the shared data structures.
 */
void Transmitter::handleClearMessage() {

    // STATISTICS: calculate the throughtput for each channel.

    EV << "SLOT: " << slot_counter << endl;

    for (int i = 0; i < local_channel_size; i++) {

        double th_i = ((double) channel_successful_slot_counter_array[i])
                / ((double) slot_counter);

        EV << "THROUGHPUT [" << i << "] " << th_i << endl;

        // Drop data before the warm-up period
        if(simTime() > getSimulation()->getWarmupPeriod())
            emit(channel_throughput_array[i], th_i);
    }

    clearChannels();

    EV << "CLEARED BEEP" << endl;
}

/**
 * Each transmitter has to handle its own packet when a collision occurs.
 */
void Transmitter::collisionDetection(Packet* p) {

    //EV << "CHECKING FOR A COLLISION ON CHANNEL " << get_extracted_channel()<< endl;

    int transmission_number_per_channel = 0;

    //Check there is just once transmitter who sends into the current slot
    for (int i = 0; i < transmitter_size; i++) {
        transmission_number_per_channel += channels[i * local_channel_size
                + get_extracted_channel()];    //If 1 adds a contribute, if 0 no
    }

    print_channel_matrix();

    if (transmission_number_per_channel > 1) {

        EV << "COLLISION DETECTED" << endl;

        // Update the number of slot time to wait to retry to transmit.
        slot_to_wait = backoffCalculator(get_collision_number());

        // Increment the number of collisions generated by that packet.
        increment_collision_number();

        EV << "BACKOFF: " << slot_to_wait << endl;
        EV << "COLLISION NUMBER: " << get_collision_number() << endl;
    }

    else {

        EV << "NO COLLISION DETECTED" << endl;

        // The transmitter sends the message only if no collision occurred.

        if (!queue.isEmpty() && get_extracted_channel() != -1) {

            Packet* packet = check_and_cast<Packet*>(queue.pop());
            send(packet, "channel_array", get_extracted_channel());

        }

        // Increment the number of slots in which there has been a success for the interested channel.
        channel_successful_slot_counter_array[get_extracted_channel()]++;

        // Increment the number of sent packets.
        sent_packets++;

        // Clear the variables that handle the packets: they will be used for the next packet.
        clear_packet_variables();

    }

}

void Transmitter::clearChannels() {

    EV << "CLEAR IN PROGRESS" << endl;

    for (int i = 0; i < (local_channel_size * transmitter_size); i++)
        channels[i] = 0;

    //Check correctness
    print_channel_matrix();

}

void Transmitter::handlePacketMessage(cMessage* msg) {

    // This handles the messages coming from its own generator.
    Packet* received_packet = check_and_cast<Packet*>(msg);

    // Insert the arrived packet in the BACK of the queue
    queue.insert(received_packet);

    //EV << "TRANSMITTER ID: " << this->getId() << "   QUEUE DIMENSION:   "<< queue.getLength() << endl;
}

void Transmitter::transmit(Packet* p) {

    // Test the bernoullian variable: if "true" try to transmit, otherwise be quite.
    if (bernoullianTest()) {

        EV << "BERNOULLIAN TEST: SUCCESS   ON CHANNEL "
                  << get_extracted_channel() << endl;

        //Increment the transmission number over this channel
        channels[id * local_channel_size + get_extracted_channel()]++;

        // Update the time at which the packet has been sent.

        p->setTransmission_time(SIMTIME_DBL(simTime()));

        /*
         * it checks for a collision whenever a transmitter tries to transmit a packet into a certain slot
         */

        sendCDBeep();

        return;

    }

    EV << "BERNOULLIAN TEST: FAILED   ON CHANNEL " << get_extracted_channel()
              << endl;

}

/* PROBABILITY FUNCTIONS */

/**
 * It extracts a Bernoullian probability.
 * It returns:
 * - true: if the value is equal to 1
 * - false: otherwise
 */
bool Transmitter::bernoullianTest() {

    return (bernoulli(bernoullian_prob) == 1) ? true : false;
}

int Transmitter::backoffCalculator(int collision_number) {

    double range = pow(2, (collision_number + 1));
    EV << "BACKOFF RANGE: " << "[  1  ;  " << range << "  ]" << endl;
    int time_to_wait = (int) uniform(1, range);

    return time_to_wait;
}

/* DEBUG FUNCTION: It prints the status of the channels. */

void Transmitter::print_channel_matrix() {
    for (int i = 0; i < transmitter_size; i++) {
        EV << endl << "Tx: " << i << " | ";
        for (int j = 0; j < local_channel_size; j++) {
            EV << "C_" << j << ":" << channels[i * local_channel_size + j]
                      << " | ";
        }
    }
}

// Packet's support functions

void Transmitter::clear_packet_variables() {

    extracted_channel = -1;
    collision_number_per_packet = 0;
}

void Transmitter::set_extracted_channel(int ch) {

    extracted_channel = ch;
}

void Transmitter::set_collision_number(int cll) {

    collision_number_per_packet = cll;
}

void Transmitter::increment_collision_number() {

    collision_number_per_packet++;
}

int Transmitter::get_extracted_channel() {

    return extracted_channel;
}

int Transmitter::get_collision_number() {

    return collision_number_per_packet;
}

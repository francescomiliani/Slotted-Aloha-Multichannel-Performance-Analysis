//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __SRAWN_TRANSMITTER_H_
#define __SRAWN_TRANSMITTER_H_

#include <omnetpp.h>
#include <math.h>

#include "Utility"
#include "Packet_m.h"
using namespace omnetpp;


// It allows to assign an ID to each Transmitter.

static int id_number = 0;

// Global variables:
// - array of integers: it takes into account the number of transmitters involved in a communication on a certain channel
// - array of integers: it supports the signal below.
// - array of signals:  it records the throughput of each channel.

int *channels = 0;
int *channel_successful_slot_counter_array = 0;
simsignal_t* channel_throughput_array = 0;

// Class.

class Transmitter: public cSimpleModule {
public:
    Transmitter();
    virtual ~Transmitter();

protected:

   // Self sent messages:
   // - sent periodically to it self to notify that a new slot-time is starting
   // - sent after slot-time/2 after a new slot-time occurse to check if the messages it sent caused any collision
   // - sent periodically only if it is the "chosen_one": it clears the support shared variables.
    cMessage *slotBeep;
    cMessage *collisionDetectionBeep;
    cMessage *clearBeep;

    cQueue queue;

    int id;                             //  Identifier of each transmitter
    int slot_to_wait;                   //  Slot number to wait before trying to transmit
    int local_channel_size;             //  Number of channels for the actual transmitter
    int transmitter_size;               //  Whole number of transmitters
    int slot_counter;                   //  Counter that increments each time a new slot-time occurs

    int extracted_channel;              //  The channel that has been extracted to send the current packet
    int collision_number_per_packet;    //  The number of collisions caused by the current packet
    double bernoullian_prob;

    // Variables for statistics:
    // - queue dimension per slot time.
    // - throughput of the transmitter.

    simsignal_t queue_dimension_per_slot_time_signal;

    simsignal_t transmitter_throughput_signal;
    double sent_packets;

protected:

    virtual void initialize();
    virtual void handleMessage( cMessage *msg );
    virtual void finish();

    void sendSlotBeep();
    void sendCDBeep();
    void sendClearBeep();

    // Packet's support functions
    void clear_packet_variables();
    void set_extracted_channel(int ch);
    void set_collision_number(int cll);
    void increment_collision_number();
    int get_extracted_channel();
    int get_collision_number();

    // Handle functions
    void handleBeepMessage();
    void handleCDMessage();
    void handleClearMessage();
    void handlePacketMessage( cMessage* msg );

    void transmit( Packet* p );
    void collisionDetection ( Packet* p );  //It checks if a collision occurs on a certain channel
    void clearChannels();

    bool bernoullianTest();
    int backoffCalculator( int collision_number );

    void print_channel_matrix();

};

#endif

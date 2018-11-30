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

#include "Generator.h"

Define_Module(Generator);


void Generator::initialize()
{

    generation_event_message = new cMessage("generation event message");
    scheduleAt( simTime(), generation_event_message );
}

void Generator::finish() {

    cancelAndDelete( generation_event_message );
}


void Generator::handleMessage(cMessage *msg)
{
    //If the arrived msg is equal to timer message -> OK, otherwise BLOCKS ALL!
    ASSERT( msg == generation_event_message );

    // Cancel event to free memory
    cancelEvent(msg);

    // Time to wait before generating and sending next packet, calculated with an exponential distribution.
    double mean_time = par("exponential_send_mean_time").doubleValue();
    double exp_time = exponential(mean_time);


    packetToSend = new Packet(packetName);
    packetToSend->setGeneration_time(SIMTIME_DBL(simTime()));           // Store the time at which the packet has been generated.
    packetToSend->setTransmission_time(0);                              // Prepare the time at which the packet will be sent by the transmitter.


    send( packetToSend, "out" );

    // Create next packet's generation event.
    scheduleAt( simTime() + exp_time, generation_event_message );

}

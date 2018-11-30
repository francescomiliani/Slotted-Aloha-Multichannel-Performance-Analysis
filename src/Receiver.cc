
#include "Receiver.h"

Define_Module(Receiver);

void Receiver::initialize()
{
    // Register at omnet++ the signal.
    delay_time_signal = registerSignal("delay_time");
}

void Receiver::handleMessage(cMessage *msg)
{

    // Calculate the delay-time of the received packet.
    Packet* p = check_and_cast<Packet*>(msg);
    simtime_t packet_delay = (simtime_t)(p->getTransmission_time() - p->getGeneration_time());

    // Emit the delay-time of the received packet.
    emit( delay_time_signal, packet_delay );


    // Get measurement unit of the slot time.
    const char* measurement_unit = getSimulation()->getModuleByPath("SRAWN.transmitter_cluster")->par("slot_time").getUnit();

    // Print statistics informations for debugging.
    //EV<<"GEN: "<<p->getGeneration_time()<<measurement_unit<<" TRANS: "<<p->getTransmission_time()<<measurement_unit<<" DELAY: "<<packet_delay<<measurement_unit<<endl;

    //Deleting of the received message
    delete p;
}

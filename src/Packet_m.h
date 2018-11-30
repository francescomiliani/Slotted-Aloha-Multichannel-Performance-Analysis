//
// Generated file, do not edit! Created by nedtool 5.0 from Packet.msg.
//

#ifndef __PACKET_M_H
#define __PACKET_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0500
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>Packet.msg:3</tt> by nedtool.
 * <pre>
 * message Packet
 * {
 *     double generation_time;
 *     double transmission_time;
 * }
 * </pre>
 */
class Packet : public ::omnetpp::cMessage
{
  protected:
    double generation_time;
    double transmission_time;

  private:
    void copy(const Packet& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Packet&);

  public:
    Packet(const char *name=nullptr, int kind=0);
    Packet(const Packet& other);
    virtual ~Packet();
    Packet& operator=(const Packet& other);
    virtual Packet *dup() const {return new Packet(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b);

    // field getter/setter methods
    virtual double getGeneration_time() const;
    virtual void setGeneration_time(double generation_time);
    virtual double getTransmission_time() const;
    virtual void setTransmission_time(double transmission_time);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Packet& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Packet& obj) {obj.parsimUnpack(b);}


#endif // ifndef __PACKET_M_H


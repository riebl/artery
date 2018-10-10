//
// Generated file, do not edit! Created by nedtool 5.4 from /home/vadym/artery/src/artery/messages/ChannelLoadReport.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __CHANNELLOADREPORT_M_H
#define __CHANNELLOADREPORT_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
    #include <vanetza/dcc/channel_load.hpp>
// }}

/**
 * Class generated from <tt>/home/vadym/artery/src/artery/messages/ChannelLoadReport.msg:22</tt> by nedtool.
 * <pre>
 * packet ChannelLoadReport
 * {
 *     vanetza::dcc::ChannelLoad ChannelLoad;
 * }
 * </pre>
 */
class ChannelLoadReport : public ::omnetpp::cPacket
{
  protected:
    vanetza::dcc::ChannelLoad ChannelLoad;

  private:
    void copy(const ChannelLoadReport& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ChannelLoadReport&);

  public:
    ChannelLoadReport(const char *name=nullptr, short kind=0);
    ChannelLoadReport(const ChannelLoadReport& other);
    virtual ~ChannelLoadReport();
    ChannelLoadReport& operator=(const ChannelLoadReport& other);
    virtual ChannelLoadReport *dup() const override {return new ChannelLoadReport(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual vanetza::dcc::ChannelLoad& getChannelLoad();
    virtual const vanetza::dcc::ChannelLoad& getChannelLoad() const {return const_cast<ChannelLoadReport*>(this)->getChannelLoad();}
    virtual void setChannelLoad(const vanetza::dcc::ChannelLoad& ChannelLoad);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const ChannelLoadReport& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, ChannelLoadReport& obj) {obj.parsimUnpack(b);}


#endif // ifndef __CHANNELLOADREPORT_M_H


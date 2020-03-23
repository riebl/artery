#ifndef OTS_GTUSINK_H_2RZI4UCS
#define OTS_GTUSINK_H_2RZI4UCS

#include "ots/GtuObject.h"

namespace ots
{

class GtuSink
{
public:
    virtual void initialize(const GtuObject&) = 0;
    virtual void update(const GtuObject&) = 0;
    virtual ~GtuSink() = default;
};

} // namespace ots

#endif /* OTS_GTUSINK_H_2RZI4UCS */


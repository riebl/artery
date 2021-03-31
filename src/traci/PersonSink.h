#ifndef PERSONSINK_H_4K7OHKT6
#define PERSONSINK_H_4K7OHKT6

#include <memory>

namespace traci
{

class API;
class PersonCache;

class PersonSink
{
public:
    virtual void initializeSink(std::shared_ptr<API>, std::shared_ptr<PersonCache>, const Boundary&) = 0;
    virtual void initializePerson(const TraCIPosition&, TraCIAngle, double speed) = 0;
    virtual void updatePerson(const TraCIPosition&, TraCIAngle, double speed) = 0;
    virtual ~PersonSink() = default;
};

} // namespace traci

#endif /* PERSONSINK_H_4K7OHKT6 */


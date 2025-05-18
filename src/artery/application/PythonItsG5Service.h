#include "artery/application/ItsG5BaseService.h"


namespace artery
{

class PythonItsG5Service : public ItsG5BaseService
{
public:
    PythonItsG5Service();

    virtual bool requiresListener() const;
    virtual void trigger();
    void addTransportDescriptor(const TransportDescriptor& td);
};

}  // namespace artery
#ifndef ARTERY_PYTHONITSG5SERVICE_H_
#define ARTERY_PYTHONITSG5SERVICE_H_

#include "artery/application/ItsG5BaseService.h"

#include <memory>
#include <string>


namespace artery
{

class PythonItsG5Service : public ItsG5BaseService
{
public:
    PythonItsG5Service();

    virtual bool requiresListener() const override;
    virtual void trigger() override;

    // omnetpp::cSimpleModule
    int numInitStages() const override;
    virtual void initialize(int stage) override;

    void addTransportDescriptor(const TransportDescriptor& td);

protected:
    void finish() override;

private:
    void initializeInterpreter();
    void initializeModules();
    void extendPythonPath(const std::string& filepath);

private:
    struct PythonContext;

    std::unique_ptr<PythonContext> mPythonContext;
};

}  // namespace artery

#endif /* ARTERY_PYTHONITSG5SERVICE_H_ */
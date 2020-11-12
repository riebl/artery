#ifndef ARTERY_POINTERCHECK_H_U9GMXBHO
#define ARTERY_POINTERCHECK_H_U9GMXBHO

#include <omnetpp/cexception.h>
#include <omnetpp/simutil.h>
#include <memory>

namespace artery
{

template<typename T>
T* notNullPtr(T* ptr)
{
    if (!ptr) {
        throw omnetpp::cRuntimeError("%s: nullptr violation", omnetpp::opp_typename(typeid(T)));
    }
    return ptr;
}

template<typename T>
T* notNullPtr(const std::unique_ptr<T>& ptr)
{
    return notNullPtr(ptr.get());
}

template<typename T>
T* notNullPtr(const std::shared_ptr<T>& ptr)
{
    return notNullPtr(ptr.get());
}

} // namespace artery

#endif /* ARTERY_POINTERCHECK_H_U9GMXBHO */


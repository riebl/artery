#pragma once

#include <artery/sionna/bridge/Defaulted.h>
#include <artery/sionna/bridge/capabilities/Core.h>

#include <type_traits>
#include <utility>

namespace artery::sionna::py
{

class DefaultedDeferredFactoryCapability
{
protected:
    template <typename>
    using ModuleResolver = const char* (*)();

    template <typename>
    using ModuleAndClassResolver = std::pair<const char*, const char*> (*)();

    template <typename T, typename Resolver>
    static DefaultedWithDeferredResolution<T, std::decay_t<Resolver>> makeDeferredDefaulted(const char* name, Resolver&& resolver)
    {
        return DefaultedWithDeferredResolution<T, std::decay_t<Resolver>>(name, std::forward<Resolver>(resolver));
    }
};

class SIONNA_BRIDGE_API DefaultedModuleProviderCapability : public virtual IPythonModuleIdentityCapability, protected DefaultedDeferredFactoryCapability
{
protected:
    template <typename T>
    static const char* moduleResolverFn()
    {
        T obj;
        return obj.moduleName();
    }

    template <typename T>
    static ModuleResolver<T> moduleResolver()
    {
        return &moduleResolverFn<T>;
    }
};

class SIONNA_BRIDGE_API DefaultedClassProviderCapability : public virtual IPythonClassIdentityCapability, protected DefaultedDeferredFactoryCapability
{
protected:
    template <typename T>
    static std::pair<const char*, const char*> moduleAndClassResolverFn()
    {
        T obj;
        return std::make_pair(obj.moduleName(), obj.className());
    }

    template <typename T>
    static ModuleAndClassResolver<T> moduleAndClassResolver()
    {
        return &moduleAndClassResolverFn<T>;
    }
};

}  // namespace artery::sionna::py

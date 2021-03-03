package ots;

module BasicGtuLifecycleController like GtuLifecycleController
{
    parameters:
        @class(BasicGtuLifecycleController);
        volatile double insertionDelay = default(uniform(0s, 0.1s)) @unit(s);

    submodules:
        creationPolicy: <default("UniformGtuCreationPolicy")> like GtuCreationPolicy {
        }
}

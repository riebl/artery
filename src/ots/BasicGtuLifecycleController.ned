package ots;

module BasicGtuLifecycleController like GtuLifecycleController
{
    parameters:
        @class(BasicGtuLifecycleController);

    submodules:
        creationPolicy: <default("UniformGtuCreationPolicy")> like GtuCreationPolicy {
        }
}

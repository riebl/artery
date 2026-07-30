#include "Constants.h"

using namespace artery::sionna;

const char* py::Constants::moduleName() const
{
    return "sionna.rt.constants";
}

const char* py::IntersectionTypes::className() const
{
    return "InteractionType";
}

const DefaultedWithDeferredResolution<typename mi::Float64, py::DefaultedModuleProviderCapability::ModuleResolver<py::Constants>>
    py::Constants::defaultThickness{"DEFAULT_THICKNESS", moduleResolver<py::Constants>()};

const DefaultedWithDeferredResolution<typename mi::Int32, py::DefaultedClassProviderCapability::ModuleAndClassResolver<py::IntersectionTypes>>
    py::IntersectionTypes::none{"NONE", moduleAndClassResolver<py::IntersectionTypes>()};

const DefaultedWithDeferredResolution<typename mi::Int32, py::DefaultedClassProviderCapability::ModuleAndClassResolver<py::IntersectionTypes>>
    py::IntersectionTypes::specular{"SPECULAR", moduleAndClassResolver<py::IntersectionTypes>()};

const DefaultedWithDeferredResolution<typename mi::Int32, py::DefaultedClassProviderCapability::ModuleAndClassResolver<py::IntersectionTypes>>
    py::IntersectionTypes::diffuse{"DIFFUSE", moduleAndClassResolver<py::IntersectionTypes>()};

const DefaultedWithDeferredResolution<typename mi::Int32, py::DefaultedClassProviderCapability::ModuleAndClassResolver<py::IntersectionTypes>>
    py::IntersectionTypes::refraction{"REFRACTION", moduleAndClassResolver<py::IntersectionTypes>()};

const DefaultedWithDeferredResolution<typename mi::Int32, py::DefaultedClassProviderCapability::ModuleAndClassResolver<py::IntersectionTypes>>
    py::IntersectionTypes::diffraction{"DIFFRACTION", moduleAndClassResolver<py::IntersectionTypes>()};

#ifndef PYTHONMODULE_H_
#define PYTHONMODULE_H_

#include "artery/storyboard/SpeedEffectFactory.h"
#include "artery/storyboard/Story.h"
#include "artery/storyboard/PolygonCondition.h"
#include "artery/storyboard/TimeCondition.h"
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>

namespace python = boost::python;

/**
 * IterableConverter
 * Converts a Python list to a C++ vector
 */
struct iterable_converter
{
    //Registers converter from a python interable type to the provided type.
    template<typename Container>
    iterable_converter& from_python()
    {
        boost::python::converter::registry::push_back(
            &iterable_converter::convertible,
            &iterable_converter::construct<Container>,
            boost::python::type_id<Container>());

        // Support chaining.
        return *this;
    }

    //Check if PyObject is iterable.
    static void* convertible(PyObject* object)
    {
        return PyObject_GetIter(object) ? object : NULL;
    }

    //Convert iterable PyObject to C++ container type.
    //
    //Container Concept requirements:
    //
    //* Container::value_type is CopyConstructable.
    //* Container can be constructed and populated with two iterators.
    //    I.e. Container(begin, end)
    template<typename Container>
    static void construct(PyObject* object, boost::python::converter::rvalue_from_python_stage1_data* data)
    {
        namespace python = boost::python;
        // Object is a borrowed reference, so create a handle indicting it is
        // borrowed for proper reference counting.
        python::handle<> handle(python::borrowed(object));

        // Obtain a handle to the memory block that the converter has allocated
        // for the C++ type.
        typedef python::converter::rvalue_from_python_storage<Container> storage_type;
        void* storage = reinterpret_cast<storage_type*>(data)->storage.bytes;

        typedef python::stl_input_iterator<typename Container::value_type> iterator;

        // Allocate the C++ type into the converter's memory block, and assign
        // its handle to the converter's convertible variable.  The C++
        // container is populated by passing the begin and end iterators of
        // the python object to the container's constructor.
        new (storage)Container(
            iterator(python::object(handle)), // begin
            iterator());                // end
        data->convertible = storage;
    }
};

/**
 * Python Wrapper for the Effect Interface
 */
struct EffectWrap : Effect, python::wrapper<Effect>
{
    EffectWrap(Story* story, Veins::TraCIMobility* car) :
        Effect(story, car)
    {
    }

    void applyEffect()
    {
        this->get_override("applyEffect") ();
    }
    void reapplyEffect()
    {
        this->get_override("reaplyEffect") ();
    }
    void removeEffect()
    {
        this->get_override("removeEffect") ();
    }
};

/**
 * Python Wrapper for the EffectFactory Interface
 */
struct EffectFactoryWrap : EffectFactory, python::wrapper<EffectFactory>
{
    std::shared_ptr<Effect> create(Veins::TraCIMobility*, Story*)
    {
        return this->get_override("create") ();
    }
};

/**
 * Python Wrapper for the Condition Interface
 */
struct ConditionWrap : Condition, python::wrapper<Condition>
{
    bool testCondition(Veins::TraCIMobility* car)
    {
        return this->get_override("testCondition") ();
    }
};

BOOST_PYTHON_MODULE(storyboard) {

    /**
     * Storyboard related classes
     */
    python::class_<Storyboard, Storyboard*, boost::noncopyable>("Storyboard", python::no_init)
    .def("registerStory", &Storyboard::registerStory);

    /**
     * Iterable Converter
     */
    iterable_converter()
    .from_python<std::vector<Condition*> >()
    .from_python<std::vector<EffectFactory*> >()
    .from_python<std::vector<Coord> >();

    /**
     * Condition related classes
     */
    python::class_<ConditionWrap, ConditionWrap*, boost::noncopyable>("Condition");

    python::class_<PolygonCondition, PolygonCondition*, python::bases<Condition> >("PolygonCondition", python::init<std::vector<Coord> >());

    python::class_<TimeCondition, TimeCondition*, python::bases<Condition> >("TimeCondition", python::init<SimTime, SimTime>())
    .def(python::init<SimTime>());

    /**
     * Effect related classes
     */
    python::class_<EffectWrap, EffectWrap*, boost::noncopyable>("Effect", python::init<Story*, Veins::TraCIMobility*>());

    python::class_<EffectFactoryWrap, EffectFactoryWrap*, boost::noncopyable>("EffectFactory");

    python::class_<SpeedEffectFactory, python::bases<EffectFactory> >("SpeedEffectFactory", python::init<double>());

    /**
     * Story related classes
     */
    python::class_<Story, std::shared_ptr<Story> >("Story", python::init<std::vector<Condition*>, std::vector<EffectFactory*>>());

    /**
     * Miscellaneous classes
     */
    python::class_<Coord>("Coord", python::init<double, double, double>());

    python::class_<SimTime>("SimTime", python::init<double>());

}


#endif /* PYTHONMODULE_H_ */

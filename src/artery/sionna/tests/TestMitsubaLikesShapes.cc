#include <gtest/gtest.h>
#include <mitsuba/core/config.h>
#include <mitsuba/core/fresolver.h>
#include <mitsuba/core/fwd.h>
#include <mitsuba/core/object.h>
#include <mitsuba/core/parser.h>
#include <mitsuba/core/plugin.h>
#include <mitsuba/core/ray.h>
#include <mitsuba/core/string.h>
#include <mitsuba/core/transform.h>
#include <mitsuba/core/vector.h>
#include <mitsuba/python/python.h>
#include <mitsuba/render/film.h>
#include <mitsuba/render/fwd.h>
#include <mitsuba/render/integrator.h>
#include <mitsuba/render/scene.h>
#include <nanobind/stl/bind_vector.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/string_view.h>
#include <nanobind/stl/vector.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>

namespace nb = nanobind;

static std::function<void()> develop_callback_fn = nullptr;
static void develop_callback()
{
    if (develop_callback_fn)
        develop_callback_fn();
}

TEST(SceneTest, Scalar_RGB)
{
    using Float = float;
    using Spectrum = mitsuba::Color<Float, 3>;

    using namespace mitsuba;
    MI_IMPORT_CORE_TYPES();

    ASSERT_EQ(MI_DEFAULT_VARIANT, "scalar_rgb");

    bool parallel{true};  // Common flags
    bool optimize{true};
    std::string name{"./simple.xml"};  // Relative path !!!PREPARE SCENE AND MESHES
    std::string name_out{"./simple.exr"};

    auto mi = nb::module_::import_("mitsuba");
    mi.attr("set_variant")(MI_DEFAULT_VARIANT);  // curr_variant hiden inside library, can't change without memory fuckery. Too dangerous to change via
                                                 // callback, but possible, check alias.cpp:162

    nb::object variant = mi.attr("variant")();  // Verification + posible expansion of variants to test
    if (variant.is_none())
        Throw("No variant was set!");

    parser::ParameterList params;  // maybe params.reserve(0) ???
    nb::str variant_str = nb::borrow<nb::str>(std::move(variant));
    parser::ParserConfig config(variant_str.c_str());
    size_t sensor_i = 0;

    config.parallel = parallel;
    config.merge_equivalent = optimize;
    config.merge_meshes = optimize;

    // Set up FileResolver like the old parser does
    fs::path filename(name);
    if (!fs::exists(filename))
        Throw("\"%s\": file does not exist!", filename);

    ref<FileResolver> fs_backup = file_resolver();  // better to prepare new file_resolver to avoid conflicts
    ref<FileResolver> fs = new FileResolver(*fs_backup);
    fs->prepend(filename.parent_path());
    set_file_resolver(fs.get());

    std::vector<ref<Object>> objects;
    try {
        nb::gil_scoped_release release;
        parser::ParserState state = parser::parse_file(config, name, params);
        parser::transform_all(config, state);
        objects = parser::instantiate(config, state);
    } catch (...) {
        set_file_resolver(fs_backup.get());
        throw;
    }
    filename = fs::path(name_out);  // output file

    // MI_INVOKE_VARIANT(variant_str.c_str(), render, objects[0].get(), sensor_i, filename);
    auto* scene = dynamic_cast<Scene<Float, Spectrum>*>(objects[0].get());
    if (!scene)
        Throw("Root element of the input file must be a <scene> tag!");
    if (scene->sensors().empty())
        Throw("No sensor specified for scene: %s", scene);
    if (sensor_i >= scene->sensors().size())
        Throw("Specified sensor index is out of bounds!");
    auto film = scene->sensors()[sensor_i]->film();

    auto integrator = scene->integrator();
    if (!integrator)
        Throw("No integrator specified for scene: %s", scene);

    develop_callback_fn = [film]() { film->develop(); };

    integrator->render(scene, (uint32_t)sensor_i, 0, 0, false, true);

    film->write(filename);

    develop_callback_fn = nullptr;

    set_file_resolver(fs_backup.get());  // set old file_resolver
}

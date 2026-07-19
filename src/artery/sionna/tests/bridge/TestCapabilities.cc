// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/bridge/SionnaBridge.h>
#include <gtest/gtest.h>

namespace nb = nanobind;

namespace
{

class ModuleIdentity : public virtual artery::sionna::py::IPythonModuleIdentityCapability
{
public:
    const char* moduleName() const override { return "mod"; }
};

struct BaseImporter final : public artery::sionna::py::BasePythonImportCapability, public ModuleIdentity {
};

struct CachedImporter final : public artery::sionna::py::CachedImportCapability, public ModuleIdentity {
};

class ClassIdentity : public virtual artery::sionna::py::IPythonClassIdentityCapability, public ModuleIdentity
{
public:
    const char* className() const override { return "Constants"; }
};

class BaseFetcher final : public artery::sionna::py::BasePythonFetchCapability, public ClassIdentity
{
};

class CachedFetcher final : public artery::sionna::py::CachedFetchCapability, public ClassIdentity
{
};

class EchoIdentity : public virtual artery::sionna::py::IPythonClassIdentityCapability, public ModuleIdentity
{
public:
    const char* className() const override { return "Echo"; }
};

class EchoInit final : public artery::sionna::py::InitPythonClassCapability, public EchoIdentity
{
public:
    nb::object object() const { return bound_; }
};

class EchoWrap final : public artery::sionna::py::ExportBoundObjectCapability, public EchoIdentity
{
};

class AnyCaller final : public artery::sionna::py::CallAnyCapability
{
};

}  // namespace

TEST(CapabilitiesImport, BaseImportLoadsModule)
{
    BaseImporter importer;
    nb::module_ mod = importer.module();

    ASSERT_TRUE(mod.is_valid());
    nb::str name = mod.attr("__name__");
    EXPECT_TRUE(name.equal(nb::str("mod")));
}

TEST(CapabilitiesImport, CachedImportIsStable)
{
    CachedImporter importer;
    nb::module_ mod1 = importer.module();
    nb::module_ mod2 = importer.module();

    ASSERT_TRUE(mod1.is_valid());
    ASSERT_TRUE(mod2.is_valid());
    EXPECT_EQ(mod1.ptr(), mod2.ptr());
}

TEST(CapabilitiesFetch, BaseFetchLoadsClass)
{
    BaseFetcher fetcher;
    nb::object type = fetcher.type();

    ASSERT_TRUE(type.is_valid());
    nb::str name = type.attr("__name__");
    EXPECT_TRUE(name.equal(nb::str("Constants")));
}

TEST(CapabilitiesFetch, CachedFetchIsStable)
{
    CachedFetcher fetcher;
    nb::object type1 = fetcher.type();
    nb::object type2 = fetcher.type();

    ASSERT_TRUE(type1.is_valid());
    ASSERT_TRUE(type2.is_valid());
    EXPECT_EQ(type1.ptr(), type2.ptr());
}

TEST(CapabilitiesCall, CallAnyInvokesFunction)
{
    using namespace artery::sionna::literals;

    AnyCaller caller;
    nb::module_ mod = nb::module_::import_("mod");
    nb::object result = caller.callAny(mod, "echo_kwargs", "x"_a = 3);
    nb::dict d = nb::cast<nb::dict>(result);
    EXPECT_EQ(nb::cast<int>(d["x"]), 3);
}

TEST(CapabilitiesInit, InitPythonClassConstructsObject)
{
    using namespace artery::sionna::literals;

    EchoInit init;
    init.init("value"_a = 5);

    nb::object obj = init.object();
    nb::object out = obj.attr("ping")(2);
    EXPECT_EQ(nb::cast<int>(out), 7);
}

TEST(CapabilitiesWrap, WrapAndExportReturnsSameObject)
{
    nb::module_ mod = nb::module_::import_("mod");
    nb::object obj = mod.attr("Echo")(9);

    EchoWrap wrap;
    wrap.init(obj);

    nb::object exported = wrap.object();
    EXPECT_EQ(exported.ptr(), obj.ptr());
}

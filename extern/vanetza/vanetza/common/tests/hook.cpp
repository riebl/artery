#include <gtest/gtest.h>
#include <vanetza/common/hook.hpp>
#include <string>
#include <vector>

using namespace vanetza;

TEST(Hook, variants) {
    Hook<int> hook_int;
    hook_int(28); // rvalue call
    int x = 29;
    hook_int(x); // lvalue (exact type)
    int& y = x;
    hook_int(y); // lvalue (compatible type)

    Hook<double, float> hook_fp;
    hook_fp(23.0, -42.0f);

    Hook<const std::string&, std::vector<int>> hook_objects;
    hook_objects("foo", {3, 2});

    Hook<std::string&&> hook_rvalue_ref;
    hook_rvalue_ref("bar");
}

TEST(Hook, invocation) {
    Hook<double, float> hook;
    double d = 3.0;
    float f = 5.3f;

    // empty hook does nothing
    hook(0.0, 1.0f);
    EXPECT_EQ(3.0, d);
    EXPECT_EQ(5.3f, f);

    // set hook and test it's magic
    hook = [&d, &f](double _d, float _f) { d = _d; f = _f; };
    hook(23.1, -384.34f);
    EXPECT_EQ(23.1, d);
    EXPECT_EQ(-384.34f, f);

    // clear hook and it should do nothing again
    hook.clear();
    hook(0.0, 3.33f);
    EXPECT_EQ(23.1, d);
    EXPECT_EQ(-384.34f, f);
}


from conans import ConanFile, CMake


class VanetzaConan(ConanFile):
    name = "Vanetza"
    version = "master"
    url = "https://github.com/riebl/vanetza"
    license = "GNU Lesser General Public License (LGPL) v3"
    settings = "os", "compiler", "build_type", "arch"
    requires = "Boost/1.60.0@lasote/stable", "cryptopp/5.6.3@riebl/testing", "GeographicLib/1.46@riebl/testing"
    generators = "cmake"
    exports = "cmake/*", "tools/*", "vanetza/*", "CMakeLists.txt"
    options = {"static": [True, False]}
    default_options = "static=False"

    def build(self):
        cmake = CMake(self.settings)
        build = "-DENABLE_CONAN=ON -DVANETZA_BUILD_SHARED=%s -DVANETZA_BUILD_STATIC=%s"
        if self.options.static:
            build = build % ('OFF', 'ON')
        else:
            build = build % ('ON', 'OFF')
        self.run('cmake "%s" %s %s' % (self.conanfile_directory, cmake.command_line, build))
        self.run('cmake --build . %s' % cmake.build_config)

    def package(self):
        self.copy(pattern="*.hpp", dst="include/vanetza", src="vanetza")
        self.copy(pattern="*.h", dst="include/vanetza/asn1/gen", src="vanetza/asn1/gen")
        self.copy(pattern="*.a", dst="lib", src="lib", keep_path=False)
        self.copy(pattern="*.so", dst="lib", src="lib", keep_path=False)

    def package_info(self):
        libs = ['asn1', 'btp', 'common', 'dcc', 'facilities',
                'geonet', 'gnss', 'net', 'security']
        self.cpp_info.libs = ['vanetza_' + lib for lib in libs]

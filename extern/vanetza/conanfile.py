from conans import ConanFile, CMake


class VanetzaConan(ConanFile):
    name = "Vanetza"
    version = "master"
    url = "https://github.com/riebl/vanetza"
    license = "GNU Lesser General Public License (LGPL) v3"
    settings = "os", "compiler", "build_type", "arch"
    requires = "Boost/1.60.0@lasote/stable", "OpenSSL/1.0.2j@lasote/stable", "cryptopp/5.6.5@riebl/testing", "GeographicLib/1.46@riebl/testing"
    generators = "cmake"
    exports = "cmake/*", "tools/*", "vanetza/*", "CMakeLists.txt"
    options = {"static": [True, False]}
    default_options = "static=False", "OpenSSL:no_electric_fence=True", "OpenSSL:no_zlib=True"

    def build(self):
        cmake = CMake(self.settings)
        build = "-DBUILD_USING_CONAN=ON -DBUILD_SHARED_LIBS=%s"
        if self.options.static:
            build = build % ('OFF')
        else:
            build = build % ('ON')
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

import os

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy


class QtCanpoolConan(ConanFile):
    """Skeleton Conan 2 recipe for the qtcanpool 3.0 CMake package.

    NOTE: this recipe has NOT been validated in CI. It is offered as a starting
    point for the community; `conan create .` needs a Qt 6 build (the `qt`
    recipe) and a matching compiler, which the project's CI does not provision.
    """

    name = "qtcanpool"
    version = "4.0.0"
    license = "MulanPSL-2.0"
    url = "https://github.com/canpool/qtcanpool"
    homepage = "https://canpool.github.io/qtcanpool/"
    description = (
        "A Qt6-first desktop UI application framework: ribbon, docking, "
        "windowing, theming and an application shell."
    )
    topics = ("qt", "qt6", "widgets", "ribbon", "docking", "gui", "framework")

    package_type = "library"
    settings = "os", "arch", "compiler", "build_type"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    exports_sources = (
        "CMakeLists.txt",
        "cmake/*",
        "src/*",
        "LICENSE",
    )

    def requirements(self):
        # The `qt` recipe bundles qtbase (Core/Gui/Widgets), which is all the
        # exported targets reference.
        self.requires("qt/6.8.3")

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["WITH_DEMOS"] = False
        tc.variables["WITH_TESTS"] = False
        tc.variables["WITH_DOCS"] = False
        tc.variables["BUILD_WITH_PCH"] = False
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        # The public headers and the CMake package live in an EXCLUDE_FROM_ALL
        # "Devel" component; a plain install would skip them.
        cmake.install(component="Devel")
        copy(self, "LICENSE", src=self.source_folder,
             dst=os.path.join(self.package_folder, "licenses"))

    def package_info(self):
        # Downstream consumers use find_package(QtCanpool) and link the
        # QtCanpool::<lib> imported targets directly, so no aggregate target is
        # advertised here.
        self.cpp_info.set_property("cmake_file_name", "QtCanpool")
        self.cpp_info.builddirs = ["lib/cmake/QtCanpool"]

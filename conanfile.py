from conan import ConanFile
from conan.tools.build import check_min_cppstd, valid_min_cppstd
from conan.tools.meson import Meson, MesonToolchain
from conan.tools.layout import basic_layout
from conan.tools.gnu import PkgConfigDeps
from conan.tools.files import copy, replace_in_file


class liboopetris_c_wrapper(ConanFile):
    name = "liboopetris_c_wrapper"
    version = "0.5.6"

    # Optional metadata
    author = "Totto16"
    url = "https://github.com/Totto16/oopetris_wrapper_c"

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": True}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = ("meson.build", "meson.options", "src/*", "tests/*", "tools/*")

    def layout(self):
        basic_layout(self)

    def validate(self):
        check_min_cppstd(self, "20")

    def _copy_pc_files_for(self, name, names):
        dep = self.dependencies[name]
        pkg_folder = dep.package_folder
        copy(self, src=pkg_folder, pattern="*.pc", dst="", keep_path=False)
        for file_name in names:
            replace_in_file(
                self,
                f"{file_name}.pc",
                search="prefix=/",
                replace=f"prefix={pkg_folder}",
            )

    def generate(self):
        deps = PkgConfigDeps(self)
        deps.generate()

        self._copy_pc_files_for("liboopetris", ["oopetris-core", "oopetris-recordings"])

        tc = MesonToolchain(self)
        tc.project_options["clang_libcpp"] = "disabled"
        tc.generate()

    def requirements(self):
        self.requires("fmt/[>=11.0.0]")
        self.requires("nlohmann_json/[>=3.11.0]")
        self.requires("magic_enum/[>=0.9.0]")
        self.requires("utfcpp/[>=4.0.0]")
        if not valid_min_cppstd(self, "23"):
            self.requires("tl-expected/[>=1.0.0]")

        self.requires("liboopetris/0.5.6")

    def build(self) -> None:

        meson = Meson(self)
        meson.configure()
        meson.build()

    def package(self):
        meson = Meson(self)
        meson.install()

    def test():
        meson = Meson(self)
        meson.test()

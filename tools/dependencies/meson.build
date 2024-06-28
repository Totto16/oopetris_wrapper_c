oopetris_recordings_dep = dependency('oopetris-recordings')

dep_compiler = oopetris_recordings_dep.get_variable(
    pkgconfig: 'compiler',
    internal: '__NOT_DEFINED__',
    default_value: cpp.get_id(),
)

if dep_compiler != cpp.get_id()
    error(
        'Dependency compiler and current compiler don\'t match: \''
        + dep_compiler
        + '\' != \''
        + cpp.get_id()
        + '\'',
    )
endif

if cpp.get_id() == 'clang'

    dep_cpp_stdlib = oopetris_recordings_dep.get_variable(
        pkgconfig: 'cpp_stdlib',
        internal: 'pkg_cpp_stdlib',
    )

    expected_cpp_stdlib = build_with_libcpp ? 'libc++' : 'libstdc++'

    if dep_cpp_stdlib != expected_cpp_stdlib
        error(
            'Dependency and current project not build with the same c++ stdlib: current project is build with \''
            + expected_cpp_stdlib
            + '\' but dependency is build with \''
            + dep_cpp_stdlib
            + '\'',
        )
    endif

endif

deps += oopetris_recordings_dep

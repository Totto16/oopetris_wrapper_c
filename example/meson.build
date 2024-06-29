example_deps = [liboopetris_c_wrapper_dep]
example_c_flags = ['-D_REPL_VERSION=0.1.0']
readline_dep = dependency('readline', required: false)

if readline_dep.found()
    example_deps += readline_dep
    example_c_flags += '-DWITH_READLINE'
endif

example = executable(
    'example',
    files(
        'main.c',
    ),
    dependencies: example_deps,
    c_args: example_c_flags,
    override_options: {
        'warning_level': '3',
        'werror': true,
        'b_coverage': false,
    },
)

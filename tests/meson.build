test_deps = [liboopetris_c_wrapper_dep]

test_deps += dependency('criterion')

tests_config = [
    ['get_recording_information.c', 'GetRecordingInformation'],
    ['information.c', 'Information'],
    ['is_recording_file.c', 'IsRecordingFile'],
    ## 'stb_ds.c', this test don't work atm

]

use_tap = get_option('tests_use_tap')

foreach config : tests_config

    file = config[0]
    name = config[1]

    test_exe = executable(
        'test_' + name,
        files(file),
        dependencies: test_deps,
        override_options: {
            'warning_level': '3',
            'werror': true,
            'b_coverage': false,
        },
    )

    test(
        'test_' + name,
        test_exe,
        protocol: use_tap ? 'tap' : 'exitcode',
        workdir: meson.project_source_root() / 'tests' / 'files',
        args: use_tap ? ['--tap'] : ['--verbose', '--full-stats'],
    )

endforeach

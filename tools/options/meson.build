cpp = meson.get_compiler('cpp')

if cpp.get_id() == 'clang'

    build_with_libcpp_option = get_option('clang_libcpp')

    build_with_libcpp = build_with_libcpp_option.enabled()

    if build_with_libcpp_option.auto()
        if host_machine.system() == 'darwin'
            build_with_libcpp = true
        else

            cpp_lib = cpp.find_library('c++', required: false)
            cppabi_lib = cpp.find_library('c++abi', required: false)

            if cpp_lib.found() and cppabi_lib.found()
                build_with_libcpp = true
            else
                build_with_libcpp = false
            endif
        endif
    endif

    if build_with_libcpp
        deps += [
            cpp.find_library('c++'),
            cpp.find_library('c++abi'),
        ]

        if not meson.is_subproject()
            add_global_link_arguments('-stdlib=libc++', language: ['cpp'])
            add_global_arguments('-stdlib=libc++', language: ['cpp'])
        else
            add_project_link_arguments('-stdlib=libc++', language: ['cpp'])
            add_project_arguments('-stdlib=libc++', language: ['cpp'])

        endif

    endif

endif

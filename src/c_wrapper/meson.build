_header_files = files(
    'windows.h',
    'wrapper.h',
)

src_files += files(
    'opaque_types.h', # not needed to be installed
    'wrapper.cpp',
)

install_headers(
    _header_files,
    install_dir: c_wrapper_include_dir,
    preserve_path: true,
)

src_files += _header_files

subdir('thirdparty')

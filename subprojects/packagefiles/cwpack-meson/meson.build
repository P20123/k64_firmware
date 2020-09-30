project('cwpack', 'c')

includes = include_directories('src')

lib_cwpack = library(
    'cwpack',
    [
        'src/cwpack_internals.h',
        'src/cwpack_config.h',
        'src/cwpack.c'
    ]
)

dep_cwpack = declare_dependency(
    link_with: lib_cwpack, include_directories: includes
)

project('k64_base', 'c')

# dependencies
dependencies = [
    subproject('base_queue').get_variable('dep_queue'),
    meson.get_compiler('c').find_library('m')
]

# Project include path.
local_headers = ['include']

## project setup
# These are configurations supplied by meson_options.txt, these variable names
# are needed by the subdir meson.build definitions.
cpu_arch = get_option('cpu_arch')
target_system = get_option('target_system')
applications = get_option('applications')

# Source files are determined in each sub-directory, based on the project
# configuration.
c_sources = []
asm_sources = []
# device drivers (not peripheral devices) needed by the project
devices_required = []
# peripheral drivers needed by the project
peripherals_required = []
# applications
subdir('src/programs')
# drivers
subdir('src/drivers')
# Kernel
subdir('src/kernel')
# custom libc
subdir('src/libc')

### DON'T EDIT BELOW THIS LINE ###

# These are additional sources that are needed for normal system init, provided
# by NXP in the BSP.
c_sources = c_sources + ['core/src/system_MK64F12.c']
asm_sources = asm_sources + ['core/src/startup_MK64F12.S']

_includes = local_headers + ['core/include']
_incl_dirs = include_directories(_includes)

asm_static_object = static_library(
    'asm_static_object',
    asm_sources,
    include_directories: _incl_dirs,
    c_args: get_option('c_args') + ['-x', 'assembler-with-cpp'],
    install: false
  )

main = executable(
    'main.elf',
    c_sources,
    include_directories: _incl_dirs,
    link_whole: asm_static_object,
    dependencies: dependencies,
    install: true,
    install_dir: false
)
main_binary = custom_target(
    'main.bin',
    command: ['@SOURCE_DIR@/scripts/create_bin.sh', 'main.elf', '@OUTPUT@'],
    depends: main,
    output: 'main.bin',
    build_by_default: true
)

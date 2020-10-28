project('k64_base', 'c')

# dependencies
dependencies = [
    subproject('base_queue').get_variable('dep_queue'),
    meson.get_compiler('c').find_library('m')
]

# Project include path.
local_headers = ['include']

# List of C sources to compile. Relative to project root.
c_sources = [
    # kernel
    'src/kernel/hardfault.c',
    'src/kernel/kernel_support.c',
    'src/kernel/environment.c',
    'src/kernel/abort.c',
    'src/kernel/boot.c',
    'src/kernel/kernel_main.c',
    'src/kernel/file_table.c',
    'src/kernel/hardfault.c',
    'src/kernel/environment.c',
    # libc
    'src/libc/errno.c',
    'src/nongnu/unistd.c',
    'src/libc/string.c',
    # drivers
    'src/drivers/kinetis/uart.c',
    # programs
    'src/programs/pcb_scratch.c'
]

# List of ASM sources to compile.  Relative to project root.
asm_sources = [
    'src/programs/enter_proc.s'
]

## project setup
# CPU to use (default: on FRDM-K64F)
add_project_arguments('-DCPU_MK64FN1M0VLL12', language: 'c')

# Set the entry point to be bootloader_entry
add_project_arguments('-D__START=bootloader_entry', language: 'c')

# use default clock?
# This is Kinetis-setup specific, and configures the Freescale MCG on boot.
add_project_arguments(['-DCLOCK_SETUP=1'], language: 'c')


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

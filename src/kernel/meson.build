c_sources = get_variable('c_sources', [])
c_sources += files([
    'hardfault.c',
    'environment.c',
    'boot.c',
    'kernel_main.c',
    'file_table.c',
    'environment.c',
    'stream_io.c',
    'service_dispatch.c',
    'schedule.c',
    'private/static_memory.c',
    'mutex.c',
    'mutex_lock.c',
])
if get_variable('cpu_arch', 'arm-cortexm4') == 'arm-cortexm4'
    if get_variable('target_system', 'MK64F12') == 'MK64F12'
        # Set the entry point to be bootloader_entry. __START is the name of the
        # entry point in NXP's BSP.
        add_project_arguments('-D__START=bootloader_entry', language: 'c')
        # use default clock?
        # This is Kinetis-setup specific, and configures the Freescale MCG
        # on boot. NOTE: this is broken with NXP BSPs provided after about 2016.
        add_project_arguments(['-DCLOCK_SETUP=1'], language: 'c')
    endif

    c_sources += files([    
        'arm/cm4/process.c',
        'arm/cm4/multitasking.c',
    ])
    asm_sources += files([
        'arm/cm4/syscall_entry.S',
        'arm/cm4/schedule_entry.S'
    ])
endif

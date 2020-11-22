c_sources = get_variable('c_sources', [])
devices_required = get_variable('devices_required', [])
peripherals_required = get_variable('peripherals_required', [])
algos_required = get_variable('algos_required', [])

if get_variable('cpu_arch', 'arm-cortexm4') == 'arm-cortexm4'
    c_sources += files([
        'arm/cm4/systick.c',
    ])
endif

if get_variable('target_system', 'MK64F12') == 'MK64F12'
    # CPU to use (default: on FRDM-K64F)
    add_project_arguments('-DCPU_MK64FN1M0VLL12', language: 'c')

    if peripherals_required.contains('i2c')
        add_project_arguments('-DKINETIS_USE_I2C', language: 'c')
        c_sources += files([
            'kinetis/i2c.c',
        ])
    endif
    if peripherals_required.contains('uart')
        add_project_arguments('-DKINETIS_USE_UART', language: 'c')
        c_sources += files([
            'kinetis/uart.c',
        ])
    endif
    if peripherals_required.contains('ftm')
        add_project_arguments('-DKINETIS_USE_FTM', language: 'c')
        c_sources += files([
            'kinetis/ftm.c',
        ])
    endif
endif

if devices_required.contains('altimu')
        add_project_arguments('-DDEVICE_EN_ALTIMU', language: 'c')
        c_sources += files([
            'devices/altimu.c',
        ])
endif
if devices_required.contains('status_leds')
        add_project_arguments('-DDEVICE_EN_STATUS_LEDS', language: 'c')
        c_sources += files([
            'devices/status_leds.c',
        ])
endif
if devices_required.contains('motors')
        add_project_arguments('-DDEVICE_EN_MOTORS', language: 'c')
        c_sources += files([
            'devices/motors.c',
        ])
endif
if devices_required.contains('servos')
        add_project_arguments('-DDEVICE_EN_SERVOS', language: 'c')
        c_sources += files([
            'devices/servos.c',
        ])
endif

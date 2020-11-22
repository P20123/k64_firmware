c_sources = get_variable('c_sources', [])
devices_required = get_variable('devices_required', [])
peripherals_required = get_variable('peripherals_required', [])
algos_required = get_variable('algos_required', [])
dependencies = get_variable('dependencies', [])

# init main
if get_variable('applications') == []
    subdir_done()
else
    c_sources += files([
        'main.c'
    ])
    devices_required += ['status_leds']
endif

# sensor read config
if get_variable('applications').contains('sensor_read')
    add_project_arguments('-DTASK_EN_SENSOR_READ', language: 'c')
    peripherals_required += ['i2c']
    devices_required += ['altimu']

    c_sources += files([
        'sensor_read.c',
    ])
endif

if get_variable('applications').contains('xpc_relay')
    add_project_arguments('-DTASK_EN_XPC_RELAY', language: 'c')
    peripherals_required += ['uart']
    devices_required += ['status_leds']

    c_sources += files([
        'xpc_relay_event_loop.c',
    ])
    dependencies += [dependency('tinyxpc', fallback: ['tinyxpc', 'dep_relay'])]
endif

# flight control config
if get_variable('applications').contains('flight_ctrl')
    add_project_arguments('-DTASK_EN_FLIGHT_CTRL', language: 'c')
    peripherals_required += ['ftm']
    devices_required += ['servos', 'motors']

    c_sources += files([
        'flight_ctrl.c',
    ])
endif

# sensor fusion config
if get_variable('applications').contains('sensor_fusion')
    add_project_arguments('-DTASK_EN_SENSOR_FUSION', language: 'c')
    peripherals_required += ['i2c']
    devices_required += ['altimu']
    algos_required += ['mahony']

    c_sources += files([
        'sensor_fusion.c',
    ])
endif

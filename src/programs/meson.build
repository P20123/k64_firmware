c_sources = get_variable('c_sources', [])
devices_required = get_variable('devices_required', [])
peripherals_required = get_variable('peripherals_required', [])

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

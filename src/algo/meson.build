c_sources = get_variable('c_sources', [])
devices_required = get_variable('devices_required', [])
peripherals_required = get_variable('peripherals_required', [])
algos_required = get_variable('algos_required', [])

if algos_required.contains('mahony')
        add_project_arguments('-DALGO_EN_MAHONY', language: 'c')
        c_sources += files([
            'mahony.c',
        ])
endif

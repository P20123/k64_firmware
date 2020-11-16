c_sources = get_variable('c_sources', [])
devices_required = get_variable('devices_required', [])
peripherals_required = get_variable('peripherals_required', [])

# init process
if get_variable('applications') == []
    subdir_done()
else
    c_sources += files([
        'init.c'
    ])
endif

# print demo config
if get_variable('applications').contains('print_demo')
    add_project_arguments('-DTASK_EN_PRINT_DEMO', language: 'c')
    peripherals_required += ['uart']

    c_sources += files([
        'print_demo.c'
    ])
endif

# LED color changer demo
if get_variable('applications').contains('blink_demo')
    add_project_arguments('-DTASK_EN_BLINK_DEMO', language: 'c')
    c_sources += files([
        'blink_demo.c'
    ])
endif

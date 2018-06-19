project('libstr', ['c', 'cpp'], version: '1.0.0', default_options : ['c_std=c99', 'cpp_std=c++14'])

if host_machine.system() == 'windows'
    add_global_arguments('-DWIN32', language : 'c')
endif

subdir('src')
subdir('tests')

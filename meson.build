project('format-meson',
        'cpp',
        version : '1.0.4',
        license : 'MIT',
        default_options :
        ['warning_level=3',
        'cpp_std=c++2a',
        'prefix=/usr'])

incldir = include_directories('./include')

dict = {
  'key': ['value']
}

'''
if foo   == bar
value = 'string'
  else    
    value = 0
    endif
'''

deps = declare_dependency(include_directories : [incldir])

bin = executable('format-meson',
                 ['src/MesonIterator.cc',
                 'src/MesonFormatter.cc',
                 'src/MesonConsole.cc',
                 'src/main.cc'],
                 cpp_args :
                 ['-std=gnu++2a',
                 '-Wall',
                 '-pedantic',
                 '-Wno-deprecated',
                 '-m64',
                 '-mtune=generic',
                 '-O2',
                 '-Os',
                 '-pthread'],
                 link_args :
                 ['-Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now'],
                 dependencies : [deps],
                 install : true)

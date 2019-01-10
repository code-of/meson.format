project('format-meson', 'cpp',
        version : '1.0.1',
        license : 'MIT',
        default_options : ['warning_level=3',
        'cpp_std=c++2a', 'prefix=/usr'])

incldir = include_directories('./include')

deps = declare_dependency(include_directories : [incldir])

bin = executable('format-meson', ['src/formatter.cc', 'src/main.cc'],
                 cpp_args : ['-std=gnu++2a', '-Wall', '-pedantic',
                 '-Wno-deprecated', '-m64', '-mtune=generic',
                 '-O2', '-Os', '-pthread'],
                 link_args : ['-Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now'],
                 dependencies : [deps],
                 install : true)

test('Basic-Test', bin)

project('meson-format', 'cpp',
        version : '1.0.0a',
        license : 'MIT',
        default_options : ['warning_level=3', 
        'cpp_std=c++2a', 'prefix=/usr'])

deps = declare_dependency(include_directory: ['./include'])

bin = executable('meson-format', 
                 ['src/formatter.cc', 
                 'src/main.cc'],
                 cpp_args : 
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
                 ['-Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now']
                 install : true)

test('Basic-Test', bin)

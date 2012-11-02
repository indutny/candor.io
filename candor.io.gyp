{
  'targets': [{
    'target_name': 'can.io',
    'type': 'executable',
    'dependencies': [
      'deps/candor/candor.gyp:candor',
      'deps/uv/uv.gyp:libuv',
    ],
    'include_dirs': [
      'deps/candor/include/',
      'deps/libuv/include/',
      'src'
    ],
    'cflags': ['-Wall', '-Wextra', '-Wno-unused-parameter',
               '-fPIC', '-fno-strict-aliasing', '-fno-exceptions',
               '-pedantic'],
    'sources': [
      'src/can.cc',
      'src/runtime.cc',
      'src/bindings/can.cc'
    ]
  }, {
    'target_name': 'can2c',
    'type': 'executable',
    'sources': [
      'tools/can2c.cc'
    ]
  }]
}

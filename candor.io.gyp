{
  'variables': {
    'library_files': [
      'lib/events.can',
      'lib/fs.can',
      'lib/tcp.can',
      'lib/module.can',
      'lib/can.can',
    ]
  },
  'targets': [{
    'target_name': 'can.io',
    'type': 'executable',
    'dependencies': [
      'deps/candor/candor.gyp:candor',
      'deps/uv/uv.gyp:libuv',
      'can2c_scripts#host'
    ],
    'include_dirs': [
      'deps/candor/include',
      'deps/libuv/include',
      '<(SHARED_INTERMEDIATE_DIR)',
      'src',
    ],
    'cflags': ['-Wall', '-Wextra', '-Wno-unused-parameter',
               '-fPIC', '-fno-strict-aliasing', '-fno-exceptions',
               '-pedantic'],
    'sources': [
      'src/main.cc',
      'src/runtime.cc',
      'src/process.cc',
      'src/natives.cc',
      'src/bindings.cc',
      'src/bindings/fs.cc',
      'src/bindings/buffer.cc',
      'src/bindings/tcp.cc',

      '<@(library_files)',
      '<(SHARED_INTERMEDIATE_DIR)/can_natives.h',
    ]
  }, {
    'target_name': 'can2c',
    'toolsets': [ 'host' ],
    'type': 'executable',
    'sources': [
      'tools/can2c.cc'
    ],
  }, {
    'target_name': 'can2c_scripts',
    'toolsets': [ 'host' ],
    'type': 'none',
    'dependencies': [ 'can2c#host' ],
    'actions': [{
      'action_name': 'can2c',
      'inputs': [
        '<@(library_files)',
      ],
      'outputs': [
        '<(SHARED_INTERMEDIATE_DIR)/can_natives.h',
      ],
      'action': [
        '<(PRODUCT_DIR)/can2c',
        '<@(_outputs)',
        '<@(_inputs)',
      ]
    }]
  }]
}

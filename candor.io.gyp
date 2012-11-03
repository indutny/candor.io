{
  'variables': {
    'library_files': [
      'lib/can.can', # This should come first
      'lib/module.can',
    ]
  },
  'targets': [{
    'target_name': 'can.io',
    'type': 'executable',
    'dependencies': [
      'deps/candor/candor.gyp:candor',
      'deps/uv/uv.gyp:libuv',
      'can2c_scripts'
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

      '<@(library_files)',
      '<(SHARED_INTERMEDIATE_DIR)/can_natives.h',
    ]
  }, {
    'target_name': 'can2c',
    'type': 'executable',
    'sources': [
      'tools/can2c.cc'
    ],
  }, {
    'target_name': 'can2c_scripts',
    'type': 'none',
    'dependencies': [ 'can2c' ],
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

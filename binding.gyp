{
  'targets': [
    {
      'target_name': 'msoconv',
      'sources': [
        'src/main.cpp',
        'src/msoconv.h',
        'src/async-base.h'
      ],
      'conditions': [
        ['OS=="win"', {
          'sources': [
            'src/comptr.h',
            'src/msoconv_win.cpp',
            'src/string-operate.h'
          ],
          'defines': [
            'UNICODE',
            'NOMINMAX'
          ],
          'msvs_settings': {
            'VCCLCompilerTool': {
              'ExceptionHandling': '1'
            }
          }
        }],
        ['OS not in ["win"]', {
          'sources': [
            'src/msoconv_posix.cpp',
          ],
          'cflags!': [ '-fno-exceptions' ],
          'cflags_cc!': [ '-fno-exceptions' ]
        }],
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
          }
        }]
      ]
    }
  ]
}
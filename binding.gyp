{
  'targets': [
    {
      'target_name': 'msoconv',
      'sources': [
        'src/main.cpp',
        'src/msoconv.h',
        'src/async-base.h'
      ],
      'cflags': [
        '-std=c++1y',
        '-D_GLIBCXX_USE_CXX11_ABI=0'
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
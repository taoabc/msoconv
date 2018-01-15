{
  'targets': [
    {
      'target_name': 'msoconv',
      'include_dirs': [ '<!(node -e "require(\'nan\')")' ],
      'sources': [
        'src/main.cpp',
        'src/msoconv.h',
      ],
      'cflags': [
        '-std=c++11',
        '-D_GLIBCXX_USE_CXX11_ABI=0'
      ],
      'conditions': [
        ['OS=="win"', {
          'sources': [
            'src/msoconv_win.cpp',
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
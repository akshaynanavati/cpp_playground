def FlagsForFile( filename, **kwargs ):
  return {
    'flags': [ '-x', 'c++', '-std=c++17', '-Ideps/googletest/googletest/include', '-I.'],
  }

set(LLVM_LINK_COMPONENTS
  Analysis
  Core
  ExecutionEngine
  InstCombine
  MC
  Object
  OrcJIT
  Passes
  RuntimeDyld
  ScalarOpts
  Support
  Target
  TransformUtils
  native
  )

add_kaleidoscope_chapter(Kaleidoscope-Ch8
  toy.cpp
  )

export_executable_symbols(Kaleidoscope-Ch8)

add_executable(module hello.cc main.cc )
target_compile_options(module PRIVATE -fmodules-ts)

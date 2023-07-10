add_executable(vectorization  main.cc )
target_compile_options(vectorization PRIVATE -mavx2 -fopenmp)
target_link_options(vectorization PRIVATE -fopenmp)
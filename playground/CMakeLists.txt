set(srcs
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/lib.h
  ${CMAKE_CURRENT_SOURCE_DIR}/lib.cc
)
add_executable(playground ${srcs})
target_link_libraries(playground ${CMAKE_THREAD_LIBS_INIT})
include(FetchContent)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.10.0
)
FetchContent_MakeAvailable(spdlog)

add_executable(logger main.cc)
target_link_libraries(logger spdlog::spdlog)
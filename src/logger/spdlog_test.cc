#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

TEST(LoggerTest, BasicLogging) {
  spdlog::set_pattern("%^[%l][%m/%d-%H:%M:%S.%e][#%t][cpp_tests][%s:%#] %v %$");
  SPDLOG_TRACE("this is a trace level log ");
  SPDLOG_INFO("this is an info level log");
  SPDLOG_WARN("this is an warn level log");
  SPDLOG_DEBUG("this is an debug level log");
  SPDLOG_CRITICAL("this is an critical level log");
}

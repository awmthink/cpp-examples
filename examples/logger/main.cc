#include <spdlog/spdlog.h>

int main() {
  spdlog::set_pattern(
      "%^[%l][%m/%d-%H:%M:%S.%e][#%t][cpp-examples][%s:%#] %v %$");
  SPDLOG_TRACE("this is a trace level log ");
  SPDLOG_INFO("this is an info level log");
  SPDLOG_WARN("this is an warn level log");
  SPDLOG_DEBUG("this is an debug level log");
  SPDLOG_ERROR("this is an error level log");
  SPDLOG_CRITICAL("this is an critical level log");
}
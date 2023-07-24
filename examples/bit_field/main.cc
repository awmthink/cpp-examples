#include <chrono>
#include <cstdint>
#include <iostream>

struct FeatureID {
  std::uint64_t location : 15, camera_id : 7, timestamp : 32, sequence : 10;
};

int main() {
  static_assert(sizeof(FeatureID) == 8);
  FeatureID fid;
  fid.location = 1234;
  fid.camera_id = 87;
  auto current_ts = std::chrono::system_clock::now().time_since_epoch();
  fid.timestamp =
      std::chrono::duration_cast<std::chrono::seconds>(current_ts).count();
  fid.sequence = 12;

  std::cout << "FeatureID " << *reinterpret_cast<std::uint64_t*>(&fid) << ": "
            << "location=" << fid.location << ", camera_id=" << fid.camera_id
            << ", timestamp=" << fid.timestamp << ", sequence=" << fid.sequence
            << std::endl;

  return 0;
}
#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace tfc::ec::common {

/// \return The network interfaces on the running hardware.
auto get_interfaces() -> std::vector<std::string> const&;

}  // namespace tfc::ec::common

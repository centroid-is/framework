#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <glaze/core/common.hpp>

#include <tfc/confman/observable.hpp>
#include <tfc/ec/common.hpp>
#include <tfc/utils/json_schema.hpp>

namespace tfc::ec::config {

struct network_interface {
  std::string value{};

  struct glaze {
    static constexpr auto value = &network_interface::value;
    static constexpr std::string_view name{ "network_interface" };
  };
};

}  // namespace tfc::ec::config

template <>
struct glz::detail::to_json_schema<tfc::ec::config::network_interface> {
  template <auto Opts>
  static void op(auto& s, auto&) noexcept {
    s.oneOf = std::vector<glz::detail::schematic>{};
    // fix in https://github.com/Skaginn3x/framework/issues/555
    // attributes was added initially
    // for (auto const& interface : tfc::ec::common::get_interfaces()) {
    //   s.oneOf->emplace_back(glz::detail::schematic{
    //       .attributes{ tfc::json::schema{ .title = interface, .description = interface, .constant = interface } } });
    // }
  }
};

namespace tfc::ec::config {
struct ethercat {
  network_interface primary_interface{ common::get_interfaces().at(0) };
  confman::observable<std::optional<std::size_t>> required_slave_count{ std::nullopt };
  std::chrono::microseconds cycle_time{ std::chrono::milliseconds{ 1 } };
  struct glaze {
    // clang-format off
    static constexpr auto value{ glz::object("primary_interface", &ethercat::primary_interface, "Primary interface",
                                             "required_slave_count", &ethercat::required_slave_count, "Required slave count",
                                             "cycle_time", &ethercat::cycle_time, "The scan time for the ethercat network, between each poll."
                                             ) };
    // clang-format on
    static constexpr std::string_view name{ "config::ethercat" };
  };
};

}  // namespace tfc::ec::config

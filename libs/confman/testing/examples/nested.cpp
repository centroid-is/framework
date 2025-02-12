#include <chrono>
#include <cstdint>
#include <string>

#include <fmt/chrono.h>
#include <fmt/core.h>
#include <mp-units/systems/si.h>
#include <boost/asio.hpp>
#include <sdbusplus/asio/connection.hpp>

#include <tfc/confman.hpp>
#include <tfc/confman/observable.hpp>
#include <tfc/dbus/sd_bus.hpp>
#include <tfc/progbase.hpp>
#include <tfc/stx/glaze_meta.hpp>
#include <tfc/utils/units_glaze_meta.hpp>

namespace asio = boost::asio;

struct third_level {
  mp_units::quantity<mp_units::si::ampere, uint16_t> amper{};
  struct glaze {
    using type = third_level;
    static constexpr auto value{ glz::object("amper", &type::amper, "amper description") };
    static constexpr auto name{ "third_level" };
  };
};

struct second_level {
  std::string a{};
  tfc::confman::observable<std::chrono::nanoseconds> sec{};
  third_level third_lvl{};
  struct glaze {
    using type = second_level;
    static constexpr auto value{ glz::object("a",
                                             &type::a,
                                             "A description",
                                             "sec",
                                             &type::sec,
                                             "sec description",
                                             "third_level",
                                             &type::third_lvl,
                                             "third_lvl description") };
    static constexpr auto name{ "second_level" };
  };
};

struct first_level {
  int a{};
  mp_units::quantity<mp_units::si::deci<mp_units::si::ampere>> amper{};
  second_level second_lvl{};
  struct glaze {
    using type = first_level;
    static constexpr auto value{ glz::object("a_int",
                                             &type::a,
                                             "A int description",
                                             "amper",
                                             &type::amper,
                                             "amper description",
                                             "second_lvl",
                                             &type::second_lvl,
                                             "second_lvl description") };
    static constexpr auto name{ "first_level" };
  };
};

int main(int argc, char** argv) {
  tfc::base::init(argc, argv);

  asio::io_context ctx{};
  auto dbus{ std::make_shared<sdbusplus::asio::connection>(ctx, tfc::dbus::sd_bus_open_system()) };

  tfc::confman::config<first_level> const config{ dbus, "key" };
  config->second_lvl.sec.observe(
      [](auto new_value, auto old_value) { fmt::print("new value: {}, old value: {}\n", new_value, old_value); });

  fmt::print("Schema is: {}\n", config.schema());
  fmt::print("Config is: {}\n", config.string().value());

  dbus->request_name(tfc::dbus::make_dbus_process_name().c_str());

  ctx.run();
  return 0;
}

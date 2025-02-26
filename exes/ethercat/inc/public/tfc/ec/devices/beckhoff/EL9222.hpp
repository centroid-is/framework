#pragma once

#include <array>
#include <cstdint>
#include <string_view>

#include "tfc/ec/devices/base.hpp"

namespace tfc::ec::devices::beckhoff {

#pragma pack(push, 1)
struct channel_input {
  bool status_enabled : 1;
  bool status_tripped : 1;
  std::uint8_t unused1 : 2;
  bool status_hardware_detection : 1;
  std::uint8_t unused2 : 2;
  bool status_current_level_warning : 1;
  bool status_cool_down_lock : 1;
  std::uint8_t unused3 : 3;
  bool status_diag : 1;
  bool status_txpdo_state : 1;
  std::uint8_t status_input_cycle_counter : 2;
  bool status_error : 1;
  bool status_state_reset : 1;
  bool status_state_switch : 1;
  std::uint8_t unused4 : 3;
  std::uint8_t unused5 : 8;
};
struct pdo_input {
  channel_input channel[2];
};
static_assert(sizeof(pdo_input) == 8, "Size of pdo_input must be 8 bytes");

struct channel_output {
  bool control_reset : 1;
  bool control_switch : 1;
  std::uint8_t unused : 6;
  std::uint8_t unused2 : 8;
};
struct pdo_output {
  channel_output channel[2];
};
static_assert(sizeof(pdo_output) == 4, "Size of pdo_output must be 4 bytes");
#pragma pack(pop)

template <typename manager_client_type>
class el9222 final : public base<el9222<manager_client_type>> {
public:
  static constexpr std::string_view name{ "EL9222" };
  explicit el9222(boost::asio::io_context& ctx, manager_client_type& client, uint16_t slave_index)
      : base<el9222<manager_client_type>>(slave_index), ctx_(ctx), client_(client) {}
  void pdo_cycle(pdo_input const& in, pdo_output& out) {
    std::size_t index = 0;
    for (auto& out_channel : out.channel) {
      out_channel.control_reset = reset_[index++].value().value_or(false);
      out_channel.control_switch = true;
    }
    index = 0;
    for (auto& in_channel : in.channel) {
      std::uint32_t state = *reinterpret_cast<std::uint32_t const*>(&in_channel);
      if (state != last_state_[index]) {
        state_[index].async_send(state_to_string(in_channel), [this](std::error_code const& err, auto) {
          if (err) {
            this->logger_.warn("Unable to send state signal: {}", err.message());
          }
        });
        last_state_[index] = state;
      }
      index++;
    }
  }
  static constexpr uint32_t product_code = 0x24063052;
  static constexpr uint32_t vendor_id = 0x2;

private:
  std::string state_to_string(channel_input const& in) {
    return fmt::format(R"({{"enabled":{},"tripped":{},"hardware_detection":{},"current_level_warning":{},)"
                       R"("cool_down_lock":{},"diag":{},"txpdo_state":{},"input_cycle_counter":{},)"
                       R"("error":{},"state_reset":{},"state_switch":{}}})",
                       in.status_enabled, in.status_tripped, in.status_hardware_detection, in.status_current_level_warning,
                       in.status_cool_down_lock, in.status_diag, in.status_txpdo_state, in.status_input_cycle_counter,
                       in.status_error, in.status_state_reset, in.status_state_switch);
  }

  boost::asio::io_context& ctx_;
  ipc_ruler::ipc_manager_client& client_;

  std::array<tfc::ipc::bool_slot, 2> reset_{
    tfc::ipc::bool_slot{ ctx_, client_, fmt::format("el9222.s{}.channel_1.reset", this->slave_index_),
                         "Reset Overcurrent protection channel 1", [](bool) {} },
    tfc::ipc::bool_slot{ ctx_, client_, fmt::format("el9222.s{}.channel_2.reset", this->slave_index_),
                         "Reset Overcurrent protection channel 2", [](bool) {} }
  };

  std::array<tfc::ipc::string_signal, 2> state_{
    tfc::ipc::string_signal{ ctx_, client_, fmt::format("el9222.s{}.channel_1.state", this->slave_index_),
                             "State Overcurrent protection channel 1" },
    tfc::ipc::string_signal{ ctx_, client_, fmt::format("el9222.s{}.channel_2.state", this->slave_index_),
                             "State Overcurrent protection channel 2" }
  };
  std::array<std::uint32_t, 2> last_state_{ 0, 0 };
};
}  // namespace tfc::ec::devices::beckhoff

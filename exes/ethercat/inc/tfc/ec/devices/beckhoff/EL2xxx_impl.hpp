#pragma once

#include <fmt/format.h>

#include <tfc/ec/devices/beckhoff/EL2xxx.hpp>
#include <tfc/ipc.hpp>

namespace tfc::ec::devices::beckhoff {

template <typename manager_client_type,
          std::size_t size,
          std::array<std::size_t, size> entries,
          uint32_t pc,
          tfc::stx::basic_fixed_string name>
el2xxx<manager_client_type, size, entries, pc, name>::el2xxx(asio::io_context& ctx,
                                                             manager_client_type& client,
                                                             uint16_t slave_index)
    : base<el2xxx>(slave_index) {
  for (size_t i = 0; i < size; i++) {
    bool_receivers_.emplace_back(std::make_shared<tfc::ipc::slot<ipc::details::type_bool, manager_client_type&>>(
        ctx, client, fmt::format("{}.s{}.out{}", name.view(), slave_index, entries[i]),
        std::bind_front(&el2xxx::set_output, this, i)));
  }
}
template <typename manager_client_type,
          std::size_t size,
          std::array<std::size_t, size> entries,
          uint32_t pc,
          tfc::stx::basic_fixed_string name>
void el2xxx<manager_client_type, size, entries, pc, name>::pdo_cycle(std::span<std::uint8_t>, output_pdo& output) noexcept {
  output[0] = output_states_.to_ulong() & 0xff;
  if constexpr (size > 8) {
    output[1] = static_cast<std::uint8_t>(output_states_.to_ulong() >> 8);
  }
}

}  // namespace tfc::ec::devices::beckhoff

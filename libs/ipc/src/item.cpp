
#include <fmt/format.h>
#include <glaze/json.hpp>
#include <tfc/utils/pragmas.hpp>

// clang-format off
// Currently the code that relies on this "feature" is not used, dynamic build would fail.
PRAGMA_CLANG_WARNING_PUSH_OFF(-Wdate-time)
#include <pcg_extras.hpp>
PRAGMA_CLANG_WARNING_POP
// clang-format on
#include <pcg_random.hpp>

#include <tfc/ipc/details/item_glaze_meta.hpp>
#include <tfc/ipc/item.hpp>

namespace tfc::ipc::item {

auto make(pcg_extras::seed_seq_from<std::random_device>& seed_source) -> item {
  auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
  pcg64 random_engine(seed_source);  // result_type uint64_t
  uuids::basic_uuid_random_generator<pcg64> random_generator{ random_engine };
  return { .item_id = random_generator(), .entry_timestamp = now, .last_exchange = now };
}

auto make() -> item {
  pcg_extras::seed_seq_from<std::random_device> seed_source;
  return make(seed_source);
}
auto item::from_json(std::string_view json) -> std::expected<item, glz::error_ctx> {
  auto temporary = glz::read_json<item>(json);
  if (!temporary.has_value()) {
    return std::unexpected(std::move(temporary.error()));
  }
  // We are reciving this item, update exchange
  temporary->last_exchange = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
  return std::expected<item, glz::error_ctx>(std::move(temporary.value()));
}
auto item::to_json() const -> std::expected<std::string, glz::error_ctx> {
  auto temporary = glz::write_json(*this);
  if (!temporary.has_value()) {
    return std::unexpected(std::move(temporary.error()));
  }
  return std::expected<std::string, glz::error_ctx>(std::move(temporary.value()));
}
auto item::id() const -> std::string {
  if (item_id.has_value()) {
    return uuids::to_string(item_id.value());
  }
  return {};
}

}  // namespace tfc::ipc::item

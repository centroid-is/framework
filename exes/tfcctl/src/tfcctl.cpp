#include <memory>
#include <string>
#include <string_view>
#include <variant>

#include <boost/asio.hpp>
#include <boost/asio/experimental/co_spawn.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <tfc/ipc.hpp>
#include <tfc/logger.hpp>
#include <tfc/progbase.hpp>

namespace asio = boost::asio;
namespace po = boost::program_options;
namespace ipc = tfc::ipc;

using any_send = std::variant<std::monostate,
                              std::shared_ptr<ipc::bool_send>,
                              std::shared_ptr<ipc::int_send>,
                              std::shared_ptr<ipc::uint_send>,
                              std::shared_ptr<ipc::double_send>,
                              std::shared_ptr<ipc::string_send>,
                              std::shared_ptr<ipc::json_send>>;
using any_recv = std::variant<std::monostate,
                              std::shared_ptr<ipc::bool_recv_cb>,
                              std::shared_ptr<ipc::int_recv_cb>,
                              std::shared_ptr<ipc::uint_recv_cb>,
                              std::shared_ptr<ipc::double_recv_cb>,
                              std::shared_ptr<ipc::string_recv_cb>,
                              std::shared_ptr<ipc::json_recv_cb>>;

inline constexpr std::string_view invalid_type{
  "\nInvalid name {}, it must include one qualified type name.\n"  // should inject slot or signal as {}
  "Any of the following: \n"
  "bool\n"
  "int\n"
  "uint\n"
  "double\n"
  "string\n"
  "json\n"
};

template <typename return_t>
inline auto create_ipc_recv(asio::io_context& ctx, std::string_view name) -> return_t {
  if (name.contains("bool")) {
    return ipc::bool_recv_cb::create(ctx, name);
  }
  if (name.contains("int")) {
    return ipc::int_recv_cb::create(ctx, name);
  }
  if (name.contains("uint")) {
    return ipc::uint_recv_cb::create(ctx, name);
  }
  if (name.contains("double")) {
    return ipc::double_recv_cb::create(ctx, name);
  }
  if (name.contains("string")) {
    return ipc::string_recv_cb::create(ctx, name);
  }
  if (name.contains("json")) {
    return ipc::json_recv_cb::create(ctx, name);
  }
  throw std::runtime_error{ fmt::format(invalid_type, name) };
}
template <typename return_t>
inline auto create_ipc_send(asio::io_context& ctx, std::string_view name) -> return_t {
  if (name.contains("bool")) {
    return ipc::bool_send::create(ctx, name).value();
  }
  if (name.contains("int")) {
    return ipc::int_send::create(ctx, name).value();
  }
  if (name.contains("uint")) {
    return ipc::uint_send::create(ctx, name).value();
  }
  if (name.contains("double")) {
    return ipc::double_send::create(ctx, name).value();
  }
  if (name.contains("string")) {
    return ipc::string_send::create(ctx, name).value();
  }
  if (name.contains("json")) {
    return ipc::json_send::create(ctx, name).value();
  }
  throw std::runtime_error{ fmt::format(invalid_type, name) };
}

inline auto stdin_coro(asio::io_context& ctx, tfc::logger::logger& logger, std::string_view signal_name)
    -> asio::awaitable<void> {
  auto executor = co_await asio::this_coro::executor;
  asio::posix::stream_descriptor input_stream(executor, STDIN_FILENO);

  auto sender{ create_ipc_send<any_send>(ctx, signal_name) };

  while (true) {
    co_await input_stream.async_wait(asio::posix::stream_descriptor::wait_read, asio::use_awaitable);
    std::array<char, 1024> buffer;
    const std::size_t bytes_read = co_await input_stream.async_read_some(asio::buffer(buffer), asio::use_awaitable);
    std::string_view buffer_str{ std::begin(buffer), bytes_read - 1 };  // strip of the new line character

    constexpr auto send{ [](std::string_view input, auto in_sender, auto& in_logger) -> void {
      try {
        using value_t = typename decltype(in_sender)::element_type::value_t;
        auto value = boost::lexical_cast<value_t>(input);
        in_sender->async_send(value, [&, value](std::error_code code, size_t bytes) {
          if (code) {
            in_logger.template log<tfc::logger::lvl_e::error>("Error: {}", code.message());
          } else {
            in_logger.template log<tfc::logger::lvl_e::info>("Sent value: {} size: {}", value, bytes);
          }
        });
      } catch (boost::bad_lexical_cast const& bad_lexical_cast) {
        in_logger.template log<tfc::logger::lvl_e::info>("Invalid input {}, error: {}", input, bad_lexical_cast.what());
      }
    } };
    if (auto* bool_sender{ std::get_if<std::shared_ptr<ipc::bool_send>>(&sender) }) {
      send(buffer_str, *bool_sender, logger);
    } else if (auto* int_sender{ std::get_if<std::shared_ptr<ipc::int_send>>(&sender) }) {
      send(buffer_str, *int_sender, logger);
    } else if (auto* uint_sender{ std::get_if<std::shared_ptr<ipc::uint_send>>(&sender) }) {
      send(buffer_str, *uint_sender, logger);
    } else if (auto* double_sender{ std::get_if<std::shared_ptr<ipc::double_send>>(&sender) }) {
      send(buffer_str, *double_sender, logger);
    } else if (auto* string_sender{ std::get_if<std::shared_ptr<ipc::string_send>>(&sender) }) {
      send(buffer_str, *string_sender, logger);
    } else if (auto* json_sender{ std::get_if<std::shared_ptr<ipc::json_send>>(&sender) }) {
      send(buffer_str, *json_sender, logger);
    }
  }
}

auto main(int argc, char** argv) -> int {
  auto description{ tfc::base::default_description() };

  std::string signal{};
  std::string slot{};

  description.add_options()("signal", po::value<std::string>(&signal), "IPC signal channel (output)")(
      "slot", po::value<std::string>(&slot), "IPC slot channel (input)");

  tfc::base::init(argc, argv, description);

  // Must provide an argument
  if (tfc::base::get_map().find("signal") == tfc::base::get_map().end()) {
    std::stringstream out;
    description.print(out);
    fmt::print("Usage: tfcctl [options] \n{}", out.str());
    std::exit(0);
  }
  tfc::logger::logger logger{ "tfc control" };

  asio::io_context ctx;

  if (!signal.empty() && slot.empty()) {
    asio::co_spawn(ctx, stdin_coro(ctx, logger, signal), asio::detached);
  }

  auto slot_ipc = [&ctx, &logger, &signal](std::string_view slot_name) -> any_recv {
    if (slot_name.empty()) {
      return std::monostate{};
    }
    auto ipc{ create_ipc_recv<any_recv>(ctx, slot_name) };
    std::visit(
        [&](auto&& receiver) {
          using receiver_t = std::remove_cvref_t<decltype(receiver)>;
          if constexpr (!std::same_as<std::monostate, receiver_t>) {
            logger.log<tfc::logger::lvl_e::trace>("Slot {} connecting to signal {}", slot_name, signal);
            auto error =
                receiver->init(signal, [&](auto const& val) { logger.log<tfc::logger::lvl_e::info>("Received {}", val); });
            if (error) {
              logger.log<tfc::logger::lvl_e::error>("Failed to connect: {}", error.message());
            }
          }
        },
        ipc);
    return ipc;
  }(slot);

  asio::signal_set signal_set{ ctx, SIGINT, SIGTERM, SIGHUP };
  signal_set.async_wait([&](const std::error_code& error, int signal_number) {
    if (error) {
      logger.log<tfc::logger::lvl_e::error>("Error waiting for signal. {}", error.message());
    }
    logger.log<tfc::logger::lvl_e::debug>("Shutting down signal ({})", signal_number);
    ctx.stop();
  });

  ctx.run();
  return 0;
}
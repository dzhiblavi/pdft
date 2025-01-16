#include <asio/as_tuple.hpp>
#include <asio/use_awaitable.hpp>

namespace pdft {

inline constexpr auto AsioCoroAsTuple = asio::as_tuple(asio::use_awaitable);

}  // namespace pdft

#ifndef NET_HPP_
#define NET_HPP_

#include <algorithm>
#include <cstdint>
#include <string>

namespace tent
{
namespace net
{

// cudos: https://stackoverflow.com/a/28364285
template <typename T>
constexpr T htont (T value) noexcept
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
  std::reverse(ptr, ptr + sizeof(T));
#endif
  return value;
}

std::string saddr_to_str(uint32_t saddr);

} // namespace net
} // namespace tent

#endif
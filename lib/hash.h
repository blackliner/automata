#pragma once

#include <cstdint>

constexpr uint64_t kMul = sizeof(std::size_t) == 4 ? uint64_t{0xcc9e2d51} : uint64_t{0x9ddfea08eb382d69};

constexpr std::uint64_t Mix(std::uint64_t state, std::uint64_t value) {
  auto m = state + value;
  m *= kMul;

  return static_cast<std::uint64_t>(m ^ (m >> (sizeof(m) * 8 / 2)));
}

constexpr std::uint64_t Hash(std::uint64_t value) {
  return Mix(0, value);
}
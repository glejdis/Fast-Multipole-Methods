#pragma once

#include <iostream>
#include <string>

#include "Vec.h"

inline Vec<int> xy(int nx, int i) {
  return {i % nx, i / nx};
}

inline int idx(int nx, Vec<int> xy) {
  return nx * xy.y + xy.x;
}

inline void printProgress(int max, int curr) {
  constexpr int len = 21;
  const std::string chars[] = {" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"};

  const int eigths = 8 * len * curr / static_cast<double>(max);

  std::cout << "┃";

  for (int i = 0; i < eigths / 8; ++i)
    std::cout << chars[8];
  std::cout << chars[eigths % 8];

  for (int i = eigths / 8 + 1; i < len; ++i)
    std::cout << " ";
  std::cout << "┃" << std::flush;
}

namespace color {
  const std::string normal  = "\033[0m";
  const std::string red     = "\033[31m";
  const std::string green   = "\033[32m";
  const std::string yellow  = "\033[33m";
  const std::string magenta = "\033[35m";
}

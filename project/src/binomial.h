#pragma once

#include <cassert>
#include <vector>

int binomial(const int n, const int k) {
  assert(n >= 0);
  assert(k >= 0);
  assert(n >= k);

  static std::vector<std::vector<int>> coeffs;
  const int maxN = coeffs.size() - 1;

  for (int n_ = maxN+1; n_ <= n; ++n_) {
    coeffs.emplace_back(n_ + 1);
    coeffs[n_][0 ] = 1;
    coeffs[n_][n_] = 1;
    for (int k_ = 1; k_ < n_; ++k_)
      coeffs[n_][k_] = coeffs[n_-1][k_-1] + coeffs[n_-1][k_];
  }

  return coeffs[n][k];
}

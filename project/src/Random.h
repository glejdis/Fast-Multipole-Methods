#pragma once

#include <random>

class Random {
private:
  std::mt19937 rng;
  std::uniform_real_distribution<double> distPos;
  std::normal_distribution<double> distξ;

public:
  Random(const double L)
    : rng(44) // fixed seed for reproducability
    , distPos(0, L)
    , distξ(0, 1)
    {}

  inline Vec<double> pos() { return {distPos(rng), distPos(rng)}; }
  inline     double  ξ  () { return distξ(rng);                   }
};

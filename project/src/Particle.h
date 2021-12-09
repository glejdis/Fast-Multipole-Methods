#pragma once

#include "Vec.h"

namespace Type {
  constexpr int A = 0;
  constexpr int B = 1;
  constexpr int F = 2;
  constexpr int W = 3;
}

//                             A    B    F    W
constexpr double a[4][4] = { { 50,  25,  25, 200} // A
                           , { 25,   1, 300, 200} // B
                           , { 25, 300,  25, 200} // F
                           , {200, 200, 200,   0} // W
                           };

struct Particle {
  int type;
  double m;
  Vec<double> x;
  Vec<double> vOld;
  Vec<double> v;
  Vec<double> aOld;
  Vec<double> aNew;

  Particle() : type(Type::F), m(1) {}
  Particle(const Vec<double> x) : type(Type::F), m(1), x(x) {}
};

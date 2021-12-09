#pragma once

#include <string>

#include "Vec.h"

struct Boundary {
  enum Type { PERIODIC, WALL };

  const Type type = PERIODIC;
  const Vec<double> vWall;
};

enum class MoleculeType { CHAIN, RING };

struct Setup {
  const std::string name;
  const Boundary north, south, west, east;
  const Vec<double> Fbody;
  const int nMolecules = 0;
  const MoleculeType molType = MoleculeType::CHAIN;
  const double KS;
  const double rS;
};

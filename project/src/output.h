#pragma once

#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>

#include "Particle.h"

void writePositions( std::filesystem::path path
                   , const std::vector<Particle>& ps
                   , const int iteration
                   ) {
  std::stringstream ss;
  ss << std::setw(6) << std::setfill('0') << iteration;
  std::ofstream fileF(path / ("f-" + ss.str() + ".dat"));
  std::ofstream fileW(path / ("w-" + ss.str() + ".dat"));
  std::ofstream fileA(path / ("a-" + ss.str() + ".dat"));
  std::ofstream fileB(path / ("b-" + ss.str() + ".dat"));

  for (const auto& p : ps)
    ( p.type == Type::F ? fileF
    : p.type == Type::W ? fileW
    : p.type == Type::A ? fileA
    :                     fileB
    ) << p.x.x << " " << p.x.y << " "
      << p.v.x << " " << p.v.y << std::endl;
}

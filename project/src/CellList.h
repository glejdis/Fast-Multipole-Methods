#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <vector>

#include "Particle.h"
#include "util.h"

class CellList {
public:
  struct Config {
    const int nCells, NC;
    const double rc;
    const Vec<double> Fbody;
  };
  using ForceFunc = std::function<void(Particle&, Particle&)>;

private:
  const int N, nCells;
  const double rc;
  const Vec<double> Fbody;

  std::shared_ptr<std::vector<Particle>> ps;
  std::vector<int> cells;
  std::vector<Vec<int>> pCells;
  std::vector<int> pNexts;

  const ForceFunc calcForce;
  void buildCellList();

public:
  CellList( const Config config, ForceFunc calcForce
          , std::shared_ptr<std::vector<Particle>> ps
          );
  void calcForces();
};

CellList::CellList( const Config config, const ForceFunc calcForce
                  , std::shared_ptr<std::vector<Particle>> ps
                  )
  : N(ps->size()), nCells(config.nCells), rc(config.rc), Fbody(config.Fbody)
  , ps(ps), cells(config.NC), pCells(N), pNexts(N, -1)
  , calcForce(calcForce)
  {}

void CellList::buildCellList() {
  std::fill(cells.begin(), cells.end(), -1);

  for (int i = 0; i < N; ++i) {
    pCells[i].x = (*ps)[i].x.x / rc;
    pCells[i].y = (*ps)[i].x.y / rc;
    const int ic = idx(nCells, pCells[i]);

    const int head = cells[ic];
    pNexts[i] = head;
    cells[ic] = i;
  }
}

void CellList::calcForces() {
  buildCellList();

  for (auto& p : *ps) {
    p.aOld = p.aNew;
    p.aNew = p.type != Type::W ? Fbody : Vec<double>();
  }

  for (int i = 0; i < ps->size(); ++i) {
    // check particles in same cell "further down"
    for (int next = pNexts[i]; next != -1; next = pNexts[next])
      calcForce((*ps)[i], (*ps)[next]);

    // check all particles in four neighboring cells
    for (const auto& neigh : std::array<Vec<int>, 4>
         {{ { pCells[i].x              , (pCells[i].y + 1         ) % nCells}
          , {(pCells[i].x + 1) % nCells, (pCells[i].y + 1         ) % nCells}
          , {(pCells[i].x + 1) % nCells,  pCells[i].y                       }
          , {(pCells[i].x + 1) % nCells, (pCells[i].y - 1 + nCells) % nCells}
         }})
      for (int next = cells[idx(nCells, neigh)]; next != -1; next = pNexts[next])
        calcForce((*ps)[i], (*ps)[next]);
  }
}

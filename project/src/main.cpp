#include <array>
#include <filesystem>
#include <iostream>
#include <sstream>

#include "CellList.h"
#include "DpdSimulation.h"
#include "Fmm.h"
#include "util.h"

constexpr double L  = 15;
constexpr double rc = 1;
constexpr double ρ  = 4;

void run( const int N, const double dt, const int nSteps, const int countStart
        , const int writeFreq, const Setup setup
        ) {
  using path = std::filesystem::path;
  std::stringstream ss;
  ss << setup.name << "-N" << N << "-dt" << dt;
  path out = path("data") / ss.str();
  std::filesystem::create_directories(out);

  const int nCells = L / rc;
  const int NC = nCells * nCells;

  std::stringstream description, result;
  description << std::setw(10) << std::setfill(' ') << setup.name
              << std::setw( 4) << std::setfill(' ') << N
              << std::setw( 6) << std::setfill(' ') << dt;

  try {
    DpdSimulation<CellList> dpd( N, {nCells, NC, rc, setup.Fbody}
                               , L, rc, dt, setup, description.str(), out
                               );
    dpd.run(nSteps, countStart, writeFreq);

    const double momentum = dpd.getTracker().momentumNorm();
    result << (momentum < 1e-12 ? color::green : color::yellow)
           << std::setw(12) << std::setfill(' ') << momentum
           << color::normal << " "
           << dpd.getTracker().T();
  } catch (...) {
    result << color::red << " Unstable" << color::normal << std::endl;
  }

  std::cout << "\r" << std::setw(45) << std::setfill(' ') << "\r"
            << description.str() << result.str() << std::endl;
}

int main(int argc, char* argv[]) {
  constexpr int N = ρ * L*L;

  const Setup periodic    { .name = "Periodic" };
  const Setup couette     { .name = "Couette"
                          , .north = {Boundary::WALL, { 5, 0}}
                          , .south = {Boundary::WALL, {-5, 0}}
                          , .nMolecules = 42
                          , .molType = MoleculeType::CHAIN
                          , .KS = 100
                          , .rS = 0.1
                          };
  const Setup poiseuille  { .name = "Poiseuille"
                          , .west = {Boundary::WALL}
                          , .east = {Boundary::WALL}
                          , .Fbody = {0, -0.03}
                          , .nMolecules = 10
                          , .molType = MoleculeType::RING
                          , .KS = 100
                          , .rS = 0.3
                          };
  const Setup lidDrivenC  { .name = "LDC"
                          , .north = {Boundary::WALL, {5, 0}}
                          , .south = {Boundary::WALL}
                          , .west  = {Boundary::WALL}
                          , .east  = {Boundary::WALL}
                          };

  // (a)
  std::cout << color::magenta
            << "Investigating dependence of temperature on time step"
            << color::normal << std::endl << std::endl;

  std::cout << "     Setup   N    dt    Momentum Temperature" << std::endl;
  for (const auto dt : std::array<double, 8>{{0.005, 0.01, 0.02, 0.04, 0.06, 0.08, 0.10, 0.12}}) {
    const int nSteps = 40 / dt;
    run(N, dt, nSteps, nSteps, nSteps, periodic);
  }

  // (b), (c)
  std::cout << color::magenta << std::endl
            << "Simulating different setups"
            << color::normal << std::endl << std::endl;

  std::cout << "     Setup   N    dt    Momentum Temperature" << std::endl;
  run(N, 0.01,  2'000,  1'000,  10, couette);
  run(N, 0.01, 50'000, 10'000, 500, poiseuille);
  run(N, 0.01,  4'000,  3'000,  20, lidDrivenC);
  return 0;
}

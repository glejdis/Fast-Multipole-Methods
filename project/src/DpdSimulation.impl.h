#include <cmath>
#include <iostream>
#include <stdexcept>

#include "output.h"
#include "util.h"

template<class ForceCalculator>
DpdSimulation<ForceCalculator>::DpdSimulation
  ( const int N, const typename ForceCalculator::Config config
  , const double L, const double rc, const double dt
  , const Setup setup, const std::string description, const std::filesystem::path out
  )
  : N(N), L(L), rc(rc), rc²(rc * rc), dt(dt), dt⁻⁰⁵(1 / std::sqrt(dt)), nF(0)
  , ps(std::make_shared<std::vector<Particle>>(N))
  , force(config, [this] (Particle& p, Particle& q) { calcForce(p, q); }, ps)
  , rnd(L), setup(setup), tracker(L), description(description), out(out)
  {}

template<class ForceCalculator>
void DpdSimulation<ForceCalculator>::run(const int nSteps, const int countStart, const int writeFreq) {
  init();
  calcForces();

  tracker.nextStep();

  int step = 0;
  for (; step < nSteps; ++step) {
    velocityVerlet();

    if (step >= countStart)
      tracker.count((*ps));
    if ((step + 1) % writeFreq == 0)
      writePositions(out, (*ps), step + 1);

    std::cout << "\r" << description << " ";
    printProgress(nSteps, step);
  }

  tracker.write(out, nSteps - countStart);
}

template<class ForceCalculator>
Tracker DpdSimulation<ForceCalculator>::getTracker() {
  return tracker;
}

template<class ForceCalculator>
void DpdSimulation<ForceCalculator>::init() {
  const int nPartsPerMol = setup.molType == MoleculeType::CHAIN ? 7 : 9;
  const int nMolParticles = setup.nMolecules * nPartsPerMol;

  for (int im = 0; im < nMolParticles; im += nPartsPerMol) {
    const auto pos = rnd.pos();
    for (int ip = 0; ip < nPartsPerMol; ++ip) {
      (*ps)[im + ip].x = pos + Vec{setup.rS*rnd.ξ(), setup.rS*rnd.ξ()};
      (*ps)[im + ip].x.x = (*ps)[im + ip].x.x - L * std::floor((*ps)[im + ip].x.x / L);
      (*ps)[im + ip].x.y = (*ps)[im + ip].x.y - L * std::floor((*ps)[im + ip].x.y / L);

      (*ps)[im + ip].type = ( ip < 2 or setup.molType == MoleculeType::RING
                         ? Type::A
                         : Type::B
                         );

      if (ip != 0)
        bonds.emplace_back(im + ip - 1, im + ip);
    }
    if (setup.molType == MoleculeType::RING)
      bonds.emplace_back(im, im + nPartsPerMol - 1);
  }

  for (int i = nMolParticles; i < N; ++i) {
    Particle& p = (*ps)[i];
    p.x = rnd.pos();

           if (L - p.x.y < rc and setup.north.type == Boundary::WALL) {
      p.type = Type::W;
      p.v = setup.north.vWall;
    } else if (    p.x.y < rc and setup.south.type == Boundary::WALL) {
      p.type = Type::W;
      p.v = setup.south.vWall;
    } else if (    p.x.x < rc and setup.west.type == Boundary::WALL) {
      p.type = Type::W;
      p.v = setup.west.vWall;
    } else if (L - p.x.x < rc and setup.east.type == Boundary::WALL) {
      p.type = Type::W;
      p.v = setup.east.vWall;
    } else
      ++nF;
  }
}

template<class ForceCalculator>
void DpdSimulation<ForceCalculator>::calcForce(Particle& p, Particle& q) {
  constexpr double σ = 1;
  constexpr double γ = 4.5;

  Vec<double> d = p.x - q.x;
  d.x = d.x - L * std::round(d.x / L);
  d.y = d.y - L * std::round(d.y / L);

  const double r² = d.x * d.x + d.y * d.y;
  const double r  = std::sqrt(r²);

  const Vec<double> v = p.v - q.v;
  const Vec<double> e = d / r;

  if (r² < rc²) {
    const double ωR = 1.0 - r/rc;
    const double ωD = ωR * ωR;

    const double FC = a[p.type][q.type] * ωR;
    const double FD = -γ * ωD * (e.x*v.x + e.y*v.y);
    const double FR =  σ * ωR * rnd.ξ() * dt⁻⁰⁵;

    if (p.type != Type::W) p.aNew += e * (FC + FD + FR);
    if (q.type != Type::W) q.aNew -= e * (FC + FD + FR);
  }
}

template<class ForceCalculator>
void DpdSimulation<ForceCalculator>::calcForces() {
  force.calcForces();

  for (const auto& bond : bonds) {
    Particle& p = (*ps)[bond.x];
    Particle& q = (*ps)[bond.y];

    Vec<double> d = p.x - q.x;
    d.x = d.x - L * std::round(d.x / L);
    d.y = d.y - L * std::round(d.y / L);

    const double r² = d.x * d.x + d.y * d.y;
    const double r  = std::sqrt(r²);

    const Vec<double> e = d / r;

    p.aNew += e * setup.KS * (1 - r / setup.rS);
    q.aNew -= e * setup.KS * (1 - r / setup.rS);
  }
}

template<class ForceCalculator>
void DpdSimulation<ForceCalculator>::velocityVerlet() {
  constexpr double λ = 0.5;
  tracker.nextStep();

  for (auto& p : *ps) {
    // (1) r(t+Δt) = r(t) + Δt v(t) + ½Δt² f(t)
    const auto dx = p.v * dt + p.aNew * 0.5 * dt*dt;
    p.x += dx;

    if (dx.x * dx.x + dx.y * dx.y > rc²)
      throw std::runtime_error("dx > rc");

    p.x.x = p.x.x - L * std::floor(p.x.x / L);
    p.x.y = p.x.y - L * std::floor(p.x.y / L);

    // (2) ṽ(t+Δt) = v(t) + λΔt f(t)
    p.vOld = p.v;
    p.v += p.aNew * λ * dt;
  }

  // (3) f(t+Δt) = f(r(t+Δt), ṽ(t+Δt))
  calcForces();

  // (4) v(t+Δt) = v(t) + ½Δt (f(t) + f(t+Δt))
  for (auto& p : *ps) {
    p.v = p.vOld + (p.aOld + p.aNew) * 0.5 * dt;

    if (p.type == Type::F) {
      tracker.momentum() += p.v;
      tracker.T() += p.v.x * p.v.x + p.v.y * p.v.y;
    }
  }
  tracker.T() /= 3.0 * nF;
}

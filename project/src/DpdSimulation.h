#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include "Particle.h"
#include "Random.h"
#include "Setup.h"
#include "Tracker.h"

template<class ForceCalculator>
class DpdSimulation {
private:
  const int N;
  const double L;
  const double rc;
  const double rc²;
  const double dt;
  const double dt⁻⁰⁵;
  int nF;

  std::shared_ptr<std::vector<Particle>> ps;
  std::vector<Vec<int>> bonds;

  ForceCalculator force;
  Random rnd;
  Setup setup;
  Tracker tracker;
  const std::string description;
  const std::filesystem::path out;

  void init();
  void calcForce(Particle& p, Particle& q);
  void calcForces();
  void velocityVerlet();

public:
  DpdSimulation( const int N, const typename ForceCalculator::Config config
               , const double L, const double rc, const double dt
               , const Setup setup, const std::string description
               , const std::filesystem::path out
               );
  void run(const int nSteps, const int rdfStart, const int writeFreq);
  Tracker getTracker();
};

#include "DpdSimulation.impl.h"

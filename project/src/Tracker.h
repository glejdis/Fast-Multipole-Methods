#pragma once

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <numeric>
#include <vector>

#include "Vec.h"

class Tracker {
private:
  static constexpr double bin = 0.5;
  const double L;

  std::vector<Vec<double>> momentum_;
  std::vector<    double > T_;

  std::vector<std::vector<int>> n;
  std::vector<int> nx, ny;

  std::vector<std::vector<Vec<double>>> v;
  std::vector<double> vx, vy;

public:
  Tracker(const int L)
    : L(L)
    , n(L/bin, std::vector<int        >(L/bin)), nx(L/bin), ny(L/bin)
    , v(L/bin, std::vector<Vec<double>>(L/bin)), vx(L/bin), vy(L/bin)
    {}

  void nextStep() {
    momentum_.emplace_back();
    T_       .emplace_back(0);
  }

  Vec<double>& momentum() { return momentum_[momentum_.size() - 1]; }
      double & T       () { return T_       [T_       .size() - 1]; }

  void count(const std::vector<Particle>& ps) {
    for (const auto& p : ps) {
      const int i = p.x.x / bin;
      const int j = p.x.y / bin;

      ++n[i][j];
      ++nx[i];
      ++ny[j];

      v[i][j] += p.v;
      vx[i] += p.v.y;
      vy[j] += p.v.x;
    }
  }

  double momentumNorm() {
    return std::sqrt(momentum().x * momentum().x + momentum().y * momentum().y);
  }

  void write(const std::filesystem::path path, const int nCountStpes) {
    writeObservables(path);
    if (nCountStpes > 0)
      writeMacroscopic(path, nCountStpes);
  }

  void writeObservables(const std::filesystem::path path) {
    std::ofstream of(path / "observables.dat");
    of << "Iteration MomentumX MomentumY T" << std::endl;

    for (int i = 0; i < momentum_.size(); ++i)
      of << i << " " << momentum_[i].x << " " << momentum_[i].y
              << " " << T_[i]
              << std::endl;
  }

  void writeMacroscopic(const std::filesystem::path path, const int nCountStpes) {
    std::ofstream fileN    (path / "density.dat");
    std::ofstream fileNavgX(path / "density-avg-x.dat");
    std::ofstream fileNavgY(path / "density-avg-y.dat");

    std::ofstream fileV    (path / "velocity.dat");
    std::ofstream fileVavgX(path / "velocity-avg-x.dat");
    std::ofstream fileVavgY(path / "velocity-avg-y.dat");

    for (int i = 0; i < n.size(); ++i) {
      const double x = (i + 0.5) * bin;
      for (int j = 0; j < n.size(); ++j) {
        const double y = (j + 0.5) * bin;

        fileN << x << " " << y << " " << (n[i][j]   / (bin * bin * nCountStpes))
                               << std::endl;
        fileV << x << " " << y << " " << (v[i][j].x / n[i][j]    )
                               << " " << (v[i][j].y / n[i][j]    )
                               << std::endl;
      }

      fileNavgX << x << " " << (nx[i] / (bin * L * nCountStpes)) << std::endl;
      fileNavgY << x << " " << (ny[i] / (bin * L * nCountStpes)) << std::endl;

      fileVavgX << x << " " << (vx[i] / nx[i]) << std::endl;
      fileVavgY << x << " " << (vy[i] / ny[i]) << std::endl;
    }
  }
};

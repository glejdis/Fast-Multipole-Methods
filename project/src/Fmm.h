#pragma once

#include <algorithm>
#include <memory>
#include <ranges>
#include <vector>

#include "Particle.h"
#include "Tree.h"

class Fmm {
public:
  struct Config {
    const int p; // order of expansion
    const int s; // max number of particles per leaf
    const Vec<double> bbMin;
    const Vec<double> bbMax;
  };
  using ForceFunc = std::function<void(Particle&, Particle&)>;

private:
  std::shared_ptr<std::vector<Particle>> ps;
  Tree tree;

  const ForceFunc calcForce;
  void buildTree();
  void upwardPass();
  void downwardPass();

public:
  Fmm( const Config config, ForceFunc calcForce
     , std::shared_ptr<std::vector<Particle>> ps
     );
  void calcForces();
};

Fmm::Fmm( const Config config, const ForceFunc calcForce
        , std::shared_ptr<std::vector<Particle>> ps
        )
  : ps(ps), tree(config.s, config.bbMin, config.bbMax, config.p)
  , calcForce(calcForce) {
  buildTree();
}

void Fmm::calcForces() {
  tree.repair();

  upwardPass();
  downwardPass();
}

void Fmm::buildTree() {
  for (auto& p : *ps)
    tree.addParticle(&p);
}

void Fmm::upwardPass() {
  tree.traversePostOrder([] (Tree& t) {
    t.expOut.reset(t.center);

    if (t.isLeaf)
      for (const auto& p : t.ps)
        t.expOut += *p;
    else
      for (const auto& child : t.childs)
        if (child != nullptr)
          t.expOut += child->expOut;
  });
}

void Fmm::downwardPass() {
  using namespace std::ranges;
  using namespace std::views;

  static const auto nonNull = [] (const auto& t) { return t != nullptr; };

  tree.expInc.reset(tree.center);
  tree.traversePreOrder([&] (Tree& t) {
    if (t.isLeaf)
      for (auto p = t.ps.begin(); p != t.ps.end(); ++p) {
        t.expInc.addForce(**p);

        // compute symmetric force only once
        for (auto q = p+1; q != t.ps.end(); ++q)
          calcForce(**p, **q);
        for (int n = 0; n < 4; ++n)
          if (t.neighs[n] != nullptr)
            for (auto q : t.neighs[n]->ps)
              calcForce(**p, *q);
      }
    else
      for (auto& child : t.childs | filter(nonNull)) {
        child->expInc.reset(child->center);
        child->expInc += t.expInc;

        for (const auto& parentNeigh : t.neighs | filter(nonNull))
          for (const auto& neighChild : parentNeigh->childs | filter(nonNull))
            if (find(child->neighs, neighChild) == child->neighs.end())
              child->expInc += neighChild->expOut;
      }
  });
}

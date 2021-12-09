#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <functional>
#include <memory>
#include <vector>

#include "Expansion.h"
#include "Particle.h"
#include "Vec.h"

class Tree {
public:
  const int s;         // max number of particles per leaf

  const Vec<double> bbMin;
  const Vec<double> bbMax;
  const Vec<double> center;
  const Vec<double> d; // diameter

  expansion::Outgoing expOut;
  expansion::Incoming expInc;

  bool isLeaf;
  std::vector<Particle*> ps;

  // ┌───┬───┐
  // │ 2 │ 3 │
  // ├───┼───┤
  // │ 0 │ 1 │
  // └───┴───┘
  std::array<std::shared_ptr<Tree>, 4> childs;
  // 3 2 1
  // 4   0
  // 5 6 7
  // TODO cyclic references cause memory leaks
  std::array<std::shared_ptr<Tree>, 8> neighs;

  Tree(const int s, const Vec<double> bbMin, const Vec<double> bbMax, const int p);
  void addParticle(Particle* const p);
  void repair();
  void traversePreOrder (std::function<void(Tree&)> f);
  void traversePostOrder(std::function<void(Tree&)> f);

private:
  int findOrMakeChild(const Particle* const p);
  void updateNeighbors(const int newChild);
  std::shared_ptr<Tree> addNeigh(const int whichChild, const int toChild);
  std::shared_ptr<Tree> addNeigh(const int whichChild, const int toNeigh, const int toNeighChild);
  void moveParticles(Tree& root);
  void minimize();
  void deleteChild(const int child);
  void mergeChilds();
};

Tree::Tree(const int s, const Vec<double> bbMin, const Vec<double> bbMax, const int p)
  : s(s), bbMin(bbMin), bbMax(bbMax), center((bbMin + bbMax) * 0.5), d(bbMax - bbMin)
  , expOut(p, center), expInc(p, center), isLeaf(true)
  {}

void Tree::addParticle(Particle* const p) {
  assert(p->x >= bbMin and p->x <= bbMax);
  // TODO handle the degenerate case, where two particles are exactly at the same position
  // TODO handle the degenerate case, where a particle is exactly at the boundary between two nodes

  if (isLeaf and ps.size() < s) // add p to this leaf
    ps.push_back(p);
  else {
    const int child = findOrMakeChild(p);

    if (isLeaf) {               // split leaf
      isLeaf = false;
      childs[child]->ps.push_back(p);

      for (auto p = ps.begin(); p != ps.end(); )
        // If we currently repair the tree and reinsert a moved particle,
        // we might encounter particles which are out of place.
        // We leave them where they are for now
        // since the repair function will handle them later.
        if ((*p)->x >= bbMin and (*p)->x <= bbMax) {
          addParticle(*p);
          p = ps.erase(p);
        } else
          ++p;
    } else                      // add p to correct child
      childs[child]->addParticle(p);
  }
}

int Tree::findOrMakeChild(const Particle* const p) {
    const int cx = std::round((p->x.x - bbMin.x) / d.x); // ∈ {0, 1}
    const int cy = std::round((p->x.y - bbMin.y) / d.y); // ∈ {0, 1}
    const int child = cx + 2*cy;

    if (childs[child] == nullptr) {
      const Vec<double> childBBMin = bbMin + Vec<double>{   cx *d.x/2,    cy *d.y/2};
      const Vec<double> childBBMax = bbMax - Vec<double>{(1-cx)*d.x/2, (1-cy)*d.y/2};
      childs[child] = std::make_shared<Tree>(s, childBBMin, childBBMax, expOut.p);

      updateNeighbors(child);
    }

    return child;
}

void Tree::updateNeighbors(const int newChild) {
  switch (newChild) {
    case 0:
      childs[newChild]->neighs =
        { addNeigh(0, 1)   , addNeigh(0, 3)   , addNeigh(0, 2)   , addNeigh(0, 4, 3)
        , addNeigh(0, 4, 1), addNeigh(0, 5, 3), addNeigh(0, 6, 2), addNeigh(0, 6, 3)
        };
      break;
    case 1:
      childs[newChild]->neighs =
        { addNeigh(1, 0, 0), addNeigh(1, 0, 2), addNeigh(1, 3)   , addNeigh(1, 2)
        , addNeigh(1, 0)   , addNeigh(1, 6, 2), addNeigh(1, 6, 3), addNeigh(1, 7, 2)
        };
      break;
    case 2:
      childs[newChild]->neighs =
        { addNeigh(2, 3)   , addNeigh(2, 2, 1), addNeigh(2, 2, 0), addNeigh(2, 3, 1)
        , addNeigh(2, 4, 3), addNeigh(2, 4, 1), addNeigh(2, 0)   , addNeigh(2, 1)
        };
      break;
    case 3:
      childs[newChild]->neighs =
        { addNeigh(3, 0, 2), addNeigh(3, 1, 0), addNeigh(3, 2, 1), addNeigh(3, 2, 0)
        , addNeigh(3, 2)   , addNeigh(3, 0)   , addNeigh(3, 1)   , addNeigh(3, 0, 0)
        };
      break;
  }
}

std::shared_ptr<Tree> Tree::addNeigh(const int whichChild, const int toChild) {
  static const int neigh[4][4] = { {-1, 4, 6, 5}, {0, -1, 7, 6}
                                 , {2, 3, -1, 4}, {1, 2, 0, -1}
                                 };
  const int neighIdx = neigh[whichChild][toChild];
  assert(neighIdx != -1);

  std::shared_ptr<Tree> toTree = childs[toChild];
  if (toTree != nullptr)
    toTree->neighs[neighIdx] = childs[whichChild];

  return toTree;
}

std::shared_ptr<Tree> Tree::addNeigh( const int whichChild
                                    , const int toNeigh, const int toNeighChild
                                    ) {
  static const int neigh[4][3][4] = { {{-1, 0, -1, 7}, {-1, -1, -1, 1}, {-1, -1, 2, 3}}
                                    , {{-1, -1, 1, 2}, {-1, -1, 3, -1}, {4, -1, 5, -1}}
                                    , {{6, 5, -1, -1}, {-1, 7, -1, -1}, {-1, 1, -1, 0}}
                                    , {{3, -1, 4, -1}, {5, -1, -1, -1}, {7, 6, -1, -1}}
                                    };
  int neighIdx = -1;
  switch (whichChild) {
    case 0: neighIdx = neigh[whichChild][ toNeigh-4   ][toNeighChild]; break;
    case 1: neighIdx = neigh[whichChild][(toNeigh+2)%8][toNeighChild]; break;
    case 2: neighIdx = neigh[whichChild][ toNeigh-2   ][toNeighChild]; break;
    case 3: neighIdx = neigh[whichChild][ toNeigh     ][toNeighChild]; break;
  }
  assert(neighIdx != -1);

  std::shared_ptr<Tree> toTree = neighs[toNeigh];
  std::shared_ptr<Tree> toTreeChild;
  if (toTree != nullptr) {
    toTreeChild = toTree->childs[toNeighChild];
    if (toTreeChild != nullptr)
      toTreeChild->neighs[neighIdx] = childs[whichChild];
  }

  return toTreeChild;
}

void Tree::repair() {
  moveParticles(*this);
  minimize();
}

void Tree::moveParticles(Tree& root) {
  for (auto p = ps.begin(); p != ps.end(); )
    if (not ((*p)->x >= bbMin and (*p)->x <= bbMax) or not isLeaf) {
      // remove particles that left the domain
      if ((*p)->x >= root.bbMin and (*p)->x <= root.bbMax)
        root.addParticle(*p);
      p = ps.erase(p);
    } else
      ++p;

  for (auto& child : childs)
    if (child != nullptr)
      child->moveParticles(root);
}

void Tree::minimize() {
  for (auto& child : childs)
    if (child != nullptr)
      child->minimize();

  bool allChildsAreLeafs = true;
  int particlesInChilds = 0;
  for (int child = 0; child < childs.size(); ++child)
    if (childs[child] != nullptr) {
      if (childs[child]->isLeaf and childs[child]->ps.size() == 0)
        deleteChild(child);
      else {
        allChildsAreLeafs &= childs[child]->isLeaf;
        particlesInChilds += childs[child]->ps.size();
      }
    }

  if (allChildsAreLeafs and particlesInChilds <= s) {
    mergeChilds();
    isLeaf = true;
  }
}

void Tree::deleteChild(const int child) {
  for (auto& n : childs[child]->neighs)
    if (n != nullptr)
      std::ranges::replace(n->neighs, childs[child], nullptr);

  childs[child] = nullptr;
}

void Tree::mergeChilds() {
  for (int child = 0; child < childs.size(); ++child)
    if (childs[child] != nullptr) {
      ps.insert(ps.end(), childs[child]->ps.begin(), childs[child]->ps.end());
      deleteChild(child);
    }
}

void Tree::traversePreOrder(std::function<void(Tree&)> f) {
  f(*this);

  for (auto& child : childs)
    if (child != nullptr)
      child->traversePreOrder(f);
}

void Tree::traversePostOrder(std::function<void(Tree&)> f) {
  for (auto& child : childs)
    if (child != nullptr)
      child->traversePostOrder(f);

  f(*this);
}

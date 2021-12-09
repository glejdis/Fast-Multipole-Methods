#pragma once

#include <string>

#include "test.h"

#include "../src/Tree.h"

void testTree() {
  test::add({"Tree insert single", []() {
    Tree tree(1, {0, 0}, {1, 1}, 0);
    Particle p;
    tree.addParticle(&p);
    test::assertTrue(tree.isLeaf, "is leaf");
    test::assertTrue(tree.ps.size() == 1, "count");
  }});

  test::add({"Tree insert each quad (s=1)", []() {
    Tree tree(1, {0, 0}, {1, 1}, 0);

    std::vector<Particle> ps = { {{0.25, 0.25}}
                               , {{0.25, 0.75}}
                               , {{0.75, 0.25}}
                               , {{0.75, 0.75}}
                               };
    for (auto& p : ps)
      tree.addParticle(&p);

    test::assertTrue(not tree.isLeaf, "is inner");
    test::assertTrue(tree.ps.size() == 0, "inner count");

    for (const auto& leaf : tree.childs) {
      test::assertTrue(leaf->isLeaf, "is leaf");
      test::assertTrue(leaf->ps.size() == 1, "leaf count");
    }

    test::assertEqual(tree.childs[0]->bbMin, {0.0, 0.0}, "sw bbMin");
    test::assertEqual(tree.childs[0]->bbMax, {0.5, 0.5}, "sw bbMax");

    test::assertEqual(tree.childs[1]->bbMin, {0.5, 0.0}, "se bbMin");
    test::assertEqual(tree.childs[1]->bbMax, {1.0, 0.5}, "se bbMax");

    test::assertEqual(tree.childs[2]->bbMin, {0.0, 0.5}, "nw bbMin");
    test::assertEqual(tree.childs[2]->bbMax, {0.5, 1.0}, "nw bbMax");

    test::assertEqual(tree.childs[3]->bbMin, {0.5, 0.5}, "ne bbMin");
    test::assertEqual(tree.childs[3]->bbMax, {1.0, 1.0}, "ne bbMax");
  }});

  test::add({"Tree insert 2 to quad 0 (s=1)", []() {
    Tree tree(1, {0, 0}, {1, 1}, 0);

    std::vector<Particle> ps = { {{0.2, 0.2}}
                               , {{0.3, 0.3}}
                               };
    for (auto& p : ps)
      tree.addParticle(&p);

    test::assertTrue(not tree.isLeaf, "is inner");
    test::assertTrue(tree.ps.size() == 0, "inner count");

    test::assertTrue(tree.childs[0] != nullptr, "quad 0");
    test::assertTrue(tree.childs[1] == nullptr, "quad 1");
    test::assertTrue(tree.childs[2] == nullptr, "quad 2");
    test::assertTrue(tree.childs[3] == nullptr, "quad 3");

    test::assertTrue(not tree.childs[0]->isLeaf, "quad 0 is inner");

    test::assertTrue(tree.childs[0]->childs[0] != nullptr, "quad 00");
    test::assertTrue(tree.childs[0]->childs[1] == nullptr, "quad 01");
    test::assertTrue(tree.childs[0]->childs[2] == nullptr, "quad 02");
    test::assertTrue(tree.childs[0]->childs[3] != nullptr, "quad 03");

    test::assertTrue(tree.childs[0]->childs[0]->isLeaf, "quad 00 leaf");
    test::assertTrue(tree.childs[0]->childs[3]->isLeaf, "quad 03 leaf");
    test::assertEqual<int>(tree.childs[0]->childs[0]->ps.size(), 1, "quad 00 count");
    test::assertEqual<int>(tree.childs[0]->childs[3]->ps.size(), 1, "quad 03 count");
  }});

  test::add({"Tree neighbors (s=1) #1", []() {
    // L1:  a  │  b  L2: e f │ g h
    //         │             │ i j
    //     ────┼────     ────┼────
    //         │         k l │ m
    //      c  │  d        n │   o

    Tree tree(1, {0, 0}, {1, 1}, 0);

    std::vector<Particle> ps = { {{0.2, 0.8}}, {{0.4, 0.8}}, {{0.6, 0.8}}, {{0.8, 0.8}}
                               ,                             {{0.6, 0.6}}, {{0.8, 0.6}}
                               , {{0.2, 0.4}}, {{0.4, 0.4}}, {{0.6, 0.4}}
                               ,               {{0.4, 0.2}},               {{0.8, 0.2}}
                               };
    for (auto& p : ps)
      tree.addParticle(&p);

    const auto a = tree.childs[2]; const auto b = tree.childs[3];
    const auto c = tree.childs[0]; const auto d = tree.childs[1];
    const auto e = a->childs[2];   const auto f = a->childs[3];
    const auto g = b->childs[2];   const auto h = b->childs[3];
    const auto i = b->childs[0];   const auto j = b->childs[1];
    const auto k = c->childs[2];   const auto l = c->childs[3];
    const auto m = d->childs[2];   const auto n = c->childs[1];
    const auto o = d->childs[1];

    using N = std::array<std::shared_ptr<Tree>, 8>;
    test::assertTrue(a->neighs == N{b, 0, 0, 0, 0, 0, c, d}, "a");
    test::assertTrue(b->neighs == N{0, 0, 0, 0, a, c, d, 0}, "b");
    test::assertTrue(c->neighs == N{d, b, a, 0, 0, 0, 0, 0}, "c");
    test::assertTrue(d->neighs == N{0, 0, b, a, c, 0, 0, 0}, "d");
    test::assertTrue(e->neighs == N{f, 0, 0, 0, 0, 0, 0, 0}, "e");
    test::assertTrue(f->neighs == N{g, 0, 0, 0, e, 0, 0, i}, "f");
    test::assertTrue(g->neighs == N{h, 0, 0, 0, f, 0, i, j}, "g");
    test::assertTrue(h->neighs == N{0, 0, 0, 0, g, i, j, 0}, "h");
    test::assertTrue(i->neighs == N{j, h, g, f, 0, l, m, 0}, "i");
    test::assertTrue(j->neighs == N{0, 0, h, g, i, m, 0, 0}, "j");
    test::assertTrue(k->neighs == N{l, 0, 0, 0, 0, 0, 0, n}, "k");
    test::assertTrue(l->neighs == N{m, i, 0, 0, k, 0, n, 0}, "l");
    test::assertTrue(m->neighs == N{0, j, i, 0, l, n, 0, o}, "m");
    test::assertTrue(n->neighs == N{0, m, l, k, 0, 0, 0, 0}, "n");
    test::assertTrue(o->neighs == N{0, 0, 0, m, 0, 0, 0, 0}, "o");
  }});

  test::add({"Tree neighbors (s=1) #2", []() {
    // L1:  a  │  b  L2: e f │    
    //         │         g h │ i j
    //     ────┼────     ────┼────
    //         │         k   │   l
    //      c  │  d      m n │ o  

    Tree tree(1, {0, 0}, {1, 1}, 0);

    std::vector<Particle> ps = { {{0.2, 0.4}}, {{0.2, 0.2}}, {{0.4, 0.2}}
                               , {{0.6, 0.6}}, {{0.8, 0.6}}
                               , {{0.2, 0.8}}, {{0.2, 0.6}}, {{0.4, 0.8}}, {{0.4, 0.6}}
                               , {{0.6, 0.2}}, {{0.8, 0.4}}
                               };
    for (auto& p : ps)
      tree.addParticle(&p);

    const auto a = tree.childs[2]; const auto b = tree.childs[3];
    const auto c = tree.childs[0]; const auto d = tree.childs[1];
    const auto e = a->childs[2];   const auto f = a->childs[3];
    const auto g = a->childs[0];   const auto h = a->childs[1];
    const auto i = b->childs[0];   const auto j = b->childs[1];
    const auto k = c->childs[2];   const auto l = d->childs[3];
    const auto m = c->childs[0];   const auto n = c->childs[1];
    const auto o = d->childs[0];

    using N = std::array<std::shared_ptr<Tree>, 8>;
    test::assertTrue(a->neighs == N{b, 0, 0, 0, 0, 0, c, d}, "a");
    test::assertTrue(b->neighs == N{0, 0, 0, 0, a, c, d, 0}, "b");
    test::assertTrue(c->neighs == N{d, b, a, 0, 0, 0, 0, 0}, "c");
    test::assertTrue(d->neighs == N{0, 0, b, a, c, 0, 0, 0}, "d");
    test::assertTrue(e->neighs == N{f, 0, 0, 0, 0, 0, g, h}, "e");
    test::assertTrue(f->neighs == N{0, 0, 0, 0, e, g, h, i}, "f");
    test::assertTrue(g->neighs == N{h, f, e, 0, 0, 0, k, 0}, "g");
    test::assertTrue(h->neighs == N{i, 0, f, e, g, k, 0, 0}, "h");
    test::assertTrue(i->neighs == N{j, 0, 0, f, h, 0, 0, l}, "i");
    test::assertTrue(j->neighs == N{0, 0, 0, 0, i, 0, l, 0}, "j");
    test::assertTrue(k->neighs == N{0, h, g, 0, 0, 0, m, n}, "k");
    test::assertTrue(l->neighs == N{0, 0, j, i, 0, o, 0, 0}, "l");
    test::assertTrue(m->neighs == N{n, 0, k, 0, 0, 0, 0, 0}, "m");
    test::assertTrue(n->neighs == N{o, 0, 0, k, m, 0, 0, 0}, "n");
    test::assertTrue(o->neighs == N{0, l, 0, 0, n, 0, 0, 0}, "o");
  }});

  test::add({"Tree neighbors (s=1) all neighbors of 0", []() {
    // L1:  a  │  b  L2:   h │ g f
    //         │           i │ x e
    //     ────┼────     ────┼────
    //         │           j │ k l
    //      c  │  d      z   │

    Tree tree(1, {0, 0}, {1, 1}, 0);

    std::vector<Particle> ps = { {{0.6, 0.6}}
                               , {{0.8, 0.6}}, {{0.8, 0.8}}, {{0.6, 0.8}}
                               , {{0.4, 0.8}}, {{0.4, 0.6}}
                               , {{0.4, 0.4}}, {{0.2, 0.2}}
                               , {{0.6, 0.4}}, {{0.8, 0.4}}
                               };
    for (auto& p : ps)
      tree.addParticle(&p);

    const auto a = tree.childs[2]; const auto b = tree.childs[3];
    const auto c = tree.childs[0]; const auto d = tree.childs[1];
    const auto e = b->childs[1];   const auto f = b->childs[3];
    const auto g = b->childs[2];   const auto h = a->childs[3];
    const auto i = a->childs[1];   const auto j = c->childs[3];
    const auto k = d->childs[2];   const auto l = d->childs[3];
    const auto x = b->childs[0];   const auto z = c->childs[0];

    using N = std::array<std::shared_ptr<Tree>, 8>;
    test::assertTrue(x->neighs == N{e, f, g, h, i, j, k, l}, "x");
    test::assertTrue(a->neighs == N{b, 0, 0, 0, 0, 0, c, d}, "a");
    test::assertTrue(b->neighs == N{0, 0, 0, 0, a, c, d, 0}, "b");
    test::assertTrue(c->neighs == N{d, b, a, 0, 0, 0, 0, 0}, "c");
    test::assertTrue(d->neighs == N{0, 0, b, a, c, 0, 0, 0}, "d");
    test::assertTrue(e->neighs == N{0, 0, f, g, x, k, l, 0}, "e");
    test::assertTrue(f->neighs == N{0, 0, 0, 0, g, x, e, 0}, "f");
    test::assertTrue(g->neighs == N{f, 0, 0, 0, h, i, x, e}, "g");
    test::assertTrue(h->neighs == N{g, 0, 0, 0, 0, 0, i, x}, "h");
    test::assertTrue(i->neighs == N{x, g, h, 0, 0, 0, j, k}, "i");
    test::assertTrue(j->neighs == N{k, x, i, 0, 0, z, 0, 0}, "j");
    test::assertTrue(k->neighs == N{l, e, x, i, j, 0, 0, 0}, "k");
    test::assertTrue(l->neighs == N{0, 0, e, x, k, 0, 0, 0}, "l");
  }});

  test::add({"Tree neighbors (s=1) all neighbors of 1", []() {
    // L1:  a  │  b  L2:   h g │ f
    //         │           i x │ e
    //     ────┼────       ────┼────
    //         │           j k │ l
    //      c  │  d            │   z

    Tree tree(1, {0, 0}, {1, 1}, 0);

    std::vector<Particle> ps = { {{0.4, 0.6}}
                               , {{0.6, 0.6}}, {{0.6, 0.8}}
                               , {{0.4, 0.8}}, {{0.2, 0.8}}, {{0.2, 0.6}}
                               , {{0.2, 0.4}}, {{0.4, 0.4}}
                               , {{0.6, 0.4}}, {{0.8, 0.2}}
                               };
    for (auto& p : ps)
      tree.addParticle(&p);

    const auto a = tree.childs[2]; const auto b = tree.childs[3];
    const auto c = tree.childs[0]; const auto d = tree.childs[1];
    const auto e = b->childs[0];   const auto f = b->childs[2];
    const auto g = a->childs[3];   const auto h = a->childs[2];
    const auto i = a->childs[0];   const auto j = c->childs[2];
    const auto k = c->childs[3];   const auto l = d->childs[2];
    const auto x = a->childs[1];   const auto z = d->childs[1];

    using N = std::array<std::shared_ptr<Tree>, 8>;
    test::assertTrue(x->neighs == N{e, f, g, h, i, j, k, l}, "x");
    test::assertTrue(a->neighs == N{b, 0, 0, 0, 0, 0, c, d}, "a");
    test::assertTrue(b->neighs == N{0, 0, 0, 0, a, c, d, 0}, "b");
    test::assertTrue(c->neighs == N{d, b, a, 0, 0, 0, 0, 0}, "c");
    test::assertTrue(d->neighs == N{0, 0, b, a, c, 0, 0, 0}, "d");
    test::assertTrue(e->neighs == N{0, 0, f, g, x, k, l, 0}, "e");
    test::assertTrue(f->neighs == N{0, 0, 0, 0, g, x, e, 0}, "f");
    test::assertTrue(g->neighs == N{f, 0, 0, 0, h, i, x, e}, "g");
    test::assertTrue(h->neighs == N{g, 0, 0, 0, 0, 0, i, x}, "h");
    test::assertTrue(i->neighs == N{x, g, h, 0, 0, 0, j, k}, "i");
    test::assertTrue(j->neighs == N{k, x, i, 0, 0, 0, 0, 0}, "j");
    test::assertTrue(k->neighs == N{l, e, x, i, j, 0, 0, 0}, "k");
    test::assertTrue(l->neighs == N{0, 0, e, x, k, 0, 0, z}, "l");
  }});

  test::add({"Tree neighbors (s=1) all neighbors of 2", []() {
    // L1:  a  │  b  L2: z   │
    //         │           h │ g f
    //     ────┼────     ────┼────
    //         │           i │ x e
    //      c  │  d        j │ k l

    Tree tree(1, {0, 0}, {1, 1}, 0);

    std::vector<Particle> ps = { {{0.6, 0.4}}
                               , {{0.8, 0.4}}, {{0.8, 0.6}}, {{0.6, 0.6}}
                               , {{0.4, 0.6}}, {{0.2, 0.8}}
                               , {{0.4, 0.4}}, {{0.4, 0.2}}
                               , {{0.6, 0.2}}, {{0.8, 0.2}}
                               };
    for (auto& p : ps)
      tree.addParticle(&p);

    const auto a = tree.childs[2]; const auto b = tree.childs[3];
    const auto c = tree.childs[0]; const auto d = tree.childs[1];
    const auto e = d->childs[3];   const auto f = b->childs[1];
    const auto g = b->childs[0];   const auto h = a->childs[1];
    const auto i = c->childs[3];   const auto j = c->childs[1];
    const auto k = d->childs[0];   const auto l = d->childs[1];
    const auto x = d->childs[2];   const auto z = a->childs[2];

    using N = std::array<std::shared_ptr<Tree>, 8>;
    test::assertTrue(x->neighs == N{e, f, g, h, i, j, k, l}, "x");
    test::assertTrue(a->neighs == N{b, 0, 0, 0, 0, 0, c, d}, "a");
    test::assertTrue(b->neighs == N{0, 0, 0, 0, a, c, d, 0}, "b");
    test::assertTrue(c->neighs == N{d, b, a, 0, 0, 0, 0, 0}, "c");
    test::assertTrue(d->neighs == N{0, 0, b, a, c, 0, 0, 0}, "d");
    test::assertTrue(e->neighs == N{0, 0, f, g, x, k, l, 0}, "e");
    test::assertTrue(f->neighs == N{0, 0, 0, 0, g, x, e, 0}, "f");
    test::assertTrue(g->neighs == N{f, 0, 0, 0, h, i, x, e}, "g");
    test::assertTrue(h->neighs == N{g, 0, 0, z, 0, 0, i, x}, "h");
    test::assertTrue(i->neighs == N{x, g, h, 0, 0, 0, j, k}, "i");
    test::assertTrue(j->neighs == N{k, x, i, 0, 0, 0, 0, 0}, "j");
    test::assertTrue(k->neighs == N{l, e, x, i, j, 0, 0, 0}, "k");
    test::assertTrue(l->neighs == N{0, 0, e, x, k, 0, 0, 0}, "l");
  }});

  test::add({"Tree neighbors (s=1) all neighbors of 3", []() {
    // L1:  a  │  b  L2:     │   z
    //         │         h g │ f
    //     ────┼────     ────┼────
    //         │         i x │ e
    //      c  │  d      j k │ l

    Tree tree(1, {0, 0}, {1, 1}, 0);

    std::vector<Particle> ps = { {{0.4, 0.4}}
                               , {{0.6, 0.4}}, {{0.6, 0.6}}, {{0.8, 0.8}}
                               , {{0.4, 0.6}}, {{0.2, 0.6}}
                               , {{0.2, 0.4}}, {{0.2, 0.2}}
                               , {{0.4, 0.2}}, {{0.6, 0.2}}
                               };
    for (auto& p : ps)
      tree.addParticle(&p);

    const auto a = tree.childs[2]; const auto b = tree.childs[3];
    const auto c = tree.childs[0]; const auto d = tree.childs[1];
    const auto e = d->childs[2];   const auto f = b->childs[0];
    const auto g = a->childs[1];   const auto h = a->childs[0];
    const auto i = c->childs[2];   const auto j = c->childs[0];
    const auto k = c->childs[1];   const auto l = d->childs[0];
    const auto x = c->childs[3];   const auto z = b->childs[3];

    using N = std::array<std::shared_ptr<Tree>, 8>;
    test::assertTrue(x->neighs == N{e, f, g, h, i, j, k, l}, "x");
    test::assertTrue(a->neighs == N{b, 0, 0, 0, 0, 0, c, d}, "a");
    test::assertTrue(b->neighs == N{0, 0, 0, 0, a, c, d, 0}, "b");
    test::assertTrue(c->neighs == N{d, b, a, 0, 0, 0, 0, 0}, "c");
    test::assertTrue(d->neighs == N{0, 0, b, a, c, 0, 0, 0}, "d");
    test::assertTrue(e->neighs == N{0, 0, f, g, x, k, l, 0}, "e");
    test::assertTrue(f->neighs == N{0, z, 0, 0, g, x, e, 0}, "f");
    test::assertTrue(g->neighs == N{f, 0, 0, 0, h, i, x, e}, "g");
    test::assertTrue(h->neighs == N{g, 0, 0, 0, 0, 0, i, x}, "h");
    test::assertTrue(i->neighs == N{x, g, h, 0, 0, 0, j, k}, "i");
    test::assertTrue(j->neighs == N{k, x, i, 0, 0, 0, 0, 0}, "j");
    test::assertTrue(k->neighs == N{l, e, x, i, j, 0, 0, 0}, "k");
    test::assertTrue(l->neighs == N{0, 0, e, x, k, 0, 0, 0}, "l");
  }});

  test::add({"Tree repair (s=1) move", []() {
    //  ·· │             │
    //  ·· │           · │ ·
    // ────┼────  →  ────┼────
    //     │           · │ ·
    //     │             │

    Tree tree(1, {0, 0}, {1, 1}, 0);

    std::vector<Particle> ps = { {{0.2, 0.8}}, {{0.4, 0.8}}
                               , {{0.2, 0.6}}, {{0.4, 0.6}}
                               };
    for (auto& p : ps)
      tree.addParticle(&p);

    ps[0].x = {0.3, 0.3}; ps[1].x = {0.7, 0.3};
    ps[2].x = {0.3, 0.7}; ps[3].x = {0.7, 0.7};
    tree.repair();

    for (int c = 0; c < 4; ++c) {
      test::assertEqual(tree.childs[c]->isLeaf, true, std::to_string(c) + " leaf");
      test::assertEqual(tree.childs[c]->ps.size(), 1lu, std::to_string(c) + " size");
      test::assertEqual(tree.childs[c]->ps[0]->x, ps[c].x, std::to_string(c) + " pos");
    }
  }});

  test::add({"Tree repair (s=1) delete child", []() {
    //  · │           │ ·
    // ───┼───  →  ───┼───
    //  · │           │ ·

    Tree tree(1, {0, 0}, {1, 1}, 0);

    std::vector<Particle> ps = {{{0.2, 0.8}}, {{0.2, 0.2}}};
    for (auto& p : ps)
      tree.addParticle(&p);

    ps[0].x = {0.7, 0.3}; ps[1].x = {0.7, 0.7};
    tree.repair();

    test::assertEqual(tree.isLeaf, false, "root leaf");
    test::assertTrue(tree.childs[0] == nullptr, "delete child 0");
    test::assertTrue(tree.childs[2] == nullptr, "delete child 2");
    test::assertEqual(tree.childs[1]->ps.size(), 1lu, "child 1 size");
    test::assertEqual(tree.childs[1]->ps[0]->x, ps[0].x, "child 1 particle pos");
    test::assertEqual(tree.childs[3]->ps.size(), 1lu, "child 3 size");
    test::assertEqual(tree.childs[3]->ps[0]->x, ps[1].x, "child 3 particle pos");
  }});

  test::add({"Tree repair (s=1) remove outside domain", []() {
    Tree tree(1, {0, 0}, {1, 1}, 0);
    Particle p{{0.5, 0.5}};
    tree.addParticle(&p);

    p.x = {2, 0.3};
    tree.repair();

    test::assertEqual(tree.isLeaf, true, "root leaf");
    test::assertEqual(tree.ps.size(), 0lu, "particle count");
  }});

  test::add({"Tree repair (s=2)", []() {
    //  21 │             │ 11
    //  13 │           1 │ 22
    // ────┼────  →  ────┼────
    //   3 │1            │
    //   2 │          3  │

    Tree tree(2, {0, 0}, {1, 1}, 0);

    std::vector<Particle> ps = { {{0.2, 0.8}}, {{0.2, 0.9}}, {{0.4, 0.8}}
                               , {{0.2, 0.6}}
                               , {{0.4, 0.6}}, {{0.4, 0.7}}, {{0.3, 0.6}}

                               , {{0.3, 0.3}}, {{0.4, 0.4}}, {{0.3, 0.4}}
                               , {{0.3, 0.1}}, {{0.4, 0.2}}

                               , {{0.6, 0.4}}
                               };
    for (auto& p : ps)
      tree.addParticle(&p);

    ps[0].x = {0.6, 0.8}; ps[1].x = {0.8, 0.8}; ps[2].x = {0.6, 0.6};
    ps[3].x = {0.7, 0.7};
    ps[4].x = {0.8, 0.6}; ps[5].x = {0.8, 0.7};

    ps[7].x = {-1, 0.6}; ps[8].x = {-1, -1}; ps[9].x = {2, 2};
    ps[10].x = {0.2, 0.2}; ps[11].x = {0.1, 0.1};
    ps[12].x = {0.1, 0.11};
    tree.repair();

    test::assertEqual(tree.isLeaf, false, "root leaf");
    test::assertEqual(tree.ps.size(), 0lu, "root size");

    test::assertEqual(tree.childs[0]->isLeaf, false, "0 leaf");
    test::assertEqual(tree.childs[0]->ps.size(), 0lu, "0 size");
    test::assertTrue(tree.childs[0]->childs[1] == nullptr, "01 null");
    test::assertTrue(tree.childs[0]->childs[2] == nullptr, "02 null");
    test::assertTrue(tree.childs[0]->childs[3] == nullptr, "03 null");
    test::assertEqual(tree.childs[0]->childs[0]->isLeaf, false, "00 leaf");
    test::assertEqual(tree.childs[0]->childs[0]->ps.size(), 0lu, "00 size");
    test::assertEqual(tree.childs[0]->childs[0]->childs[0]->isLeaf, true, "000 leaf");
    test::assertEqual(tree.childs[0]->childs[0]->childs[0]->ps.size(), 2lu, "000 size");
    test::assertEqual(tree.childs[0]->childs[0]->childs[3]->isLeaf, true, "003 leaf");
    test::assertEqual(tree.childs[0]->childs[0]->childs[3]->ps.size(), 1lu, "003 size");

    test::assertTrue(tree.childs[1] == nullptr, "1 null");

    test::assertEqual(tree.childs[2]->isLeaf, true, "2 leaf");
    test::assertEqual(tree.childs[2]->ps.size(), 1lu, "2 size");
    test::assertTrue(tree.childs[2]->childs[0] == nullptr, "20 null");
    test::assertTrue(tree.childs[2]->childs[1] == nullptr, "21 null");
    test::assertTrue(tree.childs[2]->childs[2] == nullptr, "22 null");
    test::assertTrue(tree.childs[2]->childs[3] == nullptr, "23 null");

    test::assertEqual(tree.childs[3]->isLeaf, false, "3 leaf");
    test::assertEqual(tree.childs[3]->ps.size(), 0lu, "3 size");
    test::assertEqual(tree.childs[3]->childs[0]->isLeaf, true, "30 leaf");
    test::assertEqual(tree.childs[3]->childs[0]->ps.size(), 2lu, "30 size");
    test::assertEqual(tree.childs[3]->childs[1]->isLeaf, true, "31 leaf");
    test::assertEqual(tree.childs[3]->childs[1]->ps.size(), 2lu, "31 size");
    test::assertEqual(tree.childs[3]->childs[2]->isLeaf, true, "32 leaf");
    test::assertEqual(tree.childs[3]->childs[2]->ps.size(), 1lu, "32 size");
    test::assertEqual(tree.childs[3]->childs[3]->isLeaf, true, "33 leaf");
    test::assertEqual(tree.childs[3]->childs[3]->ps.size(), 1lu, "33 size");
  }});

  test::add({"Tree repair (s=1) neighbors", []() {
    // L1:  a  │  b  L2:   h │ g f
    //         │           i │ x e
    //     ────┼────     ────┼────
    //         │           j │ k l
    //      c  │  d      z   │

    Tree tree(1, {0, 0}, {1, 1}, 0);

    std::vector<Particle> ps = { {{0.1, 0.1}}
                               , {{0.1, 0.2}}, {{0.3, 0.8}}, {{0.7, 0.8}}
                               , {{0.1, 0.3}}, {{0.3, 0.6}}
                               , {{0.1, 0.4}}, {{0.3, 0.2}}
                               , {{0.1, 0.6}}, {{0.3, 0.4}}
                               };
    for (auto& p : ps)
      tree.addParticle(&p);

    ps[0].x = {0.6, 0.6};
    ps[1].x = {0.8, 0.6}; ps[5].x = {0.8, 0.8}; ps[9].x = {0.6, 0.8};
    ps[2].x = {0.4, 0.8}; ps[6].x = {0.4, 0.6};
    ps[3].x = {0.4, 0.4}; ps[7].x = {0.2, 0.2};
    ps[4].x = {0.6, 0.4}; ps[8].x = {0.8, 0.4};
    tree.repair();

    const auto a = tree.childs[2]; const auto b = tree.childs[3];
    const auto c = tree.childs[0]; const auto d = tree.childs[1];
    const auto e = b->childs[1];   const auto f = b->childs[3];
    const auto g = b->childs[2];   const auto h = a->childs[3];
    const auto i = a->childs[1];   const auto j = c->childs[3];
    const auto k = d->childs[2];   const auto l = d->childs[3];
    const auto x = b->childs[0];   const auto z = c->childs[0];

    using N = std::array<std::shared_ptr<Tree>, 8>;
    test::assertTrue(x->neighs == N{e, f, g, h, i, j, k, l}, "x");
    test::assertTrue(a->neighs == N{b, 0, 0, 0, 0, 0, c, d}, "a");
    test::assertTrue(b->neighs == N{0, 0, 0, 0, a, c, d, 0}, "b");
    test::assertTrue(c->neighs == N{d, b, a, 0, 0, 0, 0, 0}, "c");
    test::assertTrue(d->neighs == N{0, 0, b, a, c, 0, 0, 0}, "d");
    test::assertTrue(e->neighs == N{0, 0, f, g, x, k, l, 0}, "e");
    test::assertTrue(f->neighs == N{0, 0, 0, 0, g, x, e, 0}, "f");
    test::assertTrue(g->neighs == N{f, 0, 0, 0, h, i, x, e}, "g");
    test::assertTrue(h->neighs == N{g, 0, 0, 0, 0, 0, i, x}, "h");
    test::assertTrue(i->neighs == N{x, g, h, 0, 0, 0, j, k}, "i");
    test::assertTrue(j->neighs == N{k, x, i, 0, 0, z, 0, 0}, "j");
    test::assertTrue(k->neighs == N{l, e, x, i, j, 0, 0, 0}, "k");
    test::assertTrue(l->neighs == N{0, 0, e, x, k, 0, 0, 0}, "l");
  }});

  test::add({"Tree traverse pre order", []() {
    Tree tree(1, {0, 0}, {1, 1}, 0);

    std::vector<Particle> ps = { {{0.4, 0.4}}, {{0.2, 0.2}}
                               , {{0.2, 0.4}}, {{0.4, 0.2}}
                               };
    for (auto& p : ps)
      tree.addParticle(&p);

    std::vector<double> dxs;
    tree.traversePreOrder([&] (Tree& t) { dxs.push_back(t.d.x); });

    test::assertTrue(dxs == std::vector<double>{1, 0.5, 0.25, 0.25, 0.25, 0.25}, "d");
  }});

  test::add({"Tree traverse post order", []() {
    Tree tree(1, {0, 0}, {1, 1}, 0);

    std::vector<Particle> ps = { {{0.4, 0.4}}, {{0.2, 0.2}}
                               , {{0.2, 0.4}}, {{0.4, 0.2}}
                               };
    for (auto& p : ps)
      tree.addParticle(&p);

    std::vector<double> dxs;
    tree.traversePostOrder([&] (Tree& t) { dxs.push_back(t.d.x); });

    test::assertTrue(dxs == std::vector<double>{0.25, 0.25, 0.25, 0.25, 0.5, 1}, "d");
  }});
}

#pragma once

#include <cmath>
#include <string>

#include "test.h"

#include "../src/Expansion.h"

void testExpansion() {
  const auto force = [] (const Particle& s, const Particle& t) {
    const auto d = s.x - t.x;
    return d / (d.x * d.x + d.y * d.y);
  };

  const auto accuracy = [] (const int p) {
    const double α = std::sqrt(2) / (4 - std::sqrt(2));
    return std::exp(std::log(α) * p);
  };

  test::add({"Expansion single to single", [&]() {
    for (int p = 1; p < 16; ++p) {
      expansion::Outgoing out(p, {-1, 0});
      expansion::Incoming inc(p, { 1, 0});

      Particle s({-0.8,  0.2});
      Particle t({ 1.1, -0.1});

      out += s;
      inc += out;
      inc.addForce(t);

      const auto f = force(s, t);
      const auto pDesc = "[p = " + std::to_string(p) + "] ";
      test::assertClose(t.aNew.x, f.x, accuracy(p), pDesc + "x");
      test::assertClose(t.aNew.y, f.y, accuracy(p), pDesc + "y");
    }
  }});

  test::add({"Expansion cluster to single", [&]() {
    for (int p = 1; p < 16; ++p) {
      expansion::Outgoing out(p, {-1, 0});
      expansion::Incoming inc(p, { 1, 0});
      Vec<double> f;

      std::vector<Particle> ss = { {{-1   ,  0   }}
                                 , {{-0.9 ,  0.15}}
                                 , {{-0.75,  0.25}}
                                 , {{-1.2 , -0.15}}
                                 };
      Particle t({ 1.1, -0.1});

      for (const auto& s : ss) {
        out += s;
        f += force(s, t);
      }
      inc += out;
      inc.addForce(t);

      const auto pDesc = "[p = " + std::to_string(p) + "] ";
      test::assertClose(t.aNew.x, f.x, accuracy(p), pDesc + "x");
      test::assertClose(t.aNew.y, f.y, accuracy(p), pDesc + "y");
    }
  }});

  test::add({"Expansion single to cluster", [&]() {
    for (int p = 1; p < 16; ++p) {
      expansion::Outgoing out(p, {-1, 0});
      expansion::Incoming inc(p, { 1, 0});

      Particle s({-0.8,  0.2});
      std::vector<Particle> ts = { {{1   ,  0   }}
                                 , {{0.8 ,  0.05}}
                                 , {{0.95,  0.25}}
                                 , {{1.15, -0.15}}
                                 };

      out += s;
      inc += out;
      for (auto& t : ts) {
        inc.addForce(t);
        const auto f = force(s, t);

        const auto pDesc = "[p = " + std::to_string(p) + "] ";
        test::assertClose(t.aNew.x, f.x, accuracy(p), pDesc + "x");
        test::assertClose(t.aNew.y, f.y, accuracy(p), pDesc + "y");
      }
    }
  }});

  test::add({"Expansion cluster to cluster", [&]() {
    for (int p = 1; p < 16; ++p) {
      expansion::Outgoing out(p, {-1, 0});
      expansion::Incoming inc(p, { 1, 0});

      std::vector<Particle> ss = { {{-1   ,  0   }}
                                 , {{-0.9 ,  0.15}}
                                 , {{-0.75,  0.25}}
                                 , {{-1.2 , -0.15}}
                                 };
      std::vector<Particle> ts = { {{1   ,  0   }}
                                 , {{0.8 ,  0.05}}
                                 , {{0.95,  0.25}}
                                 , {{1.15, -0.15}}
                                 };

      for (const auto& s : ss)
        out += s;

      inc += out;

      for (auto& t : ts) {
        inc.addForce(t);

        Vec<double> f;
        for (const auto& s : ss)
          f += force(s, t);

        const auto pDesc = "[p = " + std::to_string(p) + "] ";
        test::assertClose(t.aNew.x, f.x, accuracy(p), pDesc + "x");
        test::assertClose(t.aNew.y, f.y, accuracy(p), pDesc + "y");
      }
    }
  }});

  test::add({"Expansion translate outgoing", [&]() {
    for (int p = 1; p < 16; ++p) {
      expansion::Outgoing out(p, {-1, 0});
      expansion::Incoming inc(p, { 1, 0});
      Vec<double> f;

      std::vector<Particle> ss = { {{-1   ,  0   }}
                                 , {{-0.9 ,  0.15}}
                                 , {{-0.75,  0.25}}
                                 , {{-1.2 , -0.15}}
                                 };
      Particle t({ 1.1, -0.1});

      for (const auto& s : ss) {
        out += s;
        f += force(s, t);
      }

      expansion::Outgoing out1(p, {-1.1, 0.1});
      expansion::Outgoing out2(p, {-1.2, 0.2});
      out1 += out;
      out2 += out1;

      inc += out2;
      inc.addForce(t);

      const auto pDesc = "[p = " + std::to_string(p) + "] ";
      test::assertClose(t.aNew.x, f.x, accuracy(p), pDesc + "x");
      test::assertClose(t.aNew.y, f.y, accuracy(p), pDesc + "y");
    }
  }});

  test::add({"Expansion translate incoming", [&]() {
    for (int p = 1; p < 16; ++p) {
      expansion::Outgoing out(p, {-1, 0});
      expansion::Incoming inc(p, { 1, 0});

      Particle s({-0.8,  0.2});
      std::vector<Particle> ts = { {{1   ,  0   }}
                                 , {{0.8 ,  0.05}}
                                 , {{0.95,  0.25}}
                                 , {{1.15, -0.15}}
                                 };

      out += s;
      inc += out;

      expansion::Incoming inc1(p, {1.1, 0.1});
      expansion::Incoming inc2(p, {1.2, 0.2});
      inc1 += inc;
      inc2 += inc1;

      for (auto& t : ts) {
        inc2.addForce(t);
        const auto f = force(s, t);

        const auto pDesc = "[p = " + std::to_string(p) + "] ";
        test::assertClose(t.aNew.x, f.x, accuracy(p), pDesc + "x");
        test::assertClose(t.aNew.y, f.y, accuracy(p), pDesc + "y");
      }
    }
  }});

  test::add({"Expansion reset outgoing", [&]() {
    for (int p = 1; p < 16; ++p) {
      expansion::Outgoing out(p, {-1, 0});
      expansion::Incoming inc(p, { 1, 0});

      Particle s({-0.8,  0.2});
      Particle t({ 1.1, -0.1});

      for (int i = 0; i < 42; ++i)
        out += s;

      out.reset({-1, 0});
      out += s;

      inc += out;
      inc.addForce(t);

      const auto f = force(s, t);
      const auto pDesc = "[p = " + std::to_string(p) + "] ";
      test::assertClose(t.aNew.x, f.x, accuracy(p), pDesc + "x");
      test::assertClose(t.aNew.y, f.y, accuracy(p), pDesc + "y");
    }
  }});

  test::add({"Expansion reset incoming", [&]() {
    for (int p = 1; p < 16; ++p) {
      expansion::Outgoing out(p, {-1, 0});
      expansion::Incoming inc(p, { 1, 0});

      Particle s({-0.8,  0.2});
      Particle t({ 1.1, -0.1});

      out += s;

      for (int i = 0; i < 42; ++i)
        inc += out;

      inc.reset({1, 0});
      inc += out;
      inc.addForce(t);

      const auto f = force(s, t);
      const auto pDesc = "[p = " + std::to_string(p) + "] ";
      test::assertClose(t.aNew.x, f.x, accuracy(p), pDesc + "x");
      test::assertClose(t.aNew.y, f.y, accuracy(p), pDesc + "y");
    }
  }});
}

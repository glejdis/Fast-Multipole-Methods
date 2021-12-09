#pragma once

#include <algorithm>
#include <cassert>
#include <complex>
#include <vector>

#include "binomial.h"
#include "Particle.h"
#include "Vec.h"

// TODO (optimzation) never calculate v[0], it is never used
namespace expansion {
  class Incoming;

  class Outgoing {
    friend class Incoming;

  private:
    Vec<double> center;
    std::vector<std::complex<double>> q;

  public:
    const int p; // order ≥ 0

    Outgoing(const int p, const Vec<double> center);
    void reset(const Vec<double> newCenter);
    void operator+=(const Particle& p); // ofs outgoing from sources
    void operator+=(const Outgoing& o); // ofo outgoing from outgoing
  };

  class Incoming {
  private:
    Vec<double> center;
    std::vector<std::complex<double>> v;

  public:
    const int p; // order ≥ 0

    Incoming(const int p, const Vec<double> center);
    void reset(const Vec<double> newCenter);
    void operator+=(const Outgoing& o); // ifo incoming from outgoing
    void operator+=(const Incoming& i); // ifi incoming from incoming
    void addForce(Particle& p);         // tfi target from incoming
  };

  Outgoing::Outgoing(const int p, const Vec<double> center)
    : center(center), q(p+1, {0, 0}), p(p) {}

  void Outgoing::reset(const Vec<double> newCenter) {
    center = newCenter;
    std::ranges::fill(q, std::complex<double>{0, 0});
  }

  void Outgoing::operator+=(const Particle& p) {
    // q₀ += m
    // qₖ += - 1/k (x - c)ᵏ m

    std::complex<double> d  = p.x - center;
    std::complex<double> dᵏ = 1;

    q[0] += p.m;
    for (int k = 1; k < q.size(); ++k) {
      dᵏ *= d;
      q[k] += -p.m/k * dᵏ;
    }
  }

  void Outgoing::operator+=(const Outgoing& o) {
    // this.q₀ += q₀
    // this.qₖ += -q₀ 1/k (cₒ - c)ᵏ + Σⱼ₌₁ qⱼ binom(k-1, j-1)  (cₒ - c)ᵏ⁻ʲ

    assert(p == o.p);

    std::complex<double> d  = o.center - center;
    std::complex<double> dᵏ = 1;

    q[0] += o.q[0];
    for (int k = 1; k < q.size(); ++k) {
      dᵏ *= d;

      q[k] += -1./k * o.q[0] * dᵏ;
      for (int j = 1; j <= k; ++j)
        // TODO (optimzation) don't use pow
        q[k] += o.q[j] * static_cast<double>(binomial(k-1, j-1)) * std::pow(d, k-j);
    }
  }

  Incoming::Incoming(const int p, const Vec<double> center)
    : center(center), v(p+1, {0, 0}), p(p) {}

  void Incoming::reset(const Vec<double> newCenter) {
    center = newCenter;
    std::ranges::fill(v, std::complex<double>{0, 0});
  }

  void Incoming::operator+=(const Outgoing& o) {
    // v₀ += q₀ log(cᵢ - cₒ) + Σⱼ₌₁ qⱼ (-1)ʲ 1/(cₒ - cᵢ)ʲ
    // vₖ += -q₀ 1/(k (cₒ - cᵢ)ᵏ) + Σⱼ₌₁ qⱼ (-1)ʲ binom(k+j-1, j-1)  1/(cₒ - cᵢ)ᵏ⁺ʲ

    assert(p == o.p);

    std::complex<double> d  = o.center - center;
    std::complex<double> dᵏ = 1;
    int neg1ᵏ = 1;

    v[0] += o.q[0] * std::log(-d);
    for (int k = 1; k < v.size(); ++k) {
      int neg1ʲ = 1;
      dᵏ *= d;
      neg1ᵏ *= -1;

      v[0] += o.q[k] * static_cast<double>(neg1ᵏ) / dᵏ;

      v[k] += -1./k * o.q[0]/dᵏ;
      for (int j = 1; j < v.size(); ++j) {
        neg1ʲ *= -1;

        // TODO (optimzation) don't use pow
        v[k] += o.q[j] * static_cast<double>(neg1ʲ * binomial(k+j-1, j-1)) / std::pow(d, k+j);
      }
    }
  }

  void Incoming::operator+=(const Incoming& i) {
    // this.vₖ += Σⱼ₌ₖ vⱼ binom(j, k)  (c - cᵢ)ʲ⁻ᵏ

    assert(p == i.p);

    std::complex<double> d  = center - i.center;

    for (int k = 0; k < v.size(); ++k)
      for (int j = k; j < v.size(); ++j)
        // TODO (optimzation) don't use pow
        v[k] += i.v[j] * static_cast<double>(binomial(j, k)) * std::pow(d, j-k);
  }

  void Incoming::addForce(Particle& p) {
    //  Φ(x) = Σₖ   (x - c)ᵏ   vₖ
    // ∂Φ/∂x = Σₖ k (x - c)ᵏ⁻¹ vₖ
    // f = -∇Re(Φ) = -(Re(∂Φ/∂x), -Im(∂Φ/∂x))

    std::complex<double> d    = p.x - center;
    std::complex<double> dᵏ⁻¹ = 1;
    std::complex<double> dΦ   = 0; // k = 0

    for (int k = 1; k < v.size(); ++k) {
      dΦ += static_cast<double>(k) * dᵏ⁻¹ * v[k];
      dᵏ⁻¹ *= d;
    }

    const Vec<double> f = {dΦ.real(), -dΦ.imag()};
    p.aNew -= f / p.m; // subtract because gravity is attractive
  }
};

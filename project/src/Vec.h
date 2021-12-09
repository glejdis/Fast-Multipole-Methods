#pragma once

#include <compare>
#include <complex>
#include <ostream>

template<class T>
class Vec {
public:
  T x, y;

  Vec() : x(0), y(0) {}
  Vec(const T x, const T y) : x(x), y(y) {}

  void operator+=(const Vec<T>& v) { x += v.x; y += v.y; }
  void operator-=(const Vec<T>& v) { x -= v.x; y -= v.y; }
  Vec<T> operator+(const Vec<T>& v) const { return {x + v.x, y + v.y}; }
  Vec<T> operator-(const Vec<T>& v) const { return {x - v.x, y - v.y}; }
  Vec<double> operator*(const double s) const { return {x * s, y * s}; }
  Vec<double> operator/(const double s) const { return {x / s, y / s}; }

  std::partial_ordering operator<=>(const Vec<T>& v) const {
    if (x <  v.x and y <  v.y) return std::partial_ordering::less;
    if (x == v.x and y == v.y) return std::partial_ordering::equivalent;
    if (x >  v.x and y >  v.y) return std::partial_ordering::greater;
    else                       return std::partial_ordering::unordered;
  }
  bool operator==(const Vec<T>&) const = default;

  operator std::complex<T>() const { return {x, y}; }
};

template<class T>
std::ostream& operator<<(std::ostream& os, const Vec<T>& v) {
    return os << "(" << v.x << ", " << v.y << ")";
}

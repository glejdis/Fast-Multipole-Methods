#pragma once

#include <cassert>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

namespace test {
  class Test;

  namespace internal {
    std::vector<Test> tests;
    Test* current = nullptr;

    namespace color {
      const std::string normal  = "\033[0m";
      const std::string red     = "\033[31m";
      const std::string green   = "\033[32m";
    }
  }

  class Test {
    std::string desc;
    std::function<void()> f;

    bool failed;
    std::vector<std::string> failures;

    void run() {
      internal::current = this;

      f();

      if (failures.size() > 0) {
        failed = true;
        std::cout << internal::color::red
                  << "[FAIL " << std::setw(4) << failures.size() << "] "
                  << internal::color::normal;
      } else
        std::cout << internal::color::green
                  << "[PASS     ] "
                  << internal::color::normal;
      std::cout << desc << std::endl;

      for (const auto f : failures)
        std::cout << "  " << f << std::endl;

      internal::current = nullptr;
    }

    friend void assertTrue(const bool val, const std::string desc);
    template<class T>
    friend void assertEqual(const T& actual, const T& expected, const std::string desc);
    friend void assertClose( const double actual
                           , const double expected
                           , const double ϵ
                           , const std::string desc
                           );

    friend void add(Test test);
    friend bool runAll();

  public:
    Test(const std::string desc, std::function<void()> f)
      : desc(desc), f(f), failed(false)
      {}
  };

  void assertTrue(const bool val, const std::string desc) {
    assert(internal::current != nullptr);

    if (not val)
      internal::current->failures.push_back(desc);
  }

  template<class T>
  void assertEqual(const T& actual, const T& expected, const std::string desc) {
    assert(internal::current != nullptr);

    if (actual != expected) {
      std::stringstream ss;
      ss << desc << ": expected " << expected
                 << ", got "      << actual;
      internal::current->failures.push_back(ss.str());
    }
  }

  void assertClose( const double actual
                  , const double expected
                  , const double ϵ
                  , const std::string desc
                  ) {
    assert(internal::current != nullptr);

    const double d = std::abs(actual - expected);
    if (d > ϵ) {
      std::stringstream ss;
      ss << desc << ": expected " << expected
                 << ", got "      << actual
                 << ", |diff| = " << d
                 << " > "         << ϵ;
      internal::current->failures.push_back(ss.str());
    }
  }

  void add(Test test) {
    internal::tests.push_back(test);
  }

  bool runAll() {
    using namespace std::views;

    for (auto& t : internal::tests)
      t.run();

    auto failed = internal::tests | filter([] (const Test& t) { return t.failed; });

    std::cout << std::endl;
    if (std::ranges::empty(failed))
      std::cout << internal::color::green <<  "Passed"
                << internal::color::normal << std::endl;
    else {
      std::cout << "Failed Tests:" << std::endl;
      int nFailed = 0;
      for (auto& t : failed) {
        ++nFailed;
        std::cout << "  " << t.desc << std::endl;
      }

      std::cout << internal::color::red << nFailed << " failures"
                << internal::color::normal << std::endl;
    }

    return std::ranges::empty(failed);
  }
}

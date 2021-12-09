#pragma once

#include "test.h"

#include "../src/binomial.h"

void testBinomial() {
  test::add({"Binomial", []() {
    test::assertEqual(binomial(0, 0),  1, "0 choose 0");

    test::assertEqual(binomial(1, 0),  1, "1 choose 0");
    test::assertEqual(binomial(1, 1),  1, "1 choose 1");

    test::assertEqual(binomial(2, 0),  1, "2 choose 0");
    test::assertEqual(binomial(2, 1),  2, "2 choose 1");
    test::assertEqual(binomial(2, 2),  1, "2 choose 2");

    test::assertEqual(binomial(6, 4), 15, "6 choose 4");
    test::assertEqual(binomial(5, 3), 10, "5 choose 3");
  }});
}

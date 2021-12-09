#pragma once

#include "test.h"

#include "../src/Vec.h"

void testVec() {
  test::add({"Vector comparison equal", []() {
    Vec<double> a = {1, 2};
    Vec<double> b = {1, 2};
    test::assertTrue(    (a == b), "equal");
    test::assertTrue(not (a  < b), "not less");
    test::assertTrue(not (a  > b), "not greater");
  }});

  test::add({"Vector comparison ordered", []() {
    Vec<double> a = {1, 1};
    Vec<double> b = {2, 2};
    test::assertTrue(not (a == b), "not equal");
    test::assertTrue(    (a  < b), "less");
    test::assertTrue(not (a  > b), "not greater");
  }});

  test::add({"Vector comparison unordered", []() {
    Vec<double> a = {1, 2};
    Vec<double> b = {2, 2};
    test::assertTrue(not (a == b), "not equal");
    test::assertTrue(not (a  < b), "not less");
    test::assertTrue(not (a  > b), "not greater");
  }});
}

#include "test.h"

#include "binomial.h"
#include "expansion.h"
#include "tree.h"
#include "vec.h"

int main() {
  testVec();
  testTree();
  testBinomial();
  testExpansion();

  return (test::runAll() ? 0 : 1);
}

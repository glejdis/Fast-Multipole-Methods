project := dpd
tests   := tests

src = $(wildcard src/*.cpp)
obj = $(src:.cpp=.o)
dep = $(obj:.o=.d)

tests-src = $(wildcard test/*.cpp)
tests-obj = $(tests-src:.cpp=.o)
tests-dep = $(tests-obj:.o=.d)

CXXFLAGS = -MMD -std=c++20 -O3 -g

$(project): $(obj)
	$(CXX) -o $@ $^ $(LDFLAGS)

-include $(dep)

$(tests): $(tests-obj)
	$(CXX) -o $@ $^ $(LDFLAGS)

-include $(tests-dep)

.PHONY: clean
clean:
	rm -f $(obj) $(dep) $(project) $(tests-obj) $(tests-dep) $(tests)

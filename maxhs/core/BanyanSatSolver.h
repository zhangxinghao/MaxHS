
#ifndef Banyan_h
#define Banyan_h

#include "maxhs/banyan/banyan_go_types.pb.h"
#include "maxhs/core/Wcnf.h"

class BanyanSatSolver {
 private:
  banyan::Problem problem;

 public:
  BanyanSatSolver(banyan::Problem problem) : problem(problem){};
  ~BanyanSatSolver() = default;
  banyan::Result solve();
};

#endif

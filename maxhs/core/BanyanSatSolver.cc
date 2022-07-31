#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <mutex>

#ifdef GLUCOSE
#include "glucose/utils/Options.h"
#include "glucose/utils/System.h"
#else
#include "minisat/utils/Options.h"
#include "minisat/utils/System.h"
#endif

#include "maxhs/core/BanyanSatSolver.h"
#include "maxhs/core/MaxSolver.h"
#include "maxhs/core/Wcnf.h"

using std::cout;

#ifdef GLUCOSE
namespace Minisat = Glucose;
#endif
using namespace Minisat;

banyan::Result BanyanSatSolver::solve() {
  // printf("cluster_id %d\n", problem.clusterid());
  // printf("p wcnf %d %d %d\n", problem.numvars(), problem.numclauses(),
  //        problem.topweight());
  Wcnf byFormula;
  byFormula.set_dimacs_params(problem.numvars(), problem.numclauses(),
                              problem.topweight());
  for (auto clause : problem.hardclauses()) {
    // printf("%d", problem.topweight());
    std::vector<Lit> lit_vtr;
    for (auto lit : clause.lits()) {
      // printf(" %d ", lit);
      if (lit < 0) {
        lit_vtr.push_back(mkLit(-lit, true));
      } else {
        lit_vtr.push_back(mkLit(lit, false));
      }
    }
    // printf("0\n");
    byFormula.addDimacsClause(lit_vtr, (Weight)problem.topweight());
  }

  for (auto clause : problem.softclauses()) {
    // printf("%d", clause.weight());
    std::vector<Lit> lit_vtr;
    for (auto lit : clause.lits()) {
      // printf(" %d ", lit);
      if (lit < 0) {
        lit_vtr.push_back(mkLit(-lit, true));
      } else {
        lit_vtr.push_back(mkLit(lit, false));
      }
    }
    // printf("0\n");
    byFormula.addDimacsClause(lit_vtr, (Weight)clause.weight());
  }
  byFormula.computeWtInfo();
  byFormula.simplify();
  MaxHS::MaxSolver bysolver(&byFormula);
  bysolver.solve();
  fflush(stdout);
  fflush(stderr);

  banyan::Result rst;
  vector<lbool> bymodel;
  if (bysolver.isUnsat()) {
    rst.set_status(20);
  } else {
    rst.set_status(10);
    auto model = bysolver.getBestModel();
    auto exmodel = byFormula.rewriteModelToInput(model);
    printf("solve success:");
    for (int lit_id = 1; lit_id < problem.numvars() + 1; lit_id++) {
      auto m = exmodel[lit_id];
      printf(" [%d,%s] ", lit_id, m == l_False ? "false" : "true");
      if (m == l_False) {
        rst.add_lits(-lit_id);
      } else {
        rst.add_lits(lit_id);
      }
    }
    printf("\n");
  }

  return rst;
}

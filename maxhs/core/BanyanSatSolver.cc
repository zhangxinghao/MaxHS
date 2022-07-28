#include <errno.h>
#include <signal.h>

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
#include "maxhs/utils/Params.h"

using std::cout;

#ifdef GLUCOSE
namespace Minisat = Glucose;
#endif
using namespace Minisat;

static Wcnf *byFormula{};
static vector<lbool> *bymodel;

void solver_init(int num_vars, int num_clauses, int top_weight) {
  bymodel = new vector<lbool>();
  byFormula = new Wcnf();
  byFormula->set_dimacs_params(num_vars, num_clauses, (Weight)top_weight);
}

void solver_add_clause(int weight, int *lits, int size) {
  Weight w = (Weight)weight;
  std::vector<Lit> litVtr;
  for (int i = 0; i < size; i++) {
    auto litVal = lits[i];
    if (litVal < 0) {
      litVtr.push_back(mkLit(-litVal, true));
    } else {
      litVtr.push_back(mkLit(litVal, false));
    }
  }
  byFormula->addDimacsClause(litVtr, w);
}

int solver_lit_value(int lit) {
  auto litBool = bymodel->at(lit - 1);
  if (litBool == l_False) {
    return -lit;
  } else {
    return lit;
  }
}

int solver_solve() {
  params.readOptions();
  // params.printNewFormat = false;
  // params.printSoln = true;
  // cout << "c Parameter Settings\n";
  // cout << "c ============================================\n";
  // printOptionSettings("c ", cout);
  // cout << "c ============================================\n";
  // cout << "c\n";

  byFormula->computeWtInfo();
  // byFormula->printFormulaStats();
  byFormula->simplify();
  // byFormula->printSimpStats();

  MaxHS::MaxSolver bysolver(byFormula);
  bysolver.solve();
  int rst = 0;
  if (bysolver.isUnsat()) {
    rst = -1;
  } else {
    auto model = bysolver.getBestModel();
    auto exmodel = byFormula->rewriteModelToInput(model);
    for (size_t i = 1; i < exmodel.size(); i++) {
      bymodel->push_back(exmodel[i]);
    }
  }
  fflush(stdout);
  fflush(stderr);
//   bysolver.printStatsAndExit(-1, 0);
  return rst;
}

void solver_release() {
  if (byFormula != nullptr) {
    delete byFormula;
    byFormula = nullptr;
  }
  if (bymodel != nullptr) {
    bymodel->clear();
    delete bymodel;
    bymodel = nullptr;
  }
}

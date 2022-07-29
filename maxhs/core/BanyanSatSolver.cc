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

void solver_solve_problems(problem *problems, int plen) {
  params.readOptions();

  std::vector<Wcnf> byFormulas(plen);
  for (int i = 0; i < plen; i++) {
    problem pb = problems[i];
    auto &byFormula = byFormulas[i];
    byFormula.set_dimacs_params(pb.num_vars, pb.num_clauses, pb.top_weight);
    int lit_id = 0;
    for (int cid = 0; cid < pb.num_clauses; cid++) {
      auto num_lits = pb.num_lits[cid];
      lit_id += num_lits;
      auto w = pb.weight[cid];
      std::vector<Lit> lit_vtr;
      for (; lit_id < num_lits; lit_id++) {
        auto lit_val = pb.clauses[lit_id];
        if (lit_val < 0) {
          lit_vtr.push_back(mkLit(-lit_val, true));
        } else {
          lit_vtr.push_back(mkLit(lit_val, false));
        }
      }
      byFormula.addDimacsClause(lit_vtr, (Weight)w);
    }
    byFormula.computeWtInfo();
    byFormula.simplify();
    MaxHS::MaxSolver bysolver(&byFormula);
    bysolver.solve();
    vector<lbool> bymodel;
    int status = 0;
    if (bysolver.isUnsat()) {
      status = -1;
    } else {
      auto model = bysolver.getBestModel();
      auto exmodel = byFormula.rewriteModelToInput(model);
      for (size_t i = 1; i < exmodel.size(); i++) {
        bymodel.push_back(exmodel[i]);
      }
    }
    pb.status = status;
    pb.lits = (int *)malloc((pb.num_vars + 1) * sizeof(int));
    for (int lit_id = 1; lit_id <= pb.num_vars; lit_id++) {
      auto lit_val = bymodel.at(lit_id - 1);
      if (lit_val == l_False) {
        pb.lits[lit_id] = -lit_id;
      } else {
        pb.lits[lit_id] = lit_id;
      }
    }
  }
}

// static Wcnf *byFormula{};
// static vector<lbool> *bymodel;

// void solver_init(int num_vars, int num_clauses, int top_weight) {
//   bymodel = new vector<lbool>();
//   byFormula = new Wcnf();
//   byFormula->set_dimacs_params(num_vars, num_clauses, (Weight)top_weight);
// }

// void solver_add_clause(int weight, int *lits, int size) {
//   Weight w = (Weight)weight;
//   std::vector<Lit> litVtr;
//   for (int i = 0; i < size; i++) {
//     auto litVal = lits[i];
//     if (litVal < 0) {
//       litVtr.push_back(mkLit(-litVal, true));
//     } else {
//       litVtr.push_back(mkLit(litVal, false));
//     }
//   }
//   byFormula->addDimacsClause(litVtr, w);
// }

// int solver_lit_value(int lit) {
//   auto litBool = bymodel->at(lit - 1);
//   if (litBool == l_False) {
//     return -lit;
//   } else {
//     return lit;
//   }
// }

// int solver_solve() {
//   params.readOptions();
//   // params.printNewFormat = false;
//   // params.printSoln = true;
//   // cout << "c Parameter Settings\n";
//   // cout << "c ============================================\n";
//   // printOptionSettings("c ", cout);
//   // cout << "c ============================================\n";
//   // cout << "c\n";

//   byFormula->computeWtInfo();
//   // byFormula->printFormulaStats();
//   byFormula->simplify();
//   // byFormula->printSimpStats();

//   MaxHS::MaxSolver bysolver(byFormula);
//   bysolver.solve();
//   int rst = 0;
//   if (bysolver.isUnsat()) {
//     rst = -1;
//   } else {
//     auto model = bysolver.getBestModel();
//     auto exmodel = byFormula->rewriteModelToInput(model);
//     for (size_t i = 1; i < exmodel.size(); i++) {
//       bymodel->push_back(exmodel[i]);
//     }
//   }
//   fflush(stdout);
//   fflush(stderr);
//   //   bysolver.printStatsAndExit(-1, 0);
//   return rst;
// }

// void solver_release() {
//   if (byFormula != nullptr) {
//     delete byFormula;
//     byFormula = nullptr;
//   }
//   if (bymodel != nullptr) {
//     bymodel->clear();
//     delete bymodel;
//     bymodel = nullptr;
//   }
// }

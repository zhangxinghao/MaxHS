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

void solver_solve_problems(int pb_len, int* cluster_ids, int* pbs_num_vars,
                           int* pbs_num_clauses, int* pbs_top_weight,
                           int* pbs_num_lits, int* pbs_weights,
                           int* pbs_clauses, int* pbs_results) {
  params.readOptions();

  int clause_cursor = 0;
  int lit_cursor = 0;
  for (int pb_id = 0; pb_id < pb_len; pb_id++) {
    int cluster_id = cluster_ids[pb_id];
    int num_vars = pbs_num_vars[pb_id];
    int num_clauses = pbs_num_clauses[pb_id];
    int top_weight = pbs_top_weight[pb_id];
    printf("cluster_id %d num_vars %d num_clauses %d top_weight %d\n",
           cluster_id, num_vars, num_clauses, top_weight);
    Wcnf byFormula;
    byFormula.set_dimacs_params(num_vars, num_clauses, top_weight);
    int cid = clause_cursor;
    clause_cursor+=num_clauses;
    for (; cid < clause_cursor; cid++) {
      printf("w %d c", pbs_weights[cid]);
      std::vector<Lit> lit_vtr;
      int lit_id = lit_cursor;
      lit_cursor += pbs_num_lits[cid];
      for (; lit_id < lit_cursor; lit_id++) {
        int lit_val = pbs_clauses[lit_id];
        printf(" %d ", lit_val);
        if (lit_val < 0) {
          lit_vtr.push_back(mkLit(-lit_val, true));
        } else {
          lit_vtr.push_back(mkLit(lit_val, false));
        }
      }
      printf("\n");
      byFormula.addDimacsClause(lit_vtr, (Weight)pbs_weights[cid]);
    }

    byFormula.computeWtInfo();
    byFormula.simplify();
    MaxHS::MaxSolver bysolver(&byFormula);
    bysolver.solve();
  }

  // vector<lbool> bymodel;
  // int status = 0;
  // if (bysolver.isUnsat()) {
  //   result[0] = 20;
  // } else {
  //   result[0] = 10;
  //   auto model = bysolver.getBestModel();
  //   auto exmodel = byFormula.rewriteModelToInput(model);
  //   for (int lit_id = 1; lit_id < num_vars + 1; lit_id++) {
  //     auto m = exmodel[lit_id];
  //     printf(" [%d,%s] ", lit_id, m == l_False ? "false" : "true");
  //     if (m == l_False) {
  //       result[lit_id] = -lit_id;
  //     } else {
  //       result[lit_id] = lit_id;
  //     }
  //   }
  //   printf("\n");
  // }

}

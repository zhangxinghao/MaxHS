#include <errno.h>
#include <signal.h>
#include <iostream>

#ifdef GLUCOSE
#include "glucose/utils/Options.h"
#include "glucose/utils/System.h"
#else
#include "minisat/utils/Options.h"
#include "minisat/utils/System.h"
#endif

#include "maxhs/core/MaxSolver.h"
#include "maxhs/core/Wcnf.h"
#include "maxhs/utils/Params.h"

#include "maxhs/core/BanyanSatSolver.h"

using std::cout;

#ifdef GLUCOSE
namespace Minisat = Glucose;
#endif
using namespace Minisat;

static Wcnf *byFormula{};
static vector<lbool> bymodel;

void solver_init(int num_vars, int num_clauses, int top_weight)
{
    if (byFormula != nullptr)
    {
        delete byFormula;
        byFormula = nullptr;
        bymodel.clear();
    }
    byFormula = new Wcnf();
    byFormula->set_dimacs_params(num_vars, num_clauses, (Weight)top_weight);
}

void solver_add_clause(int weight, int *lits, int size)
{
    Weight w = (Weight)weight;
    std::vector<Lit> litVtr;
    for (int i = 0; i < size; i++)
    {
        auto litVal = lits[i];
        if (litVal < 0)
        {
            litVtr.push_back(mkLit(-litVal, true));
        }
        else
        {
            litVtr.push_back(mkLit(litVal, false));
        }
    }
    byFormula->addDimacsClause(litVtr, w);
}

int solver_lit_val(int lit)
{
    auto litBool = bymodel[lit - 1];
    if (litBool == l_False)
    {
        return -lit;
    }
    return lit;
}

int solver_solve()
{
    MaxHS::MaxSolver bysolver(byFormula);
    bysolver.solve();
    int rst = 0;
    if (bysolver.isUnsat())
    {
        rst = -1;
    }
    else
    {
        auto model = bysolver.getBestModel();
        for (size_t i = 0; i < model.size(); i++)
        {
            bymodel.push_back(model[i]);
        }
    }

    bysolver.printStatsAndExit(-1, 0);

    return rst;
}

void solver_release()
{
    if (byFormula != nullptr)
    {
        delete byFormula;
        byFormula = nullptr;
        bymodel.clear();
    }
}

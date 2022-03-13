
#ifndef Banyan_h
#define Banyan_h

extern "C"
{

    void solver_init(int num_vars, int num_clauses, int top_weight);

    void solver_add_clause(int weight, int *lits, int size);

    int solver_lit_value(int lit);

    int solver_solve();

    void solver_release();
}

#endif


#ifndef Banyan_h
#define Banyan_h

extern "C" {

void solver_solve_problems(int pb_len, int* cluster_ids, int* pbs_num_vars,
                           int* pbs_num_clauses, int* pbs_top_weight, int* pbs_num_lits,
                           int* pbs_weights, int* pbs_clauses, int* pbs_results);
}

#endif

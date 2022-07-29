
#ifndef Banyan_h
#define Banyan_h

extern "C" {

struct problem {
  int cluster_id;
  int num_vars;
  int num_clauses;
  int top_weight;
  int* num_lits;
  int* weight;
  int* clauses;
  int status;
  int* lits;
};

void solver_solve_problems(problem* problems, int plen);
}

#endif

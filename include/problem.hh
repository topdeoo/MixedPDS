#pragma once

#include "graph.hh"
#include "options.hh"
#include "solver.hh"
#include <memory>

class Problem {
private:
  //* Options
  Options m_options;

  //* Graph
  std::unique_ptr<Graph> m_graph;

  //* MILP Solver
  std::unique_ptr<GurobiSolver> m_solver;

  //* Best solution
  set<u32> m_best_solution;

  //* Current solution
  set<u32> m_current_solution;

  //* Pre-select vertices set
  set<u32> m_pre_solution;

  //* Excluded vertices set
  set<u32> m_excluded;

  //* Non-propagating vertices set
  set<u32> m_non_propagating;

  //* ages array to generating a better solution
  i32 *age0;
  i32 *age1;

  //* generate solutions
  bool *m_generate_solutions;

  //* Pre-processing solution
  bool *m_pre_processing_solution;
  bool *m_propagate_set;
  u32 m_propagate_count;

public:
  Problem();
  ~Problem() = default;

  Problem( const Options & );

public:
  void parse();
  void alloc_memory();

  void preprocess();
  void grasp();
  void adaptive();
  void start();
};

#pragma once

#include "graph.hh"
#include "options.hh"
#include "solver.hh"
#include <memory>

u64 process_time();

class Problem {
private:
  //* Options
  Options m_options;

  //* Graph
  Graph m_graph;

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

  //* ages array to generating a better solution
  i32 *age0;
  i32 *age1;

  //* generate solutions
  bool *m_generate_solutions;
  u32 m_generate_solutions_size;

  //* Pre-processing solution
  bool *m_pre_processing_solution;
  bool *m_pre_observed_set;
  u32 m_pre_observed_count;

  bool m_solved;

  bool m_reduction_solved;

public:
  Problem();
  ~Problem() {
    delete[] age0;
    delete[] age1;
    delete[] m_generate_solutions;
    delete[] m_pre_processing_solution;
    delete[] m_pre_observed_set;
  };

  Problem( const Options & );

public:
  inline void set_filename( const std::string &filename ) {
    m_options.filename = filename;
  }

  inline bool solved() const {
    return m_graph.observed_count() == m_graph.vertices_num();
  }
  inline bool reduction_solved() const { return m_reduction_solved; }

  inline bool is_fixed_in( u32 vertex ) const {
    return age0[vertex] == -1 && age1[vertex] >= 0;
  }

  inline bool is_fixed_out( u32 vertex ) const {
    return age1[vertex] == -1 && age0[vertex] >= 0;
  }

  void parse();
  void alloc_memory();

  void set_dominanting( u32 );

  void preprocess();
  void grasp();
  void adaptive();
  void start();

  void initialize_solver();

  void apply_solution();

  inline u32 solution_size() const { return m_best_solution.size(); }
  inline const set<u32> &solution() const { return m_best_solution; }

  inline bool *observed_set() const { return m_graph.observed_set(); }
  inline const set<u32> &vertices() const { return m_graph.vertices(); }
};

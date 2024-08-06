#include "problem.hh"
#include "graph.hh"
#include "options.hh"
#include "solver.hh"
#include "types.hh"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <memory>

Problem::Problem() {
  m_graph = std::make_unique<Graph>();
  m_solver = std::make_unique<GurobiSolver>();
}

Problem::Problem(const Options &opt) : m_options(opt) {
  m_graph = std::make_unique<Graph>();
  m_solver = std::make_unique<GurobiSolver>();
}

void Problem::parse() {
  std::ifstream fin(m_options.filename);
  std::string t;
  fin >> t;

  //* Input the whole graph
  u32 n, m, u, v;
  fin >> n >> m;
  for (u32 i = 0; i < m; i++) {
    fin >> u >> v;
    m_graph->add_edge(u, v);
    m_graph->add_edge(v, u);
  }

  //* Input the pre-dominating set
  fin >> n;
  for (u32 i = 0; i < n; i++) {
    fin >> v;
    m_pre_solution.insert(v);
  }

  //* Input the exclude set
  fin >> n;
  for (u32 i = 0; i < n; i++) {
    fin >> v;
    m_excluded.insert(v);
  }

  //* Input the non-propagating set
  fin >> n;
  for (u32 i = 0; i < n; i++) {
    fin >> v;
    m_non_propagating.insert(v);
  }

  alloc_memory();
}

void Problem::alloc_memory() {
  // TODO: More array data structure
  age0 = new i32[m_graph->vertices_num() + 1];
  age1 = new i32[m_graph->vertices_num() + 1];
  std::fill(age0, age0 + m_graph->vertices_num() + 1, -1);
  std::fill(age1, age1 + m_graph->vertices_num() + 1, -1);

  m_generate_solutions = new bool[m_graph->vertices_num() + 1]();
  m_pre_processing_solution = new bool[m_graph->vertices_num() + 1]();
  m_propagate_set = new bool[m_graph->vertices_num() + 1]();
  m_propagate_count = 0;

  std::srand(std::time(nullptr));

  m_graph->initialize();
}

void Problem::preprocess() {
  std::memset(m_generate_solutions, 0,
              (m_graph->vertices_num() + 1) * sizeof(bool));
  std::memset(m_graph->propagated_set(), 0,
              sizeof(bool) * (m_graph->vertices_num() + 1));
  m_graph->set_propagate_count(0);
  for (auto &v : m_pre_solution) {
    m_generate_solutions[v] = true;
    m_graph->propagate(v);
  }
  std::memcpy(m_propagate_set, m_graph->propagated_set(),
              m_graph->vertices_num() + 1);
  std::memcpy(m_pre_processing_solution, m_generate_solutions,
              sizeof(bool) * (m_graph->vertices_num() + 1));
  m_propagate_count = m_graph->propagated_count();
}

void Problem::grasp() {
  std::memcpy(m_generate_solutions, m_pre_processing_solution,
              (m_graph->vertices_num() + 1) * sizeof(bool));
  std::memcpy(m_graph->propagated_set(), m_propagate_set,
              sizeof(bool) * (m_graph->vertices_num() + 1));
  m_graph->set_propagate_count(m_propagate_count);
  //* Initialize solution with GRASP
  // TODO: Use initialize method in FSS
  while (m_graph->propagated_count() != m_graph->vertices_num()) {
    u32 select_v = 0;
    fp64 score = 0;
    for (auto &v : m_graph->vertices()) {
      if (!m_generate_solutions[v]) {
        auto v_score =
            m_graph->degree(v) *
            static_cast<fp64>(std::rand() / static_cast<fp64>(RAND_MAX));
        if (v_score > score) {
          score = v_score;
          select_v = v;
        }
      }
    }
    m_generate_solutions[select_v] = true;
    m_graph->propagate(select_v);
  }
}

void Problem::adaptive() {
  for (auto &v : m_graph->vertices()) {
    if (age0[v] >= 0 && age1[v] >= 0) {
      if (!m_best_solution.contains(v)) {
        age0[v] = 0;
        age1[v] += 1;
        if (age1[v] == m_options.age_max) {
          age1[v] = -1;
        }
      } else {
        age1[v] = 0;
        age0[v] += 1;
        if (age0[v] == m_options.age_max) {
          age0[v] = -1;
        }
      }
    } else {
      if (age0[v] >= 0) {
        age0[v] = 0;
      }
      if (age1[v] >= 0) {
        age1[v] = 0;
      }
    }
  }
}

void Problem::start() {
  preprocess();
  u32 timestamp = 0, cutoff = m_options.cutoff_time;
  while (timestamp < cutoff) {
    //* Generate $n_a$ solutions
    for (u32 i = 0; i < m_options.na; i++) {
      grasp();
      for (auto &v : m_graph->vertices()) {
        if (m_generate_solutions[v] && age1[v] == -1) {
          age1[v] = 0;
        }
        if (!m_generate_solutions[v] && age0[v] == -1) {
          age0[v] = 0;
        }
      }
    }
    //* Solve problem with MILP solver
    m_solver->start(age0, age1, m_options.t_milp, m_current_solution);
    if (m_current_solution.size() < m_best_solution.size()) {
      m_best_solution = m_current_solution;
    }
    adaptive();
    timestamp++;
  }
}

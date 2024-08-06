#pragma once

#include "graph.hh"
#include "types.hh"
#include <gurobi_c++.h>
#include <memory>

GRBEnv &get_gurobi_envrionment();

class GurobiSolver {

private:
  std::unique_ptr<GRBModel> m_model;
  map<u32, GRBVar> m_xi;

public:
  GurobiSolver();
  ~GurobiSolver() = default;

public:
  void initialize_model(const Graph &);
  void start(i32 *, i32 *, u32, set<u32> &);
};
#pragma once

#include "types.hh"
#include <gurobi_c++.h>
#include <memory>

GRBEnv &get_gurobi_envrionment();

class GurobiSolver {

public:
  std::unique_ptr<GRBModel> m_model;
  map<u32, GRBVar> m_xi;

public:
  GurobiSolver();
  ~GurobiSolver() = default;

public:
  void start( u32, set<u32> & );
};
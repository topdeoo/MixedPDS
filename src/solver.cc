#include "solver.hh"
#include "types.hh"
#include <gurobi_c++.h>
#include <gurobi_c.h>
#include <memory>

GRBEnv &get_gurobi_envrionment() {
  // TODO : 设置日志的输出位置
  static thread_local GRBEnv env;
  return env;
}

GurobiSolver::GurobiSolver()
    : m_model( std::make_unique<GRBModel>( get_gurobi_envrionment() ) ), m_xi() {}

void GurobiSolver::start( u32 time_limit, set<u32> &solution ) {
  m_model->set( GRB_StringParam_LogFile, "/dev/null" );
  m_model->set( GRB_IntParam_LogToConsole, false );
  m_model->set( GRB_DoubleParam_TimeLimit, static_cast<fp64>( time_limit ) );

  m_model->optimize();

  if ( m_model->get( GRB_IntAttr_Status ) == GRB_OPTIMAL ) {
    solution.clear();
    auto &xi = m_xi;
    for ( auto [v, var] : xi ) {
      if ( var.get( GRB_DoubleAttr_X ) > 0.5 ) {
        solution.insert( v );
      }
    }
  }
}

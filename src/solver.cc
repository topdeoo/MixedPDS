#include "solver.hh"
#include "types.hh"
#include <gurobi_c++.h>
#include <memory>
#include <utility>

GRBEnv &get_gurobi_envrionment() {
  // TODO : 设置日志的输出位置
  static thread_local GRBEnv env;
  return env;
}

GurobiSolver::GurobiSolver()
    : m_model( std::make_unique<GRBModel>( get_gurobi_envrionment() ) ), m_xi() {}

void GurobiSolver::initialize_model( const Graph &graph ) {
  map<u32, GRBVar> si;
  map<std::pair<u32, u32>, GRBVar> pe;
  //? how to calcuate `M`
  const fp64 M = static_cast<fp64>( graph.vertices_num() * 2 );
  {
    for ( auto &v : graph.vertices() ) {
      //! $x_i \in \{ 0, 1 \}$
      m_xi.insert( { v, m_model->addVar( 0, M, 1, GRB_BINARY ) } );
      //! $(s_i \geq 1 ) \and (s_i \leq |V|)$
      si.insert( { v, m_model->addVar( 1, M, 0, GRB_INTEGER ) } );
    }
  }
}

void GurobiSolver::start( i32 *age0, i32 *age1, u32 time_limit,
                          set<u32> &solution ) {}

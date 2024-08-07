#include "problem.hh"
#include "graph.hh"
#include "options.hh"
#include "solver.hh"
#include "types.hh"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fmt/color.h>
#include <gurobi_c++.h>
#include <gurobi_c.h>
#include <iostream>
#include <memory>

Problem::Problem() {
  m_graph = std::make_unique<Graph>();
  m_solver = std::make_unique<GurobiSolver>();
}

Problem::Problem( const Options &opt ) : m_options( opt ) {
  m_graph = std::make_unique<Graph>();
  m_solver = std::make_unique<GurobiSolver>();
}

void Problem::parse() {
  auto fp = std::freopen( m_options.filename.c_str(), "r", stdin );

  if ( fp == nullptr ) {
    std::cerr << "\033[31mError: Failed to open " << m_options.filename
              << "\033[m\n";
    exit( EXIT_FAILURE );
  }

  std::string t;
  std::cin >> t;

  //* Input the whole graph
  u32 n, m, u, v;
  std::cin >> n >> m;
  for ( u32 i = 0; i < m; i++ ) {
    std::cin >> u >> v;
    m_graph->add_edge( u, v );
    m_graph->add_edge( v, u );
  }

  //* Input the pre-dominating set
  std::cin >> n;
  for ( u32 i = 0; i < n; i++ ) {
    std::cin >> v;
    m_pre_solution.insert( v );
  }

  //* Input the exclude set
  std::cin >> n;
  for ( u32 i = 0; i < n; i++ ) {
    std::cin >> v;
    m_excluded.insert( v );
  }

  //* Input the non-propagating set
  std::cin >> n;
  for ( u32 i = 0; i < n; i++ ) {
    std::cin >> v;
    m_graph->set_non_propagating( v );
  }

  alloc_memory();
}

void Problem::alloc_memory() {
  // TODO: More array data structure
  age0 = new i32[m_graph->vertices_num() + 1];
  age1 = new i32[m_graph->vertices_num() + 1];
  std::fill( age0, age0 + m_graph->vertices_num() + 1, 0 );
  std::fill( age1, age1 + m_graph->vertices_num() + 1, 0 );

  m_generate_solutions = new bool[m_graph->vertices_num() + 1]();
  m_generate_solutions_size = 0;
  m_pre_processing_solution = new bool[m_graph->vertices_num() + 1]();
  m_pre_observed_set = new bool[m_graph->vertices_num() + 1]();
  m_pre_observed_count = 0;

  std::srand( std::time( nullptr ) );

  m_graph->initialize();
}

void Problem::set_dominanting( u32 vertex ) {
  m_generate_solutions[vertex] = true;
  std::vector<u32> queue;
  m_graph->observe_one( vertex, queue );
  for ( auto &w : m_graph->neighbors( vertex ) ) {
    m_graph->observe_one( w, queue );
  }
  m_graph->propagate( queue );
}

void Problem::preprocess() {
  std::memset( m_generate_solutions, 0,
               ( m_graph->vertices_num() + 1 ) * sizeof( bool ) );
  std::memset( m_graph->observed_set(), 0,
               sizeof( bool ) * ( m_graph->vertices_num() + 1 ) );
  m_graph->set_observed_count( 0 );
  for ( auto &v : m_pre_solution ) {
    set_dominanting( v );
  }
  std::memcpy( m_pre_observed_set, m_graph->observed_set(),
               m_graph->vertices_num() + 1 );
  std::memcpy( m_pre_processing_solution, m_generate_solutions,
               sizeof( bool ) * ( m_graph->vertices_num() + 1 ) );
  m_pre_observed_count = m_graph->observed_count();
}

void Problem::grasp() {
  std::memcpy( m_generate_solutions, m_pre_processing_solution,
               ( m_graph->vertices_num() + 1 ) * sizeof( bool ) );
  std::memcpy( m_graph->observed_set(), m_pre_observed_set,
               sizeof( bool ) * ( m_graph->vertices_num() + 1 ) );
  m_graph->set_observed_count( m_pre_observed_count );
  //* Initialize solution with GRASP
  // TODO: Use initialize method in FSS
  while ( m_graph->observed_count() != m_graph->vertices_num() ) {
    u32 select_v = 0;
    fp64 score = 0;
    for ( auto &v : m_graph->vertices() ) {
      if ( !m_generate_solutions[v] && !m_excluded.contains( v ) ) {
        auto v_score =
            m_graph->degree( v ) *
            static_cast<fp64>( std::rand() / static_cast<fp64>( RAND_MAX ) + 1 );
        if ( v_score > score ) {
          score = v_score;
          select_v = v;
        }
      }
    }
    set_dominanting( select_v );
  }
}

void Problem::adaptive() {
  for ( auto &v : m_graph->vertices() ) {
    if ( age0[v] >= 0 && age1[v] >= 0 ) {
      if ( !m_best_solution.contains( v ) ) {
        age0[v] = 0;
        age1[v] += 1;
        if ( age1[v] == m_options.age_max ) {
          age1[v] = -1;
        }
      } else {
        age1[v] = 0;
        age0[v] += 1;
        if ( age0[v] == m_options.age_max ) {
          age0[v] = -1;
        }
      }
    } else {
      if ( age0[v] >= 0 ) {
        age0[v] = 0;
      }
      if ( age1[v] >= 0 ) {
        age1[v] = 0;
      }
    }
  }
}

void Problem::initialize_solver() {
  auto &model = m_solver->m_model;
  auto &xi = m_solver->m_xi;
  map<u32, GRBVar> si;
  map<std::pair<u32, u32>, GRBVar> pe;
  const auto N = static_cast<fp64>( m_graph->vertices_num() );
  const auto M = 2 * N;
  for ( auto &v : m_graph->vertices() ) {
    //* objective function
    xi.insert( { v, model->addVar( 0, 1, 1, GRB_BINARY ) } );
    //* constraints (2)
    si.insert( { v, model->addVar( 1, N + 1, 0, GRB_CONTINUOUS ) } );
    for ( auto &w : m_graph->neighbors( v ) ) {
      pe.insert( { { v, w }, model->addVar( 0, 1, 0, GRB_BINARY ) } );
      if ( m_graph->is_non_propagating( v ) ) {
        model->addConstr( pe[{ v, w }] == 0 );
      }
    }
    if ( is_fixed_in( v ) ) {
      model->addConstr( xi[v] == 1 );
    } else if ( is_fixed_out( v ) ) {
      model->addConstr( xi[v] == 0 );
    }
  }
  for ( auto &v : m_graph->vertices() ) {
    GRBLinExpr out_sum, in_sum;
    GRBLinExpr oberving_neighbors = xi.at( v );
    for ( auto &w : m_graph->neighbors( v ) ) {
      out_sum += pe[{ v, w }];
      in_sum += pe[{ w, v }];
      oberving_neighbors += xi[w];

      //* constraints (8)
      model->addConstr( si[v] >= si[w] + 1 - M * ( 1 - pe[{ w, v }] ) );
      for ( auto &t : m_graph->neighbors( w ) ) {
        if ( t != v ) {
          model->addConstr( si[v] >= si[t] + 1 - M * ( 1 - pe[{ w, v }] ) );
        }
      }
    }
    //* constraints (4)
    model->addConstr( si[v] <= M * ( oberving_neighbors + in_sum ) );
    //* constraints (5)
    model->addConstr( out_sum <= 1 );
    //* constraints (6)
    model->addConstr( in_sum <= 1 );
    //* constraints (9)
    model->addConstr( si[v] <= N );
  }
}

// void Problem::initialize_solver() {
//   auto &model = m_solver->m_model;
//   auto &xi = m_solver->m_xi;
//   map<u32, GRBVar> si;
//   map<std::pair<u32, u32>, GRBVar> pe;
//   const auto N = static_cast<u32>( m_graph->vertices_num() );
//   const auto M = static_cast<u32>( m_graph->vertices_num() );
//   for ( auto &v : m_graph->vertices() ) {
//     //* objective function
//     xi.insert( { v, model->addVar( 0, 1, 1, GRB_BINARY ) } );
//     //* constraints (2)
//     si.insert( { v, model->addVar( 1, M, 0, GRB_INTEGER ) } );
//     for ( auto &w : m_graph->neighbors( v ) ) {
//       pe.insert( { { v, w }, model->addVar( 0, 1, 0, GRB_BINARY ) } );
//       if ( m_graph->is_non_propagating( v ) ) {
//         model->addConstr( pe[{ v, w }] == 0 );
//       }
//     }
//     if ( is_fixed_in( v ) ) {
//       model->addConstr( xi[v] == 1 );
//     } else if ( is_fixed_out( v ) ) {
//       model->addConstr( xi[v] == 0 );
//     }
//   }
//   for ( auto &v : m_graph->vertices() ) {
//     GRBLinExpr in_sum, out_sum;
//     GRBLinExpr neighbors = xi[v];
//     for ( auto &w : m_graph->neighbors( v ) ) {
//       //* constraints (3)
//       model->addConstr( si[v] <= xi[w] + M * ( 1 - xi[w] ) );
//       //* constraints (7)
//       model->addConstr( pe[{ v, w }] + pe[{ w, v }] <= 1 );
//       in_sum += pe[{ w, v }];
//       out_sum += pe[{ v, w }];
//       neighbors += xi[w];

//       //* constraints (8)
//       model->addConstr( si[v] >= si[w] + 1 - M * ( 1 - pe[{ w, v }] ) );
//       for ( auto &t : m_graph->neighbors( w ) ) {
//         if ( t != v ) {
//           model->addConstr( si[v] >= si[t] + 1 - M * ( 1 - pe[{ w, v }] ) );
//         }
//       }
//     }
//     //* constraints (4)
//     model->addConstr( si[v] <= M * ( neighbors + in_sum ) );
//     //* constraints (5)
//     model->addConstr( out_sum <= 1 );
//     //* constraints (6)
//     model->addConstr( in_sum <= 1 );

//     //* constraints (9)
//     model->addConstr( si[v] <= N );
//   }
// }

void Problem::start() {
  preprocess();
  u32 timestamp = 0, cutoff = m_options.cutoff_time;
  for ( auto &v : m_graph->vertices() ) {
    m_best_solution.insert( v );
  }
  while ( timestamp < cutoff ) {
    //* Generate $n_a$ solutions
    for ( u32 i = 0; i < m_options.na; i++ ) {
      grasp();
      for ( auto &v : m_graph->vertices() ) {
        if ( m_generate_solutions[v] && age1[v] == -1 ) {
          age1[v] = 0;
        }
        if ( !m_generate_solutions[v] && age0[v] == -1 ) {
          age0[v] = 0;
        }
      }
    }
    //* Solve problem with MILP solver
    initialize_solver();
    m_solver->start( m_options.t_milp, m_current_solution );
    if ( m_current_solution.size() < m_best_solution.size() ) {
      m_best_solution = m_current_solution;
    }
    adaptive();
    timestamp++;
  }
}

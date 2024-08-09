#include "graph.hh"
#include "types.hh"
#include <cstring>
#include <vector>

void Graph::initialize() {
  //! Initialize all data structures with correct value
  m_observed = new bool[m_max_vertex + 1]();
  m_observed_count = 0;
  for ( auto &v : m_vertices ) {
    m_unobserved_degree[v] = degree( v );
  }
}

void Graph::stash() {
  for ( auto &v : m_vertices ) {
    m_original_unobserved_degree[v] = m_unobserved_degree[v];
  }
}

void Graph::reset() {
  for ( auto &v : m_vertices ) {
    m_unobserved_degree[v] = m_original_unobserved_degree[v];
  }
}

void Graph::add_edge( u32 from, u32 to ) {
  if ( !m_vertices.contains( from ) ) {
    m_vertices.insert( from );
  }
  if ( !m_vertices.contains( to ) ) {
    m_vertices.insert( to );
  }

  m_max_vertex = std::max( m_max_vertex, std::max( from, to ) );

  if ( !m_neighbors[from].contains( to ) ) {
    m_neighbors[from].insert( to );
    m_edges.push_back( Edge( from, to ) );
  }
}

void Graph::observe_one( u32 vertex, std::vector<u32> &queue ) {
  if ( !is_observed( vertex ) ) {
    m_observed[vertex] = true;
    m_observed_count++;
    if ( m_unobserved_degree[vertex] == 1 ) {
      queue.push_back( vertex );
    }
    for ( auto &w : m_neighbors[vertex] ) {
      m_unobserved_degree[w]--;
      if ( m_unobserved_degree[w] == 1 && is_observed( w ) &&
           !m_non_propagating.contains( w ) ) {
        queue.push_back( w );
      }
    }
  }
}

void Graph::propagate( std::vector<u32> &queue ) {
  while ( !queue.empty() ) {
    auto v = queue.back();
    queue.pop_back();
    if ( is_observed( v ) && !m_non_propagating.contains( v ) &&
         m_unobserved_degree[v] == 1 ) {
      for ( auto &w : m_neighbors[v] ) {
        if ( !is_observed( w ) ) {
          observe_one( w, queue );
        }
      }
    }
  }
}

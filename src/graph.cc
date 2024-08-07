#include "graph.hh"
#include "types.hh"
#include <vector>

void Graph::initialize() {
  m_observed = new bool[vertices_num() + 1];
  m_observed_count = 0;
}

void Graph::add_edge( u32 from, u32 to ) {
  if ( !m_vertices.contains( from ) ) {
    m_vertices.insert( from );
  }
  if ( !m_vertices.contains( to ) ) {
    m_vertices.insert( to );
  }

  if ( !m_neighbors[from].contains( to ) ) {
    m_neighbors[from].insert( to );
    m_edges.push_back( Edge( from, to ) );
  }
}

void Graph::observe_one( u32 vertex, u32 origin, std::vector<u32> &queue ) {
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

void Graph::propagate( u32 vertex, std::vector<u32> &queue ) {}

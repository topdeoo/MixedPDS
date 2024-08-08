#pragma once

#include "types.hh"
#include <vector>

struct Edge {
  u32 src, dst;

  Edge( u32 from, u32 to ) : src( from ), dst( to ) {}
};

// * We assume `Graph` is a undirected graph
class Graph {
private:
  // ? use vector instead of set
  set<u32> m_vertices;
  u32 m_max_vertex;
  std::vector<Edge> m_edges;
  map<u32, set<u32>> m_neighbors;

  //* Unobserved degree
  map<u32, u32> m_unobserved_degree;

  //* Non-propagating vertices set
  set<u32> m_non_propagating;

  bool *m_observed;
  u32 m_observed_count;

public:
  Graph() = default;
  ~Graph() { delete[] m_observed; };

public:
  inline u32 vertices_num() const { return m_vertices.size(); }
  inline u32 max_vertex() const { return m_max_vertex; }
  inline u32 observed_count() const { return m_observed_count; }

  inline bool has_vertex( u32 vertex ) const {
    return m_vertices.contains( vertex );
  }

  inline bool is_observed( u32 vertex ) const { return m_observed[vertex]; }
  inline bool is_non_propagating( u32 vertex ) const {
    return m_non_propagating.contains( vertex );
  }

  inline void set_observed_count( u32 count ) { m_observed_count = count; }
  inline void set_non_propagating( u32 vertex ) {
    m_non_propagating.insert( vertex );
  }

  inline const set<u32> &vertices() const { return m_vertices; }
  inline const std::vector<Edge> &edges() const { return m_edges; }
  inline const set<u32> &neighbors( u32 vertex ) { return m_neighbors[vertex]; }
  inline u32 degree( u32 vertex ) const { return m_neighbors.at( vertex ).size(); }
  inline bool *observed_set() const { return m_observed; }

  void initialize();
  void add_edge( u32 from, u32 to );

  void propagate( std::vector<u32> &queue );
  void observe_one( u32 vertex, std::vector<u32> &queue );
};

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
  std::vector<Edge> m_edges;
  map<u32, set<u32>> m_neighbors;

  bool *m_propagated;
  u32 m_propagated_count;

public:
  Graph() = default;
  ~Graph() = default;

public:
  inline u32 vertices_num() const { return m_vertices.size(); }
  inline u32 propagated_count() const { return m_propagated_count; }
  inline void set_propagate_count( u32 count ) { m_propagated_count = count; }

  inline const set<u32> &vertices() const { return m_vertices; }
  inline const std::vector<Edge> &edges() const { return m_edges; }
  inline const set<u32> &neighbors( u32 vertex ) { return m_neighbors[vertex]; }
  inline u32 degree( u32 vertex ) const {
    return m_neighbors.at( vertex ).size();
  }
  inline bool *propagated_set() { return m_propagated; }

  void initialize();
  void add_edge( u32 from, u32 to );

  void propagate( u32 vertex );
};

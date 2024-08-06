#include "graph.hh"

void Graph::initialize() {
  m_propagated = new bool[vertices_num() + 1];
  m_propagated_count = 0;
}

void Graph::add_edge(u32 from, u32 to) {
  if (!m_vertices.contains(from)) {
    m_vertices.insert(from);
  }
  if (!m_vertices.contains(to)) {
    m_vertices.insert(to);
  }

  if (!m_neighbors[from].contains(to)) {
    m_neighbors[from].insert(to);
    m_edges.push_back(Edge(from, to));
  }
}

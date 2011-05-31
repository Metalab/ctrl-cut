#ifndef CUTGRAPH_H_
#define CUTGRAPH_H_

#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <map>

#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "CutEdge.h"
#include "CutVertex.h"
#include "GraphView.h"

using boost::adjacency_list;
using boost::undirectedS;
using boost::vecS;
using boost::add_vertex;
using boost::add_edge;
using std::vector;
using std::map;
using std::pair;

typedef adjacency_list<vecS, vecS, undirectedS, PointProperty, CutProperty> CutGraph;

typedef typename boost::graph_traits<CutGraph>
  ::vertex_descriptor Vertex;

typedef typename boost::graph_traits<CutGraph>
  ::edge_descriptor Edge;

typedef map<Point, Vertex, boost::geometry::less<Point> > PointLookup;

class CutModel {
private:
  CutGraph graph;
  PointLookup pointLookup;
  boost::graph_traits<CutGraph>::edges_size_type edge_count;
public:
  CutModel() : edge_count(0) {}
  virtual ~CutModel() {}

  void createEdge(Point &in, Point &out, LaserSettings& settings);
  void createEdge(uint32_t inX, uint32_t inY, uint32_t outX, uint32_t outY, LaserSettings& settings);
  Vertex findOrInsertVertex(const Point &point);
  CutGraph& getCutGraph() { return this->graph; }
  PointLookup& getPointLookup(){ return this->pointLookup; };
};

inline std::ostream& operator<<(std::ostream &os, const Point &p) {
  os << "{" << p.get<0>() << "," << p.get<1>() << "}";
  return os;
}

inline std::ostream& operator<<(std::ostream &os, const ConstSegment& segment) {
  os << "{" << segment.first << "," << segment.second << "}";
  return os;
}


//slow but comfortable... the property map should be retrieved directly for bulk operations
template<typename Vertex, typename Graph>
inline const Point& get_point(const Vertex& v, const Graph& graph) {
  return get(vertex_point, graph)[v];
}

template<typename Edge, typename Graph>
inline const Point& source_point(const Edge& e, const Graph& graph) {
  return get_point(source(e, graph), graph);
}

template<typename Edge, typename Graph>
inline const Point& target_point(const Edge& e, const Graph& graph) {
  return get_point(target(e, graph), graph);
}

template<typename Edge, typename Graph>
inline const ConstSegment& segment(const Edge& e, const Graph& graph) {
  return (*new ConstSegment(source_point(e, graph), target_point(e, graph)));
}

template<typename Edge, typename Graph>
inline double get_length(const Edge& e, const Graph& graph) {
  return get(edge_length, graph)[e];
}

#endif
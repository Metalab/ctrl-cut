/*
 * Mesh.h
 *
 *  Created on: Feb 13, 2011
 *      Author: elchaschab
 */

#ifndef MESH_H_
#define MESH_H_
#include <list>
#include "Vertex.h"
#include "Edge.h"

class Mesh : public std::list<Edge *> {
public:
  void createEdge(Vertex *start, Vertex *end, int power);
  void removeEdge(Edge *e, bool detach);
  Mesh::iterator removeEdge(Mesh::iterator it_e, bool detach);

  Mesh();
  virtual ~Mesh();

private:
  map<string, Vertex* > vertices;

  Vertex* mapVertex(Vertex* p);
};

#endif /* MESH_H_ */

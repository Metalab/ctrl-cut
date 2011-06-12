#ifndef EDGE_H_
#define EDGE_H_

#include <map>
#include <string>
#include <cmath>
#include <algorithm>
#include <iostream>
#include "vector/Polyline.h"

class Edge {
public:
  Edge(class Vertex *start, Vertex *end, int power=0, int speed=0, int frequency=0);
  virtual ~Edge() {}

  Vertex *start() const { return v[0]; }
  Vertex *end() const { return v[1]; }
  void setStart(Vertex *vtx) { v[0] = vtx; }
  void setEnd(Vertex *vtx) { v[1] = vtx; }
  const Vertex &operator[](size_t idx) const { return *v[idx]; }
  Vertex &operator[](size_t idx) { return *v[idx]; }

  int getID() const { return this->id; }
  int power() const { return laser_power; }
  int speed() const { return laser_speed; }
  int frequency() const { return laser_frequency; }

  float getSlope(bool invert=false);
  void invertDirection();
  Vertex *intersects(const Edge &other) const;
  float distance(const Point2D &v) const;
  void detach();
  void attach();

  bool isMemberOf(Polyline *p) { return this->parent == p; }
  bool isPolylineMember() { return this->parent != NULL; }

  void join(Polyline *parent);
  void leave(Polyline *parent);
private:
  static int cnt;
  uint32_t id;
  Polyline *parent;
  Vertex *v[2];
  int laser_power;
  int laser_speed;
  int laser_frequency;
};

std::ostream &operator<<(std::ostream &os, const Edge &e);

#endif
/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef SEGMENTTREE_H_
#define SEGMENTTREE_H_

#include "cut/geom/Concepts.hpp"
#include "Geometry.hpp"
#include <kdtree++/kdtree.hpp>
#include "boost/range.hpp"
#include "boost/foreach.hpp"

namespace kdt = KDTree;

struct SegmentNodeImpl: public Segment {
  Point center;
  uint32_t range;

  SegmentNodeImpl(const Point& center) :
    Segment(), center(center)
  {}

  SegmentNodeImpl(const Segment& seg) : Segment(seg) {
    Sphere sphere(seg);
    center = sphere.center;
    range = sphere.radius;
  }

  bool operator==(const SegmentNodeImpl& other) const {
    return Segment::operator==(other);
  }

  static inline Coord_t centerComponent(const SegmentNodeImpl& item, const int& k) {
    return item.center[k];
  }
};

template<
  template<typename , typename > class Tcontainer = std::list,
  template<typename > class Tallocator = std::allocator
>
struct IndexedSegmentNodeImpl: public Tcontainer<Segment, Tallocator<Segment> >::iterator {
private:
  typedef IndexedSegmentNodeImpl<Tcontainer, Tallocator> _Self;
  typedef typename Tcontainer<Segment, Tallocator<Segment> >::iterator _Base;

  bool _singular;
  Point _center;
  uint32_t _range;
public:

  IndexedSegmentNodeImpl(const Point& center) :
    _Base(), _singular(true), _center(center) {
  }

  IndexedSegmentNodeImpl(const _Base it) :
    _Base(it), _singular(false) {
  }

  IndexedSegmentNodeImpl(const _Base it, bool isEnd) :
    _Base(it), _singular(isEnd) {
  }

  const Point center() const {
    if(this->_singular)
      return this->_center;
    const Sphere sphere(**this);
    return sphere.center;
  }

  Coord_t range() const {
    if(this->_singular)
      return this->_range;
    const Sphere sphere(**this);
    return sphere.radius;
  }


  bool operator==(const IndexedSegmentNodeImpl& other) const {
    return ((this->_singular == true) && (other._singular == true)) || ((!this->_singular && !other._singular) && (&(**this) == &(*other)));
  }

  static inline Coord_t centerComponent(const IndexedSegmentNodeImpl& item, const int& k) {
    return item.center()[k];
  }
};

template<
  typename TsegmentNode,
  typename Taccessor
>
class SegmentTreeImpl {
private:
  typedef kdt::KDTree<2, TsegmentNode, Taccessor> KDTree_t;
  typedef TsegmentNode _SegmentNode;
  KDTree_t kdtree;
public:
  SegmentTreeImpl() :
    kdtree(std::ptr_fun(_SegmentNode::centerComponent))
  {}

  template<typename ToutIter>
  void findWithinRange(const _SegmentNode& node, ToutIter outIter) const {
    kdtree.find_within_range(node.center(), node.range(), outIter);
  }

  template<typename ToutIter>
  void findWithinRange(const _SegmentNode& node, Coord_t range, ToutIter outIter) const {
    kdtree.find_within_range(node.center(), range, outIter);
  }

  void insert(const _SegmentNode& node) {
    kdtree.insert(node);
  }

  void erase(const _SegmentNode& node) {
    typename KDTree_t::iterator it = kdtree.find_exact(node);
    // FIXME lost segments - but they don't affect the model since they would have been removed anyway
    if(it != kdtree.end())
      kdtree.erase(it);
    else
      assert(false);
  }

  size_t size() const {
    return kdtree.size();
  }

  bool empty() const {
    return kdtree.empty();
  }

  void clear() {
    kdtree.clear();
  }
};
template<
  typename TsegmentNode,
  typename Taccessor,
  template<typename , typename > class Tcontainer = std::list,
  template<typename > class Tallocator = std::allocator
  >
class IndexedSegmentTreeImpl: public SegmentTreeImpl<TsegmentNode, Taccessor> {
private:
  typedef SegmentTreeImpl<TsegmentNode, Taccessor> _Parent;
  typedef Tcontainer<Segment, Tallocator<Segment> > _SegmentIndex;
  _SegmentIndex index;
public:
  typedef IndexedSegmentNodeImpl<Tcontainer, Tallocator> iterator;
  typedef Tcontainer<iterator, Tallocator<iterator> > Result;

  IndexedSegmentTreeImpl() : _Parent()
  {}

  Result findWithinRange(const iterator& node) const {
    Result r;
    _Parent::findWithinRange(node, std::back_inserter(r));
    return r;
  }

  Result findWithinRange(const iterator& node, Coord_t range) const {
    Result r;
    _Parent::findWithinRange(node, range, std::back_inserter(r));
    return r;
  }

  void push_back(const Segment& seg) {
    for(typename _SegmentIndex::iterator it = index.begin(); it != index.end(); ++it) {
      if(*it == seg || *it == seg.swap())
        return;
    }

    index.push_back(seg);
    _Parent::insert(iterator(--(index.end()), false));
  }

  iterator erase(iterator& node) {
    _Parent::erase(node);
   return index.erase(node);
  }

  iterator begin() {
    return iterator(index.begin(), false);
  }

  iterator end() {
    return iterator(index.end(), true);
  }

  void clear() {
    index.clear();
    _Parent::clear();
  }
};

typedef SegmentTreeImpl<
    SegmentNodeImpl,
    std::pointer_to_binary_function<const SegmentNodeImpl&, const int&, Coord_t>
> SegmentTree;

typedef IndexedSegmentTreeImpl<
    IndexedSegmentNodeImpl<std::list, std::allocator>,
    std::pointer_to_binary_function<
      const IndexedSegmentNodeImpl<std::list,std::allocator>&,
      const int&,
      Coord_t
    >,
    std::list,
    std::allocator
> IndexedSegmentTree;

#endif

/*
 * EpilogCUPS - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef TWOD_H_
#define TWOD_H_

#include <cctype>
#include <iostream>

#define PCL_2D_MAX std::numeric_limits<uint32_t>::max()

typedef uint32_t coord;
typedef uint32_t dim;

class Point {
public:
  coord x;
  coord y;
  Point(coord x, coord y): x(x), y(y) {}
  friend ostream& operator <<(ostream &os, Point &p) {
    os << "(" << p.x << "," << p.y << ")";
    return os;
  }
};

class BoundingBox {
public:
  Point ul;
  Point lr;

  BoundingBox(): ul(PCL_2D_MAX, PCL_2D_MAX), lr(0,0){}

  void reset(){
    this->ul.x=PCL_2D_MAX;
    this->ul.y=PCL_2D_MAX;
    this->lr.x=0;
    this->lr.y=0;
  }

  void update(Point p) {
    update(p.x, p.y);
  }

  void update(coord x, coord y=PCL_2D_MAX) {
      this->ul.x = min(this->ul.x, x);
      this->lr.x = max(this->lr.x, x);

      if(y != PCL_2D_MAX) {
        this->ul.y = min(this->ul.y, y);
        this->lr.y = max(this->lr.y, y);
      }
  }

  coord min(coord c1, coord c2) {
    return (c1<=c2?c1:c2);
  }

  coord max(coord c1, coord c2) {
    return (c1>=c2?c1:c2);
  }

  friend ostream& operator <<(ostream &os, BoundingBox &bbox) {
    os << "(" << bbox.ul << "," << bbox.lr << ")";
    return os;
  }
};
#endif /* TWOD_H_ */
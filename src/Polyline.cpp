/*
 * polyline.cpp
 *
 *  Created on: 01.10.2009
 *      Author: amir
 */
#include <vector>
#include "LineSegment.h"
#include "Polyline.h"
#include "climits"
#include "iostream"
#include <cmath>

using namespace std;

Polyline::Polyline() {
    // TODO Auto-generated constructor stub

}

Polyline::~Polyline() {
    // TODO Auto-generated destructor stub
}

void Polyline::addLineSegment(LineSegment* ls) {
    this->segments.insert(ls);
}

void Polyline::eraseLineSegment(LineSegment* ls) {
    this->segments.erase(ls);
}

bool Polyline::hasLineSegment(LineSegment* ls) {
    return this->segments.find(ls) != segments.end();
}

int Polyline::getSegmentCount() {
    return this->segments.size();
}

LineSegment* Polyline::findEdge() {
    LineSegment* ls;
    int min_x = INT_MAX;
    Joint* most_left = NULL;
    set<LineSegment*>::iterator it;
    int sx;
    int ex;

    for (it = segments.begin(); it != segments.end(); it++) {
        ls = *it;
        sx = ls->getStart()->getX();
        ex = ls->getEnd()->getX();

        if (sx < min_x) {
            min_x = sx;
            most_left = ls->getStart();
        }

        if (ex < min_x) {
            min_x = ex;
            most_left = ls->getEnd();
        }
    }

    set<LineSegment*> connected = most_left->getConnectors();

    LineSegment* edge;
    float slope;
    float steapest = pi;

    for (it = connected.begin(); it != connected.end(); it++) {
        ls = *it;

        if (ls->getStart()->getY() > ls->getEnd()->getY())
            ls->invertDirection();

        slope = abs(ls->getSlope() - (pi / 2));
        if (slope >= 3.1415f)
            slope = 0;

        if (slope < steapest) {
            steapest = slope;
            edge = ls;
        }
    }

    if (ls->getSlope() < 0)
        ls->invertDirection();

    return edge;
}
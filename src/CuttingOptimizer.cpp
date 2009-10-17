/*
 * CuttingOptimizer.cpp
 *
 *  Created on: 26.09.2009
 *      Author: amir
 */
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <set>
#include <map>
#include <fstream>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <cmath>
#include "LineSegment.h"
#include "Polygon.h"
#include "OnionSkin.h"
#include "VectorCut.h"
#include "CuttingOptimizer.h"

using namespace std;

VectorCut *cut = new VectorCut;

CuttingOptimizer::CuttingOptimizer() {

}

CuttingOptimizer::~CuttingOptimizer() {
	// TODO Auto-generated destructor stub
}

void print_point(Joint *pnt) {
	printf("x ->%d\ty ->%d\t%d\t%s\t%d\n", pnt->getX(), pnt->getY(),
			pnt->getConnectors().size(), pnt->getKey().data(), pnt);
}

void print_line(LineSegment *ls) {
	if (ls == NULL)
		return;
	printf("Line:%d\n", ls);
	print_point((Joint*) ls->getStart());
	print_point((Joint*) ls->getEnd());
	printf("\n");
}

void splitAtIntersections() {
	Joint *intersec = new Joint(-1000, -1000);
	LineSegment *ls1, *ls2;
	list<LineSegment*> lines = cut->getLineSegements();

	list<LineSegment*>::iterator it_i;
	list<LineSegment*>::iterator it_j;

	for (it_i = lines.begin(); it_i != lines.end(); it_i++) {
		for (it_j = lines.begin(); it_j != lines.end(); it_j++) {
			ls2 = *it_j;
			ls1 = *it_i;

			if (it_i == lines.end())
				break;

			if (ls1 == ls2)
				continue;

			if ((intersec = ls1->intersects(ls2)) != NULL) {
				intersec = cut->addPoint(intersec);

				if (!ls1->getStart()->equals(intersec)
						&& !ls1->getEnd()->equals(intersec)) {
					it_i = cut->eraseLine(it_i);
					cut->addLine((Joint*) ls1->getStart(), intersec,
							ls1->getPower());
					cut->addLine((Joint*) ls1->getEnd(), intersec,
							ls1->getPower());
				}

				if (!ls2->getStart()->equals(intersec)
						&& !ls2->getEnd()->equals(intersec)) {
					it_j = cut->eraseLine(it_j);
					cut->addLine((Joint*) ls2->getStart(), intersec,
							ls2->getPower());
					cut->addLine((Joint*) ls2->getEnd(), intersec,
							ls2->getPower());
				}
			}
		}
	}
}

void find_connected(set<LineSegment*> *occupied, Polygon *polygon,
		LineSegment* current) {
	set<LineSegment*> connectors = current->getEnd()->getConnectors();
	set<LineSegment*>::iterator it;
	LineSegment* candidate;

	occupied->insert(current);

	for (it = connectors.begin(); it != connectors.end(); it++) {
		candidate = *it;
		if (candidate == current || occupied->find(candidate)
				!= occupied->end())
			continue;
		if (candidate->getStart() != current->getEnd()) {
			candidate->setEnd(candidate->getStart());
			candidate->setStart(current->getEnd());
		}
		polygon->addLineSegment(candidate);
		find_connected(occupied, polygon, candidate);
	}
}

vector<Polygon*> find_polygons() {

	vector<Polygon*> polygons;
	set<LineSegment*> *occupied = new set<LineSegment*> ();

	LineSegment* ls;
	list<LineSegment*>::iterator it;
	list<LineSegment*> lines = cut->getLineSegements();

	for (it = lines.begin(); it != lines.end(); it++) {
		ls = *it;

		Polygon *polygon = new Polygon();

		if (occupied->find(ls) == occupied->end()) {
			polygon->addLineSegment(ls);
			find_connected(occupied, polygon, ls);
			polygons.push_back(polygon);
		}
	}
	return polygons;
}

void walkTheEdge(Polygon* p, OnionSkin* skin, LineSegment* edge, bool cw) {
	set<LineSegment*> connectors = edge->getEnd()->getConnectors();
	set<LineSegment*>::iterator it;
	LineSegment* candidate;
	LineSegment* next_edge = NULL;

	float edge_slope = edge->getSlope(true);
	float candidate_slope;

	float slope_diff;
	float min_slope_diff = 2 * pi;

	//TODO resolve double check
	if(p->hasLineSegment(edge))
		skin->addLineSegment(edge);

	p->eraseLineSegment(edge);

	for (it = connectors.begin(); it != connectors.end(); it++) {
		candidate = *it;

		if (candidate == edge || !p->hasLineSegment(candidate))
			continue;

		if (candidate->getStart() != edge->getEnd())
			candidate->invertDirection();

		candidate_slope = candidate->getSlope();

		slope_diff = edge_slope - candidate_slope;
		if(slope_diff < 0)
			slope_diff+=2*pi;
		else if(slope_diff > 2*pi)
			slope_diff-=2*pi;

		if (slope_diff < min_slope_diff) {
			min_slope_diff = slope_diff;
			next_edge = candidate;
		}
	}


	if (next_edge == NULL && cw) {
		edge->invertDirection();
		walkTheEdge(p, skin, edge, !cw);
	} else if (next_edge != NULL) {
		if(!cw)
			cw = true;
		walkTheEdge(p, skin, next_edge, cw);
	}
}

vector<OnionSkin*> deonion(vector<Polygon*> polygons) {
	unsigned int i;

	Polygon* p;
	vector<OnionSkin*> skins;

	for (i = 0; i < polygons.size(); i++) {
		p = polygons.at(i);

		while (p->getSegmentCount() > 0) {
			OnionSkin* s = new OnionSkin();
			walkTheEdge(p, s, p->findEdge(), true);
			skins.push_back(s);
		}

	}

	return skins;
}

void optimize_vectors(char *vector_file) {
	string fn(vector_file);
	cut->load(&fn);
	map<string, Joint*> points;
	list<LineSegment*> lines = cut->getLineSegements();

	printf("points: %d\n", points.size());

	map<string, Joint*>::iterator it_p;

	for (it_p = points.begin(); it_p != points.end(); it_p++) {
		//print_point(it_p->second);
	}

	list<LineSegment*>::iterator it;

	for (it = lines.begin(); it != lines.end(); it++) {
		//print_line(*it);
	}

	splitAtIntersections();
	printf("lines: %d\n", lines.size());
	unsigned int i;

	vector<Polygon*> polygones = find_polygons();
	set<LineSegment*> segments;
	set<LineSegment*>::iterator it_s;

	printf("Polygons: %d\n", polygones.size());
	for (i = 0; i < polygones.size(); i++) {
		segments = polygones.at(i)->getLineSegments();
		printf("segments: %d\n", segments.size());

		for (it_s = segments.begin(); it_s != segments.end(); it_s++) {
			//print_line(*it_s);
		}
		printf("\n");
	}

	vector<OnionSkin*> skins = deonion(polygones);
	list<LineSegment*> skin_segm;
	LineSegment* ls;
	list<LineSegment*>::reverse_iterator it_i;


	ofstream outfile;
	outfile.open(vector_file, ofstream::out | ofstream::trunc);

	printf("Skins: %d\n", skins.size());

	for (i = skins.size() - 1; i >= 0; i--) {
		skin_segm = skins.at(i)->getLineSegments();
		printf("s: %d\n", skin_segm.size());

		for (it_i = skin_segm.rbegin(); it_i != skin_segm.rend(); it_i++) {
			ls = *it_i;
			if(it_i != skin_segm.rbegin())
				ls->invertDirection();
			if (ls != NULL) {
				//print_line(ls);
				outfile << "P";
				outfile << ls->getPower();
				outfile << "\n";
				outfile << "M";
				outfile << ls->getStart()->getX();
				outfile << ",";
				outfile << ls->getStart()->getY();
				outfile << "\n";
				outfile << "L";
				outfile << ls->getStart()->getX();
				outfile << ",";
				outfile << ls->getStart()->getY();
				outfile << "\n";
				outfile << "L";
				outfile << ls->getEnd()->getX();
				outfile << ",";
				outfile << ls->getEnd()->getY();
				outfile << "\n";
				/*			 outfile << "M";
				 outfile << ls->getEnd()->getX();
				 outfile << ",";
				 outfile << ls->getEnd()->getY();
				 outfile << "\n";*/
			}

		}
	}
	outfile << "X\n";
	outfile.close();

	cout << "LINES:" << lines.size() << endl;
}

/*
 * Ctrl-Cut - A laser cutter CUPS driver
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
#include "../../util/Logger.h"
#include "Join.h"
#include "../VTypes.h"
#include "../Edge.h"
#include "../Polyline.h"
#include "../Cut.h"

#include <boost/format.hpp>

using boost::format;

Join::~Join() {
  // TODO Auto-generated destructor stub
}

void find_connected(SetEdge *occupied, Polyline *polyline,
                    Edge* current) {
  SetEdge connectors = current->getEnd()->getAttachedEdges();
  SetEdge::iterator it;
  Edge* candidate;

  occupied->insert(current);

  for (it = connectors.begin(); it != connectors.end(); it++) {
    candidate = *it;
    if (candidate == current || occupied->find(candidate) != occupied->end())
      continue;
    if (candidate->getStart() != current->getEnd()) {
      candidate->setEnd(candidate->getStart());
      candidate->setStart(current->getEnd());
    }
    polyline->add(candidate);
    find_connected(occupied, polyline, candidate);
  }
}

void Join::filter(Cut *cut)
{
  LOG_INFO_STR("Join");

  SetEdge *occupied = new SetEdge();

  Edge* ls;
  list<Edge*>::iterator it;
  int cnt = 0;
  for (it = cut->freeEdges.begin(); it != cut->freeEdges.end(); it++) {
    ls = *it;

    Polyline *polyline = new Polyline();

    if (occupied->find(ls) == occupied->end()) {
      polyline->add(ls);
      find_connected(occupied, polyline, ls);
      cut->polylines.push_back(polyline);
    }
    cnt++;
  }

  for(vector<Polyline*>::iterator it = cut->polylines.begin(); it != cut->polylines.end(); it++) {
    Polyline *p = *it;
    for(vector<Edge*>::iterator it_e = p->edges.begin(); it_e != p->edges.end(); it_e++) {
      cut->removeEdge(*it_e, false);
    }
  }
}

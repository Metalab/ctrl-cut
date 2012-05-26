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
 * GNU General Public License for more de0tails.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "util/Logger.hpp"
#include "cut/model/Cut.hpp"
#include "CupsGetOpt.hpp"
#include "Document.hpp"

#include "cut/model/Clip.hpp"
#include "cut/model/Explode.hpp"
#include "cut/model/Reduce.hpp"
#include "cut/model/SvgPlot.hpp"
#include "cut/model/NearestPathSorting.h"
#include "cut/geom/sink/AddSink.hpp"

#include "cut/graph/Planar.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
/**
 * Cups filter entry point.
 *
 * @param argc The number of command line options passed to the program.
 * @param argv An array of strings where each string represents a command line
 * argument.
 * @return An integer where 0 represents successful termination, any other
 * value represents an error code.
 */

void plot(const Cut& cut, const string& prefix) {
  plot_shared_points(cut, (prefix + "_points_shared.svg").c_str());
  plot_shared_segments(cut, (prefix + "_segments_shared.svg").c_str());
  plot_path_order(cut, (prefix + "_path_order.svg").c_str());
}

int main(int argc, char *argv[]) {
  typedef DocumentSettings DS;
  Logger::init(CC_WARNING);
  clock_t start = clock();

  // Make sure status messages are not buffered
  setbuf(stderr, NULL);

  Document doc;
  CupsOptions cupsOpts = CupsGetOpt::load_document(doc, argc, argv);
  string basename = doc.get(DS::FILENAME);
  Coord_t dpi = doc.get(DS::RESOLUTION);
  Coord_t width = doc.get(DS::WIDTH).in(PX);
  Coord_t height = doc.get(DS::HEIGHT).in(PX);
  string v;
  Distance reduceMax(0.1,MM, dpi);
  if(cupsOpts.get(CupsOptions::VECTOR_REDUCE, v)) {
    reduceMax = Distance(boost::lexical_cast<uint16_t>(v), MM, dpi);
  }

  BOOST_FOREACH(Cut* p_cut, doc.cutList) {
    Cut& cut = *p_cut;
    plot(cut, basename + "_input");

    Cut clipped = make_from(cut);
    Cut planar = make_from(cut);
    Cut exploded = make_from(cut);
    Cut reduced = make_from(cut);
    Cut sorted = make_from(cut);

    clip(cut, clipped, Box(Point(0,0),Point(width,height)));
    plot(clipped, basename + "_clipped");

    explode(clipped, exploded);
    plot(exploded, basename + "_exploded");

/*    make_planar(exploded, planar);
    plot(planar, basename + "_planar");*/

    reduce(exploded, reduced, reduceMax.in(PX));
    plot(reduced, basename + "_reduced");

    nearest_path_sorting(reduced, sorted);
    plot(sorted, basename + "_sorted");

    cut.clear();
    cut = sorted;
    plot(cut, basename + "_cut");
  }

  doc.write(std::cout);

  clock_t end = clock() - start;
  float seconds = 1.0 * end / CLOCKS_PER_SEC;
  LOG_DEBUG(seconds);

  return 0;
}

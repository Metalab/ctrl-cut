#include "Translate.hpp"
#include "cut/geom/Geometry.hpp"
#include "cut/geom/algorithms/Append.hpp"

void translateTo(const Route& src, Route& sink, const Point& translate) {
  namespace tl = boost::geometry::strategy::transform;
#if BOOST_VERSION >= 105500
  tl::translate_transformer<double, 2, 2> transformer(translate.x, translate.y);
#else
  tl::translate_transformer<Point, Point> transformer(translate.x, translate.y);
#endif

  for(const Path& path : src) {
    Path translated;
    boost::geometry::transform(path, translated, transformer);
    append(sink, translated);
  }
}

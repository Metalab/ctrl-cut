#ifndef SERIALIZEONIONSKINSFILTER_H_
#define SERIALIZEONIONSKINSFILTER_H_

#include "Filter.h"

class SerializeOnionSkinsFilter: public VectorFilter {
 public:
  SerializeOnionSkinsFilter(){
    this->provides.insert(SER_OS_FILTER_ID);
    this->preFilters.insert(DO_FILTER_ID);
  }

  virtual ~SerializeOnionSkinsFilter();
  void filter(LaserPass *pass);
};

#endif /* SERIALIZEONIONSKINSFILTER_H_ */

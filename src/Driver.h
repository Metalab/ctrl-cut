/*
 * Driver.h
 *
 *  Created on: 18.10.2009
 *      Author: amir
 */

#ifndef DRIVER_H_
#define DRIVER_H_

#include "LaserJob.h"

class Driver {
private:
	void filter(LaserJob *job);

public:
    Driver();
    virtual ~Driver();
    void process(LaserJob *job);
};

#endif /* DRIVER_H_ */

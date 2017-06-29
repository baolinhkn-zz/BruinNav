//
//  support.hpp
//  BruinNav
//
//  Created by Baolinh Nguyen on 3/13/17.
//  Copyright © 2017 Baolinh Nguyen. All rights reserved.
//

#ifndef SUPPORT_INCLUDED
#define SUPPORT_INCLUDED

#include "provided.h"
using namespace std;

string directionOfLine(const GeoSegment& gs);

bool operator < (GeoCoord lhs, GeoCoord rhs);

bool operator == (GeoCoord lhs, GeoCoord rhs);
bool operator != (GeoCoord lhs, GeoCoord rhs);


#endif /* SUPPORT_INCLUDED */

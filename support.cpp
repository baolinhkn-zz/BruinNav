//
//  support.cpp
//  BruinNav
//
//  Created by Baolinh Nguyen on 3/13/17.
//  Copyright © 2017 Baolinh Nguyen. All rights reserved.
//

#include "support.h"
#include <string>
using namespace std;


string directionOfLine(const GeoSegment& gs)            //obtain direction of line
{
    double angle = angleOfLine(gs);
    if (angle >= 0 && angle <= 22.5)
        return "east";
    else if (angle <= 67.5)
        return "northeast";
    else if (angle <= 112.5)
        return "north";
    else if (angle <= 157.5)
        return "northwest";
    else if (angle <= 202.5)
        return "west";
    else if (angle <= 247.5)
        return "southwest";
    else if (angle <= 292.5)
        return "south";
    else if (angle <= 337.5)
        return "southeast";
    else
        return "east";
   
}


bool operator<(GeoCoord lhs, GeoCoord rhs)          //overload < operator for GeoCoord
{
    if (lhs.longitudeText < rhs.longitudeText)
        return true;
    if (lhs.longitudeText > rhs.longitudeText)
        return false;
    if (lhs.longitudeText == rhs.longitudeText)
    {
        if (lhs.latitudeText < rhs.latitudeText)
            return true;
        else
            return false;
    }
    return false;
}

bool operator == (GeoCoord lhs, GeoCoord rhs)           //overload == operator for GeoCoord
{
    if (lhs.longitudeText == rhs.longitudeText && lhs.latitudeText == rhs.latitudeText)
        return true;
    return false;
}
bool operator != (GeoCoord lhs, GeoCoord rhs)           //overload != operator for GeoCoord
{
    if (lhs.longitude == rhs.longitude && lhs.latitude == rhs.latitude)
        return false;
    return true;
}

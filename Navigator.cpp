#include "provided.h"
#include <string>
#include <vector>
#include <queue>
#include <list>
#include "support.h"
#include "MyMap.h"
using namespace std;


#include <iostream>

struct Node
{
    Node()
    {
    }
    Node(GeoCoord c, GeoCoord start, GeoCoord end)
    : g(distanceEarthMiles(start, c)), h(distanceEarthMiles(c, end)) , coord(c.latitudeText, c.longitudeText)
    {
        f = g + h;
    }
    Node& operator=(const Node& rhs);
    GeoCoord coord;
    double g;       //cost to get to, start to current
    double h;       //cost from current to end
    double f;       //f = g + h
    bool operator<(const Node& lhs) const;
    bool operator>(const Node& lhs) const;
    bool operator==(const Node& lhs) const;
};

Node& Node::operator= (const Node& rhs)
{
    if (this != &rhs)
    {
        this->f = rhs.f;
        this->coord = rhs.coord;
    }
    return *this;
}

bool Node::operator<(const Node& lhs) const
{
    if (this->f < lhs.f)
        return false;
    return true;
}

bool Node::operator>(const Node& lhs) const
{
    if (this->f < lhs.f)
        return false;
    return true;
}

bool Node::operator==(const Node& lhs) const
{
    if (this->coord != lhs.coord)
        return false;
    return true;
}

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;

private:

    MapLoader m_map;
    AttractionMapper m_attractions;
    SegmentMapper m_segments;
    bool search(const GeoCoord& start, const GeoCoord& end, MyMap<GeoCoord, GeoCoord>& came_from) const;
    string determineProceedDirection (const GeoSegment& line) const;
    string determineTurnDirection(const GeoSegment& initial, const GeoSegment& f) const;
    void translate(GeoCoord s, GeoCoord e, vector<NavSegment>& dir, MyMap<GeoCoord, GeoCoord>& map) const;

};

NavigatorImpl::NavigatorImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
}

bool NavigatorImpl::search(const GeoCoord& start, const GeoCoord& end, MyMap<GeoCoord, GeoCoord>& came_from) const
{
    priority_queue<Node> open;
    Node startNode(start, start, end);
    open.push(startNode);
    MyMap<GeoCoord, Node> closed;
    closed.associate(startNode.coord, startNode);
    
    //A* SEARCH ALGORITHM MODIFIED FOR GEO COORDINATES
    while (!open.empty())
    {
        Node current = open.top();
        open.pop();
        if (current.coord == end)
        {
            return true;
        }
        vector<StreetSegment> neighbors = m_segments.getSegments(current.coord);    //looks for neighboring coordinates
        for (auto nptr = neighbors.begin(); nptr != neighbors.end(); nptr++)
        {
            GeoCoord neighbor;
            GeoCoord neighbor2;
            
            bool n2check = false;
            
            if (current.coord == (*nptr).segment.end)
            {
                neighbor = (*nptr).segment.start;
                
            }
            else if (current.coord == (*nptr).segment.start)
            {
                neighbor = (*nptr).segment.end;
            }
            else
            {
                n2check = true;
                neighbor = (*nptr).segment.start;
                neighbor2 = (*nptr).segment.end;
            }
            
            vector<Attraction> attr = (*nptr).attractions;
            for (auto a = attr.begin(); a != attr.end(); a++)
            {
                if ((*a).geocoordinates == end)
                    neighbor = (*a).geocoordinates;
            }

            Node next(neighbor, start, end);
            //add to closed if coordinate is not there already or if it costs less to go here
            if (closed.find(neighbor) == nullptr || next.f < (*closed.find(neighbor)).f)
            {
                came_from.associate(neighbor, current.coord);   //add to path
                open.push(next);
            }
            if (n2check)
            {
                Node next2(neighbor2, start, end);
                came_from.associate(neighbor, current.coord);   //add to path
                open.push(next2);
            }
            //add to closed list
            closed.associate(current.coord, current);
        }
    }
    return false;
}

void NavigatorImpl::translate(GeoCoord s, GeoCoord e, vector<NavSegment>& dir, MyMap<GeoCoord, GeoCoord>& map) const
{
    //look for the street segment with the end attraction
    GeoCoord current = *map.find(e);
    vector<StreetSegment> path;
    
    //create a new segment connecting the end to the last coordinate
    vector<StreetSegment> endSeg = m_segments.getSegments(e);
    StreetSegment newSeg;
    for (auto ptr = endSeg.begin(); ptr != endSeg.end(); ptr++)
    {
        for (auto aptr = (*ptr).attractions.begin(); aptr != (*ptr).attractions.end(); aptr++)
        {
            if ((*aptr).geocoordinates == e)
            {
                newSeg.segment.start = current;
                newSeg.segment.end = e;
                newSeg.attractions = (*ptr).attractions;
                newSeg.streetName = (*ptr).streetName;
            }
        }
    }
    path.push_back(newSeg);
    //go through map and create street segments for each coordinate in the path
    while (map.find(current) != nullptr)
    {
        GeoCoord prev = *(map.find(current));
        vector<StreetSegment> currentSeg = m_segments.getSegments(current);
        StreetSegment nextSeg;
        for (auto p = currentSeg.begin(); p!= currentSeg.end(); p++)
        {
            if (((*p).segment.start == current && (*p).segment.end == prev) ||
                ((*p).segment.end == current && (*p).segment.start == prev))
            {
                nextSeg.segment.start = prev;
                nextSeg.segment.end = current;
                nextSeg.attractions = (*p).attractions;
                nextSeg.streetName = (*p).streetName;
                path.insert(path.begin(), nextSeg);
            }
        }
        current = *(map.find(current));
    }
    
    //create segment connecting the start segment
    StreetSegment firstSeg;
    vector<StreetSegment> beginSeg = m_segments.getSegments(s);
    for (auto ptr = beginSeg.begin(); ptr!= beginSeg.end(); ptr++)
    {
        for (auto aptr = (*ptr).attractions.begin(); aptr != (*ptr).attractions.end(); aptr++)
        {
            if ((*aptr).geocoordinates == s)
            {
                firstSeg.segment.start = s;
                firstSeg.segment.end = current;
                firstSeg.attractions = (*ptr).attractions;
                firstSeg.streetName = (*ptr).streetName;
            }
        }
    }
    path.insert(path.begin(), firstSeg);
    
    //go through and create nav segments
    for (int q = 0; q != path.size(); q++)
    {
        if (q != 0)
        {
            //check for a turn segment
            if (path[q].streetName != path[q-1].streetName)
            {
                //create a turn segment
                NavSegment turn(determineTurnDirection(path[q-1].segment, path[q].segment), path[q].streetName);
                dir.push_back(turn);
            }
        }
        //create a proceed segment
        double distance = distanceEarthMiles(path[q].segment.start, path[q].segment.end);
        NavSegment proceed(determineProceedDirection(path[q].segment), path[q].streetName, distance, path[q].segment);
        dir.push_back(proceed);
    }
    
}

string NavigatorImpl::determineProceedDirection (const GeoSegment& line) const
{
    double angle = angleOfLine(line);
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

string NavigatorImpl::determineTurnDirection(const GeoSegment& initial, const GeoSegment& f) const
{
    double angle = angleBetween2Lines(initial, f);
    if (angle < 180)
        return "left";
    else
        return "right";
}

bool NavigatorImpl::loadMapData(string mapFile)
{
    if (m_map.load(mapFile)==false)
        return false;
    m_attractions.init(m_map);
    m_segments.init(m_map);
	return true;
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
    GeoCoord geoStart;
    GeoCoord geoEnd;
    //check if the attraction (start/end) if valid
    if (m_attractions.getGeoCoord(start, geoStart) == false)
        return NAV_BAD_SOURCE;
    if (m_attractions.getGeoCoord(end, geoEnd) == false)
        return NAV_BAD_DESTINATION;
    
    auto p = directions.begin();
    while (p!= directions.end())        //erase the direction if there is something already
    {
        p = directions.erase(p);
    }
    
    MyMap<GeoCoord, GeoCoord> CoordPath;

    if (geoStart == geoEnd)         //check if the start and the end are the same
        return NAV_SUCCESS;
    
    if (!search(geoStart, geoEnd, CoordPath))       //calls search
        return NAV_NO_ROUTE;
    
    translate(geoStart, geoEnd, directions, CoordPath);     //translates to the directions
    
    return NAV_SUCCESS;
}


//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}

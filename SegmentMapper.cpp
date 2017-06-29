#include "provided.h"
#include <vector>
#include "MyMap.h"
#include "support.h"
using namespace std;

bool operator==(const StreetSegment& lhs, const StreetSegment& rhs)
{
    if (lhs.streetName != rhs.streetName)
        return false;
    if (lhs.segment.start == rhs.segment. start)
        return false;
    if (lhs.segment.end == rhs.segment.end)
        return false;
    if (lhs.segment.end == rhs.segment.start)
        return false;
    return true;
}


class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
    MyMap<GeoCoord, vector<StreetSegment>> m_segments;
    string geoConvert(const GeoCoord g) const;
    void insert(GeoCoord cood, StreetSegment seg);
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
    StreetSegment seg;
    int i = 0;
    while (ml.getSegment(i, seg))
    {
        GeoCoord start = seg.segment.start;
        GeoCoord end = seg.segment.end;
        insert(start, seg);
        insert(end, seg);
        for (auto p = seg.attractions.begin(); p != seg.attractions.end(); p++)
        {
            GeoCoord attractionCoord = (*p).geocoordinates;
            insert(attractionCoord, seg);
        }
        i++;
    }
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
    return *(m_segments.find(gc));
}

void SegmentMapperImpl::insert(GeoCoord coord, StreetSegment seg)
{
    if (seg.streetName == "Shaftesbury Avenue")
        string temp;
    vector<StreetSegment>* cur = m_segments.find(coord);
    if (cur == nullptr)
    {
        vector<StreetSegment> temp;
        temp.push_back(seg);
        m_segments.associate(coord, temp);
    }
    else
    {
            cur->push_back(seg);
        //m_segments.associate(coord, *cur);
    }

    
}
//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}

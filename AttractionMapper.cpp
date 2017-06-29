#include "provided.h"
#include <string>
#include "MyMap.h"
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
    MyMap<string, GeoCoord> m_attractions;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
    StreetSegment seg;
    int i = 0;
    while (ml.getSegment(i, seg))
    {
        //searches each segment's attractions
        for (auto p = seg.attractions.begin(); p != seg.attractions.end(); p++)
        {
            string name;
            for (int i = 0; i < (*p).name.size(); i++)
                (*p).name[i] = tolower((*p).name[i]);           //conver to lower for searching purposes
            m_attractions.associate((*p).name, (*p).geocoordinates);        //associate with attraction map

        }
        
        i++;
    }
   // m_attractions.print();
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
    for (int i = 0; i < attraction.size(); i++)
        attraction[i] = tolower(attraction[i]);         //changes to lowercase
    const GeoCoord* result = m_attractions.find(attraction);    //use MyMap find function
	if (result == nullptr)      //nullptr means the key is not there
        return false;
    gc = *result;
    return true;
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}

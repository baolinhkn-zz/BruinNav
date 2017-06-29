#include "provided.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>

using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
    vector<StreetSegment*> m_streetSegments;
    int m_nStreetSegments;
    int stringToInt(string s);
};

MapLoaderImpl::MapLoaderImpl()
{
    m_nStreetSegments = 0;
}

MapLoaderImpl::~MapLoaderImpl()
{
    for (int i = 0; i < m_nStreetSegments; i++)
    {
        delete m_streetSegments[i];
    }
}

bool MapLoaderImpl::load(string mapFile)
{
    ifstream map(mapFile);
    if (!map)
    {
        return false;
    }
    string line;
    int i = 1;
    int attractions = 0;
    while (getline(map, line))      //looks through the lines of the file
    {
        switch (i)
        {
            case 1:             //case 1: name of street
            {
                StreetSegment* newStreet = new StreetSegment;
                newStreet->streetName = line;
                m_streetSegments.push_back(newStreet);
                m_nStreetSegments++;
                i++;
                break;
            }
            case 2:         //case 2: start latitude of street and end latitude of street
            {
                string Blat;
                string Blong;
                string Elat;
                string Elong;
                int k = 0;
                while (line[k] != ',')
                {
                    Blat = Blat + line[k];
                    k++;
                }
                k++;
                while (line[k] != ' ')
                {
                    Blong = Blong + line[k];
                    k++;
                }
                k++;
                while (line[k] != ',')
                {
                    Elat = Elat + line[k];
                    k++;
                }
                k++;
                while (k < line.size())
                {
                    Elong = Elong + line[k];
                    k++;
                }
                GeoCoord begin = GeoCoord(Blat, Blong);
                GeoCoord end = GeoCoord(Elat, Elong);
                m_streetSegments[m_nStreetSegments-1]->segment = GeoSegment(begin, end);
                i++;
                break;
            }
            case 3:         //case 3: check attraction num
            {
                for (int i = 0; i < line.size(); i++)
                {
                    attractions = attractions * 10;
                    attractions = attractions + (line[i] - '0');
                }
                if (attractions != 0)
                {
                    i = 0;
                }
                else
                    i = 1;
                break;
            }
            case 0:         //add attractions to segment's vector
            {
                int k = 0;
                string name;
                while (line[k] != '|')
                {
                    name = name + line[k];
                    k++;
                }
                string lat;
                string lon;
                k++;
                while (line[k] != ',')
                {
                    lat = lat + line[k];
                    k++;
                }
                k++;
                while (k < line.size())
                {
                    lon = lon + line[k];
                    k++;
                }
                Attraction newAtr;
                newAtr.name = name;
                newAtr.geocoordinates = GeoCoord(lat, lon);
                m_streetSegments[m_nStreetSegments-1]->attractions.push_back(newAtr);
                attractions--;
                if (attractions == 0)
                {
                    i = 1;
                }
                break;
            }
        }
    }
    return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
	return m_nStreetSegments;
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
    if (segNum >= m_nStreetSegments)
        return false;
    seg = *(m_streetSegments.at(segNum));
    return true;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}

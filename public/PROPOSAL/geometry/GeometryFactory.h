
#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/function.hpp>

#include "PROPOSAL/geometry/Geometry.h"

namespace PROPOSAL
{

class GeometryFactory
{
    public:

    enum Enum
    {
        Sphere = 0,
        Box,
        Cylinder,
    };

    typedef boost::function<Geometry* (void)> RegisterFunction;
    typedef std::map<std::string, RegisterFunction> GeometryMapString;
    typedef std::map<Enum, RegisterFunction> GeometryMapEnum;

    void Register(const std::string&, const Enum&, RegisterFunction);

    Geometry* CreateGeometry(const std::string&);
    Geometry* CreateGeometry(const Enum&);
    Geometry* CreateGeometry(boost::property_tree::ptree const& pt);

    static GeometryFactory& Get()
    {
        static GeometryFactory instance;
        return instance;
    }

    private:
    GeometryFactory();
    ~GeometryFactory();

    GeometryMapString geometry_map_str;
    GeometryMapEnum geometry_map_enum;
    // GeometryMapPTree geometry_map_ptree;
    // ScatteringMapEnum scattering_map_enum_;
};

} /* PROPOSAL */


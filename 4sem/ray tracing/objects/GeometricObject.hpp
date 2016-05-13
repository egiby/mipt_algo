#ifndef _GEOMETRIC_OBJECT
#define _GEOMETRIC_OBJECT

#include "../geometry/Double.hpp"
#include "../geometry/Geometry.hpp"

#include <cstdint>
#include <cstdio>
#include <cmath>

#include <iostream>
#include <algorithm>
using std::cerr;

namespace NGeometricObjects
{
    struct Color
    {
        ui32 red, green, blue;
    };
    
    Color operator * (const Color &c, const NDouble::Double &d)
    {
        int r = std::min(std::max((int)std::lround(c.red * d), 0), 255),
            g = std::min(std::max((int)std::lround(c.green * d), 0), 255),
            b = std::min(std::max((int)std::lround(c.blue * d), 0), 255);
        
        return {(ui32)r, (ui32)g, (ui32)b};
    }
    
    struct Material
    {
        ui32 reflecting_coefficient;
        Color color;
    };
    
    class IGeometricObject
    {
    public:
        virtual NDouble::Double intersect(const NGeometry::Ray &r) const = 0;
        virtual Color getColor() const = 0;
        virtual NGeometry::Vector getNormal(const NGeometry::Point &p) const = 0;
        //~ virtual NGeometry::Point getCenter() const = 0;
        virtual ~IGeometricObject()
        {
        }
    };
};

#endif

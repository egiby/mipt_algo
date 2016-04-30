#ifndef _GEOMETRIC_OBJECT
#define _GEOMETRIC_OBJECT

#include "../geometry/Double.hpp"
#include "../geometry/Geometry.hpp"

#include <cstdint>
#include <cstdio>
#include <cmath>

#include <iostream>
using std::cerr;

namespace NGeometricObjects
{
    struct Color
    {
        ui32 red, green, blue;
    };
    
    class IGeometricObject
    {
    public:
        virtual NDouble::Double intersect(const NGeometry::Ray &r) const = 0;
        virtual Color getColor() const = 0;
        virtual NGeometry::Vector getNormal(const NGeometry::Point &p) const = 0;
        virtual ~IGeometricObject()
        {
        }
    };
};

#endif

#ifndef _GEOMETRIC_OBJECT
#define _GEOMETRIC_OBJECT

#include "Double.hpp"
#include "Geometry.hpp"

#include <cstdint>
#include <cstdio>
#include <cmath>

#include <iostream>
using std::cerr;

namespace NGeometricObjects
{
    class IGeometricObject
    {
    public:
        virtual NDouble::Double intersect(const NGeometry::Ray &r) const = 0;
        virtual ui32 getRed() const = 0;
        virtual ui32 getGreen() const = 0;
        virtual ui32 getBlue() const = 0;
        virtual ~IGeometricObject()
        {
        }
    };
    
    struct Color
    {
        ui32 red, green, blue;
    };
};

#endif
